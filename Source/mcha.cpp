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
#include "stdafx.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "settings.h"
#include "mchaRecordPlayer.h"
#include "AudioFileConverter.h"

using namespace mcha;

DLLEXP bool initAudioDevice()
{
	return MchaRecordPlayer::getInstance()->init();
}

DLLEXP bool initAudioDeviceFile(const char* xmlSettingsFile)
{
	return MchaRecordPlayer::getInstance()->init(xmlSettingsFile);
}

DLLEXP	bool setDebugMode(const char* dmStr)
{
	return MchaRecordPlayer::getInstance()->setDebugMode(dmStr);
}

DLLEXP	bool setMemoryMode(const char* mMode)
{
	return MchaRecordPlayer::getInstance()->setMemoryMode(mMode);
}

DLLEXP const char* getLastError()
{
	return MchaRecordPlayer::getInstance()->getLastError();
}

DLLEXP const char* getVersion()
{
	return MCHA_VERSION_NUMBER;
}

DLLEXP bool addFilter(const char* settingsFileName, bool isRecordFilter)
{
	return MchaRecordPlayer::getInstance()->addFilter(settingsFileName, isRecordFilter);
}

DLLEXP bool playFiles (const char**	 audioFiles, const int filesNumber, const int *channels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord((const char **) NULL, 0, 0.0f, NULL, 0,  audioFiles, filesNumber, 0, channels, channelCount);
}


DLLEXP bool playData_s (const float** audioData, const int chanNumber, const size_t samplesNumber, const int *channels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord((const char **) NULL, 0, 0.0f, NULL, 0, audioData, chanNumber, samplesNumber, channels, channelCount);
}


DLLEXP bool playData_d (const double** audioData, const int chanNumber, const size_t samplesNumber, const int *channels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord((const char **) NULL, 0, 0.0f, NULL, 0, audioData, chanNumber, samplesNumber, channels, channelCount);
}


DLLEXP bool recordFiles	( const char*	recordDir, const int inputChanNumber, const float duration, const int *channels, const int channelCount )
{
	return MchaRecordPlayer::getInstance()->playRecord(&recordDir, inputChanNumber, duration, channels, channelCount,(const float**) NULL, 0, 0, NULL, 0);
}


DLLEXP bool recordData	( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int channelCount)
{
	return MchaRecordPlayer::getInstance()->playRecord(inputData,	inputChanNumber,	(const float) inputSamplesNumber,	 inChannels, channelCount, (const float**)NULL, 0, 0, NULL, 0);
}

/* memory -> memory */
DLLEXP	bool playRecordDataMM_s( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const float** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount )
{
	return MchaRecordPlayer::getInstance()->playRecord(	inputData,	inputChanNumber,	(const float) inputSamplesNumber,	 inChannels, inChannelsCount,
															outputData,	outputChanNumber,	outputSamplesNumber, outChannels, outChannelsCount);

}

/* memory double -> memory  */
DLLEXP	bool playRecordDataMM_d( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
							 const double** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount )
{
	return MchaRecordPlayer::getInstance()->playRecord(	inputData,	inputChanNumber,	(const float) inputSamplesNumber,	 inChannels, inChannelsCount,
															outputData,	outputChanNumber,	outputSamplesNumber, outChannels, outChannelsCount);

}
/* memory -> disk	*/
DLLEXP	bool playRecordDataMD_s (   const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
								const float** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount)
{
	return 	MchaRecordPlayer::getInstance()->playRecord(  &recordDir, inputChanNumber, duration, inChannels, inChannelsCount, 
															outputData, outputChanNumber,  outputSamplesNumber, outChannels, outChannelsCount);
}

/* memory double -> disk	*/
DLLEXP	bool playRecordDataMD_d (   const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
								const double** outputData, const int outputChanNumber, const size_t outputSamplesNumber, const int* outChannels, const int outChannelsCount)
{
	return 	MchaRecordPlayer::getInstance()->playRecord(  &recordDir, inputChanNumber, duration, inChannels, inChannelsCount, 
															outputData, outputChanNumber,  outputSamplesNumber, outChannels, outChannelsCount);
}

/* disk -> memory */
DLLEXP	bool playRecordDataDM ( float** inputData, const int inputChanNumber, const size_t inputSamplesNumber, const int *inChannels, const int inChannelsCount,
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount )
{
	return 	MchaRecordPlayer::getInstance()->playRecord (	inputData, inputChanNumber, (const float) inputSamplesNumber, inChannels, inChannelsCount,
															audioFiles, filesNumber, 0, channels, outChannelsCount );
}

/* disk -> disk */
DLLEXP	bool playRecordDataDD ( const char* recordDir, const int inputChanNumber, const float duration, const int* inChannels, const int inChannelsCount, 
						const char**  audioFiles, const int filesNumber, const int* channels, const int outChannelsCount )
{
	return	MchaRecordPlayer::getInstance()->playRecord ( &recordDir, inputChanNumber, duration, inChannels, inChannelsCount, 
															audioFiles, filesNumber, 0,  channels, outChannelsCount);
}


DLLEXP	int		getRecordedChannelsNum() 
{	
	return 	MchaRecordPlayer::getInstance()->getRecordedChannelsNum(); 
};


DLLEXP size_t	getRecordedSamplesNum()  
{	
	return MchaRecordPlayer::getInstance()->getRecordedSamplesNum(); 
};


