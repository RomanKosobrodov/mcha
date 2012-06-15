#ifndef MCHA_AUDIOFILECONVERTER_H
#define MCHA_AUDIOFILECONVERTER_H

#include "mchaRecordPlayer.h"

namespace mcha
{
	
class ConverterTask
{
public:
	ConverterTask(String fileName, int channelCount, String aFormat,  AudioDeviceSettings&	deviceSettings)
		:	dataFileName(fileName),
			channelNumber(channelCount),
			formatName(aFormat)
	{	
		int chIn, chOut;
		deviceSettings.getSettings(deviceSamplingRate, deviceBufferSize, deviceBitDepth, chIn, chOut);
	}
	~ConverterTask() {};

	String	getFileName() const			{ return dataFileName; };
	String	getFormat()	const			{ return formatName; };
	int		getChannelNumber() const	{ return channelNumber; };
	int		getBufferSize()	const		{ return deviceBufferSize; };
	double	getSamplingRate() const		{ return deviceSamplingRate; };
	int		getBitDepth() const			{ return deviceBitDepth; };

private:
	String	dataFileName;
	String	formatName;
	int		channelNumber;
	int		deviceBufferSize;
	double	deviceSamplingRate;
	int		deviceBitDepth;
};


class AudioFileConverter:	public Thread
{
public:
	AudioFileConverter();
	~AudioFileConverter();

	void run();
	void addTask(const ConverterTask	*newTask) 
	{  
		jobQueue.add(newTask);
		if (!isThreadRunning())
		{
			startThread();
		}
	};

	void	dbgOut(const String& msg)
	{
		if (fileLogger != NULL)
			fileLogger->logMessage(msg);
	}

	juce_UseDebuggingNewOperator

private:

	OwnedArray<ConverterTask>	jobQueue;
	ConverterTask*				task;
	FileLogger*					fileLogger;

	//==============================================================================
    // (prevent copy constructor and operator= being generated..)
	AudioFileConverter (const AudioFileConverter&);
	const AudioFileConverter& operator= (const AudioFileConverter&);

public:
	juce_DeclareSingleton (AudioFileConverter, true)
};

}

#endif