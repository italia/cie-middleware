echo "Build cie_sign_sdk"

cd %cd%/../cie_sign_sdk
cmake -B build\ -A x64
cmake --build build\ --config Release
cmake --install build\ --config Release
