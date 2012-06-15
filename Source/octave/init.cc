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

DEFUN_DLD (m_init, args, nargout, "configure audio device")
{
	int nargin = args.length ();
	if (nargin > 2)
		print_usage ();

	if (nargin > 0)
	{
		/* xml settings file was specified */
		charMatrix xmlFile = args(0).char_matrix_value();
		if (! error_state)
		{
			std::string tmp = xmlFile.row_as_string (0);
			initAudioDeviceFile( tmp.c_str() );
		}
		else
		{
			octave_stdout << "The first argument should be a string containing a valid file name.\n";
		}

		/* debug mode was specified */
		if (nargin == 2)
		{
			charMatrix dbgMode = args(1).char_matrix_value();		
			if (! error_state)
			{
				std::string tmp = dbgMode.row_as_string (0);
				setDebugMode( tmp.c_str() );
			}
			else
			{
				octave_stdout << "The second argument should be a string containing a valid debug mode.\n";
			}		
		}

	}
	else
	{
		initAudioDevice();

	}
	
	charMatrix ch( getLastError() );
	return octave_value(ch);
}

