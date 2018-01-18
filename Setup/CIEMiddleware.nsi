!include "MUI2.nsh"
!include "x64.nsh"
!include "Library.nsh"

;--------------------------------
;General

  ;Name and file
  Name "CIE-Middleware"
  OutFile "CIE-Middleware.exe"

  !define MUI_ICON "..\CSP\res\CIE.ico"
  !define PRODUCT_UNINST_ROOT_KEY "HKLM"
  !define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\CIEPKI"
  
  
  BrandingText "Istituto Poligrafico e Zecca dello Stato"
  ;Default installation folder
  InstallDir "$PROGRAMFILES\CIEPKI"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKLM "Software\CIEPKI" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

;--------------------------------
;Interface Configuration

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\nsis.bmp" ; optional
  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "..\License.txt"
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "Italian"

;--------------------------------
;Installer Sections

Section "Install"

  SetOutPath $SYSDIR
    
 
  ${If} ${RunningX64}	
    !define LIBRARY_X64
    !insertmacro InstallLib REGDLL NOTSHARED REBOOT_PROTECTED "..\x64\Release\CIEPKI.dll" "$SYSDIR\CIEPKI.dll" $SYSDIR
    !undef LIBRARY_X64
    !insertmacro InstallLib REGDLL NOTSHARED REBOOT_PROTECTED "..\Win32\Release\CIEPKI.dll" "$SYSDIR\CIEPKI.dll" $SYSDIR
  ${Else}
    !insertmacro InstallLib REGDLL NOTSHARED REBOOT_PROTECTED "..\Win32\Release\CIEPKI.dll" "$SYSDIR\CIEPKI.dll" $SYSDIR
  ${EndIf}

 
  SetShellVarContext all
  # Start Menu
	createDirectory "$SMPROGRAMS\CIE Middleware"

  ;Create uninstaller
  createDirectory "$INSTDIR"
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  SetOutPath $INSTDIR    
	createShortCut "$SMPROGRAMS\CIE Middleware\Disinstalla.lnk" "$INSTDIR\Uninstall.exe"
  SetOutPath $SYSDIR
	createShortCut "$SMPROGRAMS\CIE Middleware\Cambio PIN.lnk" "$SYSDIR\rundll32.exe" "CIEPKI.dll CambioPIN" "$SYSDIR\CIEPKI.dll" 0
	createShortCut "$SMPROGRAMS\CIE Middleware\Sblocco PIN.lnk" "$SYSDIR\rundll32.exe" "CIEPKI.dll SbloccoPIN" "$SYSDIR\CIEPKI.dll" 0

  SetOutPath $INSTDIR
	
  ${If} ${RunningX64}	
	SetRegView 64
  ${EndIf}
  WriteRegStr HKLM "Software\CIEPKI" "" $INSTDIR
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "CIE Middleware"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$SYSDIR\CIEPKI.dll,0"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "IPZS"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "VersionMajor" "0"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "VersionMinor" "1"
  SetRegView default
  
  
  IfRebootFlag 0 noreboot
  MessageBox MB_YESNO "Per completare l'installazione e' necessario il riavvio del computer. Riavviare adesso?" IDNO noreboot
  Reboot
  noreboot:
SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  ${If} ${RunningX64}	
    !define LIBRARY_X64
    !insertmacro UnInstallLib REGDLL NOTSHARED REBOOT_PROTECTED "$SYSDIR\CIEPKI.dll"
    !undef LIBRARY_X64

    ${DisableX64FSRedirection}
    Delete "$SYSDIR\CIEPKI.dll"
    ${EnableX64FSRedirection}

    !insertmacro UnInstallLib REGDLL NOTSHARED REBOOT_PROTECTED "$SYSDIR\CIEPKI.dll"
	Delete "$SYSDIR\CIEPKI.dll"
  ${Else}
    !insertmacro UnInstallLib REGDLL NOTSHARED REBOOT_PROTECTED "$SYSDIR\CIEPKI.dll"
	Delete "$SYSDIR\CIEPKI.dll"
  ${EndIf}
  
  Delete "$INSTDIR\Uninstall.exe"
  RMDir "$INSTDIR"
  
  ${If} ${RunningX64}	
	SetRegView 64
  ${EndIf}
  DeleteRegKey /ifempty HKLM "Software\CIEPKI"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  SetRegView default

  SetShellVarContext all
  RMDir /r "$SMPROGRAMS\CIE Middleware"

  IfRebootFlag 0 noreboot2
  MessageBox MB_YESNO "Per completare l'installazione e' necessario il riavvio del computer. Riavviare adesso?" IDNO noreboot2
  Reboot
  noreboot2:
SectionEnd