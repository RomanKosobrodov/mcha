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
#include "DataWriter.h"

namespace mcha
{

// =================================================================================================================
DataWriter::DataWriter( String		outputFile, 
					   const int	channelsNumber, 
					   const int	samplesNumber, 
					   const int	numberOfBytesInSample, 
					   const int	bufferLenghtInBlocks,
					   const int64	endBuffer) :	
															numberOfChannels(channelsNumber),
															dataBlockSize(channelsNumber * samplesNumber * numberOfBytesInSample),
															bytesPerSample(numberOfBytesInSample),
															dataSource(NULL),
															copyBlockPosition(0),
															bufferLength(bufferLenghtInBlocks),
															outputFileName(outputFile),
															isLogEnabled(false),
															endDiskBuffer(endBuffer),
															curDiskBuffer(0),
															currentStreamPosition(0),
															Thread( "DataWriter Thread" )
{
	dataMemoryBlock = new MemoryBlock( dataBlockSize * bufferLength, true );
	dataValid = new bool[bufferLength];
	
	// Initialise dataValid and set it to false
	for (int i=0; i<bufferLength; i++)
		dataValid[i] = false;

	mchaRecordPlayer = MchaRecordPlayer::getInstance();

	// Overwrite the file if it exists
	outputDataFile	=	new File(outputFileName);
	if (outputDataFile->existsAsFile())
		outputDataFile->deleteFile();	

	fileOutputStream = outputDataFile->createOutputStream();

	// set sleep time to half buffer duration
	int bufSize, bDepth, chIn, chOut;
	double sampleFreq;
	mchaRecordPlayer->getDeviceSettings().getSettings(sampleFreq, bufSize,  bDepth, chIn, chOut);

	ms2wait = (int) ( floor(bufSize*1000/sampleFreq) / 2 );

}

// =================================================================================================================
DataWriter::~DataWriter()
{
	delete [] dataValid;
	delete dataMemoryBlock;	

	// Close data file stream
	delete fileOutputStream;

	// delete the temporary data file from disk
/*	mchaRecordPlayer->dbgOut( "DataWriter:\tDeleting temporary file:\t" + outputDataFile->getFileName() + "\t size in bytes:\t" + String(outputDataFile->getSize()));
	if (!outputDataFile->deleteFile())
		mchaRecordPlayer->dbgOut( "\t\tTemporary file:\t" + outputDataFile->getFileName() + " was not deleted." );
*/
	delete outputDataFile;	
}


// =================================================================================================================
void DataWriter::saveNext()
{
	const int bytesPerChannel = numberOfSamples*bytesPerSample;
//	mchaRecordPlayer->debugBuffer(dataValid, bufferLength, L"dataValid");
	
	for (int ch=0; ch < numberOfChannels; ch++)
	{	
		dataMemoryBlock->copyFrom((const void*)(dataSource[ch]), copyBlockPosition * dataBlockSize + ch * bytesPerChannel, bytesPerChannel);
	}
	
	dataValid[copyBlockPosition] = true;	// mark the block as valid
	
	copyBlockPosition++;
	copyBlockPosition = copyBlockPosition % bufferLength;	// move to the next block

}


// =================================================================================================================
void DataWriter::setDataSource(const float** newSource, const int channelsNumber, const int samplesNumber)
{
	dataSource = (float**) newSource;
	numberOfChannels = channelsNumber;
	numberOfSamples	= samplesNumber;
	dataBlockSize	= channelsNumber * samplesNumber * bytesPerSample;
}


// =================================================================================================================
void DataWriter::run()
{
	Time now(Time::getCurrentTime());

	mchaRecordPlayer->dbgOut( "DataWriter started\t"  + now.toString(false, true, true, true) + ":" + String(now.getMilliseconds()) );

	if (fileOutputStream == NULL)
	{
		mchaRecordPlayer->logError( L"Unable to create output stream for\t" + outputFileName );
		return;
	}
	/* Reserve disk space */
	now = Time::getCurrentTime();
	int zeroByte = 0;
	if ( (!fileOutputStream->setPosition( endDiskBuffer*dataBlockSize )) || (!fileOutputStream->write((const void*) &zeroByte, 1)))
	{
		mchaRecordPlayer->logError( L"Failed to position output stream for data file:\t" + outputFileName + ". Insufficient disk space?" );
		return;
	}
	fileOutputStream->flush();

	/* Rewind to start */
	fileOutputStream->setPosition( 0 );

	RelativeTime timeDelay = Time::getCurrentTime() - now;
	mchaRecordPlayer->dbgOut( "\tReserving disk space for\t" + outputFileName + "\t\t" + String(endDiskBuffer*dataBlockSize) + " bytes\t" + String(timeDelay.inSeconds()) +  "s elapsed." );

	const ScopedLock dataScopedLock (dataCriticalSection);	  // this locks dataCriticalSection
	float *dat = NULL;

    while ( !threadShouldExit() && (curDiskBuffer < endDiskBuffer))	
	{
		  // Find the first available block
		  int firstBlock = -1;
		  for ( int ind = 0; ind < bufferLength; ind++ )
		  {
			  if (dataValid[ind])
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
		  
		  // Count the number of valid blocks (at least one is available)
		  int availableBlocks = 0;
		  for ( int ind = firstBlock; ind < bufferLength; ind++ )
		  {
			  if (dataValid[ind])
				  availableBlocks++;
			  else
				  break;
		  }

		  // Write all available blocks		  
		  dat = (float *)dataMemoryBlock->getData() + firstBlock * numberOfSamples * numberOfChannels;
		  
		  if ( endDiskBuffer - curDiskBuffer > availableBlocks )
		  {
			  if ( !fileOutputStream->write( (void*) dat, dataBlockSize * availableBlocks ) )
			  {
				  mchaRecordPlayer->logError( L"\tDataWriter error: Disk write failed" );
				  mchaRecordPlayer->dbgOut( "\t\tcurDiskBuffer = " + String(curDiskBuffer) );
				  mchaRecordPlayer->dbgOut( "\t\tavailableBlocks = " + String(availableBlocks) );
				  mchaRecordPlayer->dbgOut( "\t\tdataBlockSize = " + String(dataBlockSize) );

				  /* terminate the thread */
				  this->threadShouldExit();
				  break;
			  }
		  }
		  else
		  {
			  if ( !fileOutputStream->write( (void*) dat,  dataBlockSize * (int)(endDiskBuffer - curDiskBuffer) ) )
			  {
					mchaRecordPlayer->logError( L"\tDataWriter error: Disk write failed" );
					mchaRecordPlayer->dbgOut( "\t\tcurDiskBuffer = " + String(curDiskBuffer) );
					mchaRecordPlayer->dbgOut( "\t\tavailableBlocks = " + String(availableBlocks) );
					mchaRecordPlayer->dbgOut( "\t\tdataBlockSize = " + String(dataBlockSize) );
					/* terminate the thread */
					this->threadShouldExit();
					break;
			  }
		  }

		  curDiskBuffer += availableBlocks;
		  currentStreamPosition = fileOutputStream->getPosition();

		  // Mark the blocks as available for copying data
		  for ( int ind = 0; ind < availableBlocks; ind++ )
			dataValid[firstBlock + ind] = false;
		  
	  }

	fileOutputStream->flush();
	now = Time::getCurrentTime();
	mchaRecordPlayer->dbgOut( "DataWriter thread terminates\t"  + now.toString(false, true, true, true) + ":" + String(now.getMilliseconds()) );

	// dataCriticalSection gets unlocked here.
}

// =================================================================================================================
int64 DataWriter::getBytesSaved()
{
	return currentStreamPosition;
}

}
