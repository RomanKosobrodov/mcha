#if (defined (_WIN32) || defined (_WIN64))
	extern "C" __declspec(dllimport)
#else
	#define __int64	long long
	extern "C"
#endif

{

bool initAudioDevice();

bool initAudioDeviceFile(const char* xmlSettingsFile);

const char* getLastError();

const char* getVersion();

bool addFilter(const char* settingsFileName, bool isInputFilter);

bool deleteFilter(bool isInputFilter);

bool playFiles (const char**	 audioFiles, const int filesNumber, const int *channels, const int channelCount);

bool playData (const float** audioData, const int chanNumber, const int samplesNumber, const int *channels, const int channelCount);

bool recordFiles	( const char*	recordDir, const int inputChanNumber, const int *channels, const int channelCount, const float duration );

bool recordData	( float** inputData, const int inputChanNumber, const __int64 inputSamplesNumber, const int *inChannels, const int channelCount);

bool playRecordDataMM( float** inputData, const int inputChanNumber, const __int64 inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const float** outputData, const int outputChanNumber, const __int64 outputSamplesNumber, const int* outChannels, const int outChannelsCount );

bool playRecordDataMD (   const char* recordDir, const int inputChanNumber, const int* inChannels, const int inChannelsCount, float duration,
								const float** outputData, const int outputChanNumber, const __int64 outputSamplesNumber, const int* outChannels, const int outChannelsCount);

bool playRecordDataDM ( float** inputData, const int inputChanNumber, const __int64 inputSamplesNumber, const int *inChannels, const int inChannelsCount,
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount );

bool playRecordDataDD ( const char* recordDir, const int inputChanNumber, const int* inChannels, const int inChannelsCount, float duration,
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount );

bool setGain (int *channels, int channelsCount, float *gains);

bool stopAudio();

bool isRunning();

double getCurrentPosition(); 

bool setPosition(double timeInSeconds);

bool getDeviceSettings( double& samplingRate, int& bufSize, int& bDepth, int& inChannels, int& outChannels);

void getFilterSettings( bool isInputFilter, int& inputsCount, int& outputsCount );

void logMessage(const char* msg);

void logDouble(const char* infoStr, const double value);

void logAddress(const char* infoStr, const __int64 addr);

}
