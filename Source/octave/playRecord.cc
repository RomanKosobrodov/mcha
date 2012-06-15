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

#include <octave/oct.h>
#include "audioMEX.h"
#include "audioMEX.cc"


enum FunctionID   { d0d0 = 0x00, d0d1 = 0x01, d1d0 = 0x04, d1d1 = 0x05,
					d0m0 = 0x02, d0m1 = 0x03, d1m0 = 0x06, d1m1 = 0x07,
					m0d0 = 0x08, m0d1 = 0x09, m1d0 = 0x0C, m1d1 = 0x0D,
					m0m0 = 0x0A, m0m1 = 0x0B, m1m0 = 0x0E, m1m1 = 0x0F,
					errorId = 0xFF };

FunctionID getFunctionId(const octave_value_list* fargs, const int args_out_count);
bool callPlayRecord(FunctionID fid, const octave_value_list& fargs);

/* ---------------------------------------------------------------------------------- */
DEFUN_DLD (m_playRecord, args, nargout, "play and record audio from/to memory or disk")
{

	charMatrix argError( "Wrong number or type of function arguments.\n" );

	FunctionID fid = getFunctionId( &args, nargout);
	if (fid == errorId)
	{	
		octave_stdout << "unable to detect suitable parameter set.\n";
		return octave_value(argError);
	}
	
	bool res = callPlayRecord( fid, args );
	if (!res)
	{	
		charMatrix playRecCallError("Function failed. See log file for more details.\n");
		return octave_value(playRecCallError);
	}

	/* return error string (empty on success) */
	charMatrix lastErrorStr( getLastError() );
	return octave_value(lastErrorStr);

}


