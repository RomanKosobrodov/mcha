/*  
	MCHA - Multichannel Audio Playback and Recording Library
    Copyright (C) 2011  Roman Kosobrodov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#define DLLIMP extern "C" __declspec(dllimport)

DLLIMP	bool initAudioDevice();

DLLIMP	bool initAudioDeviceFile(const char* xmlSettingsFile);

DLLIMP	bool setDebugMode(const char* dmStr);

DLLIMP	const char* getLastError();

DLLIMP	const char* getVersion();

DLLIMP	bool addFilter(const char* settingsFileName, bool isInputFilter);

DLLIMP	bool playFiles (const char** audioFiles, const int filesNumber, const int *channels, const int channelCount);

DLLIMP	bool playData_s (const float** audioData, const int chanNumber, const size_t samplesNumber, const int *channels, const int channelCount);
DLLIMP	bool playData_d (const double** audioData, const int chanNumber, const size_t samplesNumber, const int *channels, const int channelCount);

DLLIMP	bool recordFiles(const char* recordDir, const int inputChanNumber, const float duration, const int *channels, const int channelCount );

DLLIMP	bool recordData(float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int channelCount);

DLLIMP	bool playRecordDataMM_s( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const float** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount );
DLLIMP	bool playRecordDataMM_d( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const double** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount );

DLLIMP	bool playRecordDataMD_s (   const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
								const float** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount);

DLLIMP	bool playRecordDataMD_d (   const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
								const double** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount);

DLLIMP	bool playRecordDataDM ( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount );

DLLIMP	bool playRecordDataDD ( const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount );

DLLIMP	int	getRecordedChannelsNum();

DLLIMP	size_t	getRecordedSamplesNum();

DLLIMP bool	getData_s(float**	dat,  const int* channels = NULL, const int channelsNumber = 0, size_t startSample = 0, size_t endSample = 0);

DLLIMP bool	getData_d(double**	dat,  const int* channels = NULL, const int channelsNumber = 0, size_t startSample = 0, size_t endSample = 0);

DLLIMP 	bool setGain (int *channels, int channelsCount, float *gains);

DLLIMP bool stopAudio();

DLLIMP bool isRunning();

DLLIMP double getCurrentPosition();

DLLIMP bool setPosition(double timeInSeconds);

DLLIMP bool getDeviceSettings(double& samplingRate, int& bufSize, int& bDepth, int& inChannels, int& outChannels);

DLLIMP void getFilterSettings(bool isInputFilter, int& inputsCount, int& outputsCount);

DLLIMP void logMessage(const char* msg);

DLLIMP void	logError(const char* msg);

DLLIMP void logDouble(const char* infoStr, const double value);

DLLIMP void logAddress(const char* infoStr, const __int64 addr);