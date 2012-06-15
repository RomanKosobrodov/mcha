;NSIS Modern User Interface
;Header Bitmap Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "MUSHRA Test"
  OutFile "..\mcha_install_win32.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\MATLAB\R2011b\toolbox\mcha"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\mcha" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin


;--------------------------------
;Interface Configuration

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange.bmp" ; optional
  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "..\..\doc\License.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES


  ; Zip source files
  !system 'run7zip.bat'
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

InstType "full"
InstType "minimal"


Section "Mushra Test Software" binaries
SectionIn RO

  SetOutPath "$INSTDIR"

  CreateDirectory $INSTDIR\mex
  SetOutPath "$INSTDIR\mex" 
  File	..\..\bin\mCha\Win32\Release\*.dll
  File	..\..\bin\mCha\Win32\Release\*.lib
  File	..\..\bin\mCha\Win32\Release\*.exp
  File	..\..\lib\Win32\*.dll

  File	..\..\bin\mCha\Win32\Release\*.mexw32
  File	..\..\bin\mCha\Win32\Release\*.m
 
 
  CreateDirectory $INSTDIR\doc
  SetOutPath "$INSTDIR\doc" 

  File  ..\..\doc\*.pdf
  File  ..\..\doc\License.txt
  
  ;Store installation folder
  WriteRegStr HKCU "Software\mcha" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd


Section "Examples" examples
SectionIn 1
  SetOutPath "$INSTDIR"

  File /r ..\..\examples
  
 
SectionEnd






;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_binaries ${LANG_ENGLISH} "Binary files."
  LangString DESC_examples ${LANG_ENGLISH} "Examples."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${binaries} $(DESC_binaries)
    !insertmacro MUI_DESCRIPTION_TEXT ${examples} $(DESC_examples)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"


  Delete "$INSTDIR\Uninstall.exe"

  RMDir /r "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\mcha"

SectionEnd