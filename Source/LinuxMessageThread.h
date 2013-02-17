#ifndef LINUX_MESSAGE_THREAD_H
#define LINUX_MESSAGE_THREAD_H

#if JUCE_LINUX

#include "../JuceLibraryCode/JuceHeader.h"

namespace mcha
{

class LinuxMessageThread : public Thread
{
public:
    LinuxMessageThread()
      : Thread ("MchaMessageThread"),
        initialised (false)
    {

		DBG("Starting message thread ...");
        startThread (7);

        while (! initialised)
            sleep (1);
		
		DBG("Juce GUI Initialised")
    }

    ~LinuxMessageThread()
    {
		DBG("Stopping message thread ...");
 		
        signalThreadShouldExit();
        //JUCEApplication::quit();		
        waitForThreadToExit (5000);

		shutdownJuce_GUI();
		DBG("JUCE GUI shut down .");


		DBG("Message thread stopped.");
    }

    void run()
    {
		DBG("initialising Juce GUI")        
		initialiseJuce_GUI();
        initialised = true;

        MessageManager::getInstance()->setCurrentThreadAsMessageThread();

        while ((! threadShouldExit()) && MessageManager::getInstance()->runDispatchLoopUntil (250))
        {}
		
    }

private:
    bool initialised;
};

} // end of namespace

#endif


#endif
