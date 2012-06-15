#ifndef MCHA_DISKSAMPLEPLAYER_H
#define MCHA_DISKSAMPLEPLAYER_H

#include "mchaRecordPlayer.h"
#include "AudioSampleProcessor.h"
#include "DataReader.h"
#include "limits.h"

namespace mcha
{

class DataReader; // forward declaration

class DiskSamplePlayer : public AudioSamplePlayer
{
public:	
	DiskSamplePlayer( const char**  audioFiles, const int filesNumber, const int* outChannels, const int outChannelsCount );
	~DiskSamplePlayer();

	bool copyData(  float** outputChannelData, int totalNumOutputChannels, int numSamples  ) ;
	void startProcessing();
	void stopProcessing();
	bool setPosition(int64 newPosition);
	bool isRunning();
	String getName() { return "DiskSamplePlayer"; };
	
	juce_UseDebuggingNewOperator

private:

	MchaRecordPlayer*		mchaRecordPlayer;
	DataReader*				dataReader;
	
	AudioFormatManager				audioFormatManager;
	OwnedArray<AudioFormatReader>	audioFormatReaders;
	StringArray						audioFileNames;

	int		channelIndex;
	int		bufferInBytes;
	float	channelGain;

	//==============================================================================
    // (prevent copy constructor and operator= being generated..)
	DiskSamplePlayer (const DiskSamplePlayer&);
	const DiskSamplePlayer& operator= (const DiskSamplePlayer&);

};

}

#endif