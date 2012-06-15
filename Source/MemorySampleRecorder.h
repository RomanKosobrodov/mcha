#ifndef MCHA_MEMORYSAMPLERECORDER_H
#define MCHA_MEMORYSAMPLERECORDER_H

#include "mchaRecordPlayer.h"
#include "AudioSampleProcessor.h"

namespace mcha
{

class MemorySampleRecorder : public AudioSampleRecorder
{
public:	
	MemorySampleRecorder ( float** inputData, const int inputChanNumber, const int64 inputSamplesNumber, const int *inChannels, const int inChannelsCount );
	~MemorySampleRecorder ();

	bool copyData( const float** inputChannelData,	int totalNumInputChannels, int numSamples);
	void startProcessing() {};
	void stopProcessing() {};
	bool isRunning()  { return currentPosition < dataLength; }; 
	String getName() { return "MemorySampleRecorder"; };
	String getOutputDirectory() { return String::empty; };
	
	juce_UseDebuggingNewOperator

private:
	MchaRecordPlayer*		mchaRecordPlayer;
	float**					inputDataBuffer;
	int						samplesToCopy;
	int						sourceChannel;

	//==============================================================================
    // (prevent copy constructor and operator= being generated..)
	MemorySampleRecorder (const MemorySampleRecorder&);
	const MemorySampleRecorder& operator= (const MemorySampleRecorder&);
};

}

#endif