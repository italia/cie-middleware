import Foundation

enum CieCardState: Equatable {
    case libraryError(String)
    case noReader
    case noCard(slotDescription: String)
    case cardPresent(slotDescription: String, tokenLabel: String, serial: String)
}

enum CiePinError: LocalizedError {
    case failed(code: UInt, attempts: Int?, message: String)

    var errorDescription: String? {
        switch self {
        case .failed(_, _, let message):
            return message
        }
    }
}

final class CiePkiBridge: @unchecked Sendable {
    static let shared = CiePkiBridge()

    private let queue = DispatchQueue(label: "it.ipzs.cieid.pkcs11", qos: .userInitiated)
    private var initialized = false

    private init() {}

    func initializeIfNeeded() throws {
        try queue.sync {
            guard !initialized else { return }
            let rv = C_Initialize(nil)
            guard rv == CKR_OK else {
                throw CiePinError.failed(
                    code: rv,
                    attempts: nil,
                    message: "Impossibile inizializzare PKCS#11 (0x\(String(rv, radix: 16)))"
                )
            }
            initialized = true
        }
    }

    func finalizeLibrary() {
        queue.sync {
            guard initialized else { return }
            _ = C_Finalize(nil)
            initialized = false
        }
    }

    func libraryIdentity() -> String {
        queue.sync {
            do {
                try initializeIfNeededUnlocked()
            } catch {
                return "CIE PKCS11"
            }
            var info = CK_INFO()
            guard C_GetInfo(&info) == CKR_OK else { return "CIE PKCS11" }
            let mfr = Self.trimPKCS11String(info.manufacturerID)
            let desc = Self.trimPKCS11String(info.libraryDescription)
            return "\(mfr) · \(desc)"
        }
    }

    func refreshCardState() -> CieCardState {
        queue.sync {
            do {
                try initializeIfNeededUnlocked()
            } catch {
                return .libraryError(error.localizedDescription)
            }

            _ = C_UpdateSlotList()

            var count: CK_ULONG = 0
            let tokenPresentFlag: CK_BBOOL = 0
            var rv = C_GetSlotList(tokenPresentFlag, nil, &count)
            guard rv == CKR_OK else {
                return .libraryError("C_GetSlotList fallita (0x\(String(rv, radix: 16)))")
            }

            guard count > 0 else { return .noReader }

            var slots = Array(repeating: CK_SLOT_ID(0), count: Int(count))
            rv = C_GetSlotList(tokenPresentFlag, &slots, &count)
            guard rv == CKR_OK, count > 0 else { return .noReader }

            let slot = slots[0]
            var slotInfo = CK_SLOT_INFO()
            rv = C_GetSlotInfo(slot, &slotInfo)
            let slotDescription: String
            if rv == CKR_OK {
                slotDescription = Self.trimPKCS11String(slotInfo.slotDescription)
            } else {
                slotDescription = "Lettore CIE"
            }

            let tokenPresent = (slotInfo.flags & CKF_TOKEN_PRESENT) != 0
            guard tokenPresent else {
                return .noCard(slotDescription: slotDescription)
            }

            var tokenInfo = CK_TOKEN_INFO()
            rv = C_GetTokenInfo(slot, &tokenInfo)
            guard rv == CKR_OK else {
                return .cardPresent(
                    slotDescription: slotDescription,
                    tokenLabel: "CIE",
                    serial: ""
                )
            }

            return .cardPresent(
                slotDescription: slotDescription,
                tokenLabel: Self.trimPKCS11String(tokenInfo.label),
                serial: Self.trimPKCS11String(tokenInfo.serialNumber)
            )
        }
    }

    func changePIN(
        current: String,
        newPIN: String,
        progress: @escaping @Sendable (Int, String) -> Void
    ) throws -> Int {
        try runPINOperation(progress: progress) { callback in
            var attempts: Int32 = -1
            let rv = current.withCString { currentPtr in
                newPIN.withCString { newPtr in
                    ChangePIN(currentPtr, newPtr, &attempts, callback)
                }
            }
            return (rv, Int(attempts))
        }
    }

    func unlockPIN(
        puk: String,
        newPIN: String,
        progress: @escaping @Sendable (Int, String) -> Void
    ) throws -> Int {
        try runPINOperation(progress: progress) { callback in
            var attempts: Int32 = -1
            let rv = puk.withCString { pukPtr in
                newPIN.withCString { newPtr in
                    UnlockPIN(pukPtr, newPtr, &attempts, callback)
                }
            }
            return (rv, Int(attempts))
        }
    }

    func readPAN() throws -> String {
        try queue.sync {
            var buffer = [CChar](repeating: 0, count: 32)
            let rv = leggiPANCIE(&buffer, UInt32(buffer.count))
            guard rv == CKR_OK else {
                throw CiePinError.failed(
                    code: rv,
                    attempts: nil,
                    message: Self.message(for: rv, attempts: -1)
                )
            }
            return String(cString: buffer)
        }
    }

