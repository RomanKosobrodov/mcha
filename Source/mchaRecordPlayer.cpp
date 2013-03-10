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

#include "fftw3.h"
#include "mchaRecordPlayer.h"
#include "MemorySamplePlayer.h"
#include "MemorySampleRecorder.h"
#include "DiskSampleRecorder.h"
#include "DiskSamplePlayer.h"
#include "AudioDeviceSettings.h"

namespace mcha
{
	
juce_ImplementSingleton (MchaRecordPlayer)

// ============================================================================================
MchaRecordPlayer::MchaRecordPlayer():
										lastError(String::empty),
										audioSampleRecorder(NULL),
										audioSamplePlayer(NULL),
										recordingFilter(NULL),
										playbackFilter(NULL),
										fileLogger(NULL),
										audioSampleProcessor(NULL),
										processIsRunning(false),
										stopProcessing(false),
										debugMode(normal),
										debugRecorder(NULL),
										playbackBuffer(NULL),
										playbackChannels(0),
										playbackSamplesNum(0),
										recordBuffer(NULL),
										recordChannels(0),
										recordSamplesNum(0),
										recChanArray(NULL),
										recChanCount(0),
										memMode(safe),
										memModeString(L"safe")
{
	/* Initialise JUCE GUI stuff here */
	#if JUCE_WINDOWS
		initialiseJuce_GUI();
	#endif


	//curDirStr = File::getSpecialLocation(File::currentApplicationFile).getParentDirectory().getFullPathName() + File::separatorString;
	
	
	fileLogger = FileLogger::createDefaultAppLogger( "mcha", "mcha.log.txt", String::empty, 0 );	
	
	printSystemInfo(); 

	/* Generate full path to default XML settings file */
	File curDirFile( File::getSpecialLocation( File::userApplicationDataDirectory ).getFullPathName() + 						File::separatorString + L"mcha" );

	if ( !curDirFile.isDirectory() )
	{
		if ( !curDirFile.createDirectory() )
		{
			logError( L"Unable to create directory for XML settings file" );		
		}
	}

	curDirStr = curDirFile.getFullPathName() + File::separatorString;

	
	audioDeviceSettings = new AudioDeviceSettings();

	/* read default device settings */
	if ( !audioDeviceSettings->setXmlSettingsFile(curDirStr + XML_SETTINGS_FILE) )
	{	
		logError( audioDeviceSettings->getErrorString() );
	}

	outGainsArray.clear();
	outChannelsArray.clear();

	audioDeviceManager = new AudioDeviceManager();

}

// ============================================================================================
MchaRecordPlayer::~MchaRecordPlayer()
{
	DBG( "Deleting mchaRecordPlayer" );
	if (processIsRunning)
	{
		DBG( "\tmchaRecordPlayer is running a task: trying to stop..." );
		if ( stop() )
		{
			DBG( "\tmchaRecordPlayer: task terminated successfully" );
		}
		else
		{
			DBG( "\tmchaRecordPlayer: failed to terminate task." );
		}
	}

	/* release playback and record buffers */
	releaseBuffer(playbackBuffer, playbackChannels);
	releaseBuffer(recordBuffer, recordChannels);
	releaseBuffer(recChanArray);

	#if JUCE_WINDOWS
		shutdownJuce_GUI(); // clean up GUI
	#endif

	clearSingletonInstance();

	DBG("\t..done");

}

//==============================================================================
template <class T> bool	MchaRecordPlayer::allocateBuffer(T** &buf, size_t bufferCount, size_t singleBufferSize, bool shouldBeCleared, String bufferVariableName)
{
	try
	{
		buf = new T* [bufferCount];
	}
	catch (std::bad_alloc)
	{
		logError( "\tMemory allocation for " + bufferVariableName + " failed." );
		return false;	
	}


	for (size_t ch = 0; ch < bufferCount; ++ch)
	{
		try
		{
			buf[ch] = static_cast<T*> ( fftwf_malloc(sizeof(T) * singleBufferSize) ); // fftwf_alloc_complex( singleBufferSize );
		}
		catch (std::bad_alloc) // memory allocation failed
		{
			logError( "\tMemory allocation for " + bufferVariableName + " failed." );
			releaseBuffer(buf, ch+1); // clean up before exit
			return false;
		}

		if (shouldBeCleared)
			zeromem(buf[ch], singleBufferSize*sizeof(T));

	}
	
	return true;
}


//==============================================================================
template <class T> bool	MchaRecordPlayer::allocateBuffer(T* &buf, size_t singleBufferSize, bool shouldBeCleared, String bufferVariableName)
{
	try
	{
		buf = static_cast<T*> ( fftwf_malloc(sizeof(T) * singleBufferSize) );
	}
	catch (std::bad_alloc)
	{
		logError( "\tMemory allocation for " + bufferVariableName + " failed." );
		return false;	
	}

	if (shouldBeCleared)
		zeromem(buf, singleBufferSize*sizeof(T));

	return true;
}


// ============================================================================================
bool MchaRecordPlayer::deleteFilter(bool isRecordingFilter)
{
	if (processIsRunning)
	{
		logError( L"Unable to delete filter: playback/recording process is running" );
		return false;
	}
	
	if (isRecordingFilter)
	{
		if (recordingFilter != NULL)
		{
			dbgOut( "Deleting recording filter:\t" + recordingFilter->getName() );	
			delete recordingFilter;
			recordingFilter = NULL;
		}
	}
	else
	{
		if (playbackFilter != NULL)
		{
			dbgOut( "Deleting playback filter:\t" + playbackFilter->getName() );		
			delete playbackFilter;
			playbackFilter = NULL;
		}
	}
	return true;
}

// ============================================================================================
bool MchaRecordPlayer::addFilter(const char* settingsFileName, bool isRecordingFilter)
{
	dbgOut("");
	dbgOut(" ***  mchaRecordPlayer::addFilter ***");

	if (processIsRunning)
	{
		logError( L"Unable to add filter: playback/recording is running" );
		return false;
	}
	
	/* delete existing filter */
	deleteFilter(isRecordingFilter);
	
	/* read filter settings */
	clearError();
	FilterSettings* filterSettings = new FilterSettings(settingsFileName);
	
	/* check results */
	if (lastError.isNotEmpty())
	{
		delete filterSettings;
		return false;
	}

	/* create new filter */
	if (isRecordingFilter)
	{
		if (filterSettings->getFilterType() == FilterSettings::FIR)
			recordingFilter = new MchaFIRFilter();
		else
			recordingFilter = new MchaIIRFilter();
		recordingFilter->applySettings(filterSettings);
		dbgOut("Input processor added:\t" + recordingFilter->getName());
		dbgOut(String::formatted("\tNumber of inputs:\t%d", recordingFilter->getInputsNumber() ));
		dbgOut(String::formatted("\tNumber of outputs:\t%d", recordingFilter->getOutputsNumber() ));
	}
	else
	{
		if (filterSettings->getFilterType() == FilterSettings::FIR)
			playbackFilter = new MchaFIRFilter();
		else
			playbackFilter = new MchaIIRFilter();
		playbackFilter->applySettings(filterSettings);
		dbgOut("Output processor added:\t" + playbackFilter->getName());
		dbgOut(String::formatted("\tNumber of inputs:\t%d", playbackFilter->getInputsNumber() ));
		dbgOut(String::formatted("\tNumber of outputs:\t%d", playbackFilter->getOutputsNumber() ));
	}

	delete filterSettings;
	return true;
}

// ============================================================================================
void MchaRecordPlayer::getFilterSettings(bool isRecordingFilter, int& inputsCount, int& outputsCount)
{
	inputsCount = -1;
	outputsCount = -1;
	MchaFilter* mchaFilter = isRecordingFilter? recordingFilter : playbackFilter;

	if (mchaFilter != NULL)
	{
		inputsCount = mchaFilter->getInputsNumber();
		outputsCount = mchaFilter->getOutputsNumber();
	}
}

// ============================================================================================
void MchaRecordPlayer::clearObjects()
{
	DBG("MchaRecordPlayer::clearObjects()");
	DBG("delete rec filter");
	deleteFilter(true);
		DBG("delete play filter");
	deleteFilter(false);

	/* recChanArray will be cleared automatically by ScopedPointer */

	if (debugRecorder != NULL)
	{
		delete debugRecorder;
		debugRecorder = NULL;
	}

	DBG("audioSampleRecorder");
	delete audioSampleRecorder;		
	audioSampleRecorder = NULL;
	DBG("audioSamplePlayer");	
	delete audioSamplePlayer;		
	audioSamplePlayer = NULL;
	DBG("audioSampleProcessor");	
	delete audioSampleProcessor;
	audioSampleProcessor = NULL;

	outGainsArray.clear();
	outChannelsArray.clear();
}
// ============================================================================================
template <> bool	MchaRecordPlayer::copyPlaybackData<float> ( const float** srcData, const int channelsNumber, const size_t samplesNumber)
{
	if (playbackBuffer != NULL)
	{
		/* release old buffer */
		releaseBuffer(playbackBuffer, playbackChannels);
	}
	
	/* allocate memory for new buffer */
	if ( !allocateBuffer(playbackBuffer, channelsNumber, samplesNumber, false , L"playbackBuffer") )
		return false;

	/* copy new data */
	size_t len = samplesNumber*sizeof(float);
	for (int ch=0; ch<channelsNumber; ++ch)
		memcpy( playbackBuffer[ch], srcData[ch], len);

	playbackChannels = channelsNumber;
	playbackSamplesNum = samplesNumber;

	return true;
	
}

// ============================================================================================
template <class T> bool	MchaRecordPlayer::copyPlaybackData( const T** srcData, const int channelsNumber, const size_t samplesNumber)
{
	dbgOut( L"copyPlaybackData called" );
	dbgOut( L"\tConverting data to float" );
	
	if (playbackBuffer != NULL)
	{
		/* release old buffer */
		releaseBuffer(playbackBuffer, playbackChannels);
	}
	
	/* allocate memory for new buffer */
	if ( !allocateBuffer(playbackBuffer, channelsNumber, samplesNumber, false , L"playbackBuffer") )
		return false;

	/* copy new data */
	for (int ch=0; ch<channelsNumber; ++ch)
		for (size_t i=0; i<samplesNumber; ++i)
			playbackBuffer[ch][i] = static_cast <float> (srcData[ch][i]);

	playbackChannels = channelsNumber;
	playbackSamplesNum = samplesNumber;

	return true;
	
}

// ============================================================================================
template <class T> bool	MchaRecordPlayer::copyRecordedData( T** destination, const int* channels, const int channelsNumber, size_t startSample, size_t endSample)
{
	if (memMode == smart)
	{
		logError(L"Recorded data are not available in 'smart' mode.");
		return false;
	}

	if (recordBuffer == NULL)
	{
		logError(L"Record buffer is empty.");
		return false;	
	}
	
	int chCount = channelsNumber;

	size_t	s0 = startSample;
	size_t	s1 = endSample;
	int* chans = const_cast <int*> (channels);
	
	if (channels == NULL) // copy all channels
	{
		chCount = recordChannels;
		chans = new int[chCount];
		for (int i=0; i<chCount; ++i)
			chans[i] = i;
	}
	else	// sanity check
	{
		for (int j=0; j<chCount; ++j)
			if (( chans[j] < 0 ) || (chans[j] >= recordChannels))
				return false;
	}

	if ((startSample == 0) && (endSample == 0)) // copy all samples
	{
		s0 = 0;
		s1 = recordSamplesNum;
	}

	/* copy data for each channel */
	for (int j=0; j < chCount; ++j)
	{
		copyBuffer( destination[j], recordBuffer[ chans[j] ], s0, s1 );
	}

	return true;
}

// ============================================================================================
template <class T> void	MchaRecordPlayer::copyBuffer(T* dst, float* src, size_t ind0, size_t ind1)
{
	for (size_t k = ind0; k <= ind1; ++k)
		dst[k] = static_cast<T> (src[k]);
}

// ============================================================================================
template <> void	MchaRecordPlayer::copyBuffer(float* dst, float* src, size_t ind0, size_t ind1)
{
	size_t len = (ind1-ind0+1)*sizeof(float);
	memcpy(dst+ind0, src+ind0, len);
}

// ============================================================================================
double MchaRecordPlayer::getCurrentPosition ()
{
    double sampleRate = 1.0;
	int bufSize, bDepth, inChannels, outChannels;	
	
	int64 curPosition = 0;

	if (processIsRunning)	// the task is running - get current position
	{

		if (audioSampleRecorder != NULL)
		{
			if (audioSampleRecorder->getName() != "EmptySampleRecorder" )			
			{
				curPosition = audioSampleRecorder->getCurrentPosition();
			}
		}
		if (audioSamplePlayer != NULL) 
		{
			if (audioSamplePlayer->getName() != "EmptySamplePlayer" )
			{	
				curPosition = audioSamplePlayer->getCurrentPosition(); 
			}
		}
	}

	// Get the current playback/recording position from start (in seconds)
	if (audioDeviceSettings->getSettings(sampleRate, bufSize, bDepth, inChannels, outChannels))
	{
		return curPosition/sampleRate;
	}
	else
		return 0;
}

// ============================================================================================
bool MchaRecordPlayer::setPosition(size_t sampleOffset)
{
	if (( processIsRunning) && (audioSamplePlayer != NULL) )	// the task is running - get current position
	{
		return audioSamplePlayer->setPosition(sampleOffset);
	}
	else
	{
		dbgOut( "\tprocess is not running" );
		return false;
	}
}

// ============================================================================================
bool MchaRecordPlayer::setPosition(double timeInSeconds)
{
	dbgOut( "mchaRecordPlayer::setPosition" );

	double	sampleRate = 1.0;
	int		bufSize, bDepth, inChannels, outChannels;
	size_t	newPosition;

	// get current device settings
	if (!audioDeviceSettings->getSettings(sampleRate, bufSize, bDepth, inChannels, outChannels))
	{
		logError( "\tUnable to get audio device settings" );
		return false;
	}
	else
	{
		// get position as sample offset and set it
		newPosition = size_t(timeInSeconds*sampleRate);
		bool res = setPosition(newPosition);
		if (res)
			dbgOut( "\tnew position,s:\t" + String( static_cast<uint64>(newPosition) ) );
		else
			logError( "\tsetPosition failed." );
		return res;
	}
}

// ============================================================================================
bool MchaRecordPlayer::setGain (int *channels, int channelsCount, float *gains)
{
	dbgOut( "mchaRecordPlayer::setGain" );

	int curChannel = -1;
	/* update information about channels and gains */
	for (int i=0; i<channelsCount; i++)
	{
		curChannel = outChannelsArray.indexOf( channels[i] );
		if (curChannel == -1)	// new channel
		{
			outChannelsArray.add( channels[i] );
			outGainsArray.add( gains[i] );
		}
		else // gain has been changed
		{
			outGainsArray.set(curChannel, gains[i]);
		}

		dbgOut( "\t Gain[" + String(channels[i]) + "] = " + String::formatted( "%.2f", gains[i] ) );
	}
	
	if ((audioSamplePlayer != NULL)) // the task is running - apply the changes
		return audioSamplePlayer->setGain(channels, channelsCount, gains);
	else
		return true;
}

// ============================================================================================
bool MchaRecordPlayer::start()
{
	if (processIsRunning)
	{	
		logError( L"MchaRecordPlayer::start() failed: another task is running." );
		return false;
	}
	else
	{
		dbgOut( "MchaRecordPlayer::start" );		
	}
	
	dbgOut( "MchaRecordPlayer Thread ID\t= " + String( uint64(Thread::getCurrentThreadId()) ) );

	/* get the Xml device settings from mchaRecordPlayer::audioDeviceSettings */
	XmlElement* tmpDeviceSettings = audioDeviceSettings->getXmlDeviceSettings();

	if ( tmpDeviceSettings == NULL)
    {
		logError( L"Failed to obtain device settings" );
		return false;
    }

	/* create a local copy of mchaRecordPlayer::audioDeviceSettings.xmlSettings */
	XmlElement xmlSettings( *tmpDeviceSettings );
	
	/* reset the pointer to the local copy */
	tmpDeviceSettings = &xmlSettings;

	/* Check the channels array */
	Array<int> *channels = audioSampleRecorder->getDeviceChannelsMap(); 
	if (channels != NULL)
	{
		/* Get the input channels bit array */
		String outChanString = audioDeviceSettings->getInputChannelsStr();
		int strLength = outChanString.length();	

		if (outChanString.isEmpty())
		{
			logError( L"Wrong format of device settings: audioDeviceInChans is missing or empty\t" );
			return false;		
		}

		/* Reset all active channels */
		String outChanStringCleared = outChanString.replaceCharacter('1', '0');

		/* Set the required channels active */
		int currentChannel =-1;
		for (int ch=0 ; ch < channels->size(); ++ch)
		{
			currentChannel = channels->getUnchecked(ch);
			if ((currentChannel >=0) && (currentChannel < strLength))
			{
				outChanStringCleared = outChanStringCleared.replaceSection( strLength - 1 - currentChannel , 1, "1");
			}
			else
			{
				logError( L"Wrong input channel." );
				dbgOut(outChanString);
				dbgOut( L"channels size = " +String(channels->size()) );

				for (int j=0 ; j < channels->size(); ++j)
					dbgOut( String(channels->getUnchecked(ch)) );

				return false;			
			}
		}

		/* Modify XML settings element */
		tmpDeviceSettings->setAttribute("audioDeviceInChans", outChanStringCleared );
	}
	
	/* Initialise the device */
	lastError = audioDeviceManager->initialise (	2,	/* number of input channels */
													2,	/* number of output channels */
													tmpDeviceSettings,		/* XML settings */
													false				/* select default device on failure */ );


	if (lastError.isNotEmpty())
	{
		dbgOut( "mchaRecordPlayer::initAudioDevice failed:\t" + lastError );
		return false;
	}

	/* read MCHASETTINGS element */
	XmlElement*	el = tmpDeviceSettings->getChildByName("MCHASETTINGS");
	if (el == NULL)
	{
		dbgOut( L"Warning: wrong format of XML settings file (<MCHASETTINGS>). Using default values.");
	}
	else
	{
		/* set Memory mode */
		String memMode = el->getStringAttribute( L"memoryMode", L"normal" ); // use normal mode by default
		if ( !setMemoryMode( memMode.toUTF8() ) )
			dbgOut( L"Warning: wrong memory mode value: " + memMode + L". Using default value.");
		
		/* set Logging level */
		String logMode = el->getStringAttribute( L"loggingLevel", 	L"safe" ); // use safe mode by defaul
		if ( !setDebugMode( logMode.toUTF8() ) )
			dbgOut( L"Warning: wrong logging level value: " + logMode + L". Using default value.");
	}

	/* get the settings from the device */
	AudioIODevice* device = audioDeviceManager->getCurrentAudioDevice();
	if (device == NULL)
	{
		logError( L"AudioDeviceManager::getCurrentAudioDevice() returned NULL");
		return false;
	}

	double sampleRate = device->getCurrentSampleRate();
	int bufferSize = device->getCurrentBufferSizeSamples();
	
	dbgOut( "\tAudio device initialised successfully:");
	dbgOut( "\t\tSample Rate, Hz:\t\t" + String(sampleRate) );
	dbgOut( "\t\tBuffer Size, samples:\t\t" + String(bufferSize) );
	dbgOut( "\t\tBit Depth, bit:\t\t" + String(device->getCurrentBitDepth()) );
	dbgOut( "\t\tNumber of input channels:\t\t" + String(device->getActiveInputChannels().countNumberOfSetBits()) );
	dbgOut( "\t\tNumber of output channels:\t\t" + String(device->getActiveOutputChannels().countNumberOfSetBits()) );

	/* Prepare filters */
	lastError = String::empty;
	bool filterError = false;
	if (recordingFilter != nullptr)
	{
		recordingFilter->prepareToPlay(sampleRate, bufferSize);
		if (lastError.isNotEmpty()) // filter initialisation failed 
		{
			recordingFilter->releaseResources();
			recordingFilter = nullptr;	
			filterError = true;
		}
		else // copy channel mapping array from AudioSampleRecorder to mchaFilter
		{
			audioSampleRecorder->resetChannelsMap( recordingFilter->getOutputsNumber() );
		}
	}
	if (playbackFilter != nullptr)
	{
		DBG("playbackFilter->prepareToPlay(sampleRate, bufferSize);");
		playbackFilter->prepareToPlay(sampleRate, bufferSize);
		if (lastError.isNotEmpty()) // filter initialisation failed 
		{
			playbackFilter->releaseResources(); // clean up
			playbackFilter = nullptr;	// don't do anything
			filterError = true;
		}
		else // copy mapping array from AudioSamplePlayer to mchaFilter
		{
			playbackFilter->setChannelsMap(audioSamplePlayer->getChannels());
			audioSamplePlayer->resetChannelsMap(playbackFilter->getInputsNumber());
		}
	}

	if (!filterError)
	{
		/* initialise audioSampleProcessor */
		audioSampleProcessor->setInputProcessor(recordingFilter);
		audioSampleProcessor->setOutputProcessor(playbackFilter);
		
		audioSampleProcessor->setRecorder(audioSampleRecorder);
		audioSampleProcessor->setPlayer(audioSamplePlayer);

		/* set output gains */
		audioSamplePlayer->setGain( outChannelsArray, outGainsArray );	

		/* Create debug data logger */
		if ( (debugMode == advanced) && ( audioSamplePlayer->getName() != L"EmptySamplePlayer") )
		{
			String debugPath = fileLogger->getLogFile().getParentDirectory().getFullPathName() + File::separator 
								+ L"debug_" + String(Time::currentTimeMillis());
			
			float duration = static_cast <float> (audioSamplePlayer->getDataLength() / device->getCurrentSampleRate());			
					
			int		debugChanCount =  audioSamplePlayer->getOutputsNumber();
			ScopedPointer<int> debugChannels ( new int [debugChanCount] );
			
			for (int k=0; k<debugChanCount; ++k)
				debugChannels[k] = k;

			debugRecorder = new DiskSampleRecorder(debugPath.toUTF8(), debugChannels, debugChanCount, duration);

			audioSampleProcessor->setDebugRecorder(debugRecorder);
		}


		/* Start processing data */
		audioDeviceManager->addAudioCallback(audioSampleProcessor);

		processIsRunning = true;
		stopProcessing = false;

		return true;
	}
	else
	{
		DBG("Filter Error");
		return false;
	}

}

// ============================================================================================
void MchaRecordPlayer::changeListenerCallback (ChangeBroadcaster *source)
{
	if ( stopProcessing )
	{		
		stopProcessing = false;
		stop();
		dbgOut(" stop processing message received ");
	}
}

// ============================================================================================
bool MchaRecordPlayer::stop()
{
	dbgOut( "mchaRecordPlayer::stop called" );
	dbgOut( "MchaRecordPlayer Thread ID\t= " + String( uint64(Thread::getCurrentThreadId()) ) );

	if (processIsRunning)
	{
		/* stop the process */
		audioDeviceManager->removeAudioCallback(audioSampleProcessor);

		/* close the audio device */
		audioDeviceManager->closeAudioDevice();

		/* stop the timer if still running 
		if ( isTimerRunning() )
			stopTimer(); */
	
		/* release filters' resources */
		if (recordingFilter != nullptr)
		{
			recordingFilter->releaseResources(); 
		}
		if (playbackFilter != nullptr)
		{
			playbackFilter->releaseResources(); 
		}
		Time now = Time::getCurrentTime();
		dbgOut( "mchaRecordPlayer::stop - terminating audioSampleProcessor\t" 
				+ now.toString(false, true, true, true) + ":" + String(now.getMilliseconds()) );
		
		processIsRunning = false;

		clearObjects();

		return true;
	}
	else
	{	
		dbgOut("mchaRecordPlayer::stop - no processes running. Exiting ..");
		return true;
	}
}

// ============================================================================================
void MchaRecordPlayer::dbgOut(const String &message)
{

	if (debugMode == none)
		return;
	else
	{	
		fileLogger->logMessage(message);
	}
}


// ============================================================================================
bool MchaRecordPlayer::init(const char* xmlSettingsFile)
{	
	String tempPath(xmlSettingsFile);

	dbgOut( "mchaRecordPlayer::init('" + tempPath + "')" );
	/* Check if the path is relative */
	if ( !tempPath.containsChar(File::separator) ) 	
	{	
		// add program directory to the path
		//tempPath = File::getSpecialLocation(File::currentApplicationFile).getParentDirectory().getFullPathName() 
		//			+ File::separatorString + String(xmlSettingsFile);
		tempPath = curDirStr + String(xmlSettingsFile);		
		dbgOut( "WARNING: Relative path specified. Expanded to:\t" + tempPath );
	}

	File xmlFile(tempPath);
	if ( !xmlFile.existsAsFile() )
	{
		lastError = "Unable to find settings file:\t" + tempPath;
		dbgOut( lastError );
		return false;
	}
	bool res = audioDeviceSettings->setXmlSettingsFile(tempPath);

	if (!res)
		dbgOut( audioDeviceSettings->getErrorString() );

	return res;
}

// ============================================================================================
bool MchaRecordPlayer::init()
{
	dbgOut( "mchaRecordPlayer::init" );

	clearError();

	ChildProcess initProcess;

	if ( !initProcess.start( "./mchaInit" ) )
	{
		logError(L"Unable to start process mchaInit");
		return false;	
	}

	dbgOut( initProcess.readAllProcessOutput() );

	return true;
}

//==============================================================================
bool	MchaRecordPlayer::setDebugMode(const char* dmStr)
{ 
	debugModeString = String(dmStr);
	
	dbgOut( L"mchaRecordPlayer::setDebugMode(\"" + debugModeString + L"\")" );
	
	if (debugModeString == "advanced")
	{
		debugMode = advanced;	
	}
	else if (debugModeString == "normal")
	{
		debugMode = normal;		 
	}
	else if (debugModeString == "none")
	{
		debugMode = none;
	}
	else
	{
		logError( L"\tWrong input. Debug mode has not been changed" );
		return false;
	}
	
	dbgOut(L"\tDebug mode set to:\t" + debugModeString);
	return true;

}; 

//==============================================================================
bool	MchaRecordPlayer::setMemoryMode(const char* mMode)
{ 
	memModeString = String(mMode);

	
	dbgOut( L"mchaRecordPlayer::setMemoryMode(\"" + memModeString + L"\")" );
	
	if (memModeString == "safe")
	{
		memMode = safe;	
	}
	else if (memModeString == "smart")
	{
		memMode = smart;		 
	}
	else
	{
		logError(L"\tWrong input. Memory mode has not been changed");
		return false;
	}
	
	dbgOut(L"\tMemory mode set to:\t" + memModeString);
	return true;
}


//==============================================================================
template <class D, class S> bool MchaRecordPlayer::playRecord(D** recordData, const int recordChannelCount, const float recordSamplesNumber, const int *recDeviceChannels, const int recDeviceChannelsCount,
																const S** playbackData, const int playbackDataChannels, const size_t playbackSamplesNumber, const int* devicePlayChannels, const int devicePlayChannelsCount)
{
	dbgOut("");
	dbgOut("***  mchaRecordPlayer::playRecord  ***");

	lastError = String::empty;

	if ( isRunning() )
	{
		logError( L"mchaRecordPlayer::playRecord failed. Another task is running."); 
		return false;
	}

	/* check channel arrays - recording */
	if (!updateChannels(recDeviceChannels, recDeviceChannelsCount, false))
	{
		return false;
	}

	/* check channel arrays - playback */
	if (!updateChannels(devicePlayChannels, devicePlayChannelsCount, true))
	{
		return false;
	}

	/* update record channels if filters were added */
	releaseBuffer(recChanArray);

	if (recordingFilter != nullptr)
	{
		recChanCount = recordingFilter->getOutputsNumber();
		/* set channels to 0..(recChanCount-1) as this will be updated later */
		if ( allocateBuffer(recChanArray, recChanCount, true, L"recChanCount") )
		{		
			for (int ch=0; ch<recChanCount; ++ch)
				recChanArray[ch] = ch;
		}
		else
		{	
			return false;
		}
	}
	else
	{
		recChanCount = recDeviceChannelsCount;
		if ( allocateBuffer(recChanArray, recChanCount, false, L"recChanCount") )
		{
			memcpy(recChanArray, recDeviceChannels, recChanCount*sizeof(int));
		}
		else
		{	
			return false;
		}
	}

	/* create and initialise audioSampleRecorder object */
	if ( !createRecorder(recordData, recordChannelCount, recordSamplesNumber, recChanArray, recChanCount) )
	{
		logError( L"mchaRecordPlayer::playRecord failed:\tUnable to create recorder object.");
		return false;
	}
	else
	{
		/* save device channels in the recorder object */
		Array<int> *tmpChanArray = audioSampleRecorder->getDeviceChannelsMap();
		tmpChanArray->insertArray(0, recDeviceChannels, recDeviceChannelsCount);
	}

	/* no need to modify playback channels as their number is not changed. 
	   The channels will be updated by start()  */
	
	/* create and initialise audioSamplePlayer object */
	if ( !createPlayer(playbackData, playbackDataChannels, playbackSamplesNumber, devicePlayChannels, devicePlayChannelsCount) )
	{
		logError( L"mchaRecordPlayer::playRecord failed:\tUnable to create player object.");
		return false;
	}

	/* delete the previous instance of audioSampleProcessor */
	if (audioSampleProcessor != NULL)
		delete audioSampleProcessor;
	
	/* create a new instance and initialise it */
	audioSampleProcessor = new AudioSampleProcessor();

	if ( !start() )
	{
		clearObjects();
		return false;
	}
	else
	{
		return true;
	}
}

//==============================================================================
template <> bool MchaRecordPlayer::createRecorder(const char** recordData, const int /*recordChannelCount*/, const float recordSamplesNumber, const int *recDeviceChannels, const int recDeviceChannelsCount)
{

	if (recordData != NULL)
	{
		audioSampleRecorder = new DiskSampleRecorder( *recordData, recDeviceChannels, recDeviceChannelsCount, recordSamplesNumber); 
		if ( lastError.isNotEmpty() )
		{
			delete audioSampleRecorder;
			audioSampleRecorder = NULL;
			return false;
		}
	}
	else
	{		
		audioSampleRecorder = new EmptySampleRecorder;
	}
	
	return true;
}

//==============================================================================
template <> bool MchaRecordPlayer::createRecorder(float** recordData, const int recordChannelCount, const float recordSamplesNumber, const int *recDeviceChannels, const int recDeviceChannelsCount)
{
	 
	if ((memMode == safe) && (recordChannelCount > 0 )) /* recording into internal memory */
	{
		/* ignore recordData. Create buffer for recorded data */
		releaseBuffer(recordBuffer, recordChannels); // release the old buffer (do nothing if NULL)
		size_t	durationInSamples = static_cast <size_t> ( recordSamplesNumber );
		if (!allocateBuffer(recordBuffer, recordChannelCount, durationInSamples, true, L"recordBuffer"))
			return false;

		recordChannels = recordChannelCount;
		recordSamplesNum = durationInSamples;

		/* create recorder using internal record buffer */
		audioSampleRecorder = new MemorySampleRecorder ( recordBuffer, recordChannelCount, durationInSamples, recDeviceChannels, recDeviceChannelsCount );
		if ( lastError.isNotEmpty() )
		{
			delete audioSampleRecorder;
			audioSampleRecorder = NULL;
			releaseBuffer(recordBuffer, recordChannels);
			return false;
		}

	}
	else if (recordData != NULL)
	{
		/* recording into user-supplied buffer */
		size_t	durationInSamples = static_cast <size_t> ( recordSamplesNumber*audioDeviceSettings->getBufferSize() );
		audioSampleRecorder = new MemorySampleRecorder ( recordData, recordChannelCount, durationInSamples, recDeviceChannels, recDeviceChannelsCount );
		if ( lastError.isNotEmpty() )
		{
			delete audioSampleRecorder;
			audioSampleRecorder = NULL;
			return false;
		}
	}
	else
	{	
		/* not recording */
		audioSampleRecorder = new EmptySampleRecorder;
	}
	
	return true;
}

//==============================================================================
template <> bool MchaRecordPlayer::createRecorder(double** recordData, const int recordChannelCount, const float recordSamplesNumber, const int *recDeviceChannels, const int recDeviceChannelsCount)
{
	if ((memMode == smart) && (recordData != NULL))
	{
		dbgOut(L"\tWARNING: The buffer for recording data in 'smart' mode should be of type float.");
		dbgOut(L"\tMemory mode has been changed to 'safe'.");
		dbgOut(L"\tInternal memory will be used for recording. Use getData to get access to the recorded data.");		
		memMode = safe;
	}
		
	if ((memMode == safe) && (recordChannelCount > 0 )) /* recording into internal memory */
	{
		/* create buffer for recorded data */
		releaseBuffer(recordBuffer, recordChannels); // release the old buffer (do nothing if NULL)
		size_t	durationInSamples = static_cast <size_t> ( recordSamplesNumber );
		if (!allocateBuffer(recordBuffer, recordChannels, durationInSamples, true, L"recordBuffer"))
			return false;

		recordChannels = recordChannelCount;
		recordSamplesNum = durationInSamples;

		/* create recorder using internal record buffer */
		audioSampleRecorder = new MemorySampleRecorder ( recordBuffer, recordChannelCount, durationInSamples, recDeviceChannels, recDeviceChannelsCount );
		if ( lastError.isNotEmpty() )
		{
			delete audioSampleRecorder;
			audioSampleRecorder = NULL;
			releaseBuffer(recordBuffer, recordChannels);
			return false;
		}

	}
	else
	{	
		/* not recording */
		audioSampleRecorder = new EmptySampleRecorder;
	}
	
	return true;
}



//==============================================================================
template <> bool MchaRecordPlayer::createPlayer(const char** playbackData, const int playbackDataChannels, const size_t /*playbackSamplesNumber*/, const int* devicePlayChannels, const int devicePlayChannelsCount)
{
	if (playbackData != NULL)
	{
		audioSamplePlayer	= new DiskSamplePlayer( playbackData, playbackDataChannels, devicePlayChannels, devicePlayChannelsCount );
		if ( lastError.isNotEmpty() )
		{
			delete audioSamplePlayer;
			audioSamplePlayer = NULL;
			return false;
		}	
	}
	else
	{		
		audioSamplePlayer	= new EmptySamplePlayer;
	}
	return true;
}

//==============================================================================
template <class D> bool	 MchaRecordPlayer::createPlayer(const D** playbackData, const int playbackDataChannels, const size_t playbackSamplesNumber, const int* devicePlayChannels, const int devicePlayChannelsCount)
{
	if ((memMode == safe) && (playbackDataChannels > 0 )) /* playback from internal memory */
	{
		/* create buffer for playback data and copy data from user memory */
		if ( !copyPlaybackData( playbackData, playbackDataChannels, playbackSamplesNumber) )
			return false;

		playbackChannels = playbackDataChannels;
		playbackSamplesNum = playbackSamplesNumber;

		/* create recorder using internal playback buffer */
		audioSamplePlayer = new MemorySamplePlayer( const_cast<const float**> (playbackBuffer), playbackDataChannels, playbackSamplesNumber, devicePlayChannels, devicePlayChannelsCount );
		if ( lastError.isNotEmpty() )
		{
			delete audioSamplePlayer;
			audioSamplePlayer = NULL;
			releaseBuffer(playbackBuffer, playbackChannels);
			return false;
		}
	}
	else if (playbackData != NULL)
	{
		/* playback from user-supplied buffer */
		audioSamplePlayer = new MemorySamplePlayer( playbackData, playbackDataChannels, playbackSamplesNumber, devicePlayChannels, devicePlayChannelsCount );
		if ( lastError.isNotEmpty() )
		{
			delete audioSamplePlayer;
			audioSamplePlayer = NULL;
			return false;
		}
	}
	else
	{
		/* recording only */
		audioSamplePlayer = new EmptySamplePlayer;
	}
	
	return true;

}

//==============================================================================
template <> bool	 MchaRecordPlayer::createPlayer(const double** playbackData, const int playbackDataChannels, const size_t playbackSamplesNumber, const int* devicePlayChannels, const int devicePlayChannelsCount)
{
	/* can't play data of type double from user buffer */
	if ((memMode == smart) && (playbackData != NULL))
	{
		dbgOut(L"\tWARNING: Playback data provided in 'smart' mode should be of type float.");
		dbgOut(L"\tMemory mode has been changed to 'safe'.");
		memMode = safe;
	}

	if ((memMode == safe) && (playbackDataChannels > 0 )) /* playback from internal memory */
	{
		/* create buffer for playback data and copy data from user memory */
		if ( !copyPlaybackData( playbackData, playbackDataChannels, playbackSamplesNumber) )
			return false;

		playbackChannels = playbackDataChannels;
		playbackSamplesNum = playbackSamplesNumber;

		/* create recorder using internal playback buffer */
		audioSamplePlayer = new MemorySamplePlayer( const_cast<const float**> (playbackBuffer), playbackDataChannels, playbackSamplesNumber, devicePlayChannels, devicePlayChannelsCount );
		if ( lastError.isNotEmpty() )
		{
			delete audioSamplePlayer;
			audioSamplePlayer = NULL;
			releaseBuffer(playbackBuffer, playbackChannels);
			return false;
		}
	}
	else
	{
		/* recording only */
		audioSamplePlayer = new EmptySamplePlayer;
	}
	
	return true;

}

//==============================================================================
bool MchaRecordPlayer::updateChannels(  const int* deviceChannels, const int deviceChannelsCount, bool isPlaybackDevice )
{
	if (deviceChannels == NULL)
		return true;
	
	double samplRate;
	int bufSize, bDepth, inChannels, outChannels;
	if	(!audioDeviceSettings->getSettings(samplRate,bufSize,bDepth, inChannels,outChannels))
	{
		logError("updateChannels: Unable to get device settings.");
		return false;
	}

	int maxChannel = (isPlaybackDevice) ? inChannels : outChannels ;

	for ( int ch=0; ch<deviceChannelsCount; ++ch)
	{
		if ((deviceChannels[ch] < 0) || (deviceChannels[ch] >= maxChannel))
		{
			logError( L"updateChannels: wrong channel number \t\t channel[" + String(ch) + L"] = " + String(deviceChannels[ch]) );			
			return false;
		}
	}


	if (isPlaybackDevice)
	{
		/* Check the number of input/output channels - Player */
		int		outputAudioDeviceChannels = audioDeviceSettings->getOutputChannelsStr().length();
		bool	channelsOK = false;
		String	processorStr1 = String::empty;
		String	processorStr2 = String::empty; 
	
		if (playbackFilter != NULL)
		{
			int filterIn = playbackFilter->getInputsNumber();
			int filterOut = playbackFilter->getOutputsNumber();
			channelsOK = ( outputAudioDeviceChannels >= filterOut ) && ( filterIn == deviceChannelsCount );
			processorStr1 =  L"\tFilter inputs:\t" + String(filterIn);
			processorStr2 = L"\tFilter outputs:\t" + String(filterOut);
		}
		else
		{
			channelsOK = ( outputAudioDeviceChannels >= deviceChannelsCount );
		}
	
		if (!channelsOK)
		{
			logError( L"Wrong number of output channels" );

			dbgOut( L"\tAudio Device Output Channels:\t" + String(outputAudioDeviceChannels));
			if (processorStr1.isNotEmpty())
			{
				dbgOut(processorStr1);
				dbgOut(processorStr2);
			}
			dbgOut( L"\tPlayer Data Channels:\t" + String(deviceChannelsCount));
			return false;
		}
	}
	else 	/* Check the number of input/output channels - Recorder */
	{
		int		inputAudioDeviceChannels = audioDeviceSettings->getInputChannelsStr().length();
		bool	channelsOK = false;
		String	processorStr1(String::empty), processorStr2(String::empty); 
		if (recordingFilter != NULL)
		{
			int filterIn = recordingFilter->getInputsNumber();
			int filterOut = recordingFilter->getOutputsNumber();
			channelsOK = ( inputAudioDeviceChannels >= filterIn );
			processorStr1 =  L"\tFilter inputs:\t" + String(filterIn);
			processorStr2 = L"\tFilter outputs:\t" + String(filterOut);
		}
		else
		{
			channelsOK = ( inputAudioDeviceChannels >= deviceChannelsCount );
		}
	
		if (!channelsOK)
		{
			logError( L"Wrong number of input channels" );

			dbgOut( L"\tAudio Device Input Channels:\t" + String(inputAudioDeviceChannels));
			if (processorStr1.isNotEmpty())
			{
				dbgOut(processorStr1);
				dbgOut(processorStr2);
			}
			dbgOut( L"\tRecorder Data Channels:\t" + String(deviceChannelsCount));
			return false;
		}
	}

	return true;
}


//==============================================================================
void	MchaRecordPlayer::debugBuffer(float** buf, size_t bufferCount, size_t singleBufferSize, String bufferVariableName )
{
	String s = String::empty;
	for (size_t ch = 0; ch<bufferCount; ++ch)
	{
		s = String::empty;
		for (size_t ind =0; ind < singleBufferSize; ind++)
			s = s+ String::formatted( "%4.3e\t", *(buf[ch] + ind) );
		dbgOut(  bufferVariableName + "(:," + String( static_cast<uint64>(ch+1) ) +  ") = [" + s + "];" );
	}
}

//==============================================================================
void	MchaRecordPlayer::debugBuffer(float* buf, size_t bufferSize, String bufferVariableName )
{
	String s = String::empty;

	for (size_t ind =0; ind < bufferSize; ++ind)
		s = s+ String::formatted( "%4.3e\t", buf[ind] );

	dbgOut(  bufferVariableName + " = [" + s + "];" );
}

//==============================================================================
void	MchaRecordPlayer::debugBuffer(bool* buf, size_t bufferSize, String bufferVariableName )
{
	String s = String::empty;

	for (size_t ind =0; ind < bufferSize; ++ind)
	{
		if (buf[ind])
			s += L"1";
		else
			s += L"0"; 
	}

	dbgOut(  bufferVariableName + " = [" + s + "];" );
}

//==============================================================================
void	MchaRecordPlayer::debugBuffer(fftwf_complex* buf, size_t bufferSize, String bufferVariableName )
{
	String s = String::empty;

	for (size_t ind =0; ind < bufferSize; ++ind)
		s = s+ String::formatted( "%4.3e%+4.3ei\t", buf[ind][0],  buf[ind][1]  );

	dbgOut(  bufferVariableName + " = [" + s + "];" );

}

//==================================================================================
void MchaRecordPlayer::printSystemInfo()
{

		String systemInfo;
		systemInfo << "Operating system:\t" << SystemStats::getOperatingSystemName();
		if ( SystemStats::isOperatingSystem64Bit() )
			systemInfo << " 64 bit";
		else
			systemInfo << " 32 bit";
		dbgOut( systemInfo ); systemInfo = String::empty;

		dbgOut( "CPU vendor:\t\t" + SystemStats::getCpuVendor() );
		dbgOut( "CPU speed:\t\t" + String(SystemStats::getCpuSpeedInMegaherz()) + " MHz" ); 

		systemInfo<< "Number of CPUs:\t\t" << SystemStats::getNumCpus();
		dbgOut( systemInfo ); systemInfo = String::empty;

		systemInfo<< "Memory size:\t\t" << SystemStats::getMemorySizeInMegabytes() << " Mb";
		dbgOut( systemInfo ); systemInfo = String::empty;

		systemInfo<< "mcha version:\t" + String(MCHA_VERSION_NUMBER);
		dbgOut( systemInfo ); 

		dbgOut( "JUCE version:\t" + SystemStats::getJUCEVersion() );
		dbgOut( String::empty );
}

//==================================================================================
// Now we need to tell the compiler what blends of playRecord it needs to generate
//==================================================================================

template bool MchaRecordPlayer::playRecord<float, float>(float** recordData, const int recordChannelCount, const float recordSamplesNumber, const int *recDeviceChannels, const int recDeviceChannelsCount,
																const float** playbackData, const int playbackDataChannels, const size_t playbackSamplesNumber, const int* devicePlayChannels, const int devicePlayChannelsCount);

template bool MchaRecordPlayer::playRecord<float, double>(float** recordData, const int recordChannelCount, const float recordSamplesNumber, const int *recDeviceChannels, const int recDeviceChannelsCount,
																const double** playbackData, const int playbackDataChannels, const size_t playbackSamplesNumber, const int* devicePlayChannels, const int devicePlayChannelsCount);

template bool MchaRecordPlayer::playRecord<const char, float>(const char** recordData, const int recordChannelCount, const float recordSamplesNumber, const int *recDeviceChannels, const int recDeviceChannelsCount,
																const float** playbackData, const int playbackDataChannels, const size_t playbackSamplesNumber, const int* devicePlayChannels, const int devicePlayChannelsCount);

template bool MchaRecordPlayer::playRecord<const char, double>(const char** recordData, const int recordChannelCount, const float recordSamplesNumber, const int *recDeviceChannels, const int recDeviceChannelsCount,
																const double** playbackData, const int playbackDataChannels, const size_t playbackSamplesNumber, const int* devicePlayChannels, const int devicePlayChannelsCount);

template bool MchaRecordPlayer::playRecord<float, char>(float** recordData, const int recordChannelCount, const float recordSamplesNumber, const int *recDeviceChannels, const int recDeviceChannelsCount,
																const char** playbackData, const int playbackDataChannels, const size_t playbackSamplesNumber, const int* devicePlayChannels, const int devicePlayChannelsCount);

template bool MchaRecordPlayer::playRecord<const char, char>(const char** recordData, const int recordChannelCount, const float recordSamplesNumber, const int *recDeviceChannels, const int recDeviceChannelsCount,
																const char** playbackData, const int playbackDataChannels, const size_t playbackSamplesNumber, const int* devicePlayChannels, const int devicePlayChannelsCount);

template bool MchaRecordPlayer::getData<float> (float**	dat,  const int* channels, const int channelsNumber, size_t startSample, size_t endSample);

template bool MchaRecordPlayer::getData<double> (double** dat,  const int* channels, const int channelsNumber, size_t startSample, size_t endSample); 

template bool MchaRecordPlayer::allocateBuffer<fftwf_complex> (fftwf_complex** &buf, size_t bufferCount, size_t singleBufferSize, bool shouldBeCleared, String bufferVariableName );

template bool MchaRecordPlayer::allocateBuffer<float>(float* &buf, size_t singleBufferSize, bool shouldBeCleared, String bufferVariableName );

}
