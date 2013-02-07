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

#include "settings.h"
#include "AudioSampleProcessor.h"

namespace mcha
{
	
// ============================================================================================
void AudioSamplePlayer::initGains()
{
	outputGains.clear();
	for (int i=0; i < processorOutputs; i++)
	{
		outputGains.add(1.0f);
	}
}

// ============================================================================================
int AudioSamplePlayer::findElement (int value)
{
	int res = -1;
	for (int i=0; i< processorOutputs; i++)
		if (channelsMap[i] == value)
			res = i;
	return res;
}

// ============================================================================================
bool AudioSamplePlayer::setPosition(int64 newPosition)
{
	if ((newPosition >= 0)&&(newPosition <= dataLength))
	{
		currentPosition = newPosition;
		return true;
	}
	else
	{
		return false;
	}
}


// ============================================================================================
bool AudioSamplePlayer::setGain (int *channels, int channelsCount, float *gains)
{

	MchaRecordPlayer::getInstance() -> dbgOut( "AudioSamplePlayer::setGain" );

	/* set gain to unity for all channels */
	outputGains.clear();
	for (int i=0; i< processorOutputs; i++)
		outputGains.add(1.0f);

	/* modify gain if it was specified */
	int curChannel;
	for (int i=0; i<channelsCount; i++)
	{
		curChannel = findElement(channels[i]); // check if the channel is active
		if ( curChannel != -1 )
			outputGains.set(curChannel, gains[i]); // update the gain
	}

	String str = String::empty;
	for (int i=0; i< processorOutputs; i++)
	{
		str = "\taudioOut" + String(i) + " <- Data" + String(channelsMap[i]);
		str += "\tgain[" + String(i) + "] = " + String::formatted( "%.2f", outputGains[i] );
		MchaRecordPlayer::getInstance() -> dbgOut(str);  
	}

	return true;
}

// ============================================================================================
bool AudioSamplePlayer::setGain (Array<int> channels, Array<float> gains)
{

	MchaRecordPlayer::getInstance() -> dbgOut( "AudioSamplePlayer::setGain" );

	if ( channels.size() != gains.size() )
		return false;

	/* set gain to unity for all channels */
	outputGains.clear();
	for (int i=0; i < processorOutputs; i++)
		outputGains.add(1.0f);

	/* modify gain if it was specified */
	int curChannel;
	for (int i=0; i < gains.size() ; i++)
	{
		curChannel = findElement(channels[i]);	// check if the channel is active
		if ( curChannel != -1 )
		{	
			outputGains.set(curChannel, gains[i]); // update the gain
		}
	}

	return true;
}

// ============================================================================================
bool EmptySampleRecorder::copyData( const float** ,	int , int ) 
{ 
	return true; 
};

// ============================================================================================
bool EmptySamplePlayer::copyData(  float** , int , int   ) 
{ 
	return true; 
};

// ============================================================================================
AudioSampleProcessor::AudioSampleProcessor ()
						:	audioSampleRecorder(NULL),
							audioSamplePlayer(NULL),
							debugRecorder(NULL),
							inputProcessor(NULL),
							outputProcessor(NULL),
							totalAudioLength(0),
							currentAudioPosition(0),
							tempBufferIn(NULL),
							tempBufferOut(NULL),
							inputProcessorChannels(0),
							outputProcessorChannels(0),
							debugChannelCount(0)
{
	mchaRecordPlayer = MchaRecordPlayer::getInstance();
}

// ============================================================================================
AudioSampleProcessor::~AudioSampleProcessor()
{
}

// ============================================================================================
void AudioSampleProcessor::audioDeviceAboutToStart(AudioIODevice* )
{
	Time now;
	now = Time::getCurrentTime();

	mchaRecordPlayer->dbgOut( "AudioSampleProcessor started\t"  + now.toString(false, true, true, true) + ":" 
								+ String(now.getMilliseconds()) );
	mchaRecordPlayer->dbgOut( "Thread ID\t= " + String( uint64(Thread::getCurrentThreadId()) ) );
	mchaRecordPlayer->dbgOut( "\tRecorder\t= " + audioSampleRecorder->getName() );
	mchaRecordPlayer->dbgOut( "\tPlayer\t\t= " + audioSamplePlayer->getName() );
	
	if (inputProcessor!=NULL)
		mchaRecordPlayer->dbgOut( "\tRecording Filter\t= " + inputProcessor->getName() );
	else
		mchaRecordPlayer->dbgOut( "\tRecording Filter\t= <none>" );
	
	if (outputProcessor!=NULL)
		mchaRecordPlayer->dbgOut( "\tPlayback Filter\t= " + outputProcessor->getName() );
	else
		mchaRecordPlayer->dbgOut( "\tPlayback Filter\t= <none>" );


	/* Total number of audio buffers to be processed */
	int64 numberInputSamples = audioSampleRecorder->getDataLength();
	int64 numberOutputSamples = audioSamplePlayer->getDataLength();

	totalAudioLength = (numberOutputSamples > numberInputSamples) ? numberOutputSamples : numberInputSamples;
	int bufferSize = mchaRecordPlayer->getDeviceSettings().getBufferSize();

	/* Allocate memory for temporary signal buffers: Input */
	if (inputProcessor != NULL)
	{
		inputProcessorChannels = inputProcessor->getOutputsNumber();
		mchaRecordPlayer->allocateBuffer(tempBufferIn, inputProcessorChannels, bufferSize, true, "tempBufferIn");
	}

	/* Allocate memory for temporary signal buffers: Output */
	if (outputProcessor != NULL)
	{	
		outputProcessorChannels = outputProcessor->getInputsNumber();
		mchaRecordPlayer->allocateBuffer(tempBufferOut, outputProcessorChannels, bufferSize, true, "tempBufferOut");
	}

	/* Start processing data */
	audioSampleRecorder->startProcessing();
	audioSamplePlayer->startProcessing();
	
	/* Start debugging log */
	if (debugRecorder != NULL)
	{
		debugChannelCount = debugRecorder->getOutputsNumber();
		debugRecorder->startProcessing();
	}
}

// ============================================================================================
void AudioSampleProcessor::audioDeviceStopped()
{
	
	mchaRecordPlayer->dbgOut( "AudioSampleProcessor stopped\t");
	mchaRecordPlayer->dbgOut( "Thread ID\t= " + String( uint64(Thread::getCurrentThreadId()) ) );

	/* Stop processing data */
	audioSampleRecorder->stopProcessing();
	audioSamplePlayer->stopProcessing();

	/* Stop logging debug data */
	if (debugRecorder != NULL)
		debugRecorder->stopProcessing();

	/* Release temporary signal buffers */
	mchaRecordPlayer->releaseBuffer(tempBufferIn,  inputProcessorChannels);
	mchaRecordPlayer->releaseBuffer(tempBufferOut, outputProcessorChannels);

	/* Clear references to these objects */
	audioSampleRecorder = NULL;
	audioSamplePlayer = NULL;
	debugRecorder = NULL;
	inputProcessor = NULL;
	outputProcessor = NULL;
}

// ============================================================================================
void AudioSampleProcessor::audioDeviceIOCallback (const float** inputChannelData,
												  int totalNumInputChannels,
												  float** outputChannelData,
												  int totalNumOutputChannels,
												  int numSamples)
{


	/* clear all channels (We don't want any junk playing after audioSamplePlayer has finished its work) */
	int bytesToClear = numSamples * sizeof(float);
	for (int ch=0; ch < totalNumOutputChannels; ch++)
	{
		zeromem(outputChannelData[ch], bytesToClear);
	}

	/*	use getCurrentPosition to stop processing */
	currentAudioPosition = jmax( audioSamplePlayer->getCurrentPosition(), audioSampleRecorder->getCurrentPosition() );

	if (currentAudioPosition < totalAudioLength)
	{
		/* process output data */
		if (outputProcessor != NULL)
		{
			/* get output data from memory or disk */
			audioSamplePlayer->copyData ( tempBufferOut, outputProcessorChannels, numSamples );
			/* arrange output channels in proper order, */
			/* pass to output processor and copy to audio buffer */		
			outputProcessor->processBlock(tempBufferOut, outputChannelData, numSamples);
		}
		else
		{
			/* get output data from memory or disk and copy straight to audio buffer */
			audioSamplePlayer->copyData (outputChannelData, totalNumOutputChannels, numSamples );				
		}

		/* save output to debug logger */
		if (debugRecorder != NULL)
			debugRecorder->copyData (const_cast <const float**>  (outputChannelData),	debugChannelCount, numSamples );

		/* ---- process input data ---- */
		if (inputProcessor != NULL)
		{
			inputProcessor->processBlock(const_cast <float**> (inputChannelData), tempBufferIn, numSamples);

			/* save processed input data to memory or disk */
			if ( !audioSampleRecorder->copyData (const_cast <const float**>  (tempBufferIn),	inputProcessorChannels, numSamples) )
			{
				/* error has occured - stop processing */
				mchaRecordPlayer->stop(); //processShouldStop();
			}
		}
		else
		{
			/* save raw input data to memory or disk */
			if ( !audioSampleRecorder->copyData (const_cast <const float**>  (inputChannelData),	totalNumInputChannels, numSamples) )
			{
				/* error has occured - stop processing */
				mchaRecordPlayer->stop(); //processShouldStop();
			}		
		}
	}
	else
	{
		/* Stop processing */
		mchaRecordPlayer->stop(); //processShouldStop();
	}
	
	if (currentAudioPosition == 0)	
		mchaRecordPlayer->dbgOut( "Audio Thread ID\t= " + String( uint64(Thread::getCurrentThreadId()) ) );


 }

}
