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
#include "audioMEX.h"

/* Input Arguments */
#define	settingsFile	prhs[0]

/* Output Arguments */
#define	errorMsg		plhs[0]

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
		if (!initAudioDevice())
			errorMsg = mxCreateString( getLastError() );
		else
			errorMsg = mxCreateString("");
	}
}

