mkdir .\install\mcha

:CheckOS
IF EXIST "%PROGRAMFILES(X86)%" (GOTO 64BIT) ELSE (GOTO 32BIT)

:32BIT
echo 32-bit...

REM =============================
REM               COMPILATION
REM =============================
"C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE\devenv.com" ..\Builds\VisualStudio2010\mcha.sln  /rebuild MCHA-Release-Win32
"C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE\devenv.com" ..\Init\Builds\VisualStudio2010\mchaInit.sln  /rebuild MCHA-Release-Win32

REM =============================
REM               MEX FILES
REM =============================

cd ..\Builds\MEX
matlab -nosplash -nodesktop -wait -r build_mex
cd ..\..\Install

REM =============================
REM               INSTALLERS
REM =============================
"C:\Program Files\NSIS\makensis" mcha_Win32.nsi


:64BIT
echo 64-bit...

REM =============================
REM               COMPILATION
REM =============================
"C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.com" ..\Builds\VisualStudio2010\mcha.sln  /rebuild MCHA-Release-x64
"C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.com" ..\Init\Builds\VisualStudio2010\mchaInit.sln  /rebuild MCHA-Release-x64

REM =============================
REM               MEX FILES
REM =============================

cd ..\Builds\MEX
"C:\Program Files (x86)\MATLAB\R2011b\bin\matlab.exe" -nosplash -nodesktop -wait -r build_mex
"C:\Program Files\MATLAB\R2011b\bin\matlab.exe" -nosplash -nodesktop -wait -r build_mex
cd ..\..\Install

REM =============================
REM               INSTALLERS
REM =============================
"C:\Program Files (x86)\NSIS\makensis" mcha_Win32.nsi
"C:\Program Files (x86)\NSIS\makensis" mcha_x64.nsi

:END


REM =============================
REM               SOURCE ARCHIVE
REM =============================

REM ------- Binaries -------------
mkdir .\install\mcha\Bin

REM ------- source files ----------
mkdir .\install\mcha\Source
mkdir .\install\mcha\Source\Filter
mkdir .\install\mcha\Source\MEX
mkdir .\install\mcha\Source\m
mkdir .\install\mcha\Source\octave

copy  ..\Source\*.*	.\install\mcha\Source
copy  ..\Source\Filter	.\install\mcha\Source\Filter
copy  ..\Source\MEX	.\install\mcha\Source\MEX
copy  ..\Source\m	.\install\mcha\Source\m
copy  ..\Source\octave .\install\mcha\Source\octave 


REM ---------- lib files -----------
mkdir .\install\mcha\Lib

xcopy /s ..\Lib .\install\mcha\Lib


REM --------- Builds files -----------
mkdir .\install\mcha\Builds
mkdir .\install\mcha\Builds\Linux
mkdir .\install\mcha\Builds\MEX
mkdir .\install\mcha\Builds\MacOSX
mkdir .\install\mcha\Builds\VisualStudio2010

copy ..\Builds\VisualStudio2010\*.sln		.\install\mcha\Builds\VisualStudio2010
copy ..\Builds\VisualStudio2010\*.suo		.\install\mcha\Builds\VisualStudio2010
copy ..\Builds\VisualStudio2010\*.vcxproj.filters	.\install\mcha\Builds\VisualStudio2010
copy ..\Builds\VisualStudio2010\*.vcxproj	.\install\mcha\Builds\VisualStudio2010

xcopy /s ..\Builds\Linux .\install\mcha\Builds\Linux
xcopy /s ..\Builds\MEX .\install\mcha\Builds\MEX
xcopy /s ..\Builds\MacOSX .\install\mcha\Builds\MacOSX


REM - Init --

mkdir .\install\mcha\Init\Builds
mkdir .\install\mcha\Init\Builds\Linux
mkdir .\install\mcha\Init\Builds\MacOSX
mkdir .\install\mcha\Init\Builds\VisualStudio2010
mkdir .\install\mcha\Init\JuceLibraryCode

xcopy /s ..\Init\Builds\Linux  .\install\mcha\Init\Builds\Linux
xcopy /s ..\Init\Builds\MacOSX .\install\mcha\Init\Builds\MacOSX
xcopy /s ..\Init\JuceLibraryCode .\install\mcha\Init\JuceLibraryCode

copy ..\Init\*.jucer .\install\mcha\Init

copy ..\Init\Builds\VisualStudio2010\*.sln		.\install\mcha\Init\Builds\VisualStudio2010
copy ..\Init\Builds\VisualStudio2010\*.suo		.\install\mcha\Init\Builds\VisualStudio2010
copy ..\Init\Builds\VisualStudio2010\*.vcxproj.filters	.\install\mcha\Init\Builds\VisualStudio2010
copy ..\Init\Builds\VisualStudio2010\*.vcxproj	.\install\mcha\Init\Builds\VisualStudio2010

REM --------- Installer files -----------
mkdir .\install\mcha\Install

copy  ..\Install\*.nsi		.\install\mcha\Install
copy  ..\Install\*.bat		.\install\mcha\Install

REM ----------  Documentation files --------------
mkdir .\install\mcha\Documentation
copy  ..\Documentation\*.pdf 		.\install\mcha\Documentation
copy  ..\Documentation\License.txt 	.\install\mcha\Documentation\License.txt


REM ---------- Examples files -----------
mkdir .\install\mcha\Examples
mkdir .\install\mcha\Examples\filters
mkdir .\install\mcha\Examples\matlab
mkdir .\install\mcha\Examples\sound
mkdir .\install\mcha\Examples\sound\recorded
mkdir .\install\mcha\Examples\sound\samples

xcopy /s ..\Examples\sound\samples  .\install\mcha\Examples\sound\samples

copy ..\Examples\matlab\mcha_test.m  		.\install\mcha\Examples\matlab\mcha_test.m
copy ..\Examples\matlab\mcha_test_filters.m  	.\install\mcha\Examples\matlab\mcha_test_filters.m

copy ..\Examples\filters\a-weighting.csv 	.\install\mcha\Examples\filters\a-weighting.csv 
copy ..\Examples\filters\a-weighting_IIR.xml  .\install\mcha\Examples\filters\a-weighting_IIR.xml
copy ..\Examples\filters\notch_fir.csv 	.\install\mcha\Examples\filters\notch_fir.csv
copy ..\Examples\filters\notch_fir.xml 	.\install\mcha\Examples\filters\notch_fir.xml
copy ..\Examples\filters\third_octave.csv 	.\install\mcha\Examples\filters\third_octave.csv
copy ..\Examples\filters\third_octave_IIR.xml .\install\mcha\Examples\filters\third_octave_IIR.xml


REM ----------- compress to ZIP ----------------
"C:\Program Files\7-Zip\7z.exe" a -tzip %.\install\mcha_Source.zip .\install\mcha


REM --------------- remove folder ---------------
rmdir /Q /S .\install\mcha

