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

#include "DiskSampleRecorder.h"
#include "AudioFileConverter.h"

namespace mcha
{

DiskSampleRecorder::DiskSampleRecorder ( const char* recordDir, const int* inChannels, const int inChannelsCount, float duration )
							:	AudioSampleRecorder( 0, 0, inChannelsCount, inChannelsCount),
								mchaRecordPlayer( MchaRecordPlayer::getInstance() ),
								dataWriter(NULL),
								outputDir(String::empty),
								bytesSaved(0),
								endBuffer(0),
								samplingRate(-1.0f), 
								bufferSize(-1),
								bitDepth(-1)
{
	mchaRecordPlayer->dbgOut( "DiskSampleRecorder created" );	
	mchaRecordPlayer->dbgOut( "\tRecorder input channels:\t" + String(processorInputs) );
	mchaRecordPlayer->dbgOut( "\tRecorder output channels:\t" + String(processorOutputs) );		

	int inChans = -1;
	int outChans = -1;
	if ( !mchaRecordPlayer->getDeviceSettings().getSettings(samplingRate, bufferSize, bitDepth, inChans, outChans) )
	{
		mchaRecordPlayer->logError( L"\tDiskSampleRecorder::DiskSampleRecorder failed: unable to get device settings");
		mchaRecordPlayer->dbgOut( mchaRecordPlayer->getDeviceSettings().getErrorString() );
		return;
	}

	/* Create a copy of output directory */
	outputDir = String(recordDir);
	if ( !outputDir.endsWith(File::separatorString))
		outputDir += File::separatorString;

	/* Check if the directory exists and create it if required */
	File dirFile(outputDir);
	if (outputDir.isNotEmpty())
	{
		if ( !dirFile.isDirectory() )	/* either not a directory or does not exist */
		{
			if ( !dirFile.createDirectory() ) /* try create this directory */
			{
				mchaRecordPlayer->logError( L"\tOutput directory is invalid:\t" + outputDir );
				return;
			}
			else
			{
				mchaRecordPlayer->dbgOut( "\tDirectory created:\t" + outputDir );		
			}
		}
		else
		{
			mchaRecordPlayer->dbgOut("\tOutput directory:\t" + outputDir );		
		}
	}



	/* Check the number of input channels */
	if (processorInputs < 1)
	{
		mchaRecordPlayer->logError( L"\tWrong number of input channels (" + String(processorInputs) + ")" );
		return;	
	}

	/* Check the input channels array */
	if (inChannels == NULL)
	{
		mchaRecordPlayer->logError( L"\tInput channels are not specified" );
		return;
	}
	
	/* map device channels to data channels */
	channelsMap.insertArray(0, inChannels, inChannelsCount);

	/* Calculate the maximum available record length */
	int64	freeDiskBytes = dirFile.getBytesFreeOnVolume();
	mchaRecordPlayer->dbgOut( "\tFree disk space, b:\t" + String(freeDiskBytes) );

	/* Bytes per sample for one channel */
	int		bytesPerSample = (int) ceil( (double) bitDepth / 8); 
	
	/* Bytes per one buffer for one channel */
	int		bufferInBytes = bufferSize * bytesPerSample;

	/* The maximum number of buffers available for recording */
	int64	maxNumberOfBuffers =	( freeDiskBytes  - processorOutputs * MAX_FORMAT_HEADER ) / 
									( bufferInBytes*processorOutputs)/2;

	if (maxNumberOfBuffers < 1)
	{
		mchaRecordPlayer->logError( L"\tInsufficient disk space:\t" + String(freeDiskBytes) + " bytes free on drive " + dirFile.getVolumeLabel() );
		return ;
	}

	/* Maximum duration */
	double maxRecordDuration = maxNumberOfBuffers * bufferSize / samplingRate;
	RelativeTime tm( maxRecordDuration );
	mchaRecordPlayer->dbgOut( "\tMaximum available record duration:\t" + tm.getDescription() );


	if (duration < 0 )
	{
		endBuffer = maxNumberOfBuffers;
	}
	else
	{
		endBuffer = (int64) ceil( duration*samplingRate / bufferSize );
		if (endBuffer > maxNumberOfBuffers)
		{
			mchaRecordPlayer->logError( L"WARNING: Not enough disk space to save the files. Record duration has been decreased to " + 
											String(endBuffer*bufferSize/samplingRate) + " s." );
			endBuffer = maxNumberOfBuffers;
		}
	}
	
	/* Current record length */
	dataLength = endBuffer * bufferSize;
	double currentDuration =  dataLength / samplingRate;
	RelativeTime rectm(currentDuration);
	mchaRecordPlayer->dbgOut( "\tRecord duration:\t" + rectm.getDescription() );

	/* Reserve disk space for final audio files */
	int64 lengthInBytes = endBuffer * bufferSize * bytesPerSample + MAX_FORMAT_HEADER;
	if (!reserveDiskSpace(outputDir, lengthInBytes))
	{
		return ;
	}
	
	return ;

}

//==============================================================================
DiskSampleRecorder::~DiskSampleRecorder ()
{
}

//==============================================================================
bool DiskSampleRecorder::copyData( const float** inputChannelData,	int totalNumInputChannels, int numSamples)
{

	/* check if the device is running properly */
	if ((processorInputs > 0 ) && (totalNumInputChannels != processorOutputs))
	{
		/* return error string */
		mchaRecordPlayer->logError( L"DiskSampleRecorder::copyData failed:" );
		mchaRecordPlayer->dbgOut( "\tNumber of input data channels:\t" + String(totalNumInputChannels) );
		mchaRecordPlayer->dbgOut( "\tNumber of processor outputs:\t" + String(processorOutputs) );
		return false;
	}
	else
	{
		// Update pointers in datawriter
		dataWriter->setDataSource(inputChannelData, totalNumInputChannels, numSamples);
		// Tell datawriter to copy/save data
		dataWriter->saveNext();
		currentPosition += numSamples;
		return true;
	}
}

//==============================================================================
void DiskSampleRecorder::startProcessing()
{
	// Create dataWriter thread
	dataWriter	=	new DataWriter(outputDir + "data.dat", processorOutputs, bufferSize, sizeof(float), CIRCULAR_BUFFER_LENGTH, endBuffer);

	dataWriter->startThread();
}

//==============================================================================
void DiskSampleRecorder::stopProcessing()
{
	// Terminate DataReader
	dataWriter->signalThreadShouldExit();
	dataWriter->waitForThreadToExit(1000);
	
	bytesSaved = dataWriter->getBytesSaved();
	mchaRecordPlayer->dbgOut("Bytes saved to disk:\t" + String( bytesSaved ) );

	// Save audio files
	String datFileName = outputDir + L"data.dat";
	AudioFileConverter::getInstance()->addTask( new ConverterTask( datFileName, processorOutputs, L"wav" ,mchaRecordPlayer->getDeviceSettings() ) );

	delete dataWriter;
}

//==============================================================================
bool DiskSampleRecorder::isRunning()
{
	if (dataWriter == NULL)
		return false;
	else
		return dataWriter->isThreadRunning();
}

