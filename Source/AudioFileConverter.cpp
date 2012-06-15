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
							File::separatorString + L"mCha\\mcha.log.txt";
	File	logger(loggerName);

	if (logger.existsAsFile())
		fileLogger = new FileLogger(logger, String::empty);

}

//==============================================================================
AudioFileConverter::~AudioFileConverter()
{
	jobQueue.clear();
	
	if (fileLogger != NULL)
		delete fileLogger;

	clearSingletonInstance();
}

//==============================================================================
void AudioFileConverter::run()
{
	
	while ( jobQueue.size() > 0 )
	{	
		task  = jobQueue[0];

		/* try opening the file */
		File	inputDataFile( task->getFileName() );
		String  inputFileName( inputDataFile.getFullPathName() );

		if ( !inputDataFile.existsAsFile() || (inputDataFile.getSize() == 0) )
		{
			dbgOut(L"Invalid or corrupted temporary file:\t" + inputFileName);
			jobQueue.remove(0, true);
			continue;
		}
	
		/* try creating the input stream */
		FileInputStream*	fileInputStream	=	inputDataFile.createInputStream();
		if (fileInputStream == NULL)
		{
			dbgOut(L"Unable to create input stream for file:\t" + inputFileName);
			jobQueue.remove(0, true);
			continue;
		}
		
		dbgOut(L"");
		dbgOut(L" ***  AudioFileConverter ***");
		dbgOut(L"Converting file:\t" + inputFileName);

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
				dbgOut( L"\tDeleting temporary file:\t" + tmpDataFile.getFullPathName() );
				tmpDataFile.deleteFile();
			}
			else
			{
				dbgOut( "Unable to delete temporary file:\t\t" + tmpDataFile.getFullPathName() );
			}

		
			// Define the format (wav is default)
			if (audioFormatName == "wav")
				someAudioFormats.add( new WavAudioFormat() );			
		
			else if (audioFormatName == "aiff")
				someAudioFormats.add( new AiffAudioFormat() );
		
			else if (audioFormatName == "flac")
				someAudioFormats.add( new FlacAudioFormat() );

			else if (audioFormatName == "ogg")
				someAudioFormats.add( new OggVorbisAudioFormat() );

			else
				someAudioFormats.add( new WavAudioFormat() );	
		
			audioFileName = outputDir + File::separatorString + "channel" + String::formatted("%.2d",i) + someAudioFormats[i]->getFileExtensions()[0];
		
			tmpAudioFile = new File (audioFileName);
			if (*tmpAudioFile == File::nonexistent)
			{
				dbgOut( L"Unable to create file:\t" + audioFileName );
				audioFormatWriters.clear(true);
				someAudioFormats.clear(true);
				audioFiles.clear(true);
				outStreams.clear();

				delete fileInputStream;
				
				jobQueue.remove(0, true);
				continue;
			}
		
			audioFiles.add( tmpAudioFile );

			// Delete existing files
			if (audioFiles[i]->existsAsFile())
			{
				dbgOut( "\tDeleting existing audio file:\t\t" + audioFileName );			
				if	(!audioFiles[i]->deleteFile())
				{
					dbgOut( L"Unable to delete existing file:\t" + audioFileName );
					audioFormatWriters.clear(true);
					someAudioFormats.clear(true);
					audioFiles.clear(true);
					outStreams.clear();
					delete fileInputStream;

					jobQueue.remove(0, true);
					continue;
				}
			}

			dbgOut( "\tSaving audio file:\t\t" + audioFileName );

			/* Create output stream for this file */
			tmpStream = audioFiles[i]->createOutputStream();
			if (tmpStream == NULL)
				{
					dbgOut( L"Unable to create output stream for file:\t" + audioFileName );
					delete tmpAudioFile;
					audioFormatWriters.clear(true);
					someAudioFormats.clear(true);
					audioFiles.clear(true);
					outStreams.clear();
					delete fileInputStream;

					jobQueue.remove(0, true);
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
					dbgOut( L"Unable to create audio format writer for:\t" + audioFileName );
					delete tmpAudioFile;
					audioFormatWriters.clear(true);
					someAudioFormats.clear(true);
					audioFiles.clear(true);
					outStreams.clear();
					delete fileInputStream;

					jobQueue.remove(0, true);
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

				tempBuffer.writeToAudioWriter  (	audioFormatWriters[ch],	//AudioFormatWriter *  writer,  
													0,						//const int  startSample,  
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
		dbgOut( L"\tDeleting temporary file:\t" + inputFileName );
		inputDataFile.deleteFile();


		// Delete the task
		jobQueue.remove(0, true);
		dbgOut( "Files saved." );

	}
}

}