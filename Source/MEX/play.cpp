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

/* Input Arguments */
#define	inputData		prhs[0]
#define	channelNumbers	prhs[1]

/* Functions declarations */
bool playDisk(const mxArray *mxFiles, const mxArray *mxChannels, const mxArray* mxFilterSettings);
bool playMemory(const mxArray *mxData,  const mxArray *mxChannels, const mxArray* mxFilterSettings);


/* Entry point */
void mexFunction( int nlhs,	mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	bool res = false;

	/* Check the number of input/output arguments */
	if (nrhs < 2) 
	{
		mexErrMsgTxt("The function requires at least two input arguments.");
		return;
	}

	/* Check the type of the first argument */
	if (mxIsCell(inputData))
	{
		/* play from disk */
		if (nrhs == 2)
		{
			/* no filters */
			res = playDisk(inputData, channelNumbers, NULL);
			if ( !res )
			{	
				mexErrMsgTxt( getLastError() );
			}
		}
		/* use filters */
		else if (nrhs == 3)
		{
			/* use filters */
			res = playDisk(inputData, channelNumbers, prhs[2]);
			if ( !res )
			{	
				mexErrMsgTxt( getLastError() );
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
			res = playMemory(inputData, channelNumbers, NULL);
			if ( !res )
			{
				mexErrMsgTxt( getLastError());
			}
		}
		else if (nrhs == 3)
		{
			/* use filters */
			res = playMemory(inputData, channelNumbers, prhs[2]);
			if ( !res )
			{
				mexErrMsgTxt( getLastError());
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
		plhs[k] = mxCreateString( getLastError() );

}


//=========================================================================================================
bool playDisk(const mxArray *mxFiles, const mxArray *mxChannels, const mxArray* mxFilterSettings)
{

	char**  audioFiles = NULL;
	size_t	filesNumber(0);
	int*	channels = NULL;
	int		outChannelsCount(0);
	bool	res(false);
	bool	isInputFilter(false);
	int		filterOutputs(-1);
	
	/* Prepare playback channels */
	prepareChannels( mxChannels, channels, outChannelsCount );

	/* Prepare playback file list */
	prepareFileList( mxFiles, audioFiles, filesNumber );

	/* Prepare filters */
	res = prepareFilter( mxFilterSettings, isInputFilter, filterOutputs);
	if (!res)
	{
		delete [] audioFiles;
		delete [] channels;		
		return false;
	}

	/* call PlayRecord function */
	res = playFiles ((const char**) audioFiles, (const int) filesNumber, (const int*) channels, (const int) outChannelsCount);

	delete [] audioFiles;
	delete [] channels;
	
	return res;
}


//=========================================================================================================
bool playMemory(const mxArray *mxData,  const mxArray *mxChannels, const mxArray* mxFilterSettings)
{

	void**		outputData = NULL;
	size_t		outputChanNumber = 0; 
	size_t		outputSamplesNumber = 0;
	int*		outChannels = NULL;
	int			outChannelsCount = 0;
	bool		res = false;
	bool		isInputFilter = false;
	int			filterOutputs = -1;
	bool		isDataSingle = false;


	/* Check input channels */
	if (!mxIsNumeric (mxChannels))
	{
		mexErrMsgTxt("Input channels should be an array of numeric values.");
		return false;
	}

	/* Prepare playback channels */
	prepareChannels( mxChannels, outChannels, outChannelsCount);
	
	/* Prepare playback memory variables */

	res = prepareMemoryPlay( mxData, isDataSingle, outputData, outputSamplesNumber, outputChanNumber );
	if (!res)
	{	
		delete [] outChannels;			
		mexErrMsgTxt("Input data should be in single-precision floating-point format. Use  single  for conversion.");
		return false;
	}
	
	/* Prepare filters */
	res = prepareFilter( mxFilterSettings, isInputFilter, filterOutputs);
	if (!res)
	{
		delete [] outputData;
		delete [] outChannels;	
		return false;
	}

	/* call PlayRecord function */
	if (isDataSingle)
	{
		res = playData_s ((const float**) outputData, (const int) outputChanNumber, (const size_t) outputSamplesNumber, (const int*) outChannels, (const int) outChannelsCount);
	}
	else
	{
		res = playData_d ((const double**) outputData, (const int) outputChanNumber, (const size_t) outputSamplesNumber, (const int*) outChannels, (const int) outChannelsCount);	
	}


	delete [] outputData;
	delete [] outChannels;

	return res;
}
