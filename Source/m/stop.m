% STOP  stop the execution of current recording or playback process
%
% err = stop()
% Returns empty string on success or a string describing an error if one
% occures.
% The function tells the current process to terminate and returns control to MATLAB.
% There could be some delay in terminating RECORD function
% especially when large files are being saved on disk.
% 
% Examples:
%
%  err = record('D:\Data', [1 0])
%  pause(12);
%  err = stop()
%
% Starts recording channels 0 and 1 on disk and stops the process  12
% seconds later. 
%
% See also record, getError, play, init, getPosition
%