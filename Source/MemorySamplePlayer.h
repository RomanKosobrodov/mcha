#ifndef MCHA_MEMORYSAMPLEPLAYER_H
#define MCHA_MEMORYSAMPLEPLAYER_H

#include "mchaRecordPlayer.h"
#include "AudioSampleProcessor.h"

namespace mcha
{

class MemorySamplePlayer : public AudioSamplePlayer
{
public:	
	MemorySamplePlayer( const float** outputData, const int outputChanNumber, const int64 outputSamplesNumber, const int* outChannels, const int outChannelsCount  );
	~MemorySamplePlayer();

	bool copyData(  float** outputChannelData, int totalNumOutputChannels, int numSamples  ) ;
	void startProcessing() {};
	void stopProcessing() {};
	bool isRunning();
	String getName();

	juce_UseDebuggingNewOperator

private:
	MchaRecordPlayer*		mchaRecordPlayer;
	float**					outputDataBuffer;

	int		bufferInBytes;
	int		samplesToCopy;
	int		channelIndex;
	float	channelGain;

	//==============================================================================
    // (prevent copy constructor and operator= being generated..)
	MemorySamplePlayer (const MemorySamplePlayer&);
	const MemorySamplePlayer& operator= (const MemorySamplePlayer&);

};

}

#endif