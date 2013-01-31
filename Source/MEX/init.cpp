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

#include "mex.h"

/* Input Arguments */
#define	settingsFile	prhs[0]

/* Output Arguments */
#define	errorMsg		plhs[0]



#if ( defined (LINUX) || defined (__linux__) )

	#include <dlfcn.h>

	void mexFunction(
		int nlhs, 	mxArray *plhs[],
		int nrhs, const mxArray *prhs[])
	{
	   void *lib_handle;
	   char  *error;

	   typedef bool (*fptr)();
	   fptr  initAudioDevice;

	   typedef bool (*fptr1)(const char *);
	   fptr1 initAudioDeviceFile;

	   typedef const char* (*fptr2)();
	   fptr2 getLastError;

	   typedef bool (*fptr3)(const char *);
	   fptr3 setDebugMode;

	// loading the library dynamically
	   lib_handle = dlopen("/usr/lib/libmcha.so", RTLD_LAZY);
	   if (!lib_handle) 
	   {
		  errorMsg = mxCreateString( dlerror() );
		  return;
	   }

	// resolving getLastError
	   *(void **)(&getLastError) =  dlsym(lib_handle, "getLastError");
	   if ((error = dlerror()) != NULL)  
	   {
   		  errorMsg = mxCreateString( error );
		  return;
	   }
		
		// resolving initAudioDevice
	   *(void **)(&initAudioDevice) =  dlsym(lib_handle, "initAudioDevice");
	   if ((error = dlerror()) != NULL)  
	   {
  		  errorMsg = mxCreateString( error );
		  return;
	   }
    	
	// resolving setDebugMode
	   *(void **)(&setDebugMode) =  dlsym(lib_handle, "setDebugMode");
	   if ((error = dlerror()) != NULL)  
	   {
  		  errorMsg = mxCreateString( error );
		  return;
	   }

	// resolving initAudioDeviceFile
	   *(void **)(&initAudioDeviceFile) =  dlsym(lib_handle, "initAudioDeviceFile");
	   if ((error = dlerror()) != NULL)  
	   {
  		  errorMsg = mxCreateString( error );
		  return;
	   }

	// check input parameters		
		if (nrhs > 2)
		{
			errorMsg = mxCreateString("Wrong number of arguments");
			return;
		}
	
		if (nrhs > 0) // settings file specified
		{
			if (mxIsChar(settingsFile))
			{
				const char*	xmlFile = mxArrayToString(settingsFile);
				if ( !initAudioDeviceFile(xmlFile) )
					errorMsg = mxCreateString( getLastError() );
				else
					errorMsg = mxCreateString("");
			}
			else
			{
				errorMsg = mxCreateString("The argument should be a string containing a valid file name.");		
			}

			if (nrhs == 2) // debug mode is specified
			{
				const char* debugStr = mxArrayToString( prhs[1] );
				if ( !setDebugMode(debugStr) )
					errorMsg = mxCreateString( getLastError() );
			}
		}
		else	// show configuration dialog
		{
			if ( !initAudioDevice() )
				errorMsg = mxCreateString( getLastError() );
			else
				errorMsg = mxCreateString("");
		}
	
	// release the handle
	  dlclose(lib_handle);
  
	}
#endif


#if (defined (_WIN32) || defined (_WIN64))

	#include "audioMEX.h"

	/* Entry point */
	void mexFunction(
		int nlhs, 	mxArray *plhs[],
		int nrhs, const mxArray *prhs[])
	{
		if (nrhs > 2)
		{
			errorMsg = mxCreateString("Wrong number of arguments");
			return;
		}
	
		if (nrhs > 0) // settings file specified
		{
			if (mxIsChar(settingsFile))
			{
				const char*	xmlFile = mxArrayToString(settingsFile);
				if ( !initAudioDeviceFile(xmlFile) )
					errorMsg = mxCreateString( getLastError() );
				else
					errorMsg = mxCreateString("");
			}
			else
			{
				errorMsg = mxCreateString("The argument should be a string containing a valid file name.");		
			}

			if (nrhs == 2) // debug mode is specified
			{
				const char* debugStr = mxArrayToString( prhs[1] );
				if ( !setDebugMode(debugStr) )
					errorMsg = mxCreateString( getLastError() );
			}
		}
		else	// show configuration dialog
		{
			if ( !initAudioDevice() )
				errorMsg = mxCreateString( getLastError() );
			else
				errorMsg = mxCreateString("");
		}

	}

#endif


