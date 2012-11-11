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

using namespace mcha;

#if JUCE_WINDOWS
	#define MCHA_DLL_EXPORT __declspec(dllexport)
#else
	#define MCHA_DLL_EXPORT 
#endif



extern "C" { // beginning of extern "C" section

MCHA_DLL_EXPORT bool initAudioDevice()
{
	return MchaRecordPlayer::getInstance()->init();
}

MCHA_DLL_EXPORT bool initAudioDeviceFile(const char* xmlSettingsFile)
{
	return MchaRecordPlayer::getInstance()->init(xmlSettingsFile);
}

MCHA_DLL_EXPORT bool setDebugMode(const char* dmStr)
{
	return MchaRecordPlayer::getInstance()->setDebugMode(dmStr);
}

MCHA_DLL_EXPORT bool setMemoryMode(const char* mMode)
{
	return MchaRecordPlayer::getInstance()->setMemoryMode(mMode);
}

MCHA_DLL_EXPORT const char* getLastError()
{
	return MchaRecordPlayer::getInstance()->getLastError();
}

MCHA_DLL_EXPORT const char* getVersion()
{
	DBG( String::toHexString( (int64) MchaRecordPlayer::getInstance() ) );	
	return MCHA_VERSION_NUMBER;
}

MCHA_DLL_EXPORT bool addFilter(const char* settingsFileName, bool isRecordFilter)
{
	return MchaRecordPlayer::getInstance()->addFilter(settingsFileName, isRecordFilter);
}

MCHA_DLL_EXPORT bool playFiles (const char**	 audioFiles, const int filesNumber, const int *channels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord((const char **) NULL, 0, 0.0f, NULL, 0,  audioFiles, filesNumber, 0, channels, channelCount);
}


MCHA_DLL_EXPORT bool playData_s (const float** audioData, const int chanNumber, const size_t samplesNumber, const int *channels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord((const char **) NULL, 0, 0.0f, NULL, 0, audioData, chanNumber, samplesNumber, channels, channelCount);
}


MCHA_DLL_EXPORT bool playData_d (const double** audioData, const int chanNumber, const size_t samplesNumber, const int *channels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord((const char **) NULL, 0, 0.0f, NULL, 0, audioData, chanNumber, samplesNumber, channels, channelCount);
}


MCHA_DLL_EXPORT bool recordFiles	( const char*	recordDir, const int inputChanNumber, const float duration, const int *channels, const int channelCount )
{
	return MchaRecordPlayer::getInstance()->playRecord(&recordDir, inputChanNumber, duration, channels, channelCount,(const float**) NULL, 0, 0, NULL, 0);
}


MCHA_DLL_EXPORT bool recordData	( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord(inputData,	inputChanNumber,	(const float) inputSamplesNumber,	 inChannels, channelCount, (const float**)NULL, 0, 0, NULL, 0);
}

/* memory -> memory */
MCHA_DLL_EXPORT bool playRecordDataMM_s( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const float** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount )
{
	return MchaRecordPlayer::getInstance()->playRecord(	inputData,	inputChanNumber,	(const float) inputSamplesNumber,	 inChannels, inChannelsCount,
															outputData,	outputChanNumber,	outputSamplesNumber, outChannels, outChannelsCount);

}

/* memory double -> memory  */
MCHA_DLL_EXPORT bool playRecordDataMM_d( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const double** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount )
{
	return MchaRecordPlayer::getInstance()->playRecord(	inputData,	inputChanNumber,	(const float) inputSamplesNumber,	 inChannels, inChannelsCount,
															outputData,	outputChanNumber,	outputSamplesNumber, outChannels, outChannelsCount);

}
/* memory -> disk	*/
MCHA_DLL_EXPORT bool playRecordDataMD_s (   const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
								const float** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount)
{
	return 	MchaRecordPlayer::getInstance()->playRecord(  &recordDir, inputChanNumber, duration, inChannels, inChannelsCount, 
															outputData, outputChanNumber,  outputSamplesNumber, outChannels, outChannelsCount);
}

/* memory double -> disk	*/
MCHA_DLL_EXPORT bool playRecordDataMD_d (   const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
								const double** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount)
{
	return 	MchaRecordPlayer::getInstance()->playRecord(  &recordDir, inputChanNumber, duration, inChannels, inChannelsCount, 
															outputData, outputChanNumber,  outputSamplesNumber, outChannels, outChannelsCount);
}

/* disk -> memory */
MCHA_DLL_EXPORT bool playRecordDataDM ( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount )
{
	return 	MchaRecordPlayer::getInstance()->playRecord (	inputData, inputChanNumber, (const float) inputSamplesNumber, inChannels, inChannelsCount,
															audioFiles, filesNumber, 0, channels, outChannelsCount );
}

/* disk -> disk */
MCHA_DLL_EXPORT bool playRecordDataDD ( const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount )
{
	return	MchaRecordPlayer::getInstance()->playRecord ( &recordDir, inputChanNumber, duration, inChannels, inChannelsCount, 
															audioFiles, filesNumber, 0,  channels, outChannelsCount);
}


MCHA_DLL_EXPORT int getRecordedChannelsNum() 
{	
	return 	MchaRecordPlayer::getInstance()->getRecordedChannelsNum(); 
};


MCHA_DLL_EXPORT size_t	getRecordedSamplesNum()  
{	
	return MchaRecordPlayer::getInstance()->getRecordedSamplesNum(); 
};


MCHA_DLL_EXPORT bool	getData_s(float**	dat,  const int* channels = NULL, const int channelsNumber = 0, size_t startSample = 0, size_t endSample = 0) 
{
	return	MchaRecordPlayer::getInstance()->getData(dat, channels, channelsNumber, startSample, endSample);
}

MCHA_DLL_EXPORT bool	getData_d(double**	dat,  const int* channels = NULL, const int channelsNumber = 0, size_t startSample = 0, size_t endSample = 0) 
{
	return	MchaRecordPlayer::getInstance()->getData(dat, channels, channelsNumber, startSample, endSample);
}


MCHA_DLL_EXPORT bool setGain (int *channels, int channelsCount, float *gains)
{
	return MchaRecordPlayer::getInstance()->setGain (channels, channelsCount, gains);
}

MCHA_DLL_EXPORT bool stopAudio()
{
	return MchaRecordPlayer::getInstance()->stop();
}

MCHA_DLL_EXPORT bool isRunning()
{
	return MchaRecordPlayer::getInstance()->isRunning();
}

MCHA_DLL_EXPORT double getCurrentPosition()
{ 
	return MchaRecordPlayer::getInstance()->getCurrentPosition();
}

MCHA_DLL_EXPORT bool setPosition(double timeInSeconds)
{ 
	return MchaRecordPlayer::getInstance()->setPosition(timeInSeconds);
}

MCHA_DLL_EXPORT bool getDeviceSettings(double& samplingRate, int& bufSize, int& bDepth, int& inChannels, int& outChannels)
{
	return MchaRecordPlayer::getInstance()->getDeviceSettings().getSettings( samplingRate, bufSize, bDepth, inChannels, outChannels );
}

MCHA_DLL_EXPORT void getFilterSettings(bool isInputFilter, int& inputsCount, int& outputsCount)
{
	MchaRecordPlayer::getInstance()->getFilterSettings(isInputFilter, inputsCount, outputsCount);
}

MCHA_DLL_EXPORT void logMessage(const char* msg)
{
	MchaRecordPlayer::getInstance()->dbgOut(String(msg));
}

MCHA_DLL_EXPORT void	logError(const char* msg)
{
	MchaRecordPlayer::getInstance()->logError(String(msg));
}

MCHA_DLL_EXPORT void logDouble(const char* infoStr, const double value)
{
	MchaRecordPlayer::getInstance()->dbgOut(String(infoStr) + String(value));
}

MCHA_DLL_EXPORT void logAddress(const char* infoStr, const int64 addr)
{
	MchaRecordPlayer::getInstance()->dbgOut(String(infoStr) + String::toHexString(addr));
}


} // end of extern "C" section


/* ------------------------------------------------------------------------------ */
void onMchaLoad(void)
{
	DBG("loading mcha library ..");
	initialiseJuce_GUI();
	DBG("done");
}

/* ------------------------------------------------------------------------------ */
void onMchaUnload(void)
{
	DBG("unloading mcha library ..");	
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
	shutdownJuce_GUI();
	DBG("done");
}

/* ------------------------------------------------------------------------------ */
#if JUCE_WINDOWS
	BOOL WINAPI DllMain (HINSTANCE instance, DWORD dwReason, LPVOID)
	{
		if (dwReason == DLL_PROCESS_ATTACH)
		{
			Process::setCurrentModuleInstanceHandle (instance);
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
