;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"
;--------------------------------
;General

  ;Name and file
  Name "ClipboardActions"
  OutFile "ClipboardActions.Installer.x64.exe"
  Unicode True

  ;Default installation folder
  InstallDir "$PROGRAMFILES64\ClipboardActions"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\ClipboardActions" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "LICENSE.txt"
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Dummy Section" SecDummy

  SetOutPath "$INSTDIR"
  
  ;ADD YOUR OWN FILES HERE...
  File build\Release\ClipboardActions.exe
 
# create shortcuts
CreateDirectory "$SMPROGRAMS\ClipboardActions"
CreateShortCut "$SMPROGRAMS\ClipboardActions\ClipboardActions Uninstaller.lnk" "$INSTDIR\Uninstall.exe"
CreateShortCut "$SMPROGRAMS\ClipboardActions\ClipboardActions.lnk" "$INSTDIR\ClipboardActions.exe"
  
  ;Store installation folder
  WriteRegStr HKCU "Software\ClipboardActions" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

;Uninstaller Section

Section "Uninstall"

  # Delete installed file
Delete $INSTDIR\ClipboardActions.exe
 
# Delete the uninstaller
Delete $INSTDIR\Uninstall.exe

RMDir /r "$SMPROGRAMS\ClipboardActions"
 
# Delete the directory
RMDir /r $INSTDIR

  DeleteRegKey /ifempty HKCU "Software\ClipboardActions"

SectionEnd