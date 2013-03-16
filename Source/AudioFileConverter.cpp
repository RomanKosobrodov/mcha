#include "AudioFileConverter.h"

namespace mcha
{

juce_ImplementSingleton (AudioFileConverter)

//==============================================================================	
AudioFileConverter::AudioFileConverter()
	:	Thread( L"AudioFileConverter Thread" ),
		fileLogger(NULL)
{
	String	loggerName = File::getSpecialLocation(File::userApplicationDataDirectory).getFullPathName() +
							File::separatorString + "mcha" + File::separatorString + "mcha.log.txt";
	File	logger(loggerName);

	fileLogger = new FileLogger(logger, String::empty);
	if (fileLogger == NULL)
	{
		DBG("** AudioFileConverter ** Failed to create file logger.");
	}
}

//==============================================================================
AudioFileConverter::~AudioFileConverter()
{
	
	dbgOut("*** Deleting AudioFileConverter ***");

	if ( isThreadRunning() )	
	{
		dbgOut("** AudioFileConverter **  PANIC: The process is still running !!!");		
	}

	{ 	// Lock job queue and clear it
		const ScopedLock lock (queueLock);
		jobQueue.clear();
	}
	
	if (fileLogger != NULL)
		delete fileLogger;
	
	clearSingletonInstance();

	DBG("*** AudioFileConverter Deleted ***");
}

//==============================================================================
void AudioFileConverter::run()
{
	
	while ( getQueueSize() > 0 )
	{	
		{   // lock jobQueue before retrieving a task
			const ScopedLock lock (queueLock);
			task  = jobQueue[0];
		}

		/* try opening the file */
		File	inputDataFile( task->getFileName() );
		String  inputFileName( inputDataFile.getFullPathName() );

		if ( !inputDataFile.existsAsFile() || (inputDataFile.getSize() == 0) )
		{
			dbgOut(L"** AudioFileConverter ** Invalid or corrupted temporary file:\t" + inputFileName);
			removeFromQueue();
			continue;
		}
	
		/* try creating the input stream */
		FileInputStream*	fileInputStream	=	inputDataFile.createInputStream();
		if (fileInputStream == NULL)
		{
			dbgOut(L"** AudioFileConverter ** Unable to create input stream for file:\t" + inputFileName);
			removeFromQueue();
			continue;
		}
		
		dbgOut(L"");
		dbgOut(L" ***  AudioFileConverter ***");
		dbgOut(L"** AudioFileConverter ** Converting file:\t" + inputFileName 
               + L" (" + String( inputDataFile.getSize() ) + L" b)");

		int		processorOutputs = task->getChannelNumber();
		const int bytesPerSample = processorOutputs * sizeof(float);
		int		bufferSize		= task->getBufferSize();
		double	samplingRate	= task->getSamplingRate();
		int		bitDepth		= task->getBitDepth();
		String	audioFormatName = task->getFormat();

		AudioSampleBuffer tempBuffer(1, bufferSize);

		// declare classes needed to save the format
		OwnedArray<AudioFormat>			someAudioFormats;
		OwnedArray<AudioFormatWriter>	audioFormatWriters;
		OwnedArray<File>				audioFiles;
		Array<FileOutputStream*>		outStreams;
		String							audioFileName;

		AudioFormatWriter*	tmpWriter;
		FileOutputStream*	tmpStream;
		File*				tmpAudioFile;

		String outputDir = inputDataFile.getParentDirectory().getFullPathName();
	
		for (int i=0; i < processorOutputs ; i++)
		{
			// Delete temporary files
			File tmpDataFile(outputDir + File::separatorString + L"channel" + String::formatted("%.2d", i ) + ".dat");

			if ( tmpDataFile != File::nonexistent)
			{
				dbgOut( L"** AudioFileConverter ** \tDeleting temporary file:\t" + tmpDataFile.getFullPathName() );
				tmpDataFile.deleteFile();
			}
			else
			{
				dbgOut( "** AudioFileConverter ** Unable to delete temporary file:\t\t" + tmpDataFile.getFullPathName() );
			}

		
			// Define the format (wav is default)
			if (audioFormatName == "wav")
				someAudioFormats.add( new WavAudioFormat() );			
		
			else if (audioFormatName == "aiff")
				someAudioFormats.add( new AiffAudioFormat() );
		
			else if (audioFormatName == "flac")
				someAudioFormats.add( new FlacAudioFormat() );

//			else if (audioFormatName == "ogg")
//				someAudioFormats.add( new OggVorbisAudioFormat() );

			else
				someAudioFormats.add( new WavAudioFormat() );	
		
			audioFileName = outputDir + File::separatorString + "channel" + String::formatted("%.2d",i) + someAudioFormats[i]->getFileExtensions()[0];
		
			tmpAudioFile = new File (audioFileName);
			if (*tmpAudioFile == File::nonexistent)
			{
				dbgOut( L"** AudioFileConverter ** Unable to create file:\t" + audioFileName );
				audioFormatWriters.clear(true);
				someAudioFormats.clear(true);
				audioFiles.clear(true);
				outStreams.clear();

				delete fileInputStream;
				
				removeFromQueue();

				continue;
			}
		
			audioFiles.add( tmpAudioFile );

			// Delete existing files
			if (audioFiles[i]->existsAsFile())
			{
				dbgOut( "** AudioFileConverter ** \tDeleting existing audio file:\t\t" + audioFileName );			
				if	(!audioFiles[i]->deleteFile())
				{
					dbgOut( L"** AudioFileConverter ** Unable to delete existing file:\t" + audioFileName );
					audioFormatWriters.clear(true);
					someAudioFormats.clear(true);
					audioFiles.clear(true);
					outStreams.clear();
					delete fileInputStream;

					removeFromQueue();

					continue;
				}
			}

			dbgOut( "** AudioFileConverter ** \tSaving audio file:\t\t" + audioFileName );

			/* Create output stream for this file */
			tmpStream = audioFiles[i]->createOutputStream();
			if (tmpStream == NULL)
				{
					dbgOut( L"** AudioFileConverter ** Unable to create output stream for file:\t" + audioFileName );
					delete tmpAudioFile;
					audioFormatWriters.clear(true);
					someAudioFormats.clear(true);
					audioFiles.clear(true);
					outStreams.clear();
					delete fileInputStream;

					removeFromQueue();

					continue;
				}

			outStreams.add( tmpStream );
		

			/* Create Audio Format Writer */
			tmpWriter = someAudioFormats[i]->createWriterFor(	outStreams[i],		// streamToWriteTo,
																			samplingRate,		// sampleRateToUse,  
																			1,					// numberOfChannels,  
																			someAudioFormats[i]->getPossibleBitDepths().getLast(),	// bitsPerSample - Get the maximum possible bit depth for this format
																			NULL,				//  metadataValues,  
																			0 );


			if (tmpWriter == NULL)
			{
					dbgOut( L"** AudioFileConverter ** Unable to create audio format writer for:\t" + audioFileName );
					delete tmpAudioFile;
					audioFormatWriters.clear(true);
					someAudioFormats.clear(true);
					audioFiles.clear(true);
					outStreams.clear();
					delete fileInputStream;

					removeFromQueue();
		
					continue;
			}
			audioFormatWriters.add( tmpWriter );
		}

		// Write data to wav file
		int dataBlockSize = processorOutputs * bufferSize * bitDepth/8 ;
		MemoryBlock*	buffer = new MemoryBlock( dataBlockSize, true);
	
		int64 bytesSaved = inputDataFile.getSize();

		while ( !fileInputStream->isExhausted() && (fileInputStream->getPosition() <  bytesSaved) )
		{
			float* x = (float *) buffer->getData() ;

			int bytesRead = fileInputStream->read( (void *)x, dataBlockSize );
			int numSamples = (int)( bytesRead / bytesPerSample );

			for (int ch=0; ch < processorOutputs; ch++)
			{
//				const int numBytes = (int) (bytesRead/processorOutputs);

				tempBuffer.copyFrom(	0,					//  const int   	 destChannel,
										0,					//	const int  	destStartSample,
										x+ch*numSamples,	//	const float *  	source,
										numSamples			//	int  	numSamples	 
									);

				audioFormatWriters[ch]->write(	(const int**)(tempBuffer.getArrayOfChannels()),	//AudioFormatWriter *  writer,  
												numSamples				//const int  numSamples   
											  );
			}
		}

		// clean up
		delete	buffer;

		//	this should delete 'owned' objects 
		audioFormatWriters.clear(true);
		someAudioFormats.clear(true);
		audioFiles.clear(true);
		// clear the outStreams without deleting objects (already deleted)
		outStreams.clear();
	
		// Delete and close the stream
		delete fileInputStream;	

		// Delete the data.dat file
		dbgOut( L"** AudioFileConverter ** \tDeleting temporary file:\t" + inputFileName );
		inputDataFile.deleteFile();

		// Delete the task
		removeFromQueue();		
		
		dbgOut( "** AudioFileConverter ** Files saved." );

	}

	dbgOut( "** AudioFileConverter ** Thread terminates." );

}

} // end of namespace
