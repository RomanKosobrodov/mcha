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

#include "mex.h"
#include "audioMEX.h"

void mexFunction(
	int nlhs, 	mxArray *plhs[],
	int nrhs, const mxArray *prhs[])
{
	Mcha	mcha;

	if ( mcha.noError() )  
	{
		plhs[0] = mxCreateString( mcha.getVersion() );
	}
	else
	{
		plhs[0] = mxCreateString( mcha.getErrorStr() );
	}
}

