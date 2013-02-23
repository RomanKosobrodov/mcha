#ifndef MCHA_RECORD_PLAYER_H
#define MCHA_RECORD_PLAYER_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "fftw3.h"
#include "settings.h"

#include "AudioDeviceSettings.h"
#include "AudioSampleProcessor.h"
#include "mchaFilter.h"

#if JUCE_LINUX
	#include "LinuxMessageThread.h"
#endif


namespace mcha
{
	
/* forward declarations */
class	AudioDeviceSettings;
class	AudioSampleProcessor;
class	AudioSampleRecorder;
class	AudioSamplePlayer;
class	MchaFilter;

#if JUCE_LINUX
	class LinuxMessageThread;
#endif


// ============================================================================================
class MchaRecordPlayer: public Timer
{
public:
	MchaRecordPlayer();
    ~MchaRecordPlayer();

	/* initialise device settings */
	bool	init();
	bool	init(const char* xmlSettingsFile);
	
	/* Add or delete input and output audio processors */
	bool addFilter(const char* settingsFileName, bool isInputFilter = false);
	void getFilterSettings(bool isInputFilter, int& inputsCount, int& outputsCount);	

	/* play and record */
	template <class D, class S> bool playRecord(D** recordData, const int recordChannelCount, const float recordSamplesNumber, const int *recDeviceChannels, const int recDeviceChannelsCount,
												const S** playbackData, const int playbackDataChannels, const size_t playbackSamplesNumber, const int* devicePlayChannels, const int devicePlayChannelsCount);

	/* recorded data */
	template <class D> bool	getData(D**	dat,  const int* channels = NULL, const int channelsNumber = 0, size_t startSample = 0, size_t endSample = 0) 
									{ return copyRecordedData(dat, channels, channelsNumber, startSample, endSample); };
	int		getRecordedChannelsNum() {	return recordChannels; };
	size_t	getRecordedSamplesNum()  {	return recordSamplesNum; };


	/* playback/recording control */
	bool	setGain (int *channels, int channelsCount, float *gains);
	bool	stop();
	bool	isRunning() { return processIsRunning; };
	double	getCurrentPosition();
	bool	setPosition(double timeInSeconds);
	bool	setPosition(size_t sampleOffset);
	
	/* error handling */
	const char*	getLastError() { return lastError.toUTF8(); };
	String		getLastErrorStr() { return lastError; } ;
	void		clearError() { lastError = String::empty; };
	void		dbgOut(const String &message);
	void		logError(const String &m) {lastError = m; dbgOut(m);};

	/* debugging modes */
	enum		DebugMode { none, normal, advanced };	
	const char*	getDebugMode() 	{ return debugModeString.toUTF8(); };
	bool		setDebugMode(const char* dmStr);

	/* memory use modes */
	enum		MemoryMode { safe, smart };
	const char* getMemoryMode() { return memModeString.toUTF8(); };
	bool		setMemoryMode(const char* mMode);
	void		setMemoryMode(MemoryMode mMode) { memMode = mMode;};

	/* memory management */
	template <class T> bool	allocateBuffer(T** &buf, size_t bufferCount, size_t singleBufferSize, bool shouldBeCleared, String bufferVariableName );
	template <class T> bool allocateBuffer(T* &buf, size_t singleBufferSize, bool shouldBeCleared, String bufferVariableName );

	template <class T> void	releaseBuffer(T** &buf, size_t len) // Releases the double buffer of type T
	{
		if (buf == nullptr) return;

		for (size_t i=0; i < len; i++)
		{  
			fftwf_free(buf[i]);
		}

		delete [] buf;
		buf = nullptr;
	}

	template <class T> void	releaseBuffer(T* &buf)
	{
		if (buf != nullptr)
		{
			fftwf_free(buf);
			buf = nullptr;
		}
	}

	void	debugBuffer(float** buf, size_t bufferCount, size_t singleBufferSize, String bufferVariableName );
	void	debugBuffer(float* buf, size_t bufferSize, String bufferVariableName );
	void	debugBuffer(bool* buf, size_t bufferSize, String bufferVariableName );
	void	debugBuffer(fftwf_complex* buf, size_t bufferSize, String bufferVariableName );

	
	void	processShouldStop() { stopProcessing = true; };

	AudioDeviceSettings& getDeviceSettings() {return *audioDeviceSettings;};

	/* Timer method */
	void 	timerCallback();

private:
	#if JUCE_LINUX
	LinuxMessageThread	linuxMessageThread;
	#endif
	
	String	lastError;
	volatile bool stopProcessing;

	bool	start(); 
	void	clearObjects();
	bool	deleteFilter(bool isInputFilter);

	template <class T> bool copyPlaybackData( const T** srcData, const int channelsNumber, const size_t samplesNumber);
	template <class T> bool copyRecordedData( T** destination, const int* channels = NULL, const int channelsNumber = 0, size_t startSample = 0, size_t endSample = 0); 

	template <class T> void copyBuffer(T* dst, float* src, size_t ind0, size_t ind1);

	template <class D> bool createRecorder(D** recordData, const int recordChannelCount, const float recordSamplesNumber, const int *recDeviceChannels, const int recDeviceChannelsCount);
	template <class S> bool	createPlayer(const S** playbackData, const int playbackDataChannels, const size_t playbackSamplesNumber, const int* devicePlayChannels, const int devicePlayChannelsCount);

	bool	updateChannels(  const int* deviceChannels, const int deviceChannelsCount, bool isPlaybackDevice );

	void	printSystemInfo();

	ScopedPointer<AudioDeviceManager>	audioDeviceManager;
	ScopedPointer<AudioDeviceSettings>	audioDeviceSettings;	

	AudioSampleProcessor*	audioSampleProcessor;
	AudioSampleRecorder*	audioSampleRecorder;
	AudioSamplePlayer*		audioSamplePlayer;

	MchaFilter*			recordingFilter;
	MchaFilter*			playbackFilter;

	ScopedPointer<FileLogger> fileLogger;

	Array<float>			outGainsArray;
	Array<int>				outChannelsArray;

	String					curDirStr;

	DebugMode				debugMode;
	String					debugModeString;
	AudioSampleRecorder*	debugRecorder;

	MemoryMode				memMode;
	String					memModeString;

	float**					playbackBuffer;
	int						playbackChannels;
	size_t					playbackSamplesNum;

	float**					recordBuffer;
	int						recordChannels;
	size_t					recordSamplesNum;

	int*					recChanArray;
	int						recChanCount;

	volatile bool			processIsRunning;

	// JUCE_LEAK_DETECTOR(MchaRecordPlayer) // this does not work properly for some reason

public:
	juce_DeclareSingleton (MchaRecordPlayer, true)
};

}

#endif
