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

bool isCell(const octave_value_list* fargs, const octave_idx_type ind) { return fargs->operator()(ind).is_cell(); };
bool isString(const octave_value_list* fargs, const octave_idx_type ind) { return fargs->operator()(ind).is_string(); };
bool isNumeric(const octave_value_list* fargs, const octave_idx_type ind) { return fargs->operator()(ind).is_numeric_type(); };

/* ------------------------------------------------------------------------------------------------------------------------- */
void prepareChannels(const int32NDArray* channelList, int* &chan, int &chanCount)
{
	chanCount = channelList->nelem();
	chan = new int[chanCount];	
	
	for (int i=0; i < chanCount; ++i)
	{
		chan[i] = channelList->elem(i);
	}

}

/* ------------------------------------------------------------------------------------------------------------------------- */
void prepareMemoryPlay(const FloatMatrix *memFloat, float** &memData, size_t &samplesCount, size_t &channelCount)
{

	channelCount = memFloat->cols();
	samplesCount = memFloat->rows();

	memData = new float* [channelCount];

	for (size_t i=0; i < channelCount; ++i)
	{
		memData[i] = const_cast<float*> ( memFloat->data() ) + i*samplesCount;
	}
}

/* ------------------------------------------------------------------------------------------------------------------------- */
void prepareMemoryPlay(const Matrix *memDouble, double** &memData, size_t &samplesCount, size_t &channelCount)
{
	channelCount = memDouble->cols();
	samplesCount = memDouble->rows();

	memData = new double* [channelCount];

	for (size_t i=0; i < channelCount; ++i)
	{
		memData[i] = const_cast<double*> ( memDouble->data() ) + i*samplesCount;
	}
}

/* ------------------------------------------------------------------------------------------------------------------------- */
template <class D> void releaseMemoryBuffer( D** memData)
{
	if (memData == NULL)
		return;
	
	delete [] memData;
	memData = NULL;
}

/* ------------------------------------------------------------------------------------------------------------------------- */
void releaseFileList(char** audioFiles, size_t filesCount)
{
	if (audioFiles == NULL)
		return;

	for (size_t j=0; j<filesCount; ++j)
	{
		delete [] audioFiles[j];
		audioFiles[j] = NULL;
	}
	delete [] audioFiles;
}

/* ------------------------------------------------------------------------------------------------------------------------- */
bool prepareFileList(const Cell* fileList, char** &audioFiles, size_t &filesCount)
{

	Array<std::string> aStr = fileList->cellstr_value();
	if ( error_state )
	{
		octave_stdout << "Wrong input to function prepareFileList\n";
		return false;
	}

	filesCount = aStr.nelem();
	audioFiles = new char* [filesCount];

	/* Create the array of audio files and channel numbers */
	for (size_t i=0; i < filesCount; ++i)
	{
		if (true) //( fileList->elem(i).is_cellstr() )
		{
			std::string curStr ( aStr.elem(i) );
			size_t len = curStr.length();
			audioFiles[i] = new char[len+1];
			audioFiles[i][len] = 0;
			memcpy( audioFiles[i], curStr.c_str(), len*sizeof(char) );
		}
		else
		{
			octave_stdout << "Wrong file name.\n";
			releaseFileList(audioFiles, i-1);
			audioFiles = NULL;
			return false;
		}
	}

	return true;
}


/* ------------------------------------------------------------------------------------------------------------------------- */
bool prepareFilter(const octave_value* filterSettings, bool isInputFilter, int &filterOutputs)
{
	bool	res;
	int		filterInputs = -1;
	
	filterOutputs = -1;

	if (filterSettings != NULL)
	{	
		if (!filterSettings->is_string())
		{
			return false;
		}
		
		std::string settingsFileName ( filterSettings->string_value() );

		/* Add filter */
		res = addFilter(settingsFileName.c_str(), isInputFilter);
		if (!res)
		{
			octave_stdout << getLastError() << "\n";
			return false; 				
		}

		/* Get the number of filter outputs */
		getFilterSettings(isInputFilter,  filterInputs, filterOutputs);
		if ((filterInputs == -1)||(filterOutputs == -1))
		{
			octave_stdout << "Wrong number of filter inputs or outputs:\n\tfilterInputs = " << filterInputs << 
							"\n\tfilterOutputs = " << filterOutputs << "\n";
			return false; 	 
		}

	}
	
	return true;
}