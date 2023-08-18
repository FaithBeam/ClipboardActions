# define installer name
OutFile "ClipboardActions (x64) Installer.exe"
 
InstallDir $PROGRAMFILES64\ClipboardActions
 
# default section start
Section
 
# define output path
SetOutPath $INSTDIR
 
# specify file to go in output path
File x64\Release\ClipboardActions.exe
 
# define uninstaller name
WriteUninstaller $INSTDIR\uninstaller.exe

# create shortcuts
CreateDirectory "$SMPROGRAMS\ClipboardActions"
CreateShortCut "$SMPROGRAMS\ClipboardActions\ClipboardActions Uninstaller.lnk" "$INSTDIR\uninstaller.exe"
CreateShortCut "$SMPROGRAMS\ClipboardActions\ClipboardActions.lnk" "$INSTDIR\ClipboardActions.exe"

 
#-------
# default section end
SectionEnd
 
# create a section to define what the uninstaller does.
# the section will always be named "Uninstall"
Section "Uninstall"
 
# Delete installed file
Delete $INSTDIR\ClipboardActions.exe
 
# Delete the uninstaller
Delete $INSTDIR\uninstaller.exe

RMDir /r "$SMPROGRAMS\ClipboardActions"
 
# Delete the directory
RMDir /r $INSTDIR
SectionEnd