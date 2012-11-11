function [processStopped, timeElapsed] = wait(timeInSec)
% WAIT wait for the process to terminate
% [processStopped, timeElapsed] = wait(timeInSec)
%
% Parameter timeInSec specifies the timeout in seconds (with 0.1 s resolution). 
% If the process is still running after the timeout expires processStopped
% is false, otherwise true.
%
% On success timeElapsed parameter contains the time in seconds
% elapsed after the call to the function before the playback/recording
% process terminated.
%
% Example:
% T = 3.5;          % duration in s
% fs = 48000;     % sampling frequency
% N = fs*T + 1;   % number of samples
% dat = rand(N, 2) - 0.5; % generate T seconds of white noise
% err = play(dat, [0 1]); 
% pause(3);
% [res, elapsed] = wait(1.0);
% if (~res)
%  disp('wait returned false');
% else
%   disp(elapsed);  
% end
%
% See also: stop, running

curTime =0; 
timeInt = 0.1;

while ( (curTime < timeInSec) && running() )
    pause(timeInt);
    curTime = curTime + timeInt;
end

timeElapsed = curTime;

if running()
    processStopped = false;
else
    processStopped = true;
end