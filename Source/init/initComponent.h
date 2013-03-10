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

#ifndef MCHA_HEADER_INITCOMPONENT
#define MCHA_HEADER_INITCOMPONENT

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../AudioDeviceSettings.h"

#define	MAXNUMBEROFDEVICECHANNELS	64


namespace mcha
{

// forward declarations
class InitTabComponent;
class MchaSettingsComponent;

//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class InitComponent  : public Component,
                       public ButtonListener
{
public:
    //==============================================================================
    InitComponent ();
    ~InitComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	Result saveConfig();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	AudioDeviceManager				deviceManager;

	AudioDeviceSelectorComponent*	deviceSelector;
	MchaSettingsComponent*			mchaSettingsComponent;

	AudioDeviceSettings				audioDeviceSettings;
	String							settingsFileName;

    //[/UserVariables]

    //==============================================================================
    TabbedComponent* tabbedComponent;
    TextButton* finishButton;

	XmlElement* createXmlSettings(AudioDeviceManager& adm);	

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InitComponent);
};

}; // end of namespace

#endif   // MCHA_HEADER_INITCOMPONENT
