#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
APP_DIR="$ROOT/CIEID-mac"
APP_BUNDLE="${APP_BUNDLE:-$APP_DIR/CIEID.app}"
OPENSSL_PREFIX="$(brew --prefix openssl@3 2>/dev/null || echo /opt/homebrew/opt/openssl@3)"
MACOSX_DEPLOYMENT_TARGET="${MACOSX_DEPLOYMENT_TARGET:-14.0}"

BREW_RPATHS=(
  "$OPENSSL_PREFIX/lib"
  "$(brew --prefix libxml2)/lib"
  "$(brew --prefix freetype)/lib"
  "$(brew --prefix libpng)/lib"
  "$(brew --prefix fontconfig)/lib"
  "$(brew --prefix curl)/lib"
  "$(brew --prefix cryptopp)/lib"
  "$(brew --prefix libidn)/lib"
  "$(brew --prefix libunistring)/lib"
  "$(brew --prefix jpeg)/lib"
  "$(brew --prefix libtiff)/lib"
)

echo "==> Building libciepki.dylib + libciefirma.dylib"
make -C "$ROOT" all

echo "==> Preparing app bundle at $APP_BUNDLE"
rm -rf "$APP_BUNDLE"
mkdir -p "$APP_BUNDLE/Contents/MacOS"
mkdir -p "$APP_BUNDLE/Contents/Frameworks"
mkdir -p "$APP_BUNDLE/Contents/Resources"

cp "$APP_DIR/Resources/Info.plist" "$APP_BUNDLE/Contents/Info.plist"

install_name_tool -id "@rpath/libciepki.dylib" "$ROOT/libciepki.dylib" 2>/dev/null || true
install_name_tool -id "@rpath/libciefirma.dylib" "$ROOT/libciefirma.dylib" 2>/dev/null || true
cp "$ROOT/libciepki.dylib" "$APP_BUNDLE/Contents/Frameworks/libciepki.dylib"
cp "$ROOT/libciefirma.dylib" "$APP_BUNDLE/Contents/Frameworks/libciefirma.dylib"
install_name_tool -id "@rpath/libciepki.dylib" "$APP_BUNDLE/Contents/Frameworks/libciepki.dylib"
install_name_tool -id "@rpath/libciefirma.dylib" "$APP_BUNDLE/Contents/Frameworks/libciefirma.dylib"

RPATH_FLAGS=( -Xlinker -rpath -Xlinker @executable_path/../Frameworks )
for p in "${BREW_RPATHS[@]}"; do
  RPATH_FLAGS+=( -Xlinker -rpath -Xlinker "$p" )
done

echo "==> Compiling CIEID"
swiftc \
  -parse-as-library \
  -O \
  -target "arm64-apple-macosx${MACOSX_DEPLOYMENT_TARGET}" \
  -import-objc-header "$APP_DIR/Bridging/CiePkiBridge.h" \
  -I "$APP_DIR/Bridging" \
  "$APP_DIR/Sources/CIEIDApp.swift" \
  "$APP_DIR/Sources/ContentView.swift" \
  "$APP_DIR/Sources/CieViewModel.swift" \
  "$APP_DIR/Sources/CiePkiBridge.swift" \
  "$APP_DIR/Sources/Theme.swift" \
  -o "$APP_BUNDLE/Contents/MacOS/CIEID" \
  -L "$APP_BUNDLE/Contents/Frameworks" \
  -lciepki -lciefirma \
  "${RPATH_FLAGS[@]}"

echo "==> Built $APP_BUNDLE"
echo "    open \"$APP_BUNDLE\""