/* ------------------------------------------------------------------------------------------------------------------------- */
FunctionID getFunctionId(const octave_value_list* fargs, const int args_out_count)
{
	int nargin ( fargs->length() );	

	if (( nargin < 4 )||(args_out_count > 1))
	{
		return errorId;		
	}

	if (  isNumeric(fargs, 0) )  /* recording to internal memory */
	{
		switch (nargin)
		{
		/* no filters */
		case 4: if ( isNumeric(fargs, 2) )	/* playback from memory */
				{
					return m0m0;
				}
				else if ( isCell( fargs, 2) ) /* playback from disk */
				{
					return m0d0;
				}
				else
				{
					return errorId;
				}
				break;
		/* one filter */
		case 5: if (  isString(fargs,2) && isNumeric(fargs,3) )	/* playback from memory */
				{
					return m1m0;				
				}
				else if (  isString(fargs,2) && isCell(fargs,3) )  /* playback from disk */
				{
					return m1d0;					
				}				
				else if (  isString(fargs,4) && isNumeric(fargs,3) )  /* playback from memory */
				{
					return m0m1;					
				}				
				else if (  isString(fargs,4) && isCell(fargs,3) )  /* playback from disk */
				{
					return m0d1;					
				}
				else
				{
					return errorId;			
				}
				break;
		/* two filters */
		case 6: if (  isString(fargs,2) &&  isString(fargs,5) && isNumeric(fargs,3) )  /* playback from memory */
				{
					return m1m1;				
				}
				else if (  isString(fargs,2)&&  isString(fargs,5) && isCell(fargs,3) )  /* playback from disk */
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
	else if ( isString(fargs, 0) ) /* recording to disk */
	{
		switch (nargin)
		{
		/* no filters */
		case 5: if ( isNumeric(fargs, 3) )	/* playback from memory */
				{
					return d0m0;
				}
				else if ( isCell(fargs, 3) ) /* playback from disk */
				{
					return d0d0;
				}
				else
				{
					return errorId;
				}
				break;
		/* one filter */
		case 6: if (  isString(fargs, 3) && isNumeric(fargs, 4) )	/* playback from memory */
				{
					return d1m0;				
				}
				else if (  isString(fargs, 3) && isCell(fargs, 4) )  /* playback from disk */
				{
					return d1d0;					
				}
				else if (  isString(fargs, 5) && isNumeric(fargs, 4) )  /* playback from memory */
				{
					return d0m1;					
				}				
				else if (  isString(fargs, 5) && isCell(fargs, 4) )  /* playback from disk */
				{
					return d0d1;					
				}				
				else
				{
					return errorId;			
				}
				break;
		/* two filters */
		case 7: if (  isString(fargs, 3) &&  isString(fargs, 6) && isNumeric(fargs, 4) )  /* playback from memory */
				{
					return d1m1;				
				}
				else if (  isString(fargs, 3)&&  isString(fargs, 6) && isCell(fargs, 4) )  /* playback from disk */
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
bool callPlayRecord(FunctionID fid, const octave_value_list& fargs)
{
	/* return on error */
	if (fid == errorId)
		return false;	
	
	/* initialise variables */
	char**	audioFiles = NULL;
	size_t	filesCount(0);
	
	bool	isSingle(false);
	float**		memPlaySingle(NULL);
	double**	memPlayDouble(NULL);
	FloatMatrix dataFloat;
	Matrix		dataDouble;
	size_t	memSamplesCount(0);
	size_t	memChanCount(0);

	int*	playChans = NULL;
	int		playChanCount(0);

	char*	recDir = NULL;
	int*	recChans = NULL;
	int		recChanCount(0);

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
		std::string recDirString = fargs(parNumber++).string_value();

		if (error_state)
		{
			octave_stdout << "Unable to convert recording directory to string.\n";
			return false;
		}
		
		size_t strLen = recDirString.length();
		recDir = new char[strLen+1];
		recDir[strLen] = 0;
		memcpy(recDir, recDirString.c_str(), strLen*sizeof(char) );

	}
	
	const int32NDArray recChArray ( fargs(parNumber++).int32_array_value() ) ;
	prepareChannels( &recChArray, recChans, recChanCount);
	
	/* record duration (in seconds for files and in samples for memory recording) */
	const float recDuration ( fargs(parNumber++).float_value() );
	if (error_state)
	{
		octave_stdout << "Unable to convert duration to floating point value.\n";
		return false;
	}

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
		res = prepareFilter( &fargs(parNumber++), true, inputChanNumber);
		if (!res)
		{
			octave_stdout << "Unable to parse recording filter settings.\n";			
			return false;
		}
	}
	else
	{
		inputChanNumber = recChanCount;
	}
		
	if (isMemPlay)
	{	
		isSingle = fargs(parNumber).is_single_type();
		if (isSingle)
		{
			dataFloat = fargs(parNumber++).float_matrix_value();
			prepareMemoryPlay(&dataFloat, memPlaySingle, memSamplesCount, memChanCount);
		}
		else
		{	
			dataDouble = fargs(parNumber++).matrix_value();
			prepareMemoryPlay(&dataDouble, memPlayDouble, memSamplesCount, memChanCount);
		}

	}
	else
	{
		const Cell fileListCell( fargs(parNumber++).cell_value() );
		res = prepareFileList( &fileListCell, audioFiles, filesCount);
		if (!res)
		{
			octave_stdout << "Wrong format or type of playback audio files.\n";			
			return false;	
		}
	}

	const int32NDArray playChArray ( fargs(parNumber++).int32_array_value() ) ;
	prepareChannels( &playChArray, playChans, playChanCount);

	/* initialise playback filter */
	if (isPlayFilter)
	{
		res = prepareFilter(&fargs(parNumber++), false, outputChanNumber);
		if (!res)
		{
			octave_stdout << "Unable to parse playback filter settings.\n";			
			return false;
		}	
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
										  (const float**) memPlaySingle, memChanCount, memSamplesCount, const_cast <const int*> (playChans), playChanCount );
			}
			else /* double precision playback */
			{
				res = playRecordDataMM_d( dummyDst, inputChanNumber, inputSamplesNumber, const_cast <const int*> (recChans), recChanCount,
										  (const double**) memPlayDouble, memChanCount, memSamplesCount, const_cast <const int*> (playChans), playChanCount );
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
										  (const float**) memPlaySingle, memChanCount, memSamplesCount, const_cast <const int*> (playChans), playChanCount );
			}
			else /* double precision playback */
			{
				res = playRecordDataMD_d(  const_cast<const char*> (recDir), inputChanNumber, recDuration, const_cast <const int*> (recChans), recChanCount, 
										  (const double**) memPlayDouble, memChanCount, memSamplesCount, const_cast <const int*> (playChans), playChanCount );
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
		
	if (memPlaySingle)
	{
		releaseMemoryBuffer(memPlaySingle);
	}
	else if (memPlayDouble)
	{
		releaseMemoryBuffer(memPlayDouble);
	}
	else
	{
		releaseFileList(audioFiles, filesCount);	
	}
	
	return res;
}