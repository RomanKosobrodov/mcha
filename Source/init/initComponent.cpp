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

#include "initComponent.h"
#include "MchaSettingsComponent.h"

namespace mcha
{

//==============================================================================
InitComponent::InitComponent ()
    : tabbedComponent (0),
      finishButton (0),
	  deviceSelector(0)
{
    addAndMakeVisible (tabbedComponent = new TabbedComponent (TabbedButtonBar::TabsAtTop));
    tabbedComponent->setTabBarDepth (30);
	Colour tabBackground( Colours::aliceblue );

    addAndMakeVisible (finishButton = new TextButton (L"finish button"));
    finishButton->setButtonText (L"Finish");
    finishButton->addListener (this);
	
	deviceManager.initialise (2, 2, 0, true, String::empty, 0);
	
	
	/* Create the app data directory if required */
	File tmpDirFile( File::getSpecialLocation( File::userApplicationDataDirectory ).getFullPathName() + File::separatorString + L"mcha" );
	if ( !tmpDirFile.isDirectory() )
	{
		if ( !tmpDirFile.createDirectory() )
		{
			AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Error", L"Unable to create directory:\t" + tmpDirFile.getFullPathName(), L"OK", this);
			JUCEApplication::getInstance()->quit();
		}
	}

	File xmlFile = tmpDirFile.getChildFile(L"defaultsettings.xml");
	
	settingsFileName =	xmlFile.getFullPathName();

	if ( xmlFile.existsAsFile() )
	{	
		bool res = audioDeviceSettings.setXmlSettingsFile(settingsFileName);

		if (!res)
		{	
			AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Warning", audioDeviceSettings.getErrorString(), L"OK", this);
		}
	
	}
	else
	{
		String errorMsg( L"Unable to find settings file:\t");
		errorMsg += settingsFileName + "\n\nThe file will be created.";
		AlertWindow::showMessageBox( AlertWindow::InfoIcon, L"Warning", errorMsg, L"OK", this);
	}

	
	/* Initialise the device configuration tab */
	deviceManager.initialise (	MAXNUMBEROFDEVICECHANNELS,	/* number of input channels */
								MAXNUMBEROFDEVICECHANNELS,	/* number of output channels */
								audioDeviceSettings.getXmlDeviceSettings(),
								false				/* select default device on failure */ );

	deviceSelector = new AudioDeviceSelectorComponent (deviceManager, 0, 2, 0, 2, false, false, false, false);
    tabbedComponent->addTab (L"Audio Device Setup",tabBackground, deviceSelector, true);
    
	/* initialise subjects and stimuli configuration tab */
	mchaSettingsComponent = new MchaSettingsComponent();

	tabbedComponent->addTab (L"MCHA Settings", tabBackground, mchaSettingsComponent, true);
	tabbedComponent->setCurrentTabIndex (0);
    
//    setSize (800, 580);
    setSize (800, 550);
}

InitComponent::~InitComponent()
{
    deleteAndZero (tabbedComponent);
    deleteAndZero (finishButton);
}

//==============================================================================
void InitComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);
}

void InitComponent::resized()
{
   // tabbedComponent->setBounds (8, 16, 784, 488);
    tabbedComponent->setBounds (8, 16, 784, 448);
//    finishButton->setBounds (312, 536, 150, 24);
    finishButton->setBounds (312, 480, 150, 24);

}

void InitComponent::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == finishButton)
    {
		Result res =  saveConfig();
		if ( res.failed() )
		{
			AlertWindow::showMessageBox( AlertWindow::WarningIcon, L"Error occured", res.getErrorMessage(), L"OK", this);
		}
		else
		{
			// close the program
			JUCEApplication::getInstance()->quit();
		}

    }
}

Result InitComponent::saveConfig()
{
	AudioDeviceManager::AudioDeviceSetup	audioDeviceSetup;

	deviceManager.getAudioDeviceSetup(audioDeviceSetup);

	ScopedPointer<XmlElement> audioDeviceState ( deviceManager.createStateXml() );

	/* create audioDeviceState when no changes were applied to the device settings */
	if (audioDeviceState == NULL)
	{
		audioDeviceState = createXmlSettings(deviceManager);
	}

	/* Check if audioDeviceState is complete */
	String attrString;

	attrString = "audioDeviceRate";
	if ( !audioDeviceState->hasAttribute( attrString ) )
		audioDeviceState->setAttribute(attrString, audioDeviceSetup.sampleRate );
	
	attrString = "audioDeviceBufferSize";
	if ( !audioDeviceState->hasAttribute(attrString ) )
		audioDeviceState->setAttribute( attrString, audioDeviceSetup.bufferSize );

	attrString = "audioDeviceInChans";
	if ( !audioDeviceState->hasAttribute( attrString ) )
		audioDeviceState->setAttribute( attrString,  audioDeviceSetup.inputChannels.toString(2, 1) );

	attrString = "audioDeviceOutChans";
	if ( !audioDeviceState->hasAttribute( attrString ) )
		audioDeviceState->setAttribute( attrString,  audioDeviceSetup.outputChannels.toString(2, 1) );

	/* add more attributes to audioDeviceState */
	XmlElement*	mchaSettings = new XmlElement( "MCHASETTINGS" ); // this will be deleted automatically by the parent
	mchaSettings->setAttribute( "loggingLevel", mchaSettingsComponent->getLogLevel() );
	mchaSettings->setAttribute( "memoryMode", mchaSettingsComponent->getMemoryMode() );
	audioDeviceState->addChildElement(mchaSettings);


	File	settingsFile( settingsFileName );
	if (settingsFile == File::nonexistent)
	{
		return Result::fail( L"Settings file is absent" );;	
	}

	DBG( L"mchaInit:\n   Settings file created:\t" + settingsFile.getFileName() );

	if	( !audioDeviceState->writeToFile ( settingsFile, String::empty ) )
	{
		return Result::fail( L"Unable to save settings" );
	}

	deviceManager.closeAudioDevice();

	return Result::ok();
}

// ============================================================================================
XmlElement* InitComponent::createXmlSettings(AudioDeviceManager& adm)
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
	adm.getAudioDeviceSetup(adSetup);

	res->setAttribute ("deviceType", adm.getCurrentAudioDeviceType());
	res->setAttribute ("audioOutputDeviceName", adSetup.outputDeviceName);
	res->setAttribute ("audioInputDeviceName", adSetup.inputDeviceName);
	res->setAttribute ("audioDeviceRate", adSetup.sampleRate);
	res->setAttribute ("audioDeviceBufferSize", adSetup.bufferSize);
	res->setAttribute ("audioDeviceInChans", adSetup.inputChannels.toString (2));
	res->setAttribute ("audioDeviceOutChans", adSetup.outputChannels.toString (2));

	return res;
}

} // end of namespace
