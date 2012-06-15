#define DLLIMP extern "C" __declspec(dllimport)

DLLIMP	bool initAudioDevice();

DLLIMP	bool initAudioDeviceFile(const char* xmlSettingsFile);

DLLIMP	const char* getLastError();

DLLIMP	const char* getVersion();

DLLIMP	bool addFilter(const char* settingsFileName, bool isInputFilter);

DLLIMP	bool deleteFilter(bool isInputFilter);

DLLIMP	bool playFiles (const char**	 audioFiles, const int filesNumber, const int *channels, const int channelCount);

DLLIMP	bool playData (const float** audioData, const int chanNumber, const int samplesNumber, const int *channels, const int channelCount);

DLLIMP	bool recordFiles	( const char*	recordDir, const int inputChanNumber, const int *channels, const int channelCount, const float duration );

DLLIMP	bool recordData	( float** inputData, const int inputChanNumber, const __int64 inputSamplesNumber, const int *inChannels, const int channelCount);

DLLIMP	bool playRecordDataMM( float** inputData, const int inputChanNumber, const __int64 inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const float** outputData, const int outputChanNumber, const __int64 outputSamplesNumber, const int* outChannels, const int outChannelsCount );

DLLIMP	bool playRecordDataMD (   const char* recordDir, const int inputChanNumber, const int* inChannels, const int inChannelsCount, float duration,
								const float** outputData, const int outputChanNumber, const __int64 outputSamplesNumber, const int* outChannels, const int outChannelsCount);

DLLIMP	bool playRecordDataDM ( float** inputData, const int inputChanNumber, const __int64 inputSamplesNumber, const int *inChannels, const int inChannelsCount,
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount );

DLLIMP	bool playRecordDataDD ( const char* recordDir, const int inputChanNumber, const int* inChannels, const int inChannelsCount, float duration,
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount );

DLLIMP 	bool setGain (int *channels, int channelsCount, float *gains);

DLLIMP	bool stopAudio();

DLLIMP	bool isRunning();

DLLIMP  double getCurrentPosition(); 

DLLIMP  bool setPosition(double timeInSeconds);

DLLIMP	bool getDeviceSettings( double& samplingRate, int& bufSize, int& bDepth, int& inChannels, int& outChannels);

DLLIMP	void getFilterSettings( bool isInputFilter, int& inputsCount, int& outputsCount );

DLLIMP  void logMessage(const char* msg);

DLLIMP  void logDouble(const char* infoStr, const double value);

DLLIMP  void logAddress(const char* infoStr, const __int64 addr);