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

#include "MemorySampleRecorder.h"

namespace mcha
{

MemorySampleRecorder::MemorySampleRecorder( float** inputData, const int inputChanNumber, const int64 inputSamplesNumber, const int *inChannels, const int inChannelsCount  )
			:	AudioSampleRecorder( inputSamplesNumber, 0, inChannelsCount, inputChanNumber),
				mchaRecordPlayer( MchaRecordPlayer::getInstance() ),
				inputDataBuffer(NULL),
				samplesToCopy(0),
				sourceChannel(0)

{
	/* copy the content of inputData */
	inputDataBuffer = new float* [inputChanNumber];
	for (int i=0; i < inputChanNumber; i++)
		inputDataBuffer[i] = inputData[i];

	/* Check if the device settings have been updated */
	if (!mchaRecordPlayer->getDeviceSettings().init())
	{
		mchaRecordPlayer->logError( L"Unable to apply device settings:\t" + mchaRecordPlayer->getDeviceSettings().getXmlSettingsFile() );
		mchaRecordPlayer->dbgOut( mchaRecordPlayer->getDeviceSettings().getErrorString() );
		return;
	}
	
	/* map device channels to data channels */
	channelsMap.insertArray(0, inChannels, inChannelsCount);

	mchaRecordPlayer->dbgOut( "MemorySampleRecorder created" );	
	mchaRecordPlayer->dbgOut("\tprocessorInputs = " + String(processorInputs));
	mchaRecordPlayer->dbgOut("\tprocessorOutputs = " + String(processorOutputs));
	mchaRecordPlayer->dbgOut("\tdataLength = " + String(dataLength));
	mchaRecordPlayer->dbgOut("\tmemoryMode = " + String(mchaRecordPlayer->getMemoryMode()) );

}


//==============================================================================
MemorySampleRecorder::~MemorySampleRecorder()
{
	delete [] inputDataBuffer;
	channelsMap.clear();
}


//==============================================================================
bool MemorySampleRecorder::copyData (	const float** inputChannelData,
								int totalNumInputChannels,
								int numSamples)
{
	/* check if the device is running properly */
	if ((processorInputs > 0 ) && (totalNumInputChannels != processorOutputs))
	{
		/* return error string */
		mchaRecordPlayer->logError(L"MemorySampleRecorder::copyData failed:");
		mchaRecordPlayer->dbgOut( "\tNumber of input data channels:\t" + String(totalNumInputChannels) );
		mchaRecordPlayer->dbgOut( "\tNumber of processor outputs:\t" + String(processorOutputs) );
		return false;
	}

	/* Record until the end of data buffer has been reached */
	if (currentPosition < dataLength)
	{
		samplesToCopy = (dataLength - currentPosition >= numSamples) ? numSamples : (int)(dataLength - currentPosition);
		
		for (int ch = 0; ch < totalNumInputChannels; ch++)
		{
			sourceChannel =  channelsMap[ch];
			memcpy( inputDataBuffer[sourceChannel] + currentPosition, inputChannelData[sourceChannel], sizeof(float)*samplesToCopy );
		}
	}

	currentPosition += numSamples;

	return true;
}

}