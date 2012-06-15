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


enum FunctionID   { d0d0 = 0x00, d0d1 = 0x01, d1d0 = 0x04, d1d1 = 0x05,
					d0m0 = 0x02, d0m1 = 0x03, d1m0 = 0x06, d1m1 = 0x07,
					m0d0 = 0x08, m0d1 = 0x09, m1d0 = 0x0C, m1d1 = 0x0D,
					m0m0 = 0x0A, m0m1 = 0x0B, m1m0 = 0x0E, m1m1 = 0x0F,
					errorId = 0xFF };

FunctionID getFunctionId(int nlhs,	mxArray *plhs[], int nrhs, const mxArray *prhs[]);
bool callPlayRecord(FunctionID fid, const mxArray** params);


void mexFunction(	int nlhs, 	mxArray *plhs[],
					int nrhs,	const mxArray *prhs[] )
{
	FunctionID fid = getFunctionId( nlhs, plhs, nrhs, prhs);
	
	if (fid == errorId)
		mexErrMsgTxt("Wrong number or type of function arguments.");

	bool res = callPlayRecord( fid, prhs );
	
	if (!res)
		mexErrMsgTxt("Function failed. See log file for more details.");

	/* assign all output arguments */
	for (int k=0; k<nlhs; ++k)
		plhs[k] = mxCreateString( getLastError() );
}

/* ------------------------------------------------------------------------------------------------------------------------- */
FunctionID getFunctionId(int nlhs,	mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	if (( nrhs < 4 )||(nlhs > 1))
	{
		return errorId;		
	}

	if (  mxIsNumeric (prhs[0]) )  /* recording to internal memory */
	{
		switch (nrhs)
		{
		/* no filters */
		case 4: if ( mxIsNumeric(prhs[2]) )	/* playback from memory */
				{
					return m0m0;
				}
				else if ( mxIsCell(prhs[2]) ) /* playback from disk */
				{
					return m0d0;
				}
				else
				{
					return errorId;
				}
				break;
		/* one filter */
		case 5: if (  mxIsChar(prhs[2]) && mxIsNumeric(prhs[3]) )	/* playback from memory */
				{
					return m1m0;				
				}
				else if (  mxIsChar(prhs[2]) && mxIsCell(prhs[3]) )  /* playback from disk */
				{
					return m1d0;					
				}				
				else if (  mxIsChar(prhs[4]) && mxIsNumeric(prhs[3]) )  /* playback from memory */
				{
					return m0m1;					
				}				
				else if (  mxIsChar(prhs[4]) && mxIsCell(prhs[3]) )  /* playback from disk */
				{
					return m0d1;					
				}
				else
				{
					return errorId;			
				}
				break;
		/* two filters */
		case 6: if (  mxIsChar(prhs[2]) &&  mxIsChar(prhs[5]) && mxIsNumeric(prhs[3]) )  /* playback from memory */
				{
					return m1m1;				
				}
				else if (  mxIsChar(prhs[2])&&  mxIsChar(prhs[5]) && mxIsCell(prhs[3]) )  /* playback from disk */
				{
					return m1d1;					
				}
				else
				{
					return errorId;			
				}
				break;
		default:
			return errorId;
		}
	}
	else if ( mxIsChar(prhs[0]) ) /* recording to disk */
	{
		switch (nrhs)
		{
		/* no filters */
		case 5: if ( mxIsNumeric(prhs[3]) )	/* playback from memory */
				{
					return d0m0;
				}
				else if ( mxIsCell(prhs[3]) ) /* playback from disk */
				{
					return d0d0;
				}
				else
				{
					return errorId;
				}
				break;
		/* one filter */
		case 6: if (  mxIsChar(prhs[3]) && mxIsNumeric(prhs[4]) )	/* playback from memory */
				{
					return d1m0;				
				}
				else if (  mxIsChar(prhs[3]) && mxIsCell(prhs[4]) )  /* playback from disk */
				{
					return d1d0;					
				}
				else if (  mxIsChar(prhs[5]) && mxIsNumeric(prhs[4]) )  /* playback from memory */
				{
					return d0m1;					
				}				
				else if (  mxIsChar(prhs[5]) && mxIsCell(prhs[4]) )  /* playback from disk */
				{
					return d0d1;					
				}				
				else
				{
					return errorId;			
				}
				break;
		/* two filters */
		case 7: if (  mxIsChar(prhs[3]) &&  mxIsChar(prhs[6]) && mxIsNumeric(prhs[4]) )  /* playback from memory */
				{
					return d1m1;				
				}
				else if (  mxIsChar(prhs[3])&&  mxIsChar(prhs[6]) && mxIsCell(prhs[4]) )  /* playback from disk */
				{
					return d1d1;					
				}
				else
				{
					return errorId;			
				}
				break;
		default:
			return errorId;
		}
	}
	else
	{
		return errorId;		
	}
}


