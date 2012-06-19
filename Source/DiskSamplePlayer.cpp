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

#include "DiskSamplePlayer.h"

namespace mcha
{

// ============================================================================================
DiskSamplePlayer::DiskSamplePlayer( const char**  audioFiles, const int filesNumber, const int* outChannels, const int outChannelsCount )
			:	AudioSamplePlayer( _I64_MAX, 0, filesNumber, outChannelsCount),
				mchaRecordPlayer( MchaRecordPlayer::getInstance() ),
				dataReader(NULL),
				channelIndex(0),
				bufferInBytes(0),
				channelGain(0.0f)
{
	mchaRecordPlayer->dbgOut( "DiskSamplePlayer created" );	

	/* Check if the device settings have been updated */
	if (!mchaRecordPlayer->getDeviceSettings().init())
	{
		mchaRecordPlayer->logError( L"Unable to apply device settings:\t" + mchaRecordPlayer->getDeviceSettings().getXmlSettingsFile() );
		mchaRecordPlayer->dbgOut( mchaRecordPlayer->getDeviceSettings().getErrorString() );
		return;
	}
	/* check input parameters */
	if (audioFiles == NULL)
	{
		mchaRecordPlayer->logError( L"ERROR: The file list is empty!" );
		return;	
	}

	/* fill filenames array and check if the files exist */
	mchaRecordPlayer->dbgOut( "\tPlaying audio files:" );	
	audioFileNames.clear();
	ScopedPointer<File> aFile;
	for (int i=0; i < filesNumber; i++)
	{
		
		String fn( audioFiles[i], MCHA_MAX_STRING_SIZE );		
		
		mchaRecordPlayer->dbgOut( "\t\t" + fn );		

		aFile = new File( fn ); /* old aFile is being deleted here */
		if (aFile->existsAsFile())
		{
			audioFileNames.add( fn );
			/* no need to delete aFile here - done by ScopedPointer */
		}
		else
		{
			mchaRecordPlayer->logError( mchaRecordPlayer->getLastErrorStr() + L"\nUnable to find audio file: \t" + fn );
			return;
		}
	}

	/* fill the channelsMap array and initialise gains */
	String chOutStr = String::empty;
	for (int i=0; i<outChannelsCount; i++)
		chOutStr += String::formatted( "%02d ", outChannels[i] ) ;
	mchaRecordPlayer->dbgOut( "\tAdding output channels:\t[ " + chOutStr + "]");

	channelsMap.insertArray(0, outChannels, outChannelsCount);	
	initGains();
	
	// Register audio formats (the objects passed to registerFormat will be deleted by the class)
	audioFormatManager.registerFormat(new AiffAudioFormat(),	 false); 	
	audioFormatManager.registerFormat(new FlacAudioFormat(),	 false); 	
//	audioFormatManager.registerFormat(new OggVorbisAudioFormat(),false); 	
	audioFormatManager.registerFormat(new WavAudioFormat(),		 true);

	// Create InputFileStreams and AudioFormatReaders for each data file
	audioFormatReaders.clear(); 
	
	AudioFormatReader*	tmpReader;
	for (int i=0; i < filesNumber; i++)
	{
		tmpReader = audioFormatManager.createReaderFor( File( audioFiles[i] ) ); //tmpReader = audioFormatManager.createReaderFor(tmpStream);
		if (tmpReader != NULL)
			audioFormatReaders.add(tmpReader);	// audioFormatReaders is an OwnedArray, no need to save references to tmpReader objects
		else
		{
			mchaRecordPlayer->logError( L"Unable to register audio format reader" );
			return;
		}
	}

	// Get the lenght in samples of the shortest audio file
	for (int i=0; i < audioFormatReaders.size(); i++)
	{
		dataLength = jmin(dataLength, audioFormatReaders[i]->lengthInSamples );
	}

	mchaRecordPlayer->dbgOut( "\tOutput data length, samples:\t" + String(dataLength) );

}

// ============================================================================================
DiskSamplePlayer::~DiskSamplePlayer()
{
	channelsMap.clear();
	audioFileNames.clear();
	audioFormatManager.clearFormats();

	audioFormatReaders.clear(true);
}

// ============================================================================================
void DiskSamplePlayer::startProcessing()
{
	// Create dataReader thread
	int bsize = mchaRecordPlayer->getDeviceSettings().getBufferSize();

	dataReader		= new DataReader(audioFormatReaders, bsize, CIRCULAR_BUFFER_LENGTH );
	dataReader->startThread();
}

// ============================================================================================
void DiskSamplePlayer::stopProcessing()
{
	// Terminate DataReader
	dataReader->signalThreadShouldExit();
	dataReader->waitForThreadToExit(1000);
	delete dataReader;
}

// ============================================================================================
bool DiskSamplePlayer::setPosition(int64 newPosition)
{
	bool result = false;
	
	if (dataReader != NULL)
	{
		if (dataReader->jumpToPosition(newPosition))
		{
			currentPosition = newPosition;
			result = true;
		}
	}

	return result;
}

// ============================================================================================
bool DiskSamplePlayer::copyData(  float** outputChannelData, int totalNumOutputChannels, int numSamples  )
{
	
	// Quit if there are no output channels or the dataReader has stopped
	if (  (totalNumOutputChannels > 0)&&(isRunning())  )
	{
		bufferInBytes = numSamples*sizeof(float);
	
		/* copy data from outputTempDataBuffer to active channels */
		for (int ind=0; ind < processorInputs; ind++)
		{
			channelIndex = channelsMap.getUnchecked(ind);
			memcpy(	outputChannelData[channelIndex], dataReader->getAudioData( ind ), bufferInBytes);

			/* apply gain */
			channelGain = outputGains[ind];
			for (int i=0; i < numSamples; i++)
				outputChannelData[channelIndex][i] *= channelGain;
		}

		// Move to the next block
		dataReader->getNext();
		currentPosition += numSamples;
		return true;
	}
	else
	{
		return false;
	}
}

// ============================================================================================
bool DiskSamplePlayer::isRunning()
{
	if (dataReader == NULL)
		return false;
	else
		return !(dataReader->threadShouldExit());
}

}
