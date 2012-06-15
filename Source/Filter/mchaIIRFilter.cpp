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

#include "../mchaRecordPlayer.h"
#include "mchaFilter.h"
#include "mchaIIRClass.h"

namespace mcha
{

//==============================================================================
MchaIIRFilter::MchaIIRFilter():
					MchaFilter(),
					tempData(nullptr),
					mchaRecordPlayer( MchaRecordPlayer::getInstance() )
 {
 }

//==============================================================================
 MchaIIRFilter::~MchaIIRFilter()
 {
 }

//==============================================================================
bool MchaIIRFilter::readLine(FileInputStream* inStream, StringArray& valueStrings)
{
	const char singleQuote = 0x27; /* single quote character */
	const char csvSeparator = ','; /* separator character */
	
	String csvStr = String::empty;

	valueStrings.clear();
	
	csvStr = inStream->readNextLine();
	
	/* return the end of file is reached */	
	if (csvStr.isEmpty())
		return false;
	
	int startInd = 0, prevInd = 0;
	/* ignore comment lines and headers */
	if ( (csvStr[0]!='%') && (csvStr[0]!='#') && (csvStr[0]!=singleQuote) && (csvStr[0]!='"') )
	{
		/* analyse the string */
		startInd = 0;
		prevInd = 0;
		while ( ( startInd = csvStr.indexOfChar(startInd, csvSeparator) ) != -1 )
		{	
			valueStrings.add( csvStr.substring(prevInd, startInd) );
			startInd++;
			prevInd = startInd;
		}

		/* read the last value */
		startInd = csvStr.lastIndexOfChar(csvSeparator) + 1;
		/* if we have only one column of data (startInd == -1) we'll start from the beginning of the string */
		valueStrings.add( csvStr.substring( startInd, csvStr.length() ) );
	}
	else
		valueStrings.add( L"comment" );

	return true;
}

//==============================================================================
bool MchaIIRFilter::readCSVFile(String csvFilename)
{
	mchaRecordPlayer->clearError();
	mchaRecordPlayer->dbgOut( L"mchaIIRFilter::readCSVFile" );

	if (filterSettings == NULL)
	{
		mchaRecordPlayer->logError( L"Filter settings should be applied before calling mchaIIRFilter::readCSVFile." );
		return false;
	}


	File fs(csvFilename);
	mchaRecordPlayer->dbgOut( L"\tReading CSV file:\t" + csvFilename );

	/* check if the file exists */
	if ( !fs.existsAsFile() )
	{
		mchaRecordPlayer->logError( L"Unable to find file:\t" + csvFilename );
		return false;
	}

	/* try and open it for reading */
	ScopedPointer<FileInputStream>	csvStream ( fs.createInputStream() );
	if ( csvStream == NULL )
	{
		mchaRecordPlayer->logError( L"Unable to open file for reading:\t" + csvFilename );
		return false;
	}

	/* get the number of filters in the file and the maximum length */
	StringArray values;
	int		maxLength = 0;
	while ( readLine(csvStream, values) )
	{
		if ( values[0] != L"comment" )
		{
			filterCount++;
			maxLength = jmax( maxLength, values.size() );
		}
	}

	/* check the number of filters against inputs/outputs */
	int filtersRequired; // the number of filters in the first cascade
	inputs = filterSettings->getInputs();
	outputs = filterSettings->getOutputs();
	cascades = filterSettings->getCascades();
	oneByOneFilters = filterSettings->getOneByOne();

	if ( oneByOneFilters )
	{	
		filtersRequired = outputs;
	}
	else
		filtersRequired = inputs*outputs;

	/* count filters in remaining cascades as well */
	cascade1filterCount = filtersRequired;
	filtersRequired += (cascades - 1)*outputs; 

	if (filterCount != filtersRequired)
	{
		mchaRecordPlayer->logError( L"Wrong number of filters or file format" );
		mchaRecordPlayer->dbgOut( L"\tFilters present in the file:\t" + String(filterCount) );
		mchaRecordPlayer->dbgOut( L"\tFilters required:\t"+ String(filtersRequired)  );
		mchaRecordPlayer->dbgOut( L"\tFilter inputs:\t" + String(inputs) );
		mchaRecordPlayer->dbgOut( L"\tFilter outputs:\t" + String(outputs) );		
		mchaRecordPlayer->dbgOut( L"\tCascades\t"+ String(cascades)  );
		return false;
	}

	/* create buffer for filter coefficients */
	float**		tapsBuffer;
	if ( !mchaRecordPlayer->allocateBuffer(tapsBuffer, filterCount, maxLength, true, L"tapsBuffer") )
	{
		return false;
	}

	/* rewind the input stream */
	csvStream->setPosition(0);

	/* read data */
	int filterIndex = 0;
	int order = 0;
	while ( readLine(csvStream, values) )
	{
		/* ignore comment lines and headers */
		if ( values[0] != L"comment" )
		{
			/* convert coefficients to float */
			
			for (int i = 0; i< values.size(); i++)
			{
				tapsBuffer[filterIndex][i] = values[i].getFloatValue();
			}
			
			/* filter order is (taps/2-1) */
			order = div( values.size() , 2).quot - 1;

			/* create IIR filter objects and initialise them */
			MchaIIRClass* tempIIR = new MchaIIRClass;
			
			if ( tempIIR->init( tapsBuffer[filterIndex], order ) )
			{
				iirFilter.add(tempIIR); // it know ownes the object
				filterIndex++;
			}
			else
			{
				mchaRecordPlayer->logError( L"Filter initialisation failed" );
				break;
			}
		}
	}	

	/* check for errors */

	if (mchaRecordPlayer->getLastErrorStr().isNotEmpty())
	{
		releaseResources();
		return false;
	}
	else
	{
		mchaRecordPlayer->dbgOut( L"\tcompleted ..." );
	}
	
	mchaRecordPlayer->releaseBuffer<float>(tapsBuffer, filterCount);
	
	return true;
}

//==============================================================================
void MchaIIRFilter::prepareToPlay (double /*sampleRate*/, int samplesPerBlock)
{
	mchaRecordPlayer->dbgOut( L"mchaIIRFilter::prepareToPlay ");
	mchaRecordPlayer->clearError();

	size_t blockLength = static_cast<size_t>(samplesPerBlock);

	/* check if the filter is initialised */
	if (filterSettings == nullptr)
	{
		mchaRecordPlayer->logError( L"\tFilter settings are not specified." );
		return;
	}

	/* define inputs and outputs */
	inputs = filterSettings->getInputs();
	outputs = filterSettings->getOutputs();

	/* read coefficients from the file */
	if(!readCSVFile( filterSettings->getFileName() ) )
	{
		mchaRecordPlayer->logError( L"\tUnable to read filter coefficients from file: \t" + String(filterSettings->getFileName()) );	
		return;
	}

	/* allocate memory for temporary buffer */
	mchaRecordPlayer->allocateBuffer<float>( tempData, blockLength, true, L"tempData" );

	/* initialise channel mapping array */
	resetChannelsMap(outputs);

}

//==============================================================================
void MchaIIRFilter::releaseResources()
{
	mchaRecordPlayer->dbgOut( L"mchaIIRFilter::releaseResources ");
	
	iirFilter.clear(true);

	mchaRecordPlayer->releaseBuffer(tempData);

	/* clear channels mapping array */
	channelsMap.clear();

}


//==============================================================================
void MchaIIRFilter::processBlock (float** sourceData, float** destinationData, int numSamples)
{

	/******* Filter with the first cascade ********/
	
	if ( oneByOneFilters ) // diagonal filter matrix
	{
		for (int ch = 0; ch < inputs; ++ch)
		{
			iirFilter[ch]->process( sourceData[ch], numSamples, destinationData[channelsMap[ch]] );
		}
	}
	else // full filter matrix
	{
		/* clear destinationData */
		for (int ch = 0; ch < outputs; ++ch)
		{
			memset( destinationData[channelsMap[ch]], 0, numSamples*sizeof(float) );
		}		
		
		/* do filtering */
		for (int indInputs = 0; indInputs <  inputs; ++indInputs)
		{
			/* loop through IIR filters for channel indCols */
			for (int indOutputs = 0; indOutputs < outputs; ++indOutputs)
			{
				/* filter */
				iirFilter[indInputs*inputs + indOutputs]->process( sourceData[indInputs], numSamples, tempData );

				/* add to output */
				addBuffer( destinationData[channelsMap[indOutputs]], tempData, numSamples );
			}
		}
	}

	/******* Filter with remaining one-by-one cascades ********/
	for (int c=0; c < cascades-1; c++)
	{
		for (int ch = 0; ch < outputs; ch++)
		{
			/* filter */
			iirFilter[cascade1filterCount + c*outputs + ch]->process(  destinationData[channelsMap[ch]], numSamples, tempData );

			/* replace the output */
			memcpy( destinationData[channelsMap[ch]], tempData, numSamples*sizeof(float) );
		}	
	}

}

}
