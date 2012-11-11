% PLAY play multichannel audio from audio files or memory buffer
%
% err = play(audiofiles, channels)
% err = play(samplesmatrix, channels)
%
% The channels matrix specifies the output audio channels that could be 
% listed in arbitrary order. The number of files should be equal to the 
% number of channels.
%
% err contains empty string on success and a string describing an error if one occures.
% To check for errors that occure after PLAY returns control call
% CARLABGETERROR.
%
% The function returns control to MATLAB after starting the playback. 
% Use STOP function to terminate playback.
%
% err = play(audiofiles, channels)
% Audiofiles is a cell array containing full paths to audio files to be
% played. The supported formats are .wav, .ogg, .aif (.aiff) and .flac.
% Files of different formats could be combined in one function call.
% 
% err = play(samplesmatrix, channels)
% The data to be played should be contained in the rows of samplesmatrix.
% The number of rows should be equal to the number of output channels.
%
% Examples:
%
% T = 3;          % duration in s
% fs = 48000;     % sampling frequency
% N = fs*T + 1;   % number of samples
% dat = rand(N, 2) - 0.5; % generate T seconds of white noise
% err =play(dat,[0 1])
% pause(1)
% disp(getPosition());
% err = getError()
% err = stop()
%
% err = record([0 1], 3); % record channels 0 an 1 for 3 s.   
% pause(3.5); % wait for 3.5 s.
% x = getData(); % get recorded data
% err = play(x, [1 0]) % play on channels 1 and 0
%
% See also record, getData, getError, stop, init
