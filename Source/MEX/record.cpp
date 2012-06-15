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
#include "audioMEX.cpp"

#define	errorOutput			plhs[0]

/* helper functions */
bool recordDisk  ( const mxArray *recFolder, const mxArray *inputChannels, const mxArray *recDuration,  const mxArray *filterSettings );
bool recordMemory( const mxArray *inputChannels,  const mxArray *recDuration,  const mxArray *filterSettings );


/* Entry point */
void mexFunction(	int nlhs, 	mxArray *plhs[],
					int nrhs,	const mxArray *prhs[] )
{

	/* check if another process is running */ 
	if (isRunning())
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
		bool res = recordDisk( mxRecFolder, mxRecChannels, mxRecDuration, mxFilterSettings );
		if ( !res )
		{	
			mexErrMsgTxt( getLastError() );
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
		bool res = recordMemory( mxRecChannels, mxRecDuration, mxFilterSettings );
		if ( !res )
		{	
			mexErrMsgTxt( getLastError() );
			return;
		}
	}
	
	/* assign all output arguments */
	for (int k=0; k<nlhs; ++k)
		plhs[k] = mxCreateString( getLastError() );
}

/* ------------------------------------------------------------------------------------------------------------------------- */
bool recordMemory( const mxArray *inputChannels,  const mxArray *recDuration,  const mxArray *filterSettings )
{
	
	float**		inputData = NULL;
	int			inputChanNumber = 0; 
	size_t		inputSamplesNumber = 0;
	int*		inChannels = NULL;
	int			inChannelsCount = 0;
	mxArray*	memDestination = NULL;
	bool		res;
	bool		isInputFilter = true;
	int			filterOutputs = -1;

	/* Check input duration */
	if (!mxIsNumeric (recDuration))
	{
		logError("MEX record error: Record duration should be a positive number.");
		return false;
	}

	/* Check input channels */
	if (!mxIsNumeric (inputChannels))
	{
		logError("MEX record error: Input channels should be an array of integer values.");
		return false;
	}

	/* Get audio device parameters */
	double		sampleRate = -1;
	int			bufSize = -1;
	int			bitDepth = -1;
	int			inChan = -1;
	int			outChan = -1;

	if ( !getDeviceSettings(sampleRate, bufSize, bitDepth, inChan, outChan) )
		return false;
	
	/* get duration and calculate the number of samples */
	float	duration = getDuration(recDuration);
	
	if (duration <= 0)
	{	
		logError("MEX record error: Record duration should be a positive number.");
		return false;
	}
	else
		inputSamplesNumber = static_cast <const size_t> ( ceil( duration*sampleRate ) );

	/* Prepare recording channels */
	prepareChannels( inputChannels, inChannels, inChannelsCount );
	inputChanNumber = inChannelsCount;

	/* Prepare filters */
	res = prepareFilter( filterSettings, isInputFilter, filterOutputs);
	if (!res)
	{
		delete [] inChannels;
		return false;
	}
	
	if (filterOutputs != -1)
		inputChanNumber = filterOutputs;

	/* Call record the function */
	res = recordData ( (float **)NULL, (const int) inputChanNumber, inputSamplesNumber, inChannels, inChannelsCount);
	
	/* release memory */
	delete [] inChannels;

	return res;
}

/* ------------------------------------------------------------------------------------------------------------------------- */
bool recordDisk(const mxArray *recFolder, const mxArray *inputChannels, const mxArray *recDuration,  const mxArray *filterSettings )
{
	char*		recordDir = NULL;
	int			inputChanNumber = 0;
	int*		inChannels = NULL;
	int			inChannelsCount = 0; 
	float		duration = 0;
	bool		res;
	bool		isInputFilter = true;
	int			filterOutputs = -1;


	/* Check output folder */
	if (!mxIsChar(recFolder))
	{
		logError("MEX record error: The first argument should be a string containing output directory.");
		return false; 			
	}

	/* Check input duration */
	if (!mxIsNumeric (recDuration))
	{
		logError("MEX record error: Record duration should be a positive number.");
		return false; 
	}

	/* Check input channels */
	if (!mxIsNumeric (inputChannels))
	{
		logError("MEX record error: Input channels should be an array of integer values.");
		return false; 
	}

	/* get duration */
	duration = getDuration(recDuration);

	/* Prepare recording channels */
	prepareChannels( inputChannels, inChannels, inChannelsCount );
	inputChanNumber = inChannelsCount;

	/* Prepare filters */
	res = prepareFilter( filterSettings, isInputFilter, filterOutputs);
	if (!res)
	{
		delete [] inChannels;
		return false;
	}
	
	if (filterOutputs != -1)
		inputChanNumber = filterOutputs;

	/* Create char string with the disk folder */
	recordDir = mxArrayToString(recFolder);

	/* Call the function */
	res = recordFiles(  (const char*) recordDir, (const int) inputChanNumber, (const float)duration, (const int*) inChannels, (const int) inChannelsCount );
	
	/* release memory */
	mxFree(recordDir);
	delete [] inChannels;

	return res;
}


