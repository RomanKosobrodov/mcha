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
#include "Cell.h"
#include "audioMEX.h"
#include "audioMEX.cc"

/* Functions declarations */
bool playDisk(const Cell *mxFiles, const int32NDArray *mxChannels, const octave_value* mxFilterSettings);
bool playMemory(const octave_value *inData,  const int32NDArray *mxChannels, const octave_value* mxFilterSettings);

DEFUN_DLD (m_play, args, nargout, "play audio from memory or disk")
{

	bool res = false;
	int nargin = args.length();

	charMatrix argError( "Wrong number or type of function arguments.\n" );


	/* Check the number of input/output arguments */
	if (nargin < 2) 
	{
		return octave_value(argError);
	}

	int32NDArray inputChannels( args(1).int32_array_value() );

	if ( args(0).is_cell() )
	{
		const Cell inputData( args(0).cell_value() );
		
		/* play from disk */
		if (nargin == 2)
		{
			/* no filters */
			playDisk(&inputData, &inputChannels, NULL);
		}
		/* use filters */
		else if (nargin == 3)
		{
			playDisk(&inputData, &inputChannels, &args(2) );
		}
		else
		{
			/* return error */
			return octave_value(argError);
		}
	}
	else
	{
		/* play from memory */
		if (nargin == 2)
		{		
			/* no filters */
			playMemory(&args(0), &inputChannels, NULL);
		}
		else if (nargin == 3)
		{
			playMemory(&args(0), &inputChannels, &args(2) );
		}
		else
		{
			/* return error */
			return octave_value(argError);	
		}		
	}
	
	charMatrix mchaError( getLastError() );
	return octave_value(mchaError);
}


//=========================================================================================================
bool playDisk(const Cell *mxFiles, const int32NDArray *mxChannels, const octave_value* mxFilterSettings)
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
		releaseFileList(audioFiles, filesNumber);
		delete [] channels;		
		return false;
	}

	/* call PlayRecord function */
	res = playFiles ((const char**) audioFiles, (const int) filesNumber, (const int*) channels, (const int) outChannelsCount);

	releaseFileList(audioFiles, filesNumber);
	delete [] channels;
	
	return res;
}


//=========================================================================================================
bool playMemory(const octave_value *inData,  const int32NDArray *mxChannels, const octave_value* mxFilterSettings)
{

	size_t		outputChanNumber (0); 
	size_t		outputSamplesNumber (0);
	int*		outChannels (NULL);
	int			outChannelsCount (0);
	bool		res (false);
	bool		isInputFilter(false);
	int			filterOutputs(-1);
	bool		isDataSingle ( inData->is_single_type() );

	/* Prepare playback channels */
	prepareChannels( mxChannels, outChannels, outChannelsCount);

	/* Prepare filters */
	res = prepareFilter( mxFilterSettings, isInputFilter, filterOutputs);
	if (!res)
	{
		delete [] outChannels;	
		return false;
	}	

	/* call PlayRecord function */
	if (isDataSingle)
	{
		float**		outputData (NULL);
		
		FloatMatrix dataFloat(inData->float_matrix_value());
		if (error_state)
		{
			octave_stdout << "Unable to convert data to single precision format.\n";
			return false;
		}

		prepareMemoryPlay( &dataFloat, outputData, outputSamplesNumber, outputChanNumber );
		res = playData_s ((const float**) outputData, (const int) outputChanNumber, (const size_t) outputSamplesNumber, (const int*) outChannels, (const int) outChannelsCount);
		delete [] outputData;
	}
	else
	{
		double**	outputData (NULL);

		Matrix dataDouble(inData->matrix_value());
		if (error_state)
		{
			octave_stdout << "Unable to convert data to double precision format.\n";
			return false;
		}

		prepareMemoryPlay( &dataDouble, outputData, outputSamplesNumber, outputChanNumber );
		res = playData_d ((const double**) outputData, (const int) outputChanNumber, (const size_t) outputSamplesNumber, (const int*) outChannels, (const int) outChannelsCount);	
		delete [] outputData;
	}

	delete [] outChannels;

	return res;
}
