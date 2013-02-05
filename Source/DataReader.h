#ifndef MCHA_DATAREADER_H
#define MCHA_DATAREADER_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "settings.h"
#include "limits.h"

	#ifndef _I64_MAX
		#ifdef LLONG_MAX
			#define _I64_MAX LLONG_MAX
		#elif defined LONG_LONG_MAX
			#define _I64_MAX LONG_LONG_MAX
		#else
			#define _I64_MAX LONGLONG_MAX
		#endif
	#endif

namespace mcha
{





class DataReader:	public Thread
					/*public ChangeBroadcaster,
					public ActionBroadcaster*/

{
public:

	DataReader( OwnedArray<AudioFormatReader>	&formatReaders, 
				const int						samplesNumber, 
				const int						bufferLenghtInBlocks ) ;
	
	~DataReader();

	// Thread::run() member function
	void run();
	
	// Retrieve the next available block of audio samples (to be used by AudioIOCallback)
	float* getAudioData(const int sourceChannel);

	// Move to the next block
	void getNext();

	// Move back or forward
	bool jumpToPosition(int64 newPosition);

	juce_UseDebuggingNewOperator

private:

	void dataDump(String s1, float* ptr, int num);

	ReadWriteLock 	writeLock; // protect critical section by this lock

	volatile bool*	dataValid;			// Data-up-to-date flags

	int				ms2wait;			// sleep time in milliseconds
	int				numberOfSamples;	// Parameters of the output audio device: number Of Samples
	int				readOffset;			// Current offset in audio file
	int64			samplesCount;		// Number of samples in the shortest audio file	
	int				numberOfFiles;		// Number of audio files to be played (could be different from the number of channels

	int				playBlockPosition;	// The position of the next block in the circular buffer to be played
	int				bufferLength;		// The length of circular buffer in data blocks

	OwnedArray<AudioFormatReader>	&audioFormatReaders;
	OwnedArray<AudioSampleBuffer>	channelBuffer;
	AudioSampleBuffer*				emptyBuffer;

};

}

#endif
