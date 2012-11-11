% GETPOSITION get the position from the beginning of the current playback 
%
% [timeInSeconds] =  getPosition()
% 
%  The output parameter contains the position  in seconds from 
%  the beginning of the current playback
%
% Examples:
% 
%   err = play(rand(48000*5,2), [0, 1]) % start playing
%   pause(2); % wait for 2 seconds
%   getPosition() % get current position
%   err = setPosition(0.5); % rewind to 0.5 seconds from start
%   getPosition() % get current position again
%
% See also: play, playRecord, stop, setPosition, running
%