/* ------------------------------------------------------------------------------------------------------------------------- */
bool callPlayRecord(FunctionID fid, const mxArray** params)
{
	/* return on error */
	if (fid == errorId)
		return false;	
	
	/* initialise variables */
	char**	audioFiles = NULL;
	size_t	filesCount(0);
	
	bool	isSingle(false);
	void**	memPlay = NULL;
	size_t	memSamplesCount(0);
	size_t	memChanCount(0);

	int*	playChans = NULL;
	int		playChanCount(0);

	char*	recDir = NULL;
	int*	recChans = NULL;
	int		recChanCount(0);
	float	recDuration(0.0);

	int		inputChanNumber(0);
	size_t	inputSamplesNumber(0);

	int		outputChanNumber(0);
	size_t	outputSamplesNumber(0);

	bool	res(false);

	/* get function type */		
	bool	isMemRec = ((fid & 0x08) > 0 );
	bool	isRecFilter  = ((fid & 0x04) > 0 );
	bool	isMemPlay = ((fid & 0x02) > 0);	
	bool	isPlayFilter = ((fid & 0x01) > 0);

	unsigned int parNumber(0);
	
	/* ---- Prepare parameters ---- */

	if (!isMemRec) /* saving to disk. Get the first parameter: folder name */
	{
		size_t recDirLen =  mxGetM(params[parNumber])*mxGetN(params[parNumber]); /* get the length of path string */

		recDir = mxArrayToString( params[parNumber++] );
		if (recDir == NULL)
		{
			mexErrMsgTxt( "Unable to convert recording directory to string." );
			return false;
		}
	}
	
	prepareChannels( params[parNumber++], recChans, recChanCount);
	
	/* record duration (in seconds for files and in samples for memory recording) */
	recDuration = getDuration( params[parNumber++] );
	if (isMemRec)
	{
		double samplingRate;
		int bufSize, bDepth, inChannels, outChannels;
		res = getDeviceSettings(samplingRate, bufSize, bDepth, inChannels, outChannels);
		if (res)
			inputSamplesNumber = static_cast <size_t> (ceil(recDuration*samplingRate));
		else
			return false;
	}

	/* initialise recording filter */
	if (isRecFilter)
	{
		res = prepareFilter(params[parNumber++], true, inputChanNumber);
		if (!res)
			return false;
	}
	else
	{
		inputChanNumber = recChanCount;
	}
		
	if (isMemPlay)
	{	
		res = prepareMemoryPlay(params[parNumber++], isSingle, memPlay, memSamplesCount, memChanCount);
		if (!res)
			return false;		
	}
	else
	{
		res = prepareFileList( params[parNumber++], audioFiles, filesCount);
		if (!res)
			return false;	
	}
	
	prepareChannels( params[parNumber++], playChans, playChanCount);

	/* initialise playback filter */
	if (isPlayFilter)
	{
		res = prepareFilter(params[parNumber], false, outputChanNumber);
		if (!res)
			return false;		
	}
	else
	{
		outputChanNumber = playChanCount;
	}

	/* ---- Call the playback/record function ---- */
	if (isMemRec) /* recording into (internal) memory */
	{
		float** dummyDst = NULL;
		if (isMemPlay)
		{
			
			if (isSingle) /* single precision playback */
			{
				res = playRecordDataMM_s( dummyDst, inputChanNumber, inputSamplesNumber, const_cast <const int*> (recChans), recChanCount,
										  (const float**) memPlay, memChanCount, memSamplesCount, const_cast <const int*> (playChans), playChanCount );
			}
			else /* double precision playback */
			{
				res = playRecordDataMM_d( dummyDst, inputChanNumber, inputSamplesNumber, const_cast <const int*> (recChans), recChanCount,
										  (const double**) memPlay, memChanCount, memSamplesCount, const_cast <const int*> (playChans), playChanCount );
			}
		}
		else
		{
			res = playRecordDataDM( dummyDst, inputChanNumber, inputSamplesNumber, const_cast <const int*> (recChans), recChanCount,
									const_cast <const char**> (audioFiles), filesCount, const_cast <const int*> (playChans), playChanCount);
		}
	}
	else /* recording to the disk */
	{
		if (isMemPlay)
		{
			if (isSingle) /* single precision playback */
			{
				res = playRecordDataMD_s( const_cast<const char*> (recDir), inputChanNumber, recDuration, const_cast <const int*> (recChans), recChanCount,
										  (const float**) memPlay, memChanCount, memSamplesCount, const_cast <const int*> (playChans), playChanCount );
			}
			else /* double precision playback */
			{
				res = playRecordDataMD_d(  const_cast<const char*> (recDir), inputChanNumber, recDuration, const_cast <const int*> (recChans), recChanCount, 
										  (const double**) memPlay, memChanCount, memSamplesCount, const_cast <const int*> (playChans), playChanCount );
			}
		}
		else /* playing from the disk */
		{
			res = playRecordDataDD( const_cast<const char*> (recDir), inputChanNumber, recDuration, const_cast <const int*> (recChans), recChanCount, 
									const_cast <const char**> (audioFiles), filesCount, const_cast <const int*> (playChans), playChanCount );

		}
	}

	/* release resources */
	delete [] recChans;
	delete [] playChans;	
		
	if (isMemPlay)
	{
		releaseMemoryBuffer(memPlay);
	}
	else
	{
		releaseFileList(audioFiles, filesCount);	
	}
	
	if (!isMemRec)
	{
		mxFree(recDir);
	}
	
	return res;
}