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

/* Output Arguments */
#define	result	plhs[0]

#if ( defined (LINUX) || defined (__linux__) )

	#include <dlfcn.h>

	void mexFunction(
		int nlhs, 	mxArray *plhs[],
		int nrhs, const mxArray *prhs[])
	{
	   void *lib_handle;
	   typedef double (*fptr)();
	   char *error;
	   fptr getCurrentPosition;

	// loading the library dynamically
	   lib_handle = dlopen("/usr/lib/libmcha.so", RTLD_LAZY);
	   if (!lib_handle) 
	   {
		  plhs[0] = mxCreateString( dlerror() );
		  return;
	   }

	// resolving getCurrentPosition
	   *(void **)(&getCurrentPosition) =  dlsym(lib_handle, "getCurrentPosition");
	   if ((error = dlerror()) != NULL)  
	   {
		  plhs[0] = mxCreateString( error );
		  return;
	   }

	   result = mxCreateDoubleScalar( getCurrentPosition() );
	
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
		result = mxCreateDoubleScalar( getCurrentPosition() );
	}

#endif

