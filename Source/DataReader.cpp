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

#include "DataReader.h"
#include "mchaRecordPlayer.h"

namespace mcha
{

// =================================================================================================================
DataReader::DataReader( OwnedArray<AudioFormatReader>	&formatReaders, 
					    const int						samplesNumber, 
					    const int						bufferLenghtInBlocks ) :
																				audioFormatReaders(formatReaders),
																				numberOfSamples(samplesNumber),
																				numberOfFiles( formatReaders.size() ),
																				playBlockPosition(0),
																				bufferLength(bufferLenghtInBlocks),
																				readOffset(0),
																				samplesCount(0),
																				ms2wait(0),
																				Thread( "DataReader Thread" )

{
	// Create AudioSampleBuffer for each channel and fill it with the first set of data
	for ( int ch = 0; ch < numberOfFiles; ch++ )	
	{
		channelBuffer.add( new AudioSampleBuffer(1, numberOfSamples * bufferLength) );
		audioFormatReaders[ch]->read(	channelBuffer[ch],
										0,								//	const int  	startSample,
										numberOfSamples * bufferLength,	//	const int  	numSamples,
										readOffset,						//	const int  	readerStartSample,
										true,							//	const bool  useReaderLeftChan,
										false							//	const bool  useReaderRightChan
									); 
	}
	readOffset += numberOfSamples * bufferLength;	
	
	// Get the lenght in samples of the shortest audio file
	samplesCount = _I64_MAX;
	for (int i=0; i < numberOfFiles; i++)
		samplesCount = jmin(samplesCount, audioFormatReaders[i]->lengthInSamples );

	// Initialise dataValid and set it to true
	dataValid = new bool[bufferLength];
	for (int i=0; i < bufferLength; i++)
		dataValid[i] = true;

	// set sleep time to half buffer duration
	int bufSize, bDepth, chIn, chOut;
	double sampleFreq;
	MchaRecordPlayer::getInstance()->getDeviceSettings().getSettings(sampleFreq, bufSize,  bDepth, chIn, chOut);

	ms2wait = (int) ( floor(bufSize*1000/sampleFreq) / 2 );
}

// =================================================================================================================
DataReader::~DataReader()
{
	channelBuffer.clear(true ); // This should delete all AudioSampleBuffer objects
	delete [] dataValid;
}


// =================================================================================================================
float* DataReader::getAudioData(const int sourceChannel)
{
		float* data = channelBuffer[sourceChannel]->getSampleData( 0, playBlockPosition*numberOfSamples );
		return data;
}

// =================================================================================================================
void DataReader::getNext()
{
	dataValid[playBlockPosition]= false;	

	playBlockPosition++;
	playBlockPosition = playBlockPosition % bufferLength;	// Move to the next block

}

// =================================================================================================================
bool DataReader::jumpToPosition(int64 newPosition)
{
	/* Position the AudioFormatReader and fill the buffers with new data */
	if ((newPosition >= 0) && (newPosition < samplesCount ))
	{
		readOffset = (int) newPosition; 
		for ( int ch = 0; ch < numberOfFiles; ch++ )	
		{
			audioFormatReaders[ch]->read(  channelBuffer[ch],			//  AudioFormatReader *   	 reader
										    0,								//	const int  	startSample,
											numberOfSamples * bufferLength,	//	const int  	numSamples (If this is greater than the number of samples that the file or stream contains. the result will be padded with zeros)
											readOffset,						//	const int  	readerStartSample,
											true,							//	const bool  useReaderLeftChan,
											false							//	const bool  useReaderRightChan
										 );

		}
		readOffset += numberOfSamples * bufferLength;
		MchaRecordPlayer::getInstance()->dbgOut( "DataReader::jumpToPosition");
		MchaRecordPlayer::getInstance()->dbgOut( "\tNew offset = " + String( readOffset ) );
		return true;
	}
	else
	{
		return false;
	}
}
// =================================================================================================================
void DataReader::run()
{
	Time now(Time::getCurrentTime());

	MchaRecordPlayer::getInstance()->dbgOut("DataReader thread started "  + now.toString(false, true, true, true) + ":" + String(now.getMilliseconds()) );
 
	  while ( !threadShouldExit() )
	  {
		  // Check if the end of audio record has been reached
		 if (readOffset > samplesCount)
		 {	 
			 // Finish the thread
			 MchaRecordPlayer::getInstance()->dbgOut("End of record has been reached.");
			 break;
		 }
		 else
		 {		 
			  // Find the first available block
			  int firstBlock = -1;
			  for ( int ind = 0; ind < bufferLength; ind++ )
			  {
				  if (!dataValid[ind])
				  {
					  firstBlock = ind;
					  break;
				  }
			  }

			  // If no blocks found - skip the rest
			  if (firstBlock == -1)
			  {
				  sleep(ms2wait);
				  continue;
			  }

			  // Count the number of available blocks
			  int availableBlocks = 0;
			  for ( int ind = firstBlock; ind < bufferLength; ind++ )
			  {
				  if (!dataValid[ind])
					  availableBlocks++;
			  }

			  // Load all available blocks. Lock write access to data
			{
			const ScopedWriteLock myScopedLock (writeLock);

			  for ( int ch = 0; ch < numberOfFiles; ch++ )
			  {
				audioFormatReaders[ch]->read (	channelBuffer[ch]	,			//  AudioFormatReader *   	 reader
												firstBlock*numberOfSamples,		//	const int  	startSample,
												availableBlocks*numberOfSamples,//	const int  	numSamples,
												readOffset,						//	const int  	readerStartSample,
												true,							//	const bool  useReaderLeftChan,
												false							//	const bool  useReaderRightChan
											 );
			  }
				
			  readOffset += availableBlocks*numberOfSamples;

			  // Mark blocks as available for playback
			  for (int i=0; i<availableBlocks; i++)
				  dataValid[firstBlock + i] = true;
			
			} // lock is released here

			
		 } // else
	  } // while

	MchaRecordPlayer::getInstance()->dbgOut("DataReader thread terminates."); 
	  // dataCriticalSection gets unlocked here.
}

}
