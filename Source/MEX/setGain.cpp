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

/* Input Arguments */
#define	channelsArray	prhs[0]
#define gainsArray		prhs[1]

/* Output Arguments */
#define	errorMsg	plhs[0]

/* Entry point */
void mexFunction(
	int nlhs, 	mxArray *plhs[],
	int nrhs, const mxArray *prhs[])
{
	int*	channels;
	int		channelsNumber;
	float*	gains;
	int		gainsNumber;

	/* Load MCHA library */
	Mcha	mcha;
	if ( !mcha.noError() )
	{	
		mexErrMsgTxt( mcha.getErrorStr() );	
		return;
	}
    
	errorMsg = mxCreateString("");

	/* Check the number of input/output arguments */
	if ((nrhs != 2) || (nlhs>1)) 
	{
		errorMsg = mxCreateString("Wrong call to function.");
		return;
	}

	/* Check if the data is valid */
	if (!mxIsNumeric(channelsArray) || !mxIsNumeric(gainsArray)  )
	{
		errorMsg = mxCreateString("Wrong call to function: channels and gains arrays should be numeric vectors");
		return;
	}

	/* Get the size of the channels matrix */
	double* channelsData = mxGetPr(channelsArray);
	channelsNumber = (int) mxGetNumberOfElements(channelsArray);
    double* gainsData = mxGetPr(gainsArray);
	gainsNumber = (int) mxGetNumberOfElements(gainsArray);
	
    /* Check is the lengths are the same */
    if (channelsNumber != gainsNumber)
    {
 		errorMsg = mxCreateString("Wrong call to function: the lenghts of the channels and gains arrays should be the same");
		return;   
    }
    
	/* Create and fill channels and gains arrays */
	if (channelsNumber > 0)
	{
		channels = new int[channelsNumber];
		gains = new float[channelsNumber];
		for (int i= 0; i<channelsNumber; i++)
		{
			channels[i] = (int) channelsData[i];
			gains[i] = (float) gainsData[i];
		}
		
		/* Call the function */
		if ( !mcha.setGain (channels, channelsNumber, gains) )
		{
			errorMsg = mxCreateString( mcha.getLastError() );	
		}
	}

	/* Clean up */
	delete [] channels;
	delete [] gains;
}
