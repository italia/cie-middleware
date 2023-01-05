@echo OFF
set "filePresent="
for %%G in (
    Middleware-produzione ^
    sign_sha256.bat ^
    sign_sha256_local.bat ^
    signtool.exe ^
    signtool5.exe ^
    cie_sign_sdk\Dependencies ^
    CSP\LOGGER\LICENSE ^
    .astylerc ^
    script.bat
) do if exist %%G  (
    echo Error: '%%G' is present
    set "filePresent=y"
)

if "%filePresent%"=="y" (exit /B 1) else (echo No private files found.)
