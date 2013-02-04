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

//#include <math.h>
#include "mex.h"
#include "audioMEX.h"

/* Input Arguments */
#define	inputData		prhs[0]
#define	channelNumbers	prhs[1]


/* Entry point */
void mexFunction( int nlhs,	mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

	/* Load MCHA library */
	Mcha	mcha;
	if ( !mcha.noError() )
	{	
		mexErrMsgTxt( mcha.getErrorStr() );	
		return;
	}

	bool res = false;

	/* Check the number of input/output arguments */
	if (nrhs < 2) 
	{
		mexErrMsgTxt("The function requires at least two input arguments.");
		return;
	}

	/* Check the type of the first argument */
	if ( mxIsCell(inputData) )
	{
		/* play from disk */
		if (nrhs == 2)
		{
			/* no filters */
			res = mcha.playDisk(inputData, channelNumbers, NULL);
			if ( !res )
			{	
				mexErrMsgTxt( mcha.getLastError() );
			}
		}
		/* use filters */
		else if (nrhs == 3)
		{
			/* use filters */
			res = mcha.playDisk(inputData, channelNumbers, prhs[2]);
			if ( !res )
			{	
				mexErrMsgTxt( mcha.getLastError() );
			}
		}
		else
		{
			/* return error */
			mexErrMsgTxt("Wrong number or type of function arguments.");	
		}
	}
	else
	{
		/* play from memory */
		if (nrhs == 2)
		{		
			/* no filters */
			res = mcha.playMemory(inputData, channelNumbers, NULL);
			if ( !res )
			{
				mexErrMsgTxt( mcha.getLastError());
			}
		}
		else if (nrhs == 3)
		{
			/* use filters */
			res = mcha.playMemory(inputData, channelNumbers, prhs[2]);
			if ( !res )
			{
				mexErrMsgTxt( mcha.getLastError());
			}		
		}
		else
		{
			/* return error */
			mexErrMsgTxt("Wrong number or type of function arguments.");	
		}		
	}

	/* get rid of 'One or more output arguments not assigned' error */
	for (int k=0; k<nlhs; ++k)
		plhs[k] = mxCreateString( mcha.getLastError() );

}


