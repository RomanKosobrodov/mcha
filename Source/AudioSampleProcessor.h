#ifndef AUDIOSAMPLEPROCESSOR_H
#define AUDIOSAMPLEPROCESSOR_H

#include "../JuceLibraryCode/JuceHeader.h"
#include "mchaRecordPlayer.h"
#include "Filter/mchaFilter.h"


namespace mcha
{
	
class MchaRecordPlayer;	// forward declaration
class MchaFilter;	// forward declaration

/* ---------------------------------------------------------- */
class AudioProcessorBase
{
public:
	AudioProcessorBase() :	dataLength(0),
							currentPosition(0),
							processorInputs(0),
							processorOutputs(0)
	{};
	
	AudioProcessorBase( int64	dLength,
						int64	curPosition,
						int		pInputs,
						int		pOutputs) :
											dataLength(dLength),
											currentPosition(curPosition),
											processorInputs(pInputs),
											processorOutputs(pOutputs)
	{};

	virtual ~AudioProcessorBase () {};

	virtual void startProcessing() = 0;
	virtual void stopProcessing() =0;
	virtual bool isRunning() = 0;
	virtual String getName() = 0;		

	virtual int getInputsNumber() { return processorInputs; };
	virtual int getOutputsNumber() { return processorOutputs; };
	virtual	int64 getDataLength() { return dataLength; };
	virtual int64 getCurrentPosition() {return currentPosition; };

	virtual void resetChannelsMap(int channelsCount)
	{
		channelsMap.clear();
		for (int i=0; i<channelsCount; i++)
			channelsMap.add(i);
	}
	
	virtual Array<int>*	getChannels() { return &channelsMap; }

	juce_UseDebuggingNewOperator

protected:
	Array<int>		channelsMap;
	int64			dataLength;
	int64			currentPosition;
	int				processorInputs;
	int				processorOutputs;
};

/* ---------------------------------------------------------- */
class AudioSampleRecorder : public AudioProcessorBase
{
public:
	AudioSampleRecorder() {};
	AudioSampleRecorder(int64	dLength,
						int64	curPosition,
						int		pInputs,
						int		pOutputs) :
											AudioProcessorBase(dLength, curPosition, pInputs, pOutputs)
	{};

	virtual ~AudioSampleRecorder () {}; 

	virtual bool copyData( const float** inputChannelData,	int totalNumInputChannels, int numSamples) = 0 ;
	virtual String getOutputDirectory() = 0;

	virtual Array<int>* getDeviceChannelsMap() { return &deviceChannelsMap; }

	juce_UseDebuggingNewOperator

protected:
	Array<int>		deviceChannelsMap;

};

/* ---------------------------------------------------------- */
class AudioSamplePlayer : public AudioProcessorBase
{
public:
	AudioSamplePlayer() {};

	AudioSamplePlayer( int64	dLength,
						int64	curPosition,
						int		pInputs,
						int		pOutputs) :
											AudioProcessorBase(dLength, curPosition, pInputs, pOutputs)
	{};

	virtual ~AudioSamplePlayer () 
	{
		outputGains.clear();
		channelsMap.clear();
	};

	virtual bool copyData (  float** outputChannelData, int totalNumOutputChannels, int numSamples  ) = 0 ;

	virtual bool setPosition(int64 newPosition);
	virtual bool setGain (int *channels, int channelsCount, float *gains);
	virtual bool setGain (Array<int> channels, Array<float> gains);
	virtual Array<float>* getGain() { return &outputGains; };

	juce_UseDebuggingNewOperator

protected:
	void initGains();
	Array<float>			outputGains;

private:
	int findElement(int value);
};

/* ---------------------------------------------------------- */
class EmptySampleRecorder : public AudioSampleRecorder
{
public:
	EmptySampleRecorder() {};
	virtual ~EmptySampleRecorder() {};

	bool copyData( const float** inputChannelData,	int totalNumInputChannels, int numSamples); 
	void startProcessing() {};
	void stopProcessing() {};
	bool isRunning() { return false; };
	String getName() { return L"EmptySampleRecorder"; };	
	String getOutputDirectory() { return String::empty; };

	juce_UseDebuggingNewOperator
};

/* ---------------------------------------------------------- */
class EmptySamplePlayer: public AudioSamplePlayer
{
public:
	EmptySamplePlayer(){};
	virtual ~EmptySamplePlayer() {};

	bool copyData(  float** outputChannelData, int totalNumOutputChannels, int numSamples  );
	void startProcessing() {};
	void stopProcessing() {};
	bool isRunning() { return false; };
	String getName() { return L"EmptySamplePlayer"; };	
	bool setGain (int *, int , float *) {return true;};
	bool setGain (Array<int> channels, Array<float> gain) {return true;};

	juce_UseDebuggingNewOperator
};

/* ---------------------------------------------------------- */
class AudioSampleProcessor: public AudioIODeviceCallback,
							 public ChangeBroadcaster
{ 
public:

	AudioSampleProcessor();
	~AudioSampleProcessor();

	/* Set recorder and player objects. The objects will be deleted at thread termination */
	void setRecorder(AudioSampleRecorder* recorder) { audioSampleRecorder = recorder; };
	void setPlayer (AudioSamplePlayer* player) { audioSamplePlayer = player; };
	void setDebugRecorder(AudioSampleRecorder*	debugRec) { debugRecorder = debugRec; };
	
	/* Define input and output audio processors*/
	void setInputProcessor(MchaFilter* inpProc) {inputProcessor = inpProc; };
	void setOutputProcessor(MchaFilter* outProc) {outputProcessor = outProc; };

	/* Implementation of the AudioIODeviceCallback method. */
	virtual void audioDeviceIOCallback (const float** inputChannelData,
								int totalNumInputChannels,
								float** outputChannelData,
								int totalNumOutputChannels,
								int numSamples);

	/* Implementation of the AudioIODeviceCallback method. */
	virtual void audioDeviceAboutToStart (AudioIODevice* device);

	/* Implementation of the AudioIODeviceCallback method. */
	virtual void audioDeviceStopped();

	bool	isStopped () { return deviceStopped; }

//==============================================================================
	juce_UseDebuggingNewOperator

protected:

	MchaRecordPlayer*		mchaRecordPlayer;

	AudioSampleRecorder*	audioSampleRecorder;
	AudioSamplePlayer*		audioSamplePlayer;
	AudioSampleRecorder*	debugRecorder;

	MchaFilter*			inputProcessor;
	MchaFilter*			outputProcessor;

	/* total number of audio buffers to be processed */
	int64				totalAudioLength;
	int64				currentAudioPosition;

	/* temporary input and output signal buffers */
	float**				tempBufferIn;
	float**				tempBufferOut;
	int					inputProcessorChannels;
	int					outputProcessorChannels;
	int					debugChannelCount;

	bool				deviceStopped;

private:
	//==============================================================================
    // (prevent copy constructor and operator= being generated.)
	const AudioSampleProcessor& operator= (const AudioSampleProcessor&);

};

}

#endif

