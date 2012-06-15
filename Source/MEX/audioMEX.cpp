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

#include <stdio.h>
#include "mex.h"

void releaseFileList(char** audioFiles, size_t filesCount);


/* ------------------------------------------------------------------------------------------------------------------------- */
void prepareChannels(const mxArray* channelList, int* &chan, int &chanCount)
{
	chanCount = (int) mxGetNumberOfElements(channelList);
	chan = new int[chanCount];
	
	double* chanOrigin = mxGetPr(channelList);
	for (int i=0; i < chanCount; i++)
	{
		chan[i] = (int)chanOrigin[i];
	}

}

/* ------------------------------------------------------------------------------------------------------------------------- */
bool prepareMemoryPlay(const mxArray *mem, bool &isSingle, void** &memData, size_t &samplesCount, size_t &channelCount)
{
	channelCount = mxGetN(mem);
	samplesCount = mxGetM(mem);

	/* Check if the input data are in single-precision format */
	if (mxIsSingle(mem))
	{
		isSingle = true;		
		float* dataOrigin = (float*) mxGetPr(mem);
		memData = new void* [channelCount];

		for (size_t i=0; i < channelCount; ++i)
			memData[i] = (void*) (dataOrigin + i*samplesCount);
		return true;
	}
	else
	{
		isSingle = false;
		double* dataOrigin = (double*) mxGetPr(mem);
		memData = new void* [channelCount];

		for (size_t i=0; i < channelCount; ++i)
			memData[i] = (void*) (dataOrigin + i*samplesCount);
		return true;
	}
}

/* ------------------------------------------------------------------------------------------------------------------------- */
void releaseMemoryBuffer( void** memData)
{
	if (memData == NULL)
		return;
	
	delete [] memData;
	memData = NULL;
}

/* ------------------------------------------------------------------------------------------------------------------------- */
float getDuration(const mxArray* duration)
{
	double* pData = mxGetPr(duration);
	return (float)*pData;
}

/* ------------------------------------------------------------------------------------------------------------------------- */
bool prepareFileList(const mxArray* fileList, char** &audioFiles, size_t &filesCount)
{

	if (!mxIsCell(fileList))
		return false;

	filesCount = mxGetNumberOfElements(fileList);
	audioFiles = new char* [filesCount];

	/* Create the array of audio files and channel numbers */
	for (size_t i=0; i < filesCount; ++i)
	{
		mxArray* curArray = mxGetCell(fileList, i);
		if (curArray != NULL)
		{
			audioFiles[i] = mxArrayToString(curArray);
		}
		else
		{
			releaseFileList(audioFiles, i-1);
			audioFiles = NULL;
			return false;
		}
	}

	return true;
}

/* ------------------------------------------------------------------------------------------------------------------------- */
void releaseFileList(char** audioFiles, size_t filesCount)
{
	if (audioFiles == NULL)
		return;

	for (size_t j=0; j<filesCount; ++j)
	{
		mxFree(audioFiles[j]);
		audioFiles[j] = NULL;
	}
	delete [] audioFiles;
}

/* ------------------------------------------------------------------------------------------------------------------------- */
bool prepareFilter(const mxArray* filterSettings, bool isInputFilter, int &filterOutputs)
{
	bool	res;
	int		filterInputs = -1;
	
	filterOutputs = -1;

	if (filterSettings != NULL)
	{
		/* Return if the matrix is empty */
		if (mxIsEmpty(filterSettings))
		{
			return true;
		}
		
		/* Check filter settings */
		if (!mxIsChar(filterSettings))
		{
			mexErrMsgTxt("Filter settings should be a string containing a valid file name.");
			return false; 			
		}

		char* settingsFileName = mxArrayToString(filterSettings);
		if (settingsFileName == NULL)
		{
			mexErrMsgTxt("Unable to convert filter settings to a string.");
			return false; 				
		}

		/* Add filter */
		res = addFilter(settingsFileName, isInputFilter);
		if (!res)
		{
			mexErrMsgTxt( getLastError() );
			return false; 				
		}

		/* Get the number of filter outputs */
		getFilterSettings(isInputFilter,  filterInputs, filterOutputs);
		if ((filterInputs == -1)||(filterOutputs == -1))
		{
			char buffer[256];
			sprintf(buffer, "Wrong number of filter inputs or outputs: filterInputs = %d; filterOutputs = %d", filterInputs, filterOutputs);
			mexErrMsgTxt( buffer );
			return false; 	 
		}
		
		mxFree(settingsFileName);

		return true;		
	}
	else
	{
		return true;
	}
}