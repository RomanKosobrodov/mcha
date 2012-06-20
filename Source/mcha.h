#if (defined (_WIN32) || defined (_WIN64))
	#define MCHAIMPORT extern "C" __declspec(dllimport)
#else
	#define MCHAIMPORT extern "C"
#endif


MCHAIMPORT	bool initAudioDevice();

MCHAIMPORT	bool initAudioDeviceFile(const char* xmlSettingsFile);

MCHAIMPORT	const char* getLastError();

MCHAIMPORT	const char* getVersion();

MCHAIMPORT	bool addFilter(const char* settingsFileName, bool isInputFilter);

MCHAIMPORT	bool deleteFilter(bool isInputFilter);

MCHAIMPORT	bool playFiles (const char**	 audioFiles, const int filesNumber, const int *channels, const int channelCount);

MCHAIMPORT	bool playData (const float** audioData, const int chanNumber, const int samplesNumber, const int *channels, const int channelCount);

MCHAIMPORT	bool recordFiles	( const char*	recordDir, const int inputChanNumber, const int *channels, const int channelCount, const float duration );

MCHAIMPORT	bool recordData	( float** inputData, const int inputChanNumber, const __int64 inputSamplesNumber, const int *inChannels, const int channelCount);

MCHAIMPORT	bool playRecordDataMM( float** inputData, const int inputChanNumber, const __int64 inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const float** outputData, const int outputChanNumber, const __int64 outputSamplesNumber, const int* outChannels, const int outChannelsCount );

MCHAIMPORT	bool playRecordDataMD (   const char* recordDir, const int inputChanNumber, const int* inChannels, const int inChannelsCount, float duration,
								const float** outputData, const int outputChanNumber, const __int64 outputSamplesNumber, const int* outChannels, const int outChannelsCount);

MCHAIMPORT	bool playRecordDataDM ( float** inputData, const int inputChanNumber, const __int64 inputSamplesNumber, const int *inChannels, const int inChannelsCount,
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount );

MCHAIMPORT	bool playRecordDataDD ( const char* recordDir, const int inputChanNumber, const int* inChannels, const int inChannelsCount, float duration,
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount );

MCHAIMPORT 	bool setGain (int *channels, int channelsCount, float *gains);

MCHAIMPORT	bool stopAudio();

MCHAIMPORT	bool isRunning();

MCHAIMPORT  double getCurrentPosition(); 

MCHAIMPORT  bool setPosition(double timeInSeconds);

MCHAIMPORT	bool getDeviceSettings( double& samplingRate, int& bufSize, int& bDepth, int& inChannels, int& outChannels);

MCHAIMPORT	void getFilterSettings( bool isInputFilter, int& inputsCount, int& outputsCount );

MCHAIMPORT  void logMessage(const char* msg);

MCHAIMPORT  void logDouble(const char* infoStr, const double value);

MCHAIMPORT  void logAddress(const char* infoStr, const __int64 addr);