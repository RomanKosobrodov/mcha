/*  
	MCHA - Multichannel Audio Playback and Recording Library
    Copyright (C) 2011-2013  Roman Kosobrodov

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

#define	errorOutput			plhs[0]


/* Entry point */
void mexFunction(	int nlhs, 	mxArray *plhs[],
					int nrhs,	const mxArray *prhs[] )
{

	/* Load MCHA library */
	Mcha	mcha;
	if ( !mcha.noError() )
	{	
		mexErrMsgTxt( mcha.getErrorStr() );	
		return;
	}


	/* check if another process is running */ 
	if ( mcha.isRunning() )
	{
		mexErrMsgTxt("Function failed. Another process is running.");	
		return;
	}
	
	if (( nrhs < 2 )||(nlhs > 1))
	{
		mexErrMsgTxt("Wrong number or type of function arguments.");	
		return;		
	}
	
	if ( mxIsChar(prhs[0]) ) /* recording to disk - first parameter is a string */
	{
		bool useFilters = false;

		switch (nrhs)	
		{
		case 3: useFilters = false; break;	/* filters are not used */
		case 4: useFilters = true; break;  /* use filters */
		default: mexErrMsgTxt("Wrong number or type of function arguments."); return; /* wrong call */
		}

		const mxArray*	mxRecFolder    = prhs[0];		
		const mxArray*	mxRecChannels  = prhs[1];
		const mxArray*	mxRecDuration  = prhs[2];
		const mxArray*	mxFilterSettings = (useFilters) ? prhs[3] : NULL;

		/* Start recording */
		bool res = mcha.recordDisk( mxRecFolder, mxRecChannels, mxRecDuration, mxFilterSettings );
		if ( !res )
		{	
			mexErrMsgTxt( mcha.getLastError() );
			return;
		}
		
	}
	else if ( mxIsNumeric (prhs[0]) ) /* recording to internal memory */
	{
		bool useFilters = false;
		if (nrhs ==2) /* no filters */
		{
			useFilters = false;
		}
		else if (nrhs == 3) /* use filters */
		{
			useFilters = true;
		}
		else
		{
			mexErrMsgTxt("Wrong number or type of function arguments.");	
			return;			
		}

		const mxArray*	mxRecChannels = prhs[0];
		const mxArray*	mxRecDuration = prhs[1];
		const mxArray*	mxFilterSettings  = (useFilters) ? prhs[2] : NULL ;	
			
		/* Start recording */
		bool res = mcha.recordMemory( mxRecChannels, mxRecDuration, mxFilterSettings );
		if ( !res )
		{	
			mexErrMsgTxt( mcha.getLastError() );
			return;
		}
	}
	
	/* assign all output arguments */
	for (int k=0; k<nlhs; ++k)
		plhs[k] = mxCreateString( mcha.getLastError() );
}


