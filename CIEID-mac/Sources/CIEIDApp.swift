import SwiftUI

@main
struct CIEIDApp: App {
    @State private var model = CieViewModel()

    var body: some Scene {
        WindowGroup("CIE ID") {
            ContentView(model: model)
                .onAppear { model.start() }
                .onDisappear { model.stop() }
        }
        .windowResizability(.contentSize)
        .defaultSize(width: 820, height: 620)
    }
}
