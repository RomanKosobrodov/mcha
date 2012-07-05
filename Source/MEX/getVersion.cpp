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
#include <dlfcn.h>

void mexFunction(
	int nlhs, 	mxArray *plhs[],
	int nrhs, const mxArray *prhs[])
{
   void *lib_handle;
   typedef const char* (*fptr)();
   char *error;
   fptr getVersion;

// loading the library dynamically
   lib_handle = dlopen("/usr/lib/libmcha.so", RTLD_LAZY);
   if (!lib_handle) 
   {
      plhs[0] = mxCreateString( dlerror() );
      return;
   }

// resolving getVersion
   *(void **)(&getVersion) =  dlsym(lib_handle, "getVersion");
   if ((error = dlerror()) != NULL)  
   {
			plhs[0] = mxCreateString( error );
      return;
   }
	else
	{
		plhs[0] = mxCreateString( getVersion() );
	}
	
// release the handle
  dlclose(lib_handle);
  
}

