function build_mex(varargin)
% build64 builds all MEX files in the MEX directory
clc;

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

if strcmp(extStr, 'mexw64')
    platformStr = 'x64';
else
    if strcmp(extStr, 'mexw32')
        platformStr = 'Win32';
    else
        if strcmp(extStr, 'mexa64')
            platformStr = 'Linux-x86_64';
        else
            if strcmp(extStr, 'mexa32')
                platformStr = 'Linux-i386';
            else
                error('Unsupported platform');
            end
        end
    end
end

disp('========================================================')
disp(['        Compiling ' platformStr ' ' confStr ' version                   '])
disp('========================================================')      

%% Compile Windows versions
if (strcmp(platformStr, 'x64')|| strcmp(platformStr, 'Win32'))
	srcFolder = '..\..\Source\MEX\';
    outPath = ['..\..\Bin\' platformStr '\' confStr '\'];
    copyfile('..\..\Source\m\*.m', outPath );     
    copyfile(['..\..\Lib\' platformStr '\libfftw3f-3.dll'], outPath );
    for k=1:length(fileList)
        cmdStr = ['mex -largeArrayDims -outdir ' outPath ' ' srcFolder fileList{k} ' ' outPath 'mCha-' platformStr '.lib'];
        fn = fileList{k};
        disp([fn(1:(end-3)) extStr]);
        eval(cmdStr); 
    end
end

%% Compile Linux versions
if (strcmp(platformStr, 'Linux-x86_64')|| strcmp(platformStr, 'Linux-i386'))
    srcFolder = '../../Source/MEX/';
    outPath = ['../../Bin/' platformStr '/' confStr '/'];
    copyfile('../../Source/m/*.m', outPath );       
    for k=1:length(fileList)
        cmdStr = ['mex -largeArrayDims -outdir ' outPath ' ' srcFolder fileList{k} ' -L' outPath ' -lmcha'];
        fn = fileList{k};
        disp([fn(1:(end-3)) extStr]);
        eval(cmdStr); 
    end
end
