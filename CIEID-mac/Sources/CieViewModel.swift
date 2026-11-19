import Foundation
import Observation

enum CieSection: String, CaseIterable, Identifiable {
    case home = "Stato"
    case changePIN = "Cambia PIN"
    case unlockPIN = "Sblocca PIN"
    case signPDF = "Firma PDF"

    var id: String { rawValue }
}

@MainActor
@Observable
final class CieViewModel {
    var section: CieSection = .home
    var cardState: CieCardState = .noReader
    var libraryName: String = "CIE PKCS11"
    var isBusy = false
    var progress: Double = 0
    var progressMessage: String = ""
    var statusBanner: String?
    var statusIsError = false

    var currentPIN = ""
    var newPIN = ""
    var confirmPIN = ""
    var puk = ""
    var signPIN = ""
    var inputPDFPath = ""
    var outputPDFPath = ""
    var detectedPAN = ""

    private var pollTask: Task<Void, Never>?

    func start() {
        libraryName = CiePkiBridge.shared.libraryIdentity()
        refresh()
        pollTask?.cancel()
        pollTask = Task { [weak self] in
            while !Task.isCancelled {
                try? await Task.sleep(for: .seconds(2))
                guard let self, !self.isBusy else { continue }
                self.refresh()
            }
        }
    }

    func stop() {
        pollTask?.cancel()
        pollTask = nil
        CiePkiBridge.shared.finalizeLibrary()
    }

    func refresh() {
        cardState = CiePkiBridge.shared.refreshCardState()
    }

    func changePIN() {
        clearBanner()
        guard validateNewPIN(usingPUK: false) else { return }

        let current = currentPIN
        let next = newPIN
        isBusy = true
        progress = 0
        progressMessage = "Avvio cambio PIN…"

        let progressHandler = makeProgressHandler()
        Task {
            do {
                _ = try await withCheckedThrowingContinuation { (cont: CheckedContinuation<Int, Error>) in
                    DispatchQueue.global(qos: .userInitiated).async {
                        do {
                            let attempts = try CiePkiBridge.shared.changePIN(
                                current: current,
                                newPIN: next,
                                progress: progressHandler
                            )
                            cont.resume(returning: attempts)
                        } catch {
                            cont.resume(throwing: error)
                        }
                    }
                }
                finishSuccess("PIN aggiornato correttamente")
                currentPIN = ""
                newPIN = ""
                confirmPIN = ""
            } catch {
                finishFailure(error.localizedDescription)
            }
        }
    }

    func unlockPIN() {
        clearBanner()
        guard validateNewPIN(usingPUK: true) else { return }

        let pukValue = puk
        let next = newPIN
        isBusy = true
        progress = 0
        progressMessage = "Avvio sblocco PIN…"

        let progressHandler = makeProgressHandler()
        Task {
            do {
                _ = try await withCheckedThrowingContinuation { (cont: CheckedContinuation<Int, Error>) in
                    DispatchQueue.global(qos: .userInitiated).async {
                        do {
                            let attempts = try CiePkiBridge.shared.unlockPIN(
                                puk: pukValue,
                                newPIN: next,
                                progress: progressHandler
                            )
                            cont.resume(returning: attempts)
                        } catch {
                            cont.resume(throwing: error)
                        }
                    }
                }
                finishSuccess("PIN sbloccato e reimpostato")
                puk = ""
                newPIN = ""
                confirmPIN = ""
            } catch {
                finishFailure(error.localizedDescription)
            }
        }
    }

    private func makeProgressHandler() -> @Sendable (Int, String) -> Void {
        { [weak self] value, message in
            Task { @MainActor in
                self?.progress = Double(value) / 100.0
                self?.progressMessage = message
            }
        }
    }

    func refreshPAN() {
        clearBanner()
        do {
            detectedPAN = try CiePkiBridge.shared.readPAN()
            statusBanner = "PAN letto: \(detectedPAN)"
            statusIsError = false
        } catch {
            detectedPAN = ""
            finishFailure(error.localizedDescription)
        }
    }

    func signPDF() {
        clearBanner()
        guard !inputPDFPath.isEmpty else {
            finishFailure("Seleziona un PDF da firmare")
            return
        }
        guard FileManager.default.fileExists(atPath: inputPDFPath) else {
            finishFailure("Il file PDF non esiste")
            return
        }
        guard signPIN.count == 8, signPIN.allSatisfy(\.isNumber) else {
            finishFailure("Il PIN deve essere di 8 cifre")
            return
        }

        let outPath: String
        if outputPDFPath.isEmpty {
            let url = URL(fileURLWithPath: inputPDFPath)
            outPath = url.deletingPathExtension().appendingPathExtension("signed.pdf").path
        } else {
            outPath = outputPDFPath
        }

        let pin = signPIN
        let input = inputPDFPath
        let pan = detectedPAN
        isBusy = true
        progress = 0
        progressMessage = "Avvio firma PDF…"

        let progressHandler = makeProgressHandler()
        Task {
            do {
                try await withCheckedThrowingContinuation { (cont: CheckedContinuation<Void, Error>) in
                    DispatchQueue.global(qos: .userInitiated).async {
                        do {
                            try CiePkiBridge.shared.signPDF(
                                inputPath: input,
                                outputPath: outPath,
                                pin: pin,
                                pan: pan,
                                progress: progressHandler
                            )
                            cont.resume()
                        } catch {
                            cont.resume(throwing: error)
                        }
                    }
                }
                outputPDFPath = outPath
                finishSuccess("PDF firmato: \(outPath)")
                signPIN = ""
            } catch {
                finishFailure(error.localizedDescription)
            }
        }
    }

    private func validateNewPIN(usingPUK: Bool) -> Bool {
        if usingPUK {
            guard puk.count == 8, puk.allSatisfy(\.isNumber) else {
                finishFailure("Il PUK deve essere di 8 cifre")
                return false
            }
        } else {
            guard currentPIN.count == 8, currentPIN.allSatisfy(\.isNumber) else {
                finishFailure("Il PIN attuale deve essere di 8 cifre")
                return false
            }
        }

        guard newPIN.count == 8, newPIN.allSatisfy(\.isNumber) else {
            finishFailure("Il nuovo PIN deve essere di 8 cifre")
            return false
        }
        guard newPIN == confirmPIN else {
            finishFailure("I due PIN non coincidono")
            return false
        }
        return true
    }

    private func finishSuccess(_ message: String) {
        isBusy = false
        progress = 1
        progressMessage = message
        statusBanner = message
        statusIsError = false
        refresh()
    }

    private func finishFailure(_ message: String) {
        isBusy = false
        progressMessage = message
        statusBanner = message
        statusIsError = true
    }

    private func clearBanner() {
        statusBanner = nil
        statusIsError = false
    }
}
