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


// ============================================================================================
class MainDocumentWindow  : public DocumentWindow
{
public:
    //==============================================================================
    MainDocumentWindow() 
        : DocumentWindow (L"mcha configuration",
                          Colours::azure, 
                          DocumentWindow::closeButton | DocumentWindow::minimiseButton,
                          true)
    {

        setContentOwned (new mcha::InitComponent(), true);

        centreWithSize (getWidth(), getHeight());

        setVisible (true);
    }

    ~MainDocumentWindow()
    {
		clearContentComponent();
    }

    //==============================================================================
    void closeButtonPressed()
    {
		JUCEApplication::getInstance()->systemRequestedQuit();
    }
};

class guiApplication : public JUCEApplication
{
    MainDocumentWindow*		mainDocumentWindow;

public:
    //==============================================================================
    guiApplication()
        : mainDocumentWindow (0)
    {
    }

    ~guiApplication()
    {
    }

    //==============================================================================
    void initialise (const String& )
    {
        mainDocumentWindow = new MainDocumentWindow();
    }

    void shutdown()
    {
        if (mainDocumentWindow != 0)
		{           
			delete mainDocumentWindow;
		}
    }

    //==============================================================================
    const String getApplicationName()
    {
        return L"mcha Configuration Utility";
    }

    const String getApplicationVersion()
    {
        return L"0.1";
    }

    bool moreThanOneInstanceAllowed()
    {
        return false;
    }

    void anotherInstanceStarted (const String& )
    {
    }

};


//==============================================================================
START_JUCE_APPLICATION (guiApplication)


