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

  ;ReadRegStr $2 HKLM "SOFTWARE\MathWorks\MATLAB\$1" "MATLABROOT"
 ; DetailPrint "Matlab root directory: $2"

  ReadRegStr $2 HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders" "PERSONAL"
  DetailPrint "Matlab files directory: $2"
  StrCpy $INSTDIR "$2\MATLAB\mcha"


  ReadRegDword $3 HKLM "SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x64" "Installed"
  
  IntCmp $3 1 VCredistInstalled VCDisplayWarning
  VCDisplayWarning:
	MessageBox MB_YESNO "audioMEX toolbox requires Visual C++ 2010 Redistributable Package (x64) to be installed. Do you want to quit set up and install the package now?" IDYES true IDNO false
	true:
  		Abort 
	false:
		Goto done
  VCredistInstalled:
  	DetailPrint "Visual C++ 2010 Redistributable Package (x64) is installed"
  done:
FunctionEnd



;--------------------------------
;General

  ;Name and file
  Name "MCHA"
  OutFile "..\install\mcha_install_x64.exe"

  ;Default installation folder
  ;InstallDir "$2\toolbox\mcha"
  
  ;Get installation folder from registry if available
  ;InstallDirRegKey HKCU "Software\MCHA" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel highest

;--------------------------------
;Interface Configuration

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange.bmp" ; optional
  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "..\Documentation\License.txt"
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


Section "MCHA toolbox" binaries
SectionIn RO

  SetOutPath "$INSTDIR"

  CreateDirectory $INSTDIR\mex
  SetOutPath "$INSTDIR\mex" 
  File	..\Bin\MCHA-Release-x64\mcha-x64.dll
  File	..\Lib\x64\*.dll

  File	..\Bin\MCHA-Release-x64\*.mexw64
  File	..\Bin\MCHA-Release-x64\*.m
 
 
  CreateDirectory $INSTDIR\doc
  SetOutPath "$INSTDIR\doc" 

  File  ..\Documentation\*.pdf
  File  ..\Documentation\License.txt
  
  ;Store installation folder
  WriteRegStr HKCU "Software\MCHA" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd


Section "Examples" examples
SectionIn 1

  SetOutPath "$INSTDIR"

  CreateDirectory $INSTDIR\Examples
  SetOutPath "$INSTDIR\Examples"   
  File /r ..\Examples\Samples

  CreateDirectory $INSTDIR\Examples\Sound\Recorded


  CreateDirectory $INSTDIR\Examples\Matlab
  SetOutPath "$INSTDIR\Examples\Matlab" 
  File ..\Examples\Matlab\mcha_test.m
  File ..\Examples\Matlab\mcha_test_filters.m  


  CreateDirectory $INSTDIR\Examples\Filters
  SetOutPath "$INSTDIR\Examples\Filters" 
  File ..\Examples\Filters\a-weighting.csv
  File ..\Examples\Filters\a-weighting_IIR.xml  
  File ..\Examples\Filters\notch_fir.csv
  File ..\Examples\Filters\notch_fir.xml  
  File ..\Examples\Filters\third_octave.csv
  File ..\Examples\Filters\third_octave_IIR.xml  

 
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

  DeleteRegKey /ifempty HKCU "Software\MCHA"

SectionEnd