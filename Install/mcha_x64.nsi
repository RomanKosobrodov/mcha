;NSIS Modern User Interface
;Header Bitmap Example Script
;Written by Joost Verburg

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
Function .onInit

  SetRegView 64

  ReadRegStr $0 HKCR "Matlab.Application\CurVer" ""
  StrCpy $1 $0 4 19;
  DetailPrint "Matlab version: $1"

  ReadRegStr $2 HKLM "SOFTWARE\MathWorks\MATLAB\$1" "MATLABROOT"
  DetailPrint "Matlab root directory: $2"

  StrCpy $INSTDIR "$2\toolbox\mcha"
FunctionEnd



;--------------------------------
;General

  ;Name and file
  Name "MCHA toolbox"
  OutFile "..\mcha_install_x64.exe"

  ;Default installation folder
  ;InstallDir "$2\toolbox\mcha"
  
  ;Get installation folder from registry if available
  ;InstallDirRegKey HKCU "Software\mcha" ""

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


Section "MCHA Toolbox" binaries
SectionIn RO


  SetOutPath "$INSTDIR"

  CreateDirectory $INSTDIR\mex
  SetOutPath "$INSTDIR\mex" 
  File	..\..\bin\mCha\x64\Release\*.dll
  File	..\..\bin\mCha\x64\Release\*.lib
  File	..\..\bin\mCha\x64\Release\*.exp
  File	..\..\lib\x64\*.dll

  File	..\..\bin\mCha\x64\Release\*.mexw64
  File	..\..\bin\mCha\x64\Release\*.m
 
 
  CreateDirectory $INSTDIR\doc
  SetOutPath "$INSTDIR\doc" 

  ;File  ..\..\doc\*.pdf
  File  ..\..\doc\License.txt
  
  ;Store installation folder
  WriteRegStr HKCU "Software\mcha" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd


Section "Examples" examples
SectionIn 1

  SetOutPath "$INSTDIR"

  CreateDirectory $INSTDIR\examples
  SetOutPath "$INSTDIR\examples"   
  File /r ..\..\examples\samples

  CreateDirectory $INSTDIR\examples\sound\recorded


  CreateDirectory $INSTDIR\examples\matlab
  SetOutPath "$INSTDIR\examples\matlab" 
  File ..\..\examples\matlab\mcha_test.m
  File ..\..\examples\matlab\mcha_test_filters.m  


  CreateDirectory $INSTDIR\examples\filters
  SetOutPath "$INSTDIR\examples\filters" 
  File ..\..\examples\filters\a-weighting.csv
  File ..\..\examples\filters\a-weighting_IIR.xml  
  File ..\..\examples\filters\notchfilters.csv
  File ..\..\examples\filters\notch_fir.xml  
  File ..\..\examples\filters\third_octave.csv
  File ..\..\examples\filters\third_octave_IIR.xml  

 
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