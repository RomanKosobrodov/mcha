function build_mex(varargin)
% build64 builds all MEX files in the MEX directory
clc;

srcFolder = '..\..\Source\MEX\';
fileList = { 'playRecord.cpp' 
             'init.cpp'
             'getVersion.cpp'
             'setGain.cpp'  
             'play.cpp'
             'record.cpp'
             'getData.cpp'
             'stop.cpp' 
             'running.cpp' 
             'getError.cpp'         
             'getPosition.cpp'
             'setPosition.cpp'
              };

extStr = mexext;

if nargin == 0
    confStr = 'Release';
else
    if strcmpi( varargin{1}, 'DEBUG')
        confStr = 'Debug';
    else
        confStr = 'Release';
    end
end

if strcmp(mexext, 'mexw64')
    platformStr = 'x64';
else
    if strcmp(mexext, 'mexw32')
        platformStr = 'Win32';
    else
        error('Unsupported platform');
    end
end
    
if (strcmp(platformStr, 'x64')|| strcmp(platformStr, 'Win32'))
    disp('========================================================')
    disp(['        Compiling ' platformStr ' ' confStr ' version                   '])
    disp('========================================================')           

    outPath = ['..\..\Bin\' platformStr '\' confStr '\'];
    copyfile(['..\..\Lib\' platformStr '\libfftw3f-3.dll'], outPath );
    copyfile('..\..\Source\m\*.m', outPath );    

    for k=1:length(fileList)
        cmdStr = ['mex -largeArrayDims -outdir ' outPath ' ' srcFolder fileList{k} ' ' outPath 'mCha-' platformStr '.lib'];
        fn = fileList{k};
        disp([fn(1:(end-3)) extStr]);
        eval(cmdStr); 
    end
end