    func signPDF(
        inputPath: String,
        outputPath: String,
        pin: String,
        pan: String,
        progress: @escaping @Sendable (Int, String) -> Void
    ) throws {
        try queue.sync {
            let box = ProgressBox(handler: progress)
            let progressCallback: PROGRESS_CALLBACK = { value, message in
                let text: String
                if let message {
                    text = String(cString: message)
                } else {
                    text = ""
                }
                ProgressBox.current?.handler(Int(value), text)
                return CKR_OK
            }
            let completedCallback: SIGN_COMPLETED_CALLBACK = { _ in CKR_OK }

            ProgressBox.current = box
            defer { ProgressBox.current = nil }

            let rv = inputPath.withCString { inPtr in
                outputPath.withCString { outPtr in
                    pin.withCString { pinPtr in
                        pan.withCString { panPtr in
                            firmaConCIE(
                                inPtr,
                                "pdf",
                                pinPtr,
                                panPtr,
                                0,
                                0,
                                0,
                                0,
                                0,
                                nil,
                                outPtr,
                                progressCallback,
                                completedCallback
                            )
                        }
                    }
                }
            }

            guard rv == CKR_OK else {
                throw CiePinError.failed(
                    code: rv,
                    attempts: nil,
                    message: Self.message(for: rv, attempts: -1)
                )
            }
        }
    }

    private func runPINOperation(
        progress: @escaping @Sendable (Int, String) -> Void,
        body: (@escaping PROGRESS_CALLBACK) -> (CK_RV, Int)
    ) throws -> Int {
        try queue.sync {
            // ChangePIN/UnlockPIN gestiscono PC/SC in autonomia.
            let box = ProgressBox(handler: progress)
            let callback: PROGRESS_CALLBACK = { value, message in
                let text: String
                if let message {
                    text = String(cString: message)
                } else {
                    text = ""
                }
                ProgressBox.current?.handler(Int(value), text)
                return CKR_OK
            }

            ProgressBox.current = box
            defer { ProgressBox.current = nil }

            let (rv, attempts) = body(callback)
            guard rv == CKR_OK else {
                throw CiePinError.failed(
                    code: rv,
                    attempts: attempts >= 0 ? attempts : nil,
                    message: Self.message(for: rv, attempts: attempts)
                )
            }
            return attempts
        }
    }

    private func initializeIfNeededUnlocked() throws {
        guard !initialized else { return }
        let rv = C_Initialize(nil)
        guard rv == CKR_OK else {
            throw CiePinError.failed(
                code: rv,
                attempts: nil,
                message: "Impossibile inizializzare PKCS#11 (0x\(String(rv, radix: 16)))"
            )
        }
        initialized = true
    }

    private static func trimPKCS11String<T>(_ buffer: T) -> String {
        withUnsafeBytes(of: buffer) { raw in
            let bytes = raw.bindMemory(to: UInt8.self)
            var end = bytes.count
            while end > 0 && (bytes[end - 1] == 0 || bytes[end - 1] == UInt8(ascii: " ")) {
                end -= 1
            }
            return String(decoding: bytes.prefix(end), as: UTF8.self)
        }
    }

    private static func message(for rv: CK_RV, attempts: Int) -> String {
        switch rv {
        case CKR_PIN_INCORRECT:
            if attempts >= 0 {
                return "PIN/PUK non corretto. Tentativi rimasti: \(attempts)"
            }
            return "PIN/PUK non corretto"
        case CKR_PIN_INVALID:
            return "PIN non valido"
        case CKR_PIN_LEN_RANGE:
            return "Lunghezza PIN non valida (attesi 8 caratteri)"
        case CKR_PIN_LOCKED:
            return "PIN bloccato. Usa lo sblocco con PUK"
        case CKR_PIN_EXPIRED:
            return "PIN scaduto"
        case CKR_TOKEN_NOT_PRESENT:
            return "Carta non presente nel lettore"
        case CKR_TOKEN_NOT_RECOGNIZED:
            return "Carta non riconosciuta"
        case CKR_DEVICE_ERROR:
            return "Errore del lettore o della carta"
        case CKR_CANCEL:
            return "Operazione annullata"
        case 0x000000F1:
            return "PAN della CIE non corrispondente"
        default:
            return "Operazione fallita (0x\(String(rv, radix: 16)))"
        }
    }
}

private final class ProgressBox: @unchecked Sendable {
    static var current: ProgressBox?
    let handler: @Sendable (Int, String) -> Void

    init(handler: @escaping @Sendable (Int, String) -> Void) {
        self.handler = handler
    }
}
