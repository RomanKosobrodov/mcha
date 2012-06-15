% carlabPlayRecord play and simultaneously record
% multichannel audio from/to audio files or memory buffers
%
% [err, inputData] = carlabPlayRecord(inputChannels, inputDuration, outputData, outputChannels)
% [err] = carlabPlayRecord(recordDir, inputChannels, inputDuration, outputData, outputChannels)
% [err, inputData] = carlabPlayRecord(inputChannels, inputDuration, outputFiles, outputChannels)
% [err] = carlabPlayRecord(recordDir, inputChannels, inputDuration, outputFiles, outputChannels)
%
% The function returns control to MATLAB after starting the playback. 
% Use carlabStop function to stop it before it plays back all the data.
%
% The inputChannels and outputChannels matrices specify input and output audio channels that could be 
% listed in arbitrary order. The number of files (or rows in data matrices) should be equal to the 
% number of channels.
%
% err contains empty string on success and a string describing an error if one occures.
% To check for errors that occure after carlabPlay returns control call
% CARLABGETERROR.
%
% outputFiles is a cell array containing full paths to audio files to be
% played. The supported formats are .wav, .ogg, .aif (.aiff) and .flac.
% Files of different formats could be combined in one function call.
%
% recordDir is a string specifying the directory where the files will be saved.
% If the directory does not exist it will be created. 
% The length of the record in seconds is determined by inputDuration argument.
% If duration is zero or negative the lenght of record is only limited by available disk space.
%  
% The data to be played should be contained in the rows of the outputData matrix.
% The number of rows should be equal to the number of output channels.
%
%
% See also carlabRecord, carlabPlay, carlabGetError, carlabStop, carlabInit
