% SETPOSITION set the position of the current playback 
%
% [err] =  setPosition(delayInSeconds)
% 
% Parameter delayInSeconds is the delay from the start of the file or audio
% data matrix measured in seconds. The output parameter is empty on success
% or a string descibing an error.
%
% Examples:
% 
%   audiodata = rand(48000*5, 2) - 0.5 ; % 5 seconds of white noise
%   err = play(audiodata, [0 1]) % start playing
%   pause(2); % wait for 2 seconds
%   getPosition() % get current position
%   err = setPosition(0.5); % rewind to 0.5 seconds from the start
%   getPosition() % get current position again
%
% See also: play, playRecord, stop, getPosition