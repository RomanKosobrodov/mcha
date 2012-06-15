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

DEFUN_DLD (m_setGain, args, nargout, "set gain for individual playback channels")
{
	int nargin ( args.length() );

	charMatrix argError( "Wrong number or type of function arguments.\n" );

	/* Check the number of input/output arguments */
	if ( (nargin != 2) || (nargout > 1) ) 
	{
		return octave_value(argError);
	}

	/* Check if the data is valid */
	if (!isNumeric( &args, 0 ) || !isNumeric( &args, 1 )  )
	{
		return octave_value(argError);
	}

	/* Get the size of the channels matrix */
	int32NDArray channelsData( args(0).int32_array_value() );
	if (error_state)
	{
		charMatrix chanError("Unable to convert channels to array of integer values.\n");
		return octave_value(chanError);
	}

	FloatMatrix	 gainsData( args(1).float_matrix_value() );
	if (error_state)
	{
		charMatrix gainError("Unable to convert gains to array of floating point values.\n");
		return octave_value(gainError);
	}

	int channelsNumber( channelsData.numel() );
	int gainsNumber( gainsData.numel() );
	
    /* Check is the lengths are the same */
    if (channelsNumber != gainsNumber)
    {
 		charMatrix numError("Wrong call to function: the lenghts of the channels and gains arrays should be the same");
		return octave_value(numError);   
    }
    
	/* Create and fill channels and gains arrays */
	if (channelsNumber > 0)
	{
		int* chans(NULL);
		int  chansCount(0);

		prepareChannels( &channelsData, chans, chansCount);
		
		/* Call the MCHA function */
		if (!setGain ( chans, chansCount, const_cast<float*> ( gainsData.data() ) ))
		{
			charMatrix funcError( "Function failed.\n" );
			return octave_value(funcError); 
		}
	}
	
	charMatrix mchaError( getLastError() );	
	return octave_value(mchaError); 
}