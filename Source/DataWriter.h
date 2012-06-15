#ifndef MCHA_DATAWRITER_H
#define MCHA_DATAWRITER_H

#include "mchaRecordPlayer.h"

namespace mcha
{

class MchaRecordPlayer;

class DataWriter:	public Thread
{
public:

	DataWriter( String outputFile, const int	channelsNumber, const int samplesNumber, const int numberOfBytesInSample, const int	bufferLenghtInBlocks, const int64 endBuffer );
	
	~DataWriter();

	// Thread::run() member function
	void run();
	
	// Wrapper functions for MemoryBlock
	void setBufferLength(const int numberOfBlocks) { bufferLength = numberOfBlocks;}
	void setDataSource(const float** newSource, const int channelsNumber, const int samplesNumber);
	void saveNext();
	
	// The number of bytes saved in the temporary data file
	int64 DataWriter::getBytesSaved();
	
	juce_UseDebuggingNewOperator

private:
	CriticalSection dataCriticalSection;

	MchaRecordPlayer*	mchaRecordPlayer;

	MemoryBlock*	dataMemoryBlock;	// Storage for sampled data
	int				dataBlockSize;		// The length of data block in bytes
	volatile bool*	dataValid;			// Data-up-to-date flags

	float**			dataSource;
	int				numberOfChannels;
	int				numberOfSamples;
	int				bytesPerSample;
	int				ms2wait;			// sleep time in milliseconds

	int				copyBlockPosition;	// The current position of the next block in the circular buffer to be filled with samples
	int				bufferLength;		// The length of circular buffer in data blocks

	File*					outputDataFile;
	FileOutputStream*		fileOutputStream;
	String					outputFileName;

	bool			isLogEnabled;
	int64			endDiskBuffer;
	int64			curDiskBuffer;
	int64			currentStreamPosition;
};

}

#endif