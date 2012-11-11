% INIT   configure audio device
%
% err = init(xmlsettings)
% err = init()
% 
% Configures the audio device using the settings saved in xml file. The
% file name is specified by the xmlsettigs parameter. 
% 
% When called without the parameter the function displays a dialog that allows
% the user to configure any audio device available to the system.
% The settings are saved in defaultsettings.xml file.
% Normally all input and output channels should be activated.
%
% The function returns empty string on success 
% and a string describing an error if one occures.
%
% Example:
% init('edirol.xml');
%
% See also record, getError, play, stop
%
