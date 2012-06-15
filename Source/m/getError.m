% GETERROR return the string containing the last error 
%
% err = getError()
%
% Use the function to get the last error message. The error message is not
% cleared after the call to the function, so it can be used several times
% in your code.
%
% Example:
%
% clear all; % clear all objects including recorded samples
% try
%   x = getData;  % try obtaining data from memory
% catch         % do nothing here
% end
% err = getError() % display the error
%
% See also record, play, stop, init
%
