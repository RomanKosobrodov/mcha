% record record multichannel data to memory or disk
%
% err = record( outputdirectory, channels, duration )
% Saves audio data on disk in WAV format.
% outputdirectory is a string specifying the directory where the files will be saved.
% If the directory does not exist it will be created. The channels matrix
% specifies input audio channels to be recorded. The channels could be listed in arbitrary order (unsorted).
% The length of the record in seconds is determined by optional argument duration.
% If duration is zero, negative or omitted the lenght of record is limited by available disk space.
% 
% The function returns control to MATLAB almost immediately, the recording being made in the background. 
% Call STOP() to stop the process before the specified record duration has been reached.
% 
% The function returns empty string on success and a string describing an error if one occures.
% To check for errors that occure after RECORD returns control call GETERROR.
% 
% RECORD creates a temporary data file (data.dat) in the output directory and uses it to
% store raw data that are converted to audio format files after recording is finished.
% The function also reserves disk space for final audio files so the maximum record duration is
% a little less than half the available disk space. This is done to optimise disk performance and 
% avoid accidental data loss. 
% 
% It should also be noted that reserving disk space could be slow on some file systems, for example FAT,
% which results in significant delays (tens of seconds) before recording
% starts. The delays could be checked in the log file that is saved in the
% (system specific) user folder. For Windows it is :
% C:\Documents and Settings\<user_name>\Application Data\CarlabAudio\carlab.log.txt 
%
% err = record( channels, duration )
% Saves recorded samples in memory.
% The channels matrix specifies input audio channels than could be listed in any order. 
% 
% duration specifies the record length in seconds. 
%
% err contains empty string on success and a string describing an error if one occures.
% To check for errors that occure after carlabRecord returns control call
% GETERROR.
%
% err = record([0 1], 12)
%
% Records channels 0 and 1 into internal memory. Use GETDATA to read
% recorded samples from memory.
%
% Examples:
%
% err = record('D:\Data', [0 1], 12)
% 
% Records channels 0 and 1 for the duration of 12 seconds and saves the
% results as channel00.wav and channel01.wav in directory D:\Data.
% 
% err = record('D:\Data', [1 0], 12)
% pause(12);
% getError()
% err = stop()
%
% Same as above but the duration could be less than 12 seconds depending on
% the operating and file systems on your computer.
%
% err = record([0 1], 5)
% pause(5.5);
% stop();
% x = getData();
% 
% Records data from channels 0 and 1 for 5 seconds, waits for recording to
% finish and retrieve obtained samples. 
%
% See also stop, getError, play, init and getData




