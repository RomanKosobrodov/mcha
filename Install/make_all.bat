REM "C:\Program Files (x86)\NSIS\makensis" %1\..\installer\InstallBuild\mcha_Win32.nsi
REM "C:\Program Files (x86)\NSIS\makensis" %1\..\installer\InstallBuild\mcha_x64.nsi

mkdir %1..\install\mcha

REM ------- source files ----------
mkdir %1..\install\mcha\src
mkdir %1..\install\mcha\src\Filter
mkdir %1..\install\mcha\src\MEX
mkdir %1..\install\mcha\src\mFiles
mkdir %1..\install\mcha\src\wavFiles

copy  %1..\src\*.*	%1..\install\mcha\src
copy  %1..\src\Filter	%1..\install\mcha\src\Filter
copy  %1..\src\MEX	%1..\install\mcha\src\MEX
copy  %1..\src\mFiles	%1..\install\mcha\src\mFiles
copy  %1..\src\wavFiles %1..\install\mcha\src\wavFiles


REM ---------- lib files -----------
mkdir %1..\install\mcha\lib

xcopy /s %1..\lib %1..\install\mcha\lib


REM --------- build files -----------
mkdir %1..\install\mcha\build
mkdir %1..\install\mcha\build\win
mkdir %1..\install\mcha\build\win\mCha
mkdir %1..\install\mcha\build\MEX

copy  %1win\mCha\*.sln		%1..\install\mcha\build\win\mCha
copy  %1win\mCha\*.suo		%1..\install\mcha\build\win\mCha
copy  %1win\mCha\*.vcxproj.filters	%1..\install\mcha\build\win\mCha
copy  %1win\mCha\*.vcxproj	%1..\install\mcha\build\win\mCha

copy  %1..\build\MEX %1..\install\mcha\build\MEX


REM ----------  doc files --------------
mkdir %1..\install\mcha\doc
copy  %1..\doc\*.pdf 		%1..\install\mcha\doc
copy  %1..\doc\License.txt 	%1..\install\mcha\doc\License.txt


REM ---------- examples files -----------
mkdir %1..\install\mcha\examples
mkdir %1..\install\mcha\examples\filters
mkdir %1..\install\mcha\examples\matlab
mkdir %1..\install\mcha\examples\sound
mkdir %1..\install\mcha\examples\sound\recorded
mkdir %1..\install\mcha\examples\sound\samples

xcopy /s %1..\examples\sound\samples  %1..\install\mcha\examples\sound\samples

copy %1..\examples\matlab\mcha_test.m  		%1..\install\mcha\examples\matlab\mcha_test.m
copy %1..\examples\matlab\mcha_test_filters.m  	%1..\install\mcha\examples\matlab\mcha_test_filters.m



copy %1..\examples\filters\a-weighting.csv 	%1..\install\mcha\examples\filters\a-weighting.csv 
copy %1..\examples\filters\a-weighting_IIR.xml  %1..\install\mcha\examples\filters\a-weighting_IIR.xml
copy %1..\examples\filters\notchfilters.csv 	%1..\install\mcha\examples\filters\notchfilters.csv
copy %1..\examples\filters\notch_fir.xml 	%1..\install\mcha\examples\filters\notch_fir.xml
copy %1..\examples\filters\third_octave.csv 	%1..\install\mcha\examples\filters\third_octave.csv
copy %1..\examples\filters\third_octave_IIR.xml %1..\install\mcha\examples\filters\third_octave_IIR.xml


REM ----------- compress to ZIP ----------------
"C:\Program Files\7-Zip\7z.exe" a -tzip %1\..\install\mcha_Source.zip %1\..\install\mcha


REM --------------- remove folder ---------------
rmdir /Q /S %1\..\install\mcha

