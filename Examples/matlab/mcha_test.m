function mcha_test(deviceconfig)
% test of mcha record and playback toolbox
% mcha_test(deviceconfig)
%
% deviceconfig is the name of the xml file containing audio device
% configuration (normally, 'defaultsettings.xml')
% If the function is called without the parameter the Audio Device Settings
% dialog is displayed.

%% initialise device
clc;
disp('Record and playback test')
disp('==================================')
disp('Initialising audio device ...')

if nargin == 1
    err = init(deviceconfig);
    if (~isempty(err))
        error(err);
    else
        disp('done');
    end
else
    err = init();
    if (~isempty(err))
        error(err);
    else
        disp('done');
    end    
end    
    
%% Playback test
% prepare data
T = 3;          % duration in s
fsample = 48000;     % sampling frequency
N = fsample*T + 5;   % number of samples
dat = 0.75 * (rand(N, 2) - 0.5); % generate T seconds of white noise

err =[];

% start playing data
disp('==================================')
disp('Playback test. Memory')

err = play(dat,[0 1]);
if (~isempty(err))
    error(err);
else
    disp('   playback started ...');
end

pause(1); % wait 1 second

% get current playback position
pos = getPosition();
disp(['   current position = ' num2str(pos,2)]);

% rewind
disp('   rewind')
pause(0.5);
err = setPosition(0.0); 
if (~isempty(err))
    error(err);
else
    pos = getPosition();
    disp(['   current position = ' num2str(pos,2)]);    
end

% stop
pause(1);
disp('   stopping playback ... ');  
err = stop(); 
if (~isempty(err))
    error(err);
else
    disp('stopped.');    
end

% play from disk
disp('==================================');
disp('Playback test. Disk')
disp('   creating WAV files');
wavwrite(dat(:,1),fsample,32,'noise01.wav');
wavwrite(dat(:,2),fsample,32,'noise02.wav');

disp('   playing WAV files')
err = play({'noise01.wav', 'noise02.wav'},[0 1]);
if (~isempty(err))
    error(err);
else
    disp('   started ...');
end

% wait
pause(3);
disp('   waiting ...');
[processStopped, timeElapsed] = wait(3);
if ~processStopped
    disp('   stopping ...');
    err = stop();
    if (~isempty(err))
        error(err);
    else
        disp('the process was forced to stop.');
    end
else
    disp('stopped.');
end

%% Recording test
disp('==================================')
disp('Recording test. Memory')

err = record(0, 3);
if (~isempty(err))
    error(err);
else
    disp('   recording started ...');
end

disp('   waiting ...');

pause(3); % wait for 5 minutes

[processStopped, timeElapsed] = wait(3);

if ~processStopped
    disp('   stopping ...');
    err = stop();
    if (~isempty(err))
        error(err);
    else
        disp('the process was forced to stop.');
    end
else
    disp('stopped.');
end

disp('Reading data from memory')
recData = getData();
err = getError();
if (~isempty(err))
    error(err);
else
    disp('   data obtained ...');
end


disp('==================================')
disp('Recording test. Disk')

err = record(pwd, [0, 1] , 5);
if (~isempty(err))
    error(err);
else
    disp('   recording started ...');
end

disp('   waiting ...');

pause(5); % wait for 5 minutes

[processStopped, timeElapsed] = wait(3);

if ~processStopped
    disp('   stopping ...');
    err = stop();
    if (~isempty(err))
        error(err);
    else
        disp('the process was forced to stop.');
    end
else
    disp('stopped.');
end


%% Playback and Recording Test
disp('==================================')
disp('Playback and Recording Test')
disp(' ')

% --------------------------------------------------------
% disk to disk
disp('disk -> disk')

filesToPlay = {[pwd '/noise01.wav'] [pwd '/noise02.wav']};
err = playRecord(pwd, 0, 3, filesToPlay, [1 0]);
if (~isempty(err))
    error(err);
else
    disp('   recording/playback started ...');
end
pause(5);

disp('   waiting ...');
[processStopped, timeElapsed] = wait(5);
if ~processStopped
    disp('   stopping ...');
    err = stop();
    if (~isempty(err))
        error(err);
    else
        disp('the process was forced to stop.');
    end
else
    disp('stopped.');
end
disp(' ')
wait(2);

% --------------------------------------------------------
% disk to memory
disp('disk -> memory')

err = playRecord(0, 3, filesToPlay, [1 0]);
if (~isempty(err))
    error(err);
else
    disp('   recording/playback started ...');
end
pause(2);

disp('   waiting ...');
[processStopped, timeElapsed] = wait(2);
if ~processStopped
    disp('   stopping ...');
    err = stop();
    if (~isempty(err))
        error(err);
    else
        disp('the process was forced to stop.');
    end
else
    disp('stopped.');
end
disp(' ')

% --------------------------------------------------------
% memory to memory
disp('memory -> memory')
err = playRecord(0, 2, dat, [0 1]);
if (~isempty(err))
    error(err);
else
    disp('   recording/playback started ...');
end

pause(3);

disp('   waiting ...');
[processStopped, timeElapsed] = wait(2);

if ~processStopped
    disp('stopping ...');
    err = stop();
    if (~isempty(err))
        error(err);
    else
        disp('the process was forced to stop.');
    end
else
    disp('stopped.');
end
disp(' ')

% --------------------------------------------------------
% memory to disk
disp('memory -> disk')
err = playRecord(pwd, 0, 3, dat, [0 1]);
if (~isempty(err))
    error(err);
else
    disp('   recording/playback started ...');
end

pause(2);

disp('   waiting ...');
[processStopped, timeElapsed] = wait(2);

if ~processStopped
    disp('stopping ...');
    err = stop();
    if (~isempty(err))
        error(err);
    else
        disp('the process was forced to stop.');
    end
else
    disp('stopped.');
end
% --------------------------------------------------------

% delete files
%% Playback and Recording Test
disp('==================================')
disp('Deleting Files')
disp(' ')

delete('noise01.wav');
delete('noise02.wav');
pause(3);
delete([pwd '/channel*.wav']);
