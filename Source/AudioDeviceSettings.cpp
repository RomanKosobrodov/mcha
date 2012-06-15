/*  
	MCHA - Multichannel Audio Playback and Recording Library
    Copyright (C) 2011  Roman Kosobrodov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "AudioDeviceSettings.h"

namespace mcha
{

//=========================================================================================================
 bool AudioDeviceSettings::init() 
{ 
	if (settingsApplied)
		return true;
	else
		return  readSettings(); 
}

//=========================================================================================================
bool AudioDeviceSettings::readSettings() 
{
	lastResult = Result::ok();

	if (!settingsApplied)
	{

		File fs( xmlSettings );
		if ( !fs.existsAsFile() )
		{
			lastResult = Result::fail( L"Unable to open settings file:\t" + xmlSettings); 
			return false;
		}
		
		XmlDocument myDocument (fs);
		
		if (deviceSettings != NULL) // clear previous settings
			delete deviceSettings;
		
		deviceSettings = myDocument.getDocumentElement();

		if (deviceSettings == NULL)
		{
			lastResult = Result::fail( L"Unable to open settigns file:\t" + xmlSettings);
			return false;
		}
		else
		{
			sampleRate	= deviceSettings->getIntAttribute( "audioDeviceRate", -1 );
			if (sampleRate == -1)
			{
				lastResult = Result::fail( L"Incorrect parameter: audioDeviceRate ");
			}
			
			bufferSize	= deviceSettings->getIntAttribute( "audioDeviceBufferSize", -1 );
			if (bufferSize == -1)
			{
				lastResult = Result::fail( lastResult.getErrorMessage() + L"Incorrect parameter: audioDeviceBufferSize ");
			}
			bitDepth	= 32;
			
			inputChannelsStr = deviceSettings->getStringAttribute( "audioDeviceInChans", String::empty );
			inputChannels = inputChannelsStr.length();
			if ( inputChannelsStr.isEmpty() )
			{
				lastResult = lastResult = Result::fail( lastResult.getErrorMessage() + L"Incorrect parameter: audioDeviceInChans " );
			}

			outputChannelsStr = deviceSettings->getStringAttribute( "audioDeviceOutChans", String::empty );
			outputChannels = outputChannelsStr.length();
			if ( outputChannelsStr.isEmpty() )
			{
				lastResult = lastResult = Result::fail( lastResult.getErrorMessage() + L"Incorrect parameter: audioDeviceOutChans ");
			}
		}
	}
	
	if ( lastResult.failed() )
	{
		settingsApplied = false;
		delete deviceSettings;
		deviceSettings = NULL;
		return false;
	}
	else
	{
		settingsApplied = true;
		return true;
	}

}

// ============================================================================================
bool AudioDeviceSettings::setXmlSettingsFile(String xmlSettingsFile) 
{ 
	xmlSettings = xmlSettingsFile;
	settingsApplied = false;
	return readSettings();
};

// ============================================================================================
 bool AudioDeviceSettings::getSettings(double &samplRate,int &bufSize, int &bDepth, int &inChannels, int &outChannels) 
{
	if (settingsApplied)
	{
		samplRate = sampleRate;
		bufSize = bufferSize;
		bDepth = bitDepth;
		inChannels = inputChannels;
		outChannels = outputChannels;
		return true;
	}
	else
	{
		if ( readSettings() )
		{
			samplRate = sampleRate;
			bufSize = bufferSize;
			bDepth = bitDepth;
			inChannels = inputChannels;
			outChannels = outputChannels;
			return true;
		}
		else
		{
			samplRate = -1.0;
			bufSize = -1;
			bDepth = -1;
			inChannels = -1;
			outChannels = -1;
			return false;
		}
	}

}

// ============================================================================================
XmlElement*	AudioDeviceSettings::getXmlDeviceSettings()
{
	if (!settingsApplied)
		readSettings();

	return deviceSettings;
}

// ============================================================================================
XmlElement* AudioDeviceSettings::createXmlSettings(AudioDeviceManager* adm)
{
	XmlElement* res;
	
	try
	{
		res = new XmlElement ("DEVICESETUP");
	}
	catch (std::bad_alloc)
	{
		return NULL;
	}

	AudioDeviceManager::AudioDeviceSetup	adSetup;
	adm->getAudioDeviceSetup(adSetup);

	res->setAttribute ("deviceType", adm->getCurrentAudioDeviceType());
	res->setAttribute ("audioOutputDeviceName", adSetup.outputDeviceName);
	res->setAttribute ("audioInputDeviceName", adSetup.inputDeviceName);
	res->setAttribute ("audioDeviceRate", adSetup.sampleRate);
	res->setAttribute ("audioDeviceBufferSize", adSetup.bufferSize);
	res->setAttribute ("audioDeviceInChans", adSetup.inputChannels.toString (2));
	res->setAttribute ("audioDeviceOutChans", adSetup.outputChannels.toString (2));

	return res;
}

}// end of namespace defs