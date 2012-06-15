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

DEFUN_DLD (m_getData, args, nargout, "get recorded samples from internal memory")
{

	if (isRunning())
	{
		charMatrix errorStr( "Function failed. The process is still running.\n" );
		return octave_value(errorStr);
	}
	
	if (( args.length() > 0 )||(nargout != 1))
	{
		charMatrix errorStr("Wrong number of function arguments.");	
		return octave_value(errorStr);
	}	
	
	octave_idx_type channelCount = getRecordedChannelsNum();
	octave_idx_type samplesCount = getRecordedSamplesNum();

	FloatMatrix	dat( samplesCount, channelCount );

	float** memData = new float* [channelCount];

	for (octave_idx_type i=0; i < channelCount; ++i)
	{
		memData[i] = (float*) ( dat.data() + i*samplesCount );
	}


	if ( !getData_s(memData) )
	{
		delete [] memData;
		charMatrix errorStr("Unable to retrieve data from memory. Call getError() for details.");
		return octave_value(errorStr);
	}
	else
	{
		delete [] memData;
		return octave_value(dat);
	}

}

