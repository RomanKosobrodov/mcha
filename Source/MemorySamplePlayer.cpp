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

#include "MemorySamplePlayer.h"

namespace mcha
{

MemorySamplePlayer::MemorySamplePlayer( const float** outputData, const int outputChanNumber, const int64 outputSamplesNumber, const int* outChannels, const int outChannelsCount )
					:	AudioSamplePlayer( outputSamplesNumber, 0, outputChanNumber, outChannelsCount),
						mchaRecordPlayer( MchaRecordPlayer::getInstance() ),
						outputDataBuffer(NULL),
						bufferInBytes(0),
						samplesToCopy(0),
						channelIndex(0),
						channelGain(0.0f)
{
	mchaRecordPlayer->dbgOut("MemorySamplePlayer created.");

	/* Check if the device settings have been updated */
	if (!mchaRecordPlayer->getDeviceSettings().init())
	{
		mchaRecordPlayer->logError( L"Unable to apply device settings:\t" + mchaRecordPlayer->getDeviceSettings().getXmlSettingsFile() );
		mchaRecordPlayer->dbgOut( mchaRecordPlayer->getDeviceSettings().getErrorString() );
		return;
	}


	/* fill the channelsMap array and initialise gains */
	channelsMap.insertArray(0, outChannels, outChannelsCount);
	initGains();

	String chOutStr = String::empty;
	for (int i=0; i<outChannelsCount; i++)
		chOutStr += String::formatted( "%02d ", outChannels[i] ) ;
	mchaRecordPlayer->dbgOut( "\tAdding playback channels:\t[ " + chOutStr + "]");

	/* create a copy of outputData */
	outputDataBuffer = new float* [outputChanNumber];
	for (int i=0; i<outputChanNumber; i++)
		outputDataBuffer[i] = const_cast <float*> (outputData[i]);

	mchaRecordPlayer->dbgOut("\tprocessorInputs = " + String(processorInputs));
	mchaRecordPlayer->dbgOut("\tprocessorOutputs = " + String(processorOutputs));
	mchaRecordPlayer->dbgOut("\tdataLength = " + String(dataLength));
	mchaRecordPlayer->dbgOut("\tcurrentPosition = " + String(currentPosition));
	mchaRecordPlayer->dbgOut("\tmemoryMode = " + String(mchaRecordPlayer->getMemoryMode()) );

}

//==============================================================================
MemorySamplePlayer::~MemorySamplePlayer()
{
	delete [] outputDataBuffer;
}

//==============================================================================
bool MemorySamplePlayer::copyData(float **outputChannelData, int /*totalNumOutputChannels*/, int numSamples)
{
	
	bufferInBytes = numSamples*sizeof(float);
	
	if (currentPosition < dataLength)
	{
		samplesToCopy = (dataLength - currentPosition >= numSamples) ? numSamples : (int)(dataLength - currentPosition);
		bufferInBytes = samplesToCopy*sizeof(float);		

		/* copy data from outputTempDataBuffer to active channels */
		for (int ind=0; ind < processorInputs; ind++)
		{
			channelIndex = channelsMap.getUnchecked(ind);			
			memcpy(outputChannelData[channelIndex], outputDataBuffer[ind] + currentPosition, bufferInBytes);
			
			/* apply gain */
			channelGain = outputGains[ind];
			/* this is slow - use IPP instead  */
			for (int i=0; i < numSamples; i++)
			{
				outputChannelData[channelIndex][i] *= channelGain;
			}
		}

	}
	
	currentPosition += numSamples;
	return true;
}

//==============================================================================
bool MemorySamplePlayer::isRunning() 
{ 
	return (currentPosition < dataLength); 
} 

//==============================================================================
String MemorySamplePlayer::getName() 
{ 
	return "MemorySamplePlayer"; 
}

}
