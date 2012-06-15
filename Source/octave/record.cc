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

/* Functions declarations */
bool recordDisk(const octave_value *mxFiles, const int32NDArray *mxChannels, const double recDuration, const octave_value* mxFilterSettings);
bool recordMemory(const int32NDArray *mxChannels, const double recDuration,  const octave_value* mxFilterSettings);

DEFUN_DLD (m_record, args, nargout, "record audio to memory or disk")
{
	int nargin ( args.length() );

	charMatrix argError( "Wrong number or type of function arguments.\n" );

	/* check if another process is running */
	if ( isRunning() )
	{
		charMatrix procError("Another process is running.\n");	
		return octave_value(procError); 
	}
	
	if (( nargin < 2 )||(nargout > 1))
	{
		return octave_value(argError);
	}

	if ( args(0).is_string() ) /* recording to disk - first parameter is a string */
	{
		bool useFilters = false;

		switch (nargin)	
		{
			case 3: useFilters = false; break;	/* filters are not used */
			case 4: useFilters = true; break;  /* use filters */
			default: 	return octave_value(argError); /* wrong call */
		}

		const int32NDArray	mxRecChannels ( args(1).int32_array_value() );
		const double		mxRecDuration ( args(2).double_value() );
		if (error_state)
		{
			return octave_value(argError);
		}

		/* Start recording */
		if (useFilters) 
		{
			recordDisk( &args(0), &mxRecChannels, mxRecDuration, &args(3) );
		}
		else
		{
			recordDisk( &args(0), &mxRecChannels, mxRecDuration, NULL );		
		}

	}
	else if ( args(0).is_numeric_type() ) /* recording to internal memory */
	{
		bool useFilters = false;
		if (nargin == 2) /* no filters */
		{
			useFilters = false;
		}
		else if (nargin == 3) /* use filters */
		{
			useFilters = true;
		}
		else
		{
			return octave_value(argError);	
		}

		const int32NDArray	mxRecChannels ( args(0).int32_array_value() );
		const double		mxRecDuration ( args(1).double_value() );
		
		if (error_state)
		{
			return octave_value(argError);
		}

		/* Start recording */
		bool res(false);
		if (useFilters) 
		{
			res = recordMemory( &mxRecChannels, mxRecDuration, &args(2) );
		}
		else
		{
			res = recordMemory( &mxRecChannels, mxRecDuration, NULL );
		}
		if (!res)
		{
			return octave_value(argError);		
		}
	}
	else
	{
		return octave_value(argError);
	}

	/* return error string (empty on success) */
	charMatrix ch( getLastError() );
	return octave_value(ch);
}

/* ------------------------------------------------------------------------------------------------------------------------- */
bool recordMemory(const int32NDArray *mxChannels, const double recDuration,  const octave_value* mxFilterSettings)
{
	
	float**		inputData(NULL);
	int			inputChanNumber(0); 
	size_t		inputSamplesNumber(0);
	int*		inChannels(NULL);
	int			inChannelsCount(0);

	bool		res(false);
	bool		isInputFilter(true);
	int			filterOutputs(-1);

	/* Get audio device parameters */
	double		sampleRate(-1);
	int			bufSize(-1);
	int			bitDepth(-1);
	int			inChan(-1);
	int			outChan(-1);

	if ( !getDeviceSettings(sampleRate, bufSize, bitDepth, inChan, outChan) )
		return false;
	
	/* get duration and calculate the number of samples */
	float	duration ( static_cast<float> (recDuration) );
	
	if (duration <= 0)
	{	
		octave_stdout<<"MEX record error: Record duration should be a positive number.\n";
		return false;
	}
	else
		inputSamplesNumber = static_cast <const size_t> ( ceil( duration*sampleRate ) );

	/* Prepare recording channels */
	prepareChannels( mxChannels, inChannels, inChannelsCount );
	inputChanNumber = inChannelsCount;

	/* Prepare filters */
	res = prepareFilter( mxFilterSettings, isInputFilter, filterOutputs);
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
bool recordDisk(const octave_value *mxFiles, const int32NDArray *mxChannels, const double recDuration, const octave_value* mxFilterSettings)
{
	int			inputChanNumber(0);
	int*		inChannels(NULL);
	int			inChannelsCount(0); 
	float		duration ( static_cast<float> (recDuration) );
	bool		res(false);
	bool		isInputFilter(true);
	int			filterOutputs(-1);

	if (!mxFiles->is_string())
	{
		return false;
	}

	/* Prepare recording channels */
	prepareChannels( mxChannels, inChannels, inChannelsCount );
	inputChanNumber = inChannelsCount;

	/* Prepare filters */
	res = prepareFilter( mxFilterSettings, isInputFilter, filterOutputs);
	if (!res)
	{
		delete [] inChannels;
		return false;
	}
	
	if (filterOutputs != -1)
		inputChanNumber = filterOutputs;

	/* Create char string with the disk folder */
	std::string tmp( mxFiles->string_value() );
	const char* recordDir = tmp.c_str();

	/* Call the function */
	res = recordFiles(  recordDir, (const int) inputChanNumber, (const float)duration, (const int*) inChannels, (const int) inChannelsCount );
	
	/* release memory */
	delete [] inChannels;

	return res;
}