 // =================================================================================================================
 bool DiskSampleRecorder::reserveDiskSpace(String outDir, int64 lengthInBytes)
 {
	String		audioFileName(outDir);
	File*		tempDataFile;
	Time		now;

	if ( !audioFileName.endsWith(File::separatorString))
		audioFileName += File::separatorString;

	for (int i=0; i < processorOutputs; i++)
	{
		now = Time::getCurrentTime();

		tempDataFile = new File(audioFileName + "channel" + String::formatted("%.2d", i) + ".dat");

		if (*tempDataFile == File::nonexistent)
		{
			mchaRecordPlayer->logError( L"Failed to reserve disk space for data file:\t" + tempDataFile->getFullPathName() );
			delete tempDataFile;
			return false;		
		}
		else
		{
			FileOutputStream* tempStream = tempDataFile->createOutputStream();
			if (tempStream == NULL)
			{
				mchaRecordPlayer->logError( L"Failed to create output stream for data file:\t" + tempDataFile->getFullPathName() );
				delete tempStream;
				delete tempDataFile;
				return false;
			}
			else
			{
				if (!tempStream->setPosition(lengthInBytes))
				{
					mchaRecordPlayer->logError( L"Failed to position output stream for data file:\t" + tempDataFile->getFullPathName() + ". Insufficient disk space?" );
					delete tempStream;
					delete tempDataFile;
					return false;				
				}
				else
				{
					int zeroByte = 0;
					tempStream->write( (void *) &zeroByte, 1);
					tempStream->flush();
				}
			}
			RelativeTime timeDelay = Time::getCurrentTime() - now;
			mchaRecordPlayer->dbgOut( "\tReserving disk space for\t" + tempDataFile->getFullPathName() + "\t" + String(lengthInBytes) + " bytes\t" + String(timeDelay.inSeconds())+ " s elapsed." );
			delete tempStream;
			delete tempDataFile;			
		}

	}
	
	return true;
 }

}