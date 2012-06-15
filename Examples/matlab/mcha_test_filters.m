function mcha_test_filters(deviceconfig)
% test of mcha record and playback toolbox - filters
% mcha_test_filters(deviceconfig)
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
% start playing data
disp('==================================')
disp('Playback Filters')

folder = '..\sound\samples\';
snd = { [folder 'tone_1kHz.wav'] ,  [folder 'tone_2kHz.wav'] };

for ch = 1:length(snd)
    disp(['   channel' num2str(ch-1, '%02d') ': ' snd{ch}])
end

filterfile = '..\filters\notch_fir.xml';
disp(['   filter: ' filterfile]); 
disp('');

% play unfiltered sound
disp('Playing unfiltered sound');
err = play(snd,[0 1]);
if (~isempty(err))
    error(err);
end

pause(4); % wait 4 seconds

% stop
[processStopped, timeElapsed] = wait(1);
if (~processStopped)
    error(getError());
end


% play using filters
disp('Playing using notch filters');
err = play(snd,[0 1], filterfile);
if (~isempty(err))
    error(err);
end

pause(4); % wait 4 seconds

% stop
[processStopped, timeElapsed] = wait(1);
if (~processStopped)
    error(getError());
end



%% Recording test
disp('==================================')
disp('Recording Filters')

folder = '..\sound\recorded\';
filterRec = '..\filters\third_octave_IIR.xml';

disp(['   folder: ' folder])
disp(['   filter: ' filterRec]); 
disp('');

err = record(folder, 0, 5, filterRec);
if (~isempty(err))
    error(err);
end

pause(5); % wait for 5 minutes

[processStopped, timeElapsed] = wait(2);
if (~processStopped)
    error(getError());
end

% rename the files
disp('rewriting files ...');
curdir = pwd;
cd(folder);
% wait till the files are rewritten
while ~isempty(dir('data.dat'))
    pause(0.5);
end

for ch=0:29
    movefile( ['channel' num2str(ch,'%02d') '.wav'], ['third_octave_channel' num2str(ch,'%02d') '.wav'] );
end
cd(curdir);
disp('done.')

%% Playback and Recording Test
disp('==================================')
disp('Playback and Recording Test')
disp(' ')

folderPlay = '..\sound\samples\';
folderRec = '..\sound\recorded\';
snd = { [folderPlay 'noise01.wav'] };

filterPlayback = '..\filters\a-weighting_IIR.xml';
filterRecord = '..\filters\third_octave_IIR.xml';

err = playRecord(folderRec, 0, 3, filterRecord, snd, 0, filterPlayback);
if (~isempty(err))
    error(err);
end

pause(3);

[processStopped, timeElapsed] = wait(2);
if (~processStopped)
    error(getError());
end

