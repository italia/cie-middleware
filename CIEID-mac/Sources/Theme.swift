import SwiftUI

enum CIETheme {
    static let green = Color(red: 0.00, green: 0.45, blue: 0.33)
    static let deep = Color(red: 0.02, green: 0.18, blue: 0.16)
    static let mist = Color(red: 0.86, green: 0.93, blue: 0.90)
    static let sand = Color(red: 0.96, green: 0.95, blue: 0.91)
    static let ink = Color(red: 0.08, green: 0.14, blue: 0.13)
    static let danger = Color(red: 0.72, green: 0.18, blue: 0.16)

    static let brandFont = Font.custom("Avenir Next Condensed", size: 54).weight(.bold)
    static let titleFont = Font.custom("Avenir Next", size: 22).weight(.semibold)
    static let bodyFont = Font.custom("Avenir Next", size: 15)
    static let monoFont = Font.custom("Menlo", size: 13)
}

struct AtmosphereBackground: View {
    var body: some View {
        ZStack {
            LinearGradient(
                colors: [
                    CIETheme.sand,
                    CIETheme.mist,
                    Color(red: 0.78, green: 0.88, blue: 0.84)
                ],
                startPoint: .topLeading,
                endPoint: .bottomTrailing
            )

            GeometryReader { geo in
                Circle()
                    .fill(CIETheme.green.opacity(0.12))
                    .frame(width: geo.size.width * 0.7)
                    .blur(radius: 40)
                    .offset(x: geo.size.width * 0.35, y: -geo.size.height * 0.15)

                Circle()
                    .fill(CIETheme.deep.opacity(0.10))
                    .frame(width: geo.size.width * 0.55)
                    .blur(radius: 50)
                    .offset(x: -geo.size.width * 0.25, y: geo.size.height * 0.45)
            }
        }
        .ignoresSafeArea()
    }
}
