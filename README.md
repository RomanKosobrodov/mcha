mcha
====

A cross-platform toolbox for simultaneous recording, processing and playback of multichannel audio in Matlab and Octave.
Main features:

 - Optimised for multichannel audio recording and playback
 - Support of 64 and 32 bit Matlab on Windows (Linux coming soon)
 - Support of Octave on Windows (Linux coming soon)
 - MEX functions return control to Matlab/Octave immediately after starting recording/playback thread and could be terminated by the user at any time
 - Supported audio interfaces: ASIO, WindowsAudio and DirectSound (on Windows) and ALSA/JACK (Linux)
 - Audio device settings could be stored in *.xml files allowing flexible change of configurations
 - Playback file formats: WAV, OGG and AIFF with 16, 24 and 32 bit resolution
 - Recording file format: 32-bit floating point WAV (one file for each channel)
 - Direct recording and playback to/from Matlab and Octave matrices
 - Real-time fast convolution of input and output signals using partitioned convolution technique
 - Real-time filtering of input and output signals using user-defined IIR filters
 - Multithreaded disk read/write operations.

The software is written in C++ using open source JUCE and FFTW libraries.