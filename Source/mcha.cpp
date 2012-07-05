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
	extern "C" __declspec(dllexport) 
#else
	extern "C" 
#endif

{ // beginning of extern "C" section

bool initAudioDevice()
{
	return MchaRecordPlayer::getInstance()->init();
}

bool initAudioDeviceFile(const char* xmlSettingsFile)
{
	return MchaRecordPlayer::getInstance()->init(xmlSettingsFile);
}

bool setDebugMode(const char* dmStr)
{
	return MchaRecordPlayer::getInstance()->setDebugMode(dmStr);
}

bool setMemoryMode(const char* mMode)
{
	return MchaRecordPlayer::getInstance()->setMemoryMode(mMode);
}

const char* getLastError()
{
	return MchaRecordPlayer::getInstance()->getLastError();
}

const char* getVersion()
{
	DBG( String::toHexString( (int64) MchaRecordPlayer::getInstance() ) );	
	return MCHA_VERSION_NUMBER;
}

bool addFilter(const char* settingsFileName, bool isRecordFilter)
{
	return MchaRecordPlayer::getInstance()->addFilter(settingsFileName, isRecordFilter);
}

bool playFiles (const char**	 audioFiles, const int filesNumber, const int *channels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord((const char **) NULL, 0, 0.0f, NULL, 0,  audioFiles, filesNumber, 0, channels, channelCount);
}


bool playData_s (const float** audioData, const int chanNumber, const size_t samplesNumber, const int *channels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord((const char **) NULL, 0, 0.0f, NULL, 0, audioData, chanNumber, samplesNumber, channels, channelCount);
}


bool playData_d (const double** audioData, const int chanNumber, const size_t samplesNumber, const int *channels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord((const char **) NULL, 0, 0.0f, NULL, 0, audioData, chanNumber, samplesNumber, channels, channelCount);
}


bool recordFiles	( const char*	recordDir, const int inputChanNumber, const float duration, const int *channels, const int channelCount )
{
	return MchaRecordPlayer::getInstance()->playRecord(&recordDir, inputChanNumber, duration, channels, channelCount,(const float**) NULL, 0, 0, NULL, 0);
}


bool recordData	( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord(inputData,	inputChanNumber,	(const float) inputSamplesNumber,	 inChannels, channelCount, (const float**)NULL, 0, 0, NULL, 0);
}

/* memory -> memory */
bool playRecordDataMM_s( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const float** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount )
{
	return MchaRecordPlayer::getInstance()->playRecord(	inputData,	inputChanNumber,	(const float) inputSamplesNumber,	 inChannels, inChannelsCount,
															outputData,	outputChanNumber,	outputSamplesNumber, outChannels, outChannelsCount);

}

/* memory double -> memory  */
bool playRecordDataMM_d( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const double** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount )
{
	return MchaRecordPlayer::getInstance()->playRecord(	inputData,	inputChanNumber,	(const float) inputSamplesNumber,	 inChannels, inChannelsCount,
															outputData,	outputChanNumber,	outputSamplesNumber, outChannels, outChannelsCount);

}
/* memory -> disk	*/
bool playRecordDataMD_s (   const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
								const float** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount)
{
	return 	MchaRecordPlayer::getInstance()->playRecord(  &recordDir, inputChanNumber, duration, inChannels, inChannelsCount, 
															outputData, outputChanNumber,  outputSamplesNumber, outChannels, outChannelsCount);
}

/* memory double -> disk	*/
bool playRecordDataMD_d (   const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
								const double** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount)
{
	return 	MchaRecordPlayer::getInstance()->playRecord(  &recordDir, inputChanNumber, duration, inChannels, inChannelsCount, 
															outputData, outputChanNumber,  outputSamplesNumber, outChannels, outChannelsCount);
}

/* disk -> memory */
bool playRecordDataDM ( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount )
{
	return 	MchaRecordPlayer::getInstance()->playRecord (	inputData, inputChanNumber, (const float) inputSamplesNumber, inChannels, inChannelsCount,
															audioFiles, filesNumber, 0, channels, outChannelsCount );
}

/* disk -> disk */
bool playRecordDataDD ( const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount )
{
	return	MchaRecordPlayer::getInstance()->playRecord ( &recordDir, inputChanNumber, duration, inChannels, inChannelsCount, 
															audioFiles, filesNumber, 0,  channels, outChannelsCount);
}


int getRecordedChannelsNum() 
{	
	return 	MchaRecordPlayer::getInstance()->getRecordedChannelsNum(); 
};


size_t	getRecordedSamplesNum()  
{	
	return MchaRecordPlayer::getInstance()->getRecordedSamplesNum(); 
};


bool	getData_s(float**	dat,  const int* channels = NULL, const int channelsNumber = 0, size_t startSample = 0, size_t endSample = 0) 
{
	return	MchaRecordPlayer::getInstance()->getData(dat, channels, channelsNumber, startSample, endSample);
}

bool	getData_d(double**	dat,  const int* channels = NULL, const int channelsNumber = 0, size_t startSample = 0, size_t endSample = 0) 
{
	return	MchaRecordPlayer::getInstance()->getData(dat, channels, channelsNumber, startSample, endSample);
}


	bool setGain (int *channels, int channelsCount, float *gains)
{
	return MchaRecordPlayer::getInstance()->setGain (channels, channelsCount, gains);
}

bool stopAudio()
{
	return MchaRecordPlayer::getInstance()->stop();
}

bool isRunning()
{
	return MchaRecordPlayer::getInstance()->isRunning();
}

double getCurrentPosition()
{ 
	return MchaRecordPlayer::getInstance()->getCurrentPosition();
}

bool setPosition(double timeInSeconds)
{ 
	return MchaRecordPlayer::getInstance()->setPosition(timeInSeconds);
}

bool getDeviceSettings(double& samplingRate, int& bufSize, int& bDepth, int& inChannels, int& outChannels)
{
	return MchaRecordPlayer::getInstance()->getDeviceSettings().getSettings( samplingRate, bufSize, bDepth, inChannels, outChannels );
}

void getFilterSettings(bool isInputFilter, int& inputsCount, int& outputsCount)
{
	MchaRecordPlayer::getInstance()->getFilterSettings(isInputFilter, inputsCount, outputsCount);
}

void logMessage(const char* msg)
{
	MchaRecordPlayer::getInstance()->dbgOut(String(msg));
}

void	logError(const char* msg)
{
	MchaRecordPlayer::getInstance()->logError(String(msg));
}

void logDouble(const char* infoStr, const double value)
{
	MchaRecordPlayer::getInstance()->dbgOut(String(infoStr) + String(value));
}

void logAddress(const char* infoStr, const int64 addr)
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
