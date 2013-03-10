/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  5 Mar 2013 5:43:33pm

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_MCHASETTINGSCOMPONENT_MCHASETTINGSCOMPONENT_C3098AEA__
#define __JUCER_HEADER_MCHASETTINGSCOMPONENT_MCHASETTINGSCOMPONENT_C3098AEA__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"

namespace mcha
{
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Jucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MchaSettingsComponent  : public Component,
                               public ButtonListener
{
public:
    //==============================================================================
    MchaSettingsComponent ();
    ~MchaSettingsComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	String	getLogLevel();	
	String	getMemoryMode();
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    GroupComponent* logGroupComponent;
    ToggleButton* noneToggleButton;
    ToggleButton* normalToggleButton;
    ToggleButton* detailedToggleButton;
    TextEditor* infoTextEditor;
    GroupComponent* memGroupComponent;
    ToggleButton* safeMemToggleButton;
    ToggleButton* fastMemToggleButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MchaSettingsComponent);
};

//[EndFile] You can add extra defines here...
} // end of namespace
//[/EndFile]

#endif   // __JUCER_HEADER_MCHASETTINGSCOMPONENT_MCHASETTINGSCOMPONENT_C3098AEA__
