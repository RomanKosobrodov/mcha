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

#include <math.h>
#include "mex.h"
#include "audioMEX.h"

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
	
	/* check if the process is running */ 
	if ( mcha.isRunning() )
	{
		mexErrMsgTxt("Function failed. The process is still running.");	
		return;
	}
	
	if (( nrhs > 0 )||(nlhs != 1))
	{
		mexErrMsgTxt("Wrong number of function arguments.");	
		return;		
	}
	
	int channelCount = mcha.getRecordedChannelsNum();
	size_t samplesCount = mcha.getRecordedSamplesNum();

	mxArray* mem = mxCreateNumericMatrix( samplesCount, channelCount, mxSINGLE_CLASS, mxREAL );
	
	if (mem == NULL)
	{
		mexErrMsgTxt("Memory allocation for the output matrix failed.");
	}

	float** memData = new float* [channelCount];
	float* dataOrigin = (float *)mxGetPr(mem);
	
	for (int i=0; i < channelCount; i++)
	{
		memData[i] = dataOrigin + i*samplesCount;
	}
	
	if ( !mcha.getData_s(memData, NULL, 0, 0, 0) )
	{
		delete [] memData;
		mexErrMsgTxt("Unable to retrieve data from memory. Call getError() for details.");
	}
	else
	{
		plhs[0] = mem;
		delete [] memData;
	}
}
