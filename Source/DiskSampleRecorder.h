#ifndef MCHA_DISKSAMPLERECORDER_H
#define MCHA_DISKSAMPLERECORDER_H

#include "mchaRecordPlayer.h"
#include "AudioSampleProcessor.h"
#include "DataWriter.h"
#include "AudioFileConverter.h"

namespace mcha
{

class DataWriter; // forward declaration

class DiskSampleRecorder : public AudioSampleRecorder
{
public:	
	DiskSampleRecorder ( const char* recordDir, const int* inChannels, const int inChannelsCount, float duration );
	~DiskSampleRecorder ();

	bool copyData( const float** inputChannelData,	int totalNumInputChannels, int numSamples);
	void startProcessing();
	void stopProcessing();
	bool isRunning() ; 
	String getName() { return L"DiskSampleRecorder"; };
	String getOutputDirectory() { return String::empty; };

	/* Reserve disk space for audio files */
	bool reserveDiskSpace(String outDir, int64 lengthInBytes);

	juce_UseDebuggingNewOperator

private:
	MchaRecordPlayer*		mchaRecordPlayer;

	CriticalSection			readLock;
	DataWriter*				dataWriter;
	String					outputDir;
	int64					bytesSaved;
	int64					endBuffer;


	/* Current device settings */
	double	samplingRate; 
	int		bufferSize; 
	int		bitDepth; 

	//==============================================================================
    // (prevent copy constructor and operator= being generated..)
	DiskSampleRecorder (const DiskSampleRecorder&);
	const DiskSampleRecorder& operator= (const DiskSampleRecorder&);
};

}

#endif