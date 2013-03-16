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
   #include "stdafx.h"
#endif

#include "../JuceLibraryCode/JuceHeader.h"
#include "settings.h"
#include "mchaRecordPlayer.h"
#include "AudioFileConverter.h"

#if JUCE_WINDOWS
	#define MCHAEXPORT extern "C" __declspec(dllexport) 
#else
	#define MCHAEXPORT extern "C" 
#endif

#if JUCE_LINUX
	#include "LinuxMessageThread.h"
#endif


using namespace mcha;

// -----------------------------------------------------------------

MCHAEXPORT bool initAudioDevice()
{
	return MchaRecordPlayer::getInstance()->init();;	
}

MCHAEXPORT bool initAudioDeviceFile(const char* xmlSettingsFile)
{
	return MchaRecordPlayer::getInstance()->init(xmlSettingsFile);
}

MCHAEXPORT	bool setDebugMode(const char* dmStr)
{
	return MchaRecordPlayer::getInstance()->setDebugMode(dmStr);
}

MCHAEXPORT	bool setMemoryMode(const char* mMode)
{
	return MchaRecordPlayer::getInstance()->setMemoryMode(mMode);
}

MCHAEXPORT const char* getLastError()
{
	return MchaRecordPlayer::getInstance()->getLastError();
}

MCHAEXPORT const char* getVersion()
{
	return MCHA_VERSION_NUMBER;
}

MCHAEXPORT bool addFilter(const char* settingsFileName, bool isRecordFilter)
{
	return MchaRecordPlayer::getInstance()->addFilter(settingsFileName, isRecordFilter);
}

MCHAEXPORT bool playFiles (const char**	 audioFiles, const int filesNumber, const int *channels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord((const char **) NULL, 0, 0.0f, NULL, 0,  audioFiles, filesNumber, 0, channels, channelCount);
}


MCHAEXPORT bool playData_s (const float** audioData, const int chanNumber, const size_t samplesNumber, const int *channels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord((const char **) NULL, 0, 0.0f, NULL, 0, audioData, chanNumber, samplesNumber, channels, channelCount);
}


MCHAEXPORT bool playData_d (const double** audioData, const int chanNumber, const size_t samplesNumber, const int *channels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord((const char **) NULL, 0, 0.0f, NULL, 0, audioData, chanNumber, samplesNumber, channels, channelCount);
}


MCHAEXPORT bool recordFiles	( const char*	recordDir, const int inputChanNumber, const float duration, const int *channels, const int channelCount )
{
	return MchaRecordPlayer::getInstance()->playRecord(&recordDir, inputChanNumber, duration, channels, channelCount,(const float**) NULL, 0, 0, NULL, 0);
}


MCHAEXPORT bool recordData	( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord(inputData,	inputChanNumber,	(const float) inputSamplesNumber,	 inChannels, channelCount, (const float**)NULL, 0, 0, NULL, 0);
}

/* memory -> memory */
MCHAEXPORT	bool playRecordDataMM_s( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const float** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount )
{
	return MchaRecordPlayer::getInstance()->playRecord(	inputData,	inputChanNumber,	(const float) inputSamplesNumber,	 inChannels, inChannelsCount,
															outputData,	outputChanNumber,	outputSamplesNumber, outChannels, outChannelsCount);

}

/* memory double -> memory  */
MCHAEXPORT	bool playRecordDataMM_d( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const double** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount )
{
	return MchaRecordPlayer::getInstance()->playRecord(	inputData,	inputChanNumber,	(const float) inputSamplesNumber,	 inChannels, inChannelsCount,
															outputData,	outputChanNumber,	outputSamplesNumber, outChannels, outChannelsCount);

}
/* memory -> disk	*/
MCHAEXPORT	bool playRecordDataMD_s (   const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
								const float** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount)
{
	return 	MchaRecordPlayer::getInstance()->playRecord(  &recordDir, inputChanNumber, duration, inChannels, inChannelsCount, 
															outputData, outputChanNumber,  outputSamplesNumber, outChannels, outChannelsCount);
}

/* memory double -> disk	*/
MCHAEXPORT	bool playRecordDataMD_d (   const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
								const double** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount)
{
	return 	MchaRecordPlayer::getInstance()->playRecord(  &recordDir, inputChanNumber, duration, inChannels, inChannelsCount, 
															outputData, outputChanNumber,  outputSamplesNumber, outChannels, outChannelsCount);
}

/* disk -> memory */
MCHAEXPORT	bool playRecordDataDM ( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount )
{
	return 	MchaRecordPlayer::getInstance()->playRecord (	inputData, inputChanNumber, (const float) inputSamplesNumber, inChannels, inChannelsCount,
															audioFiles, filesNumber, 0, channels, outChannelsCount );
}

/* disk -> disk */
MCHAEXPORT	bool playRecordDataDD ( const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount )
{
	return	MchaRecordPlayer::getInstance()->playRecord ( &recordDir, inputChanNumber, duration, inChannels, inChannelsCount, 
															audioFiles, filesNumber, 0,  channels, outChannelsCount);
}


