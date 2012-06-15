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

DEFUN_DLD (m_setPosition, args, nargout, "set position of the current playback")
{
	int nargin ( args.length() );

	charMatrix argError( "Wrong number or type of function arguments.\n" );

	/* Check the number of input/output arguments */
	if ( (nargin != 1) || (nargout > 1) ) 
	{
		return octave_value(argError);
	}
 
	if (!args(0).is_real_scalar())
	{
		charMatrix typeError( "Position should be a single value of numeric type.\n" );
		return octave_value(typeError);	
	}
	
	double newPos = args(0).double_value();
	if (error_state)
	{
		charMatrix dataError("Unable to convert new position to a double value.\n");
		return octave_value(dataError);
	}
    
    if (!setPosition(newPos))
    {
		charMatrix funcError("Function failed.\n");
		return octave_value(funcError);
    }

	charMatrix mchaError( getLastError() );	
	return octave_value(mchaError); 
}
