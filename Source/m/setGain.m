%SETGAIN adjust the gain of individual playback channels
%
% [err] =  setGain(channels, gains)
% 
% The parameter channels is a vector of channel numbers and the vector
% gains contains the new values for the corresponding gains.
% On failure the function returns an error string describing the problem.
%
% Examples:
% 
% err = play( rand(48000*10.5, 2), [0 1]);
% for g=0:0.05:1
%     setGain([0 1], [g  1-g]);
%     pause(0.5);
% end
%
% See also: play, playRecord, stop
