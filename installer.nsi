; Screen Mirror App Installer
; NSIS Installer Script

!include "MUI2.nsh"

; Name and file
Name "Screen Mirror App"
OutFile "ScreenMirror-Setup.exe"
InstallDir "$PROGRAMFILES\ScreenMirrorApp"

; MUI Settings
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "Turkish"

; Installer sections
Section "Install"
  SetOutPath "$INSTDIR"
  
  ; Dosyaları kopyala
  File "dist\ScreenMirror.exe"
  File "config.json"
  File "requirements.txt"
  
  ; Start Menu kısayolu oluştur
  CreateDirectory "$SMPROGRAMS\Screen Mirror App"
  CreateShortcut "$SMPROGRAMS\Screen Mirror App\Screen Mirror.lnk" "$INSTDIR\ScreenMirror.exe"
  CreateShortcut "$SMPROGRAMS\Screen Mirror App\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
  ; Desktop kısayolu oluştur
  CreateShortcut "$DESKTOP\Screen Mirror.lnk" "$INSTDIR\ScreenMirror.exe"
  
  ; Uninstaller oluştur
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  ; Registry'ye ekle (Programs ve Features'da görmek için)
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ScreenMirrorApp" \
    "DisplayName" "Screen Mirror App"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ScreenMirrorApp" \
    "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ScreenMirrorApp" \
    "DisplayVersion" "1.0.0"
SectionEnd

; Uninstaller section
Section "Uninstall"
  Delete "$INSTDIR\ScreenMirror.exe"
  Delete "$INSTDIR\config.json"
  Delete "$INSTDIR\requirements.txt"
  Delete "$INSTDIR\Uninstall.exe"
  
  RMDir "$INSTDIR"
  
  Delete "$SMPROGRAMS\Screen Mirror App\Screen Mirror.lnk"
  Delete "$SMPROGRAMS\Screen Mirror App\Uninstall.lnk"
  RMDir "$SMPROGRAMS\Screen Mirror App"
  
  Delete "$DESKTOP\Screen Mirror.lnk"
  
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ScreenMirrorApp"
SectionEnd
