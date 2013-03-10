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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "MchaSettingsComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
namespace mcha
{
//[/MiscUserDefs]

//==============================================================================
MchaSettingsComponent::MchaSettingsComponent ()
    : logGroupComponent (0),
      noneToggleButton (0),
      normalToggleButton (0),
      detailedToggleButton (0),
      infoTextEditor (0),
      memGroupComponent (0),
      safeMemToggleButton (0),
      fastMemToggleButton (0)
{
    addAndMakeVisible (logGroupComponent = new GroupComponent ("log group",
                                                               "Logging"));

    addAndMakeVisible (noneToggleButton = new ToggleButton ("none toggle button"));
    noneToggleButton->setButtonText ("no logging");
    noneToggleButton->setRadioGroupId (1);
    noneToggleButton->addListener (this);

    addAndMakeVisible (normalToggleButton = new ToggleButton ("normal toggle button"));
    normalToggleButton->setButtonText ("normal");
    normalToggleButton->setRadioGroupId (1);
    normalToggleButton->addListener (this);
    normalToggleButton->setToggleState (true, false);

    addAndMakeVisible (detailedToggleButton = new ToggleButton ("detailed toggle button"));
    detailedToggleButton->setButtonText ("detailed");
    detailedToggleButton->setRadioGroupId (1);
    detailedToggleButton->addListener (this);

    addAndMakeVisible (infoTextEditor = new TextEditor ("info text editor"));
    infoTextEditor->setMultiLine (true);
    infoTextEditor->setReturnKeyStartsNewLine (true);
    infoTextEditor->setReadOnly (true);
    infoTextEditor->setScrollbarsShown (false);
    infoTextEditor->setCaretVisible (false);
    infoTextEditor->setPopupMenuEnabled (true);
    infoTextEditor->setText (String::empty);

    addAndMakeVisible (memGroupComponent = new GroupComponent ("memory group",
                                                               "Memory settings"));

    addAndMakeVisible (safeMemToggleButton = new ToggleButton ("safeMem toggle button"));
    safeMemToggleButton->setButtonText ("safe");
    safeMemToggleButton->setRadioGroupId (2);
    safeMemToggleButton->addListener (this);
    safeMemToggleButton->setToggleState (true, false);

    addAndMakeVisible (fastMemToggleButton = new ToggleButton ("fastMem toggle button"));
    fastMemToggleButton->setButtonText ("fast");
    fastMemToggleButton->setRadioGroupId (2);
    fastMemToggleButton->addListener (this);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (700, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

MchaSettingsComponent::~MchaSettingsComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (logGroupComponent);
    deleteAndZero (noneToggleButton);
    deleteAndZero (normalToggleButton);
    deleteAndZero (detailedToggleButton);
    deleteAndZero (infoTextEditor);
    deleteAndZero (memGroupComponent);
    deleteAndZero (safeMemToggleButton);
    deleteAndZero (fastMemToggleButton);


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MchaSettingsComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MchaSettingsComponent::resized()
{
    logGroupComponent->setBounds (24, 24, 336, 128);
    noneToggleButton->setBounds (40, 50, 150, 24);
    normalToggleButton->setBounds (40, 82, 150, 24);
    detailedToggleButton->setBounds (40, 114, 150, 24);
    infoTextEditor->setBounds (24, 296, 336, 88);
    memGroupComponent->setBounds (24, 168, 336, 112);
    safeMemToggleButton->setBounds (40, 200, 150, 24);
    fastMemToggleButton->setBounds (40, 232, 150, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MchaSettingsComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == noneToggleButton)
    {
        //[UserButtonCode_noneToggleButton] -- add your button handler code here..
        //[/UserButtonCode_noneToggleButton]
    }
    else if (buttonThatWasClicked == normalToggleButton)
    {
        //[UserButtonCode_normalToggleButton] -- add your button handler code here..
        //[/UserButtonCode_normalToggleButton]
    }
    else if (buttonThatWasClicked == detailedToggleButton)
    {
        //[UserButtonCode_detailedToggleButton] -- add your button handler code here..
        //[/UserButtonCode_detailedToggleButton]
    }
    else if (buttonThatWasClicked == safeMemToggleButton)
    {
        //[UserButtonCode_safeMemToggleButton] -- add your button handler code here..
        //[/UserButtonCode_safeMemToggleButton]
    }
    else if (buttonThatWasClicked == fastMemToggleButton)
    {
        //[UserButtonCode_fastMemToggleButton] -- add your button handler code here..
        //[/UserButtonCode_fastMemToggleButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

String	MchaSettingsComponent::getLogLevel()
{
	if ( noneToggleButton->getToggleState() )
		return "none";
	else if ( normalToggleButton->getToggleState() )
		return "normal";
	else
		return "advanced";
}
	
String	MchaSettingsComponent::getMemoryMode()
{
	if ( safeMemToggleButton->getToggleState() )
		return "safe";
	else
		return "smart";
}

} // end of mcha namespace
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Jucer information section --

    This is where the Jucer puts all of its metadata, so don't change anything in here!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MchaSettingsComponent" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="0" initialWidth="700" initialHeight="400">
  <BACKGROUND backgroundColour="ffffffff"/>
  <GROUPCOMPONENT name="log group" id="b91f714e7f8d2dd" memberName="logGroupComponent"
                  virtualName="" explicitFocusOrder="0" pos="24 24 336 128" title="Logging"/>
  <TOGGLEBUTTON name="none toggle button" id="937c0796219b02b" memberName="noneToggleButton"
                virtualName="" explicitFocusOrder="0" pos="40 50 150 24" buttonText="no logging"
                connectedEdges="0" needsCallback="1" radioGroupId="1" state="0"/>
  <TOGGLEBUTTON name="normal toggle button" id="165ab9e5882e3394" memberName="normalToggleButton"
                virtualName="" explicitFocusOrder="0" pos="40 82 150 24" buttonText="normal"
                connectedEdges="0" needsCallback="1" radioGroupId="1" state="1"/>
  <TOGGLEBUTTON name="detailed toggle button" id="fad12ff66909625b" memberName="detailedToggleButton"
                virtualName="" explicitFocusOrder="0" pos="40 114 150 24" buttonText="detailed"
                connectedEdges="0" needsCallback="1" radioGroupId="1" state="0"/>
  <TEXTEDITOR name="info text editor" id="5a40972cdf876905" memberName="infoTextEditor"
              virtualName="" explicitFocusOrder="0" pos="24 296 336 88" initialText=""
              multiline="1" retKeyStartsLine="1" readonly="1" scrollbars="0"
              caret="0" popupmenu="1"/>
  <GROUPCOMPONENT name="memory group" id="8eac5edf03d4a53b" memberName="memGroupComponent"
                  virtualName="" explicitFocusOrder="0" pos="24 168 336 112" title="Memory settings"/>
  <TOGGLEBUTTON name="safeMem toggle button" id="da2a7f73bd00e9f4" memberName="safeMemToggleButton"
                virtualName="" explicitFocusOrder="0" pos="40 200 150 24" buttonText="safe"
                connectedEdges="0" needsCallback="1" radioGroupId="2" state="1"/>
  <TOGGLEBUTTON name="fastMem toggle button" id="10207bb8308901e8" memberName="fastMemToggleButton"
                virtualName="" explicitFocusOrder="0" pos="40 232 150 24" buttonText="fast"
                connectedEdges="0" needsCallback="1" radioGroupId="2" state="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif



//[EndFile] You can add extra defines here...
//[/EndFile]
