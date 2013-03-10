#ifndef MCHA_AUDIODEVICESETTINGS_H
#define MCHA_AUDIODEVICESETTINGS_H

#include "../JuceLibraryCode/JuceHeader.h"
//#include "mchaRecordPlayer.h"

namespace mcha
{

// ============================================================================================
class AudioDeviceSettings
{
public:
	AudioDeviceSettings():	sampleRate(-1.0),
							bufferSize(-1),
							bitDepth(-1),
							inputChannels(-1),
							inputChannelsStr(String::empty),
							outputChannels(-1),
							outputChannelsStr(String::empty),
							xmlSettings(String::empty),
							settingsApplied(false),
							deviceSettings(NULL),
							lastResult( Result::ok() )
	{
	};

	~AudioDeviceSettings() 
	{
		delete deviceSettings;
	};
	

	String	getErrorString() const
	{ 
		return lastResult.getErrorMessage();
	};

	bool	init();
	
	XmlElement* createXmlSettings(AudioDeviceManager* adm);


	bool	getSettings(double &samplRate,int &bufSize, int &bDepth, int &inChannels, int &outChannels) ;

	String	getXmlSettingsFile() const { return xmlSettings; }
	bool	setXmlSettingsFile(String xmlSettingsFile);

	String	getInputChannelsStr() {return inputChannelsStr;}
	String	getOutputChannelsStr() {return outputChannelsStr;}

	int		getBufferSize() const { return bufferSize; };

	XmlElement*	getXmlDeviceSettings();

	juce_UseDebuggingNewOperator

private:
	int			errorStatus;	
	bool		settingsApplied;
	double		sampleRate;
	int			bufferSize;
	int			bitDepth;
	int			inputChannels;
	String		inputChannelsStr;
	int			outputChannels;
	String		outputChannelsStr;

	String		xmlSettings;
	XmlElement*	deviceSettings;
	Result		lastResult;	
	
	bool	readSettings();
};

}

#endif
