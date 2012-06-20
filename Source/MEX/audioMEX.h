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
#if (defined (_WIN32) || defined (_WIN64))
	#define MCHAIMPORT extern "C" __declspec(MCHAIMPORTort)
#else
	#define MCHAIMPORT extern "C"
#endif
        
MCHAIMPORT	bool initAudioDevice();

MCHAIMPORT	bool initAudioDeviceFile(const char* xmlSettingsFile);

MCHAIMPORT	bool setDebugMode(const char* dmStr);

MCHAIMPORT	const char* getLastError();

MCHAIMPORT	const char* getVersion();

MCHAIMPORT	bool addFilter(const char* settingsFileName, bool isInputFilter);

MCHAIMPORT	bool playFiles (const char** audioFiles, const int filesNumber, const int *channels, const int channelCount);

MCHAIMPORT	bool playData_s (const float** audioData, const int chanNumber, const size_t samplesNumber, const int *channels, const int channelCount);
MCHAIMPORT	bool playData_d (const double** audioData, const int chanNumber, const size_t samplesNumber, const int *channels, const int channelCount);

MCHAIMPORT	bool recordFiles(const char* recordDir, const int inputChanNumber, const float duration, const int *channels, const int channelCount );

MCHAIMPORT	bool recordData(float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int channelCount);

MCHAIMPORT	bool playRecordDataMM_s( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const float** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount );
MCHAIMPORT	bool playRecordDataMM_d( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const double** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount );

MCHAIMPORT	bool playRecordDataMD_s (   const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
								const float** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount);

MCHAIMPORT	bool playRecordDataMD_d (   const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
								const double** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount);

MCHAIMPORT	bool playRecordDataDM ( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount );

MCHAIMPORT	bool playRecordDataDD ( const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount );

MCHAIMPORT	int	getRecordedChannelsNum();

MCHAIMPORT	size_t	getRecordedSamplesNum();

MCHAIMPORT bool	getData_s(float**	dat,  const int* channels = NULL, const int channelsNumber = 0, size_t startSample = 0, size_t endSample = 0);

MCHAIMPORT bool	getData_d(double**	dat,  const int* channels = NULL, const int channelsNumber = 0, size_t startSample = 0, size_t endSample = 0);

MCHAIMPORT 	bool setGain (int *channels, int channelsCount, float *gains);

MCHAIMPORT bool stopAudio();

MCHAIMPORT bool isRunning();

MCHAIMPORT double getCurrentPosition();

MCHAIMPORT bool setPosition(double timeInSeconds);

MCHAIMPORT bool getDeviceSettings(double& samplingRate, int& bufSize, int& bDepth, int& inChannels, int& outChannels);

MCHAIMPORT void getFilterSettings(bool isInputFilter, int& inputsCount, int& outputsCount);

MCHAIMPORT void logMessage(const char* msg);

MCHAIMPORT void	logError(const char* msg);

MCHAIMPORT void logDouble(const char* infoStr, const double value);

MCHAIMPORT void logAddress(const char* infoStr, const long long addr);