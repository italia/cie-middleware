import SwiftUI
import UniformTypeIdentifiers
import AppKit

struct ContentView: View {
    @Bindable var model: CieViewModel
    @State private var statusOpacity: Double = 1

    var body: some View {
        ZStack {
            AtmosphereBackground()

            VStack(alignment: .leading, spacing: 28) {
                header
                sectionPicker
                content
                if let banner = model.statusBanner {
                    bannerView(banner)
                        .transition(.move(edge: .bottom).combined(with: .opacity))
                }
                Spacer(minLength: 0)
            }
            .padding(36)
            .frame(maxWidth: 720, alignment: .leading)
        }
        .frame(minWidth: 860, minHeight: 580)
        .animation(.easeInOut(duration: 0.35), value: model.section)
        .animation(.spring(response: 0.45, dampingFraction: 0.85), value: model.statusBanner)
        .onChange(of: model.cardState) { _, _ in
            withAnimation(.easeInOut(duration: 0.4)) {
                statusOpacity = 0.35
            }
            withAnimation(.easeInOut(duration: 0.45).delay(0.05)) {
                statusOpacity = 1
            }
        }
    }

    private var header: some View {
        VStack(alignment: .leading, spacing: 8) {
            Text("CIE ID")
                .font(CIETheme.brandFont)
                .foregroundStyle(CIETheme.deep)
                .tracking(1.5)

            Text("Gestisci PIN e stato della Carta d’Identità Elettronica.")
                .font(CIETheme.bodyFont)
                .foregroundStyle(CIETheme.ink.opacity(0.72))

            Text(model.libraryName)
                .font(CIETheme.monoFont)
                .foregroundStyle(CIETheme.green.opacity(0.9))
                .padding(.top, 2)
        }
    }

    private var sectionPicker: some View {
        HStack(spacing: 0) {
            ForEach(CieSection.allCases) { section in
                Button {
                    model.section = section
                } label: {
                    Text(section.rawValue)
                        .font(CIETheme.bodyFont.weight(.semibold))
                        .padding(.horizontal, 16)
                        .padding(.vertical, 10)
                        .frame(maxWidth: .infinity)
                        .foregroundStyle(model.section == section ? CIETheme.sand : CIETheme.ink.opacity(0.7))
                        .background(
                            RoundedRectangle(cornerRadius: 10, style: .continuous)
                                .fill(model.section == section ? CIETheme.deep : Color.clear)
                        )
                }
                .buttonStyle(.plain)
            }
        }
        .padding(4)
        .background(
            RoundedRectangle(cornerRadius: 12, style: .continuous)
                .fill(Color.white.opacity(0.35))
        )
    }

    @ViewBuilder
    private var content: some View {
        switch model.section {
        case .home:
            homeSection
                .opacity(statusOpacity)
        case .changePIN:
            pinForm(
                title: "Cambia PIN",
                subtitle: "Inserisci il PIN attuale e scegline uno nuovo di 8 cifre.",
                primaryField: ("PIN attuale", $model.currentPIN),
                actionTitle: "Aggiorna PIN",
                action: model.changePIN
            )
        case .unlockPIN:
            pinForm(
                title: "Sblocca PIN",
                subtitle: "Usa il PUK della CIE per impostare un nuovo PIN.",
                primaryField: ("PUK", $model.puk),
                actionTitle: "Sblocca e reimposta",
                action: model.unlockPIN
            )
        case .signPDF:
            signPDFSection
        }
    }

    private var signPDFSection: some View {
        VStack(alignment: .leading, spacing: 16) {
            Text("Firma PDF")
                .font(CIETheme.titleFont)
                .foregroundStyle(CIETheme.deep)
            Text("Firma PAdES con la CIE (senza grafica). Serve PIN a 8 cifre e carta sul lettore.")
                .font(CIETheme.bodyFont)
                .foregroundStyle(CIETheme.ink.opacity(0.75))

            HStack(spacing: 10) {
                Button("Scegli PDF…") { pickInputPDF() }
                    .buttonStyle(.bordered)
                    .tint(CIETheme.green)
                Button("Leggi PAN") { model.refreshPAN() }
                    .buttonStyle(.bordered)
                    .tint(CIETheme.deep)
                    .disabled(model.isBusy)
            }

            Text(model.inputPDFPath.isEmpty ? "Nessun file selezionato" : model.inputPDFPath)
                .font(CIETheme.monoFont)
                .foregroundStyle(CIETheme.ink.opacity(0.8))
                .lineLimit(2)

            if !model.detectedPAN.isEmpty {
                Text("PAN: \(model.detectedPAN)")
                    .font(CIETheme.monoFont)
                    .foregroundStyle(CIETheme.green)
            }

            SecureField("PIN (8 cifre)", text: $model.signPIN)
                .textFieldStyle(.roundedBorder)
                .font(CIETheme.bodyFont)
                .frame(maxWidth: 320)

            if model.isBusy || model.progress > 0 {
                VStack(alignment: .leading, spacing: 8) {
                    ProgressView(value: model.progress)
                        .tint(CIETheme.green)
                        .animation(.easeInOut(duration: 0.25), value: model.progress)
                    Text(model.progressMessage)
                        .font(CIETheme.bodyFont)
                        .foregroundStyle(CIETheme.ink.opacity(0.7))
                }
                .frame(maxWidth: 420)
            }

            Button("Firma PDF", action: model.signPDF)
                .font(CIETheme.bodyFont.weight(.semibold))
                .buttonStyle(.borderedProminent)
                .tint(CIETheme.deep)
                .disabled(model.isBusy)
        }
        .padding(.top, 8)
    }

    private func pickInputPDF() {
        let panel = NSOpenPanel()
        panel.allowedContentTypes = [.pdf]
        panel.allowsMultipleSelection = false
        panel.canChooseDirectories = false
        if panel.runModal() == .OK, let url = panel.url {
            model.inputPDFPath = url.path
            let signed = url.deletingPathExtension().appendingPathExtension("signed.pdf")
            model.outputPDFPath = signed.path
        }
    }

    private var homeSection: some View {
        VStack(alignment: .leading, spacing: 18) {
            Text(statusTitle)
                .font(CIETheme.titleFont)
                .foregroundStyle(CIETheme.deep)

            Text(statusDetail)
                .font(CIETheme.bodyFont)
                .foregroundStyle(CIETheme.ink.opacity(0.8))
                .fixedSize(horizontal: false, vertical: true)

            HStack(spacing: 12) {
                statusDot
                Text(statusHeadline)
                    .font(CIETheme.bodyFont.weight(.semibold))
                    .foregroundStyle(CIETheme.ink)
            }
            .padding(.top, 6)

            Button("Aggiorna ora") {
                model.refresh()
            }
            .font(CIETheme.bodyFont.weight(.semibold))
            .buttonStyle(.bordered)
            .tint(CIETheme.green)
            .disabled(model.isBusy)
        }
        .padding(.top, 8)
    }

    private func pinForm(
        title: String,
        subtitle: String,
        primaryField: (String, Binding<String>),
        actionTitle: String,
        action: @escaping () -> Void
    ) -> some View {
        VStack(alignment: .leading, spacing: 16) {
            Text(title)
                .font(CIETheme.titleFont)
                .foregroundStyle(CIETheme.deep)
            Text(subtitle)
                .font(CIETheme.bodyFont)
                .foregroundStyle(CIETheme.ink.opacity(0.75))

            SecureField(primaryField.0, text: primaryField.1)
                .textFieldStyle(.roundedBorder)
                .font(CIETheme.bodyFont)
                .frame(maxWidth: 320)

            SecureField("Nuovo PIN", text: $model.newPIN)
                .textFieldStyle(.roundedBorder)
                .font(CIETheme.bodyFont)
                .frame(maxWidth: 320)

            SecureField("Conferma nuovo PIN", text: $model.confirmPIN)
                .textFieldStyle(.roundedBorder)
                .font(CIETheme.bodyFont)
                .frame(maxWidth: 320)

            if model.isBusy || model.progress > 0 {
                VStack(alignment: .leading, spacing: 8) {
                    ProgressView(value: model.progress)
                        .tint(CIETheme.green)
                        .animation(.easeInOut(duration: 0.25), value: model.progress)
                    Text(model.progressMessage)
                        .font(CIETheme.bodyFont)
                        .foregroundStyle(CIETheme.ink.opacity(0.7))
                }
                .frame(maxWidth: 420)
            }

            Button(actionTitle, action: action)
                .font(CIETheme.bodyFont.weight(.semibold))
                .buttonStyle(.borderedProminent)
                .tint(CIETheme.deep)
                .disabled(model.isBusy)
                .padding(.top, 4)
        }
        .padding(.top, 8)
    }

    private func bannerView(_ text: String) -> some View {
        Text(text)
            .font(CIETheme.bodyFont.weight(.medium))
            .foregroundStyle(model.statusIsError ? CIETheme.danger : CIETheme.deep)
            .padding(.horizontal, 14)
            .padding(.vertical, 10)
            .background(
                RoundedRectangle(cornerRadius: 10, style: .continuous)
                    .fill(Color.white.opacity(0.7))
            )
    }

    private var statusTitle: String {
        switch model.cardState {
        case .libraryError:
            return "Libreria non disponibile"
        case .noReader:
            return "Nessun lettore"
        case .noCard:
            return "Carta assente"
        case .cardPresent:
            return "CIE rilevata"
        }
    }

    private var statusHeadline: String {
        switch model.cardState {
        case .libraryError:
            return "Errore"
        case .noReader:
            return "Collega un lettore smart card"
        case .noCard:
            return "Appoggia la CIE sul lettore"
        case .cardPresent:
            return "Pronta per cambio o sblocco PIN"
        }
    }

    private var statusDetail: String {
        switch model.cardState {
        case .libraryError(let message):
            return message
        case .noReader:
            return "Non risulta alcun lettore PC/SC collegato. Collega il lettore CIE e attendi il refresh automatico."
        case .noCard(let slotDescription):
            return "Lettore: \(slotDescription). Nessuna carta presente nello slot."
        case .cardPresent(let slotDescription, let tokenLabel, let serial):
            var parts = ["Lettore: \(slotDescription)", "Token: \(tokenLabel)"]
            if !serial.isEmpty {
                parts.append("Seriale: \(serial)")
            }
            return parts.joined(separator: "\n")
        }
    }

    private var statusDot: some View {
        Circle()
            .fill(statusColor)
            .frame(width: 10, height: 10)
            .shadow(color: statusColor.opacity(0.5), radius: 6)
    }

    private var statusColor: Color {
        switch model.cardState {
        case .libraryError:
            return CIETheme.danger
        case .noReader, .noCard:
            return Color.orange
        case .cardPresent:
            return CIETheme.green
        }
    }
}