MCHAEXPORT	int		getRecordedChannelsNum() 
{	
	return 	MchaRecordPlayer::getInstance()->getRecordedChannelsNum(); 
};


MCHAEXPORT size_t	getRecordedSamplesNum()  
{	
	return MchaRecordPlayer::getInstance()->getRecordedSamplesNum(); 
};


MCHAEXPORT bool	getData_s(float**	dat,  const int* channels = NULL, const int channelsNumber = 0, size_t startSample = 0, size_t endSample = 0) 
{
	return	MchaRecordPlayer::getInstance()->getData(dat, channels, channelsNumber, startSample, endSample);
}

MCHAEXPORT bool	getData_d(double**	dat,  const int* channels = NULL, const int channelsNumber = 0, size_t startSample = 0, size_t endSample = 0) 
{
	return	MchaRecordPlayer::getInstance()->getData(dat, channels, channelsNumber, startSample, endSample);
}


MCHAEXPORT 	bool setGain (int *channels, int channelsCount, float *gains)
{
	return MchaRecordPlayer::getInstance()->setGain (channels, channelsCount, gains);
}

MCHAEXPORT bool stopAudio()
{
	return MchaRecordPlayer::getInstance()->stop();
}

MCHAEXPORT bool isRunning()
{
	return MchaRecordPlayer::getInstance()->isRunning();
}

MCHAEXPORT double getCurrentPosition()
{ 
	return MchaRecordPlayer::getInstance()->getCurrentPosition();
}

MCHAEXPORT bool setPosition(double timeInSeconds)
{ 
	return MchaRecordPlayer::getInstance()->setPosition(timeInSeconds);
}

MCHAEXPORT bool getDeviceSettings(double& samplingRate, int& bufSize, int& bDepth, int& inChannels, int& outChannels)
{
	return MchaRecordPlayer::getInstance()->getDeviceSettings().getSettings( samplingRate, bufSize, bDepth, inChannels, outChannels );
}

MCHAEXPORT void getFilterSettings(bool isInputFilter, int& inputsCount, int& outputsCount)
{
	MchaRecordPlayer::getInstance()->getFilterSettings(isInputFilter, inputsCount, outputsCount);
}

MCHAEXPORT void logMessage(const char* msg)
{
	MchaRecordPlayer::getInstance()->dbgOut(String(msg));
}

MCHAEXPORT void	logError(const char* msg)
{
	MchaRecordPlayer::getInstance()->logError(String(msg));
}

MCHAEXPORT void logDouble(const char* infoStr, const double value)
{
	MchaRecordPlayer::getInstance()->dbgOut(String(infoStr) + String(value));
}

MCHAEXPORT void logAddress(const char* infoStr, const int64 addr)
{
	MchaRecordPlayer::getInstance()->dbgOut(String(infoStr) + String::toHexString(addr));
}


/* ------------------------------------------------------------------------------ */
void onMchaLoad()
{
	DBG("Loading MCHA library ...");

	#if JUCE_LINUX
		LinuxMessageThread::getInstance(); // this will create the message thread and initialise GUI on Linux
	#endif
}

/* ------------------------------------------------------------------------------ */
void onMchaUnload()
{
	DBG("Unloading MCHA library ...");	
	
	MchaRecordPlayer* mchaRecordPlayer = MchaRecordPlayer::getInstanceWithoutCreating();
	if ( mchaRecordPlayer != nullptr )
	{	
		MchaRecordPlayer::deleteInstance();
	}

	AudioFileConverter* conv = AudioFileConverter::getInstanceWithoutCreating();
	if ( conv != nullptr )
	{		
		conv->waitForThreadToExit(-1); // wait forever
		conv->deleteInstance();
	}

	// kill the message thread
	#if JUCE_LINUX
		LinuxMessageThread* linuxMessageThread = LinuxMessageThread::getInstanceWithoutCreating();
		if ( linuxMessageThread != nullptr )
		{	
			LinuxMessageThread::deleteInstance();
		}
	#endif
}

/* ------------------------------------------------------------------------------ */
#if JUCE_WINDOWS
	BOOL WINAPI DllMain (HINSTANCE instance, DWORD dwReason, LPVOID)
	{
		if (dwReason == DLL_PROCESS_ATTACH)
		{
			Process::setCurrentModuleInstanceHandle (instance);
			
			// Prevent library from being unloaded when the MEX function that called it terminates
			const size_t moduleNameSize = 1024;
			TCHAR moduleName[moduleNameSize];
			
			/* get the full qualified name to the DLL */
			if ( 0 != GetModuleFileName( instance, moduleName, moduleNameSize ) )
			{
				/* make the library stay in memory */
				HMODULE module;
				GetModuleHandleEx( GET_MODULE_HANDLE_EX_FLAG_PIN, moduleName, &module );
			} 
			
			/* call platform-independent startup code */
			onMchaLoad();
		}
		else if (dwReason == DLL_PROCESS_DETACH)
		{
			onMchaUnload();
		}
		return TRUE;
	}
#endif

/* ------------------------------------------------------------------------------ */
#if JUCE_LINUX
	void __attribute__ ((constructor))	onMchaLoad(void);
	void __attribute__ ((destructor))	onMchaUnload(void);
#endif