DLLEXP bool	getData_s(float**	dat,  const int* channels = NULL, const int channelsNumber = 0, size_t startSample = 0, size_t endSample = 0) 
{
	return	MchaRecordPlayer::getInstance()->getData(dat, channels, channelsNumber, startSample, endSample);
}

DLLEXP bool	getData_d(double**	dat,  const int* channels = NULL, const int channelsNumber = 0, size_t startSample = 0, size_t endSample = 0) 
{
	return	MchaRecordPlayer::getInstance()->getData(dat, channels, channelsNumber, startSample, endSample);
}


DLLEXP 	bool setGain (int *channels, int channelsCount, float *gains)
{
	return MchaRecordPlayer::getInstance()->setGain (channels, channelsCount, gains);
}

DLLEXP bool stopAudio()
{
	return MchaRecordPlayer::getInstance()->stop();
}

DLLEXP bool isRunning()
{
	return MchaRecordPlayer::getInstance()->isRunning();
}

DLLEXP double getCurrentPosition()
{ 
	return MchaRecordPlayer::getInstance()->getCurrentPosition();
}

DLLEXP bool setPosition(double timeInSeconds)
{ 
	return MchaRecordPlayer::getInstance()->setPosition(timeInSeconds);
}

DLLEXP bool getDeviceSettings(double& samplingRate, int& bufSize, int& bDepth, int& inChannels, int& outChannels)
{
	return MchaRecordPlayer::getInstance()->getDeviceSettings().getSettings( samplingRate, bufSize, bDepth, inChannels, outChannels );
}

DLLEXP void getFilterSettings(bool isInputFilter, int& inputsCount, int& outputsCount)
{
	MchaRecordPlayer::getInstance()->getFilterSettings(isInputFilter, inputsCount, outputsCount);
}

DLLEXP void logMessage(const char* msg)
{
	MchaRecordPlayer::getInstance()->dbgOut(String(msg));
}

DLLEXP void	logError(const char* msg)
{
	MchaRecordPlayer::getInstance()->logError(String(msg));
}

DLLEXP void logDouble(const char* infoStr, const double value)
{
	MchaRecordPlayer::getInstance()->dbgOut(String(infoStr) + String(value));
}

DLLEXP void logAddress(const char* infoStr, const int64 addr)
{
	MchaRecordPlayer::getInstance()->dbgOut(String(infoStr) + String::toHexString(addr));
}


/* ------------------------------------------------------------------------------ */
BOOL WINAPI DllMain (HINSTANCE instance, DWORD dwReason, LPVOID)
    {
        if (dwReason == DLL_PROCESS_ATTACH)
        {
            Process::setCurrentModuleInstanceHandle (instance);
            initialiseJuce_GUI();

			MchaRecordPlayer* mchaRecordPlayer = MchaRecordPlayer::getInstance();

			/* log system information */

			String systemInfo;
			
			systemInfo << SystemStats::getJUCEVersion();
			mchaRecordPlayer->dbgOut( systemInfo ); systemInfo = String::empty;

			systemInfo<< "Operating system:\t" << SystemStats::getOperatingSystemName();
			if ( SystemStats::isOperatingSystem64Bit() )
				systemInfo << " 64 bit";
			else
				systemInfo << " 32 bit";
			mchaRecordPlayer->dbgOut( systemInfo ); systemInfo = String::empty;

			systemInfo<< "CPU vendor:\t\t" << SystemStats::getCpuVendor();
			mchaRecordPlayer->dbgOut( systemInfo ); systemInfo = String::empty;

			systemInfo<< "CPU speed:\t\t" << SystemStats::getCpuSpeedInMegaherz() << " MHz";
			mchaRecordPlayer->dbgOut( systemInfo ); systemInfo = String::empty;

			systemInfo<< "Number of CPUs:\t\t" << SystemStats::getNumCpus();
			mchaRecordPlayer->dbgOut( systemInfo ); systemInfo = String::empty;

			systemInfo<< "Memory size:\t\t" << SystemStats::getMemorySizeInMegabytes() << " Mb";
			mchaRecordPlayer->dbgOut( systemInfo ); systemInfo = String::empty;

			systemInfo<< "mchaRecordPlayer version:\t" + String(MCHA_VERSION_NUMBER);
			mchaRecordPlayer->dbgOut( systemInfo ); systemInfo = String::empty;

			mchaRecordPlayer->dbgOut( systemInfo );

		}
        else if (dwReason == DLL_PROCESS_DETACH)
        {

			MchaRecordPlayer* mchaRecordPlayer = MchaRecordPlayer::getInstanceWithoutCreating();

			if (mchaRecordPlayer != NULL)
			{
				mchaRecordPlayer->dbgOut( "mchaRecordPlayer is being deleted." );
				if ( mchaRecordPlayer->isRunning() )
				{
					mchaRecordPlayer->dbgOut( "\tmchaRecordPlayer is running a task: trying to stop..." );
					if ( mchaRecordPlayer->stop() )
					{
						mchaRecordPlayer->dbgOut( "\tmchaRecordPlayer: task terminated successfully" );
					}
					else
					{
						mchaRecordPlayer->dbgOut( "\tmchaRecordPlayer: failed to terminate a task." );				
					}
				}
			
				MchaRecordPlayer::deleteInstance();
			}

			AudioFileConverter* conv = AudioFileConverter::getInstanceWithoutCreating();
			if (conv != NULL)
			{
				conv->waitForThreadToExit(-1); // wait forever
				conv->deleteInstance();
			}

			shutdownJuce_GUI();
		}

        return TRUE;
    }

