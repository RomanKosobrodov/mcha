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

#include "fftw3.h"
#include "mchaFilter.h"
#include "../mchaRecordPlayer.h"
#include "../mchaWavAudioFormat.h"

#include <limits>

namespace mcha
{

//==============================================================================
MchaFIRFilter::MchaFIRFilter():
					MchaFilter(),
					K(0),
					KBytes(0),
					Kfft(0),
					L(0),
					cbBytes(0),
					swap(false),
					circularBuffer(nullptr),
					irData(nullptr),
					tempData(nullptr),
					sigfft(nullptr),
					resBuffer(nullptr),
					tempDataFloat(nullptr),
					tmpc(nullptr),
					resIndex(0),
					writeIndex(0),
					resBufferLen(0),
					irBlocksNum(0),
					irfft(nullptr),
					irNumber(0),
					irLength(0),
					totalFilterCount(0),
					curBufferLen(0),
					curBufferLenBytes(0)
 {
 }

//==============================================================================
 MchaFIRFilter::~MchaFIRFilter()
 {
 }


//==============================================================================
bool MchaFIRFilter::readCSVFile(String csvFilename, int &irNumber, int64 &irLength)
{
	const char singleQuote = 0x27; /* single quote character */
	const char csvSeparator = ','; /* value separator character */

	mchaRecordPlayer->clearError();
	mchaRecordPlayer->dbgOut( "mchaFIRFilter::readCSVFile" );

	File fs(csvFilename);
	mchaRecordPlayer->dbgOut( "\tReading CSV file:\t" + csvFilename );

	/* check if the file exists */
	if ( !fs.existsAsFile() )
	{
		mchaRecordPlayer->logError( "Unable to find file:\t" + csvFilename );
		return false;	
	}

	/* try and open it for reading */
	ScopedPointer<FileInputStream>	csvStream ( fs.createInputStream() );
	if (csvStream == nullptr)
	{
		mchaRecordPlayer->logError( "Unable to open file for reading:\t" + csvFilename );
		return false;	
	}

	/* get the number of rows and columns */
	int		csvRows = 0;
	int		csvCols = 0;
	String	csvStr("is not empty"); /* this is to enter the while loop below */

	while ( csvStr.isNotEmpty() )
	{
		csvStr = csvStream->readNextLine();
		/* ignore comment lines and headers */
		if ( csvStr.isNotEmpty() && (csvStr[0]!='%') && (csvStr[0]!='#') && (csvStr[0]!=singleQuote) && (csvStr[0]!='"') )
		{
			csvRows++;
			/* analyse the first string to get the number of columns */
			if (csvRows == 1)
			{	int startInd =0;
				bool continueLoop = true;
				while (continueLoop)
				{	
					startInd = csvStr.indexOfChar(startInd, csvSeparator);
					if (startInd != -1)
					{
						csvCols++;
						startInd++;
					}
					else
						continueLoop = false;
					
				}
			    /* count the last value */
				if ( csvStr.isNotEmpty() )
					csvCols++;
			}
		}
	}
	/* check the results */
	if ((csvRows == 0)||(csvCols == 0))
	{
		mchaRecordPlayer->logError( "Wrong format of CSV file" );
		return false;	
	}

	/* rewind the file */
	csvStream->setPosition(0);

	/* allocate memory and read data */
	irData = fftwf_alloc_real(csvCols*csvRows);
	if (irData == NULL)
	{
		mchaRecordPlayer->logError( "Unable to allocate memory for csvData." );
		return false;	
	}

	String tmpStr(String::empty);
	int		colInd = 0;
	int		rowInd = 0;
	int		startInd =0;
	int		prevInd = 0;
	bool	continueLoop = true;
	csvStr = "is not empty"; /* this is to enter the while loop below */

	while ( csvStr.isNotEmpty() )
	{
		csvStr = csvStream->readNextLine();

		/* ignore comment lines and headers */
		if ( csvStr.isNotEmpty() && (csvStr[0]!='%') && (csvStr[0]!='#') && (csvStr[0]!=singleQuote) && (csvStr[0]!='"') )
		{
			/* analyse the string and convert to float */
			startInd = 0;
			prevInd = 0;
			continueLoop = true;
			while (continueLoop)
			{	
				startInd = csvStr.indexOfChar(startInd, csvSeparator);
				if (startInd != -1)
				{
					tmpStr = csvStr.substring(prevInd, startInd);
					startInd++;
					prevInd = startInd;
					/* conversion */
					irData[rowInd*csvRows + colInd] = tmpStr.getFloatValue();
					rowInd++;
				}
				else
					continueLoop = false;
			}
			/* read the last value */
			startInd = csvStr.lastIndexOfChar(csvSeparator) + 1;
			/* if we have only one column of data (startInd == -1) we'll start from the beginning of the string */
			tmpStr = csvStr.substring( startInd, csvStr.length() );
			/* conversion */
			irData[rowInd*csvRows + colInd] = tmpStr.getFloatValue();

			/* next column */
			rowInd = 0;
			colInd++;
		}
	}	

	
	mchaRecordPlayer->dbgOut("\tcompleted ...");

	irNumber = csvCols;	/* the matrix has been transposed */
	irLength = csvRows;

	return true;
}

//==============================================================================
bool MchaFIRFilter::readWAVFile(String audioFilename, int &irNumber, int64 &irLength)
{
	mchaRecordPlayer->clearError();
	mchaRecordPlayer->dbgOut( "mchaFilter::readWAVFile" );
	mchaRecordPlayer->dbgOut( "\tReading audio file:\t" + audioFilename );	


	File af(audioFilename);
	if ( !af.existsAsFile() )
	{
	   mchaRecordPlayer->logError( "mchaFilter::readWAVFile failed. Unable to open audio file " + audioFilename );		
	   return false;	
	}

	ScopedPointer<FileInputStream> audioStream ( af.createInputStream() );
	if (audioStream == nullptr)
	{
	   mchaRecordPlayer->logError( "mchaFilter::readWAVFile failed. Unable to open audio file as an input stream: " + audioFilename );		
	   return false;		
	}

	/* create a reader class */
	MchaWavAudioFormat wavAudioFormat;
	ScopedPointer<AudioFormatReader> wavReader ( wavAudioFormat.createReaderFor(audioStream, true) );

	/* get the number of channels and file length */
	irNumber = wavReader->numChannels;
	irLength = wavReader->lengthInSamples;	

	/* allocate memory for irData */
	try
	{
		irData = fftwf_alloc_real( (size_t)(irNumber*irLength) );
	}
	catch (std::bad_alloc)
	{
	   mchaRecordPlayer->logError( "mchaFilter::readWAVFile failed. Unable to allocate memory for irData." );
	   return false;	
	}

	/* prepare the array of pointers to 'deinterleaved' samples */
	float** pDest = nullptr;
	try 
	{
		pDest = (float**) fftwf_malloc(irNumber * sizeof(float));
	}
	catch (std::bad_alloc)
	{
	   mchaRecordPlayer->logError( "mchaFilter::readWAVFile failed. Unable to allocate memory for pDest." );
	   return false;	
	}

	for (int chInd = 0; chInd < irNumber; chInd ++)
	{
		pDest[chInd] = irData + chInd*irLength;
	}

	bool res = wavReader->readSamples( (int**) pDest, irNumber, 0, 0, (int) irLength );
	if (!res)
	{
	   mchaRecordPlayer->logError( "mchaFilter::readWAVFile failed. Unable to read data from " + audioFilename  );
	   delete wavReader;
	   fftwf_free(pDest);
	   return false;	
	}

	fftwf_free(pDest);	
	return true;   
}

//==============================================================================
bool MchaFIRFilter::isAudioFile(String audioFilename)
{
	File af(audioFilename);
	if ( af.existsAsFile() )
	{
		if (af.getFileExtension().toUpperCase() == ".WAV")
			return true;
	}
	
	return false;
}

//==============================================================================
void MchaFIRFilter::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	mchaRecordPlayer->dbgOut("mchaFIRFilter::prepareToPlay ");
	mchaRecordPlayer->clearError();


	/* check if the filter is initialised */
	if (filterSettings == NULL)
	{
		mchaRecordPlayer->logError( "\tFilter settings are not specified." );
		return;
	}
	
	/* define inputs and outputs */
	inputs = filterSettings->getInputs();
	outputs = filterSettings->getOutputs();	

	/* check if the sample rate is the same */
	if ( sampleRate != filterSettings->getSamplingFrequency() )
	{
		mchaRecordPlayer->logError( "\tFilter sampling rate is different from device sampling rate." );
		return;
	}

	/* signal buffer lenght and overlap buffer length */
	K = samplesPerBlock;
	Kfft = (int) pow( 2.0f, ceil( log(float(K))/log(2.0f) )  );	// base 2 log of buffer length rounded to the next integer
	L = 2*Kfft;

	KBytes = K*sizeof(float);
	cbBytes = (K+1)*sizeof(fftwf_complex);

	/* check if the buffer length is an order of two */
	if ( Kfft != K ) // not an order of two
		mchaRecordPlayer->dbgOut( "\tWARNING: buffer length is not an order of two" );


	/* allocate and clear memory for circular signal buffers */
	swap = false; // no shift required at the first run
	if (!mchaRecordPlayer->allocateBuffer( circularBuffer, inputs, L, true, "circularBuffer" ) )
		return;

	/* allocate memory for signal FFTs */
	try 
	{
		sigfft = fftwf_alloc_complex( cbBytes );
	}
	catch( std::bad_alloc ) // memory allocation failed
	{
		mchaRecordPlayer->logError( "\tMemory allocation for sigfft failed." );
		return;
	}

	/* read IR filter files */
	/* irData contains filter samples in 'flattened' format similar to Matlab matrices (as one-dimentional array)*/
	if ( isAudioFile( filterSettings->getFileName() ) )	// WAV, AIFF etc.
	{
		if(!readWAVFile( filterSettings->getFileName(), irNumber, irLength) )
		{
			mchaRecordPlayer->logError( "\tUnable to read IR file." );	
			fftwf_free(irData);	/* release temporary IR data */
			return;
		}
	}
	else // CSV file
	{
		if (!readCSVFile( filterSettings->getFileName(), irNumber, irLength ) ) // function failed
		{
			mchaRecordPlayer->logError( "\tUnable to read IR file." );
			return;
		}
	}
	/* check data format and convert irData if required */
	int		singleIRLength = filterSettings->getFilterLength(); // filter length from settings file

	if ( !convertData() )
	{
		mchaRecordPlayer->logError("\tWrong format of filter settings or IR files: invalid length or number of filters.");
		mchaRecordPlayer->dbgOut( "\t\tnumber of channels in data file:\t" + String(irNumber) );
		mchaRecordPlayer->dbgOut( "\t\tlength in samples of each channel:\t" + String(irLength) );
		mchaRecordPlayer->dbgOut( "\t\tfilter settings:" );
		mchaRecordPlayer->dbgOut( "\t\tlength:\t"+String(singleIRLength) );
		mchaRecordPlayer->dbgOut( "\t\tinputs:\t"+String(inputs) );
		mchaRecordPlayer->dbgOut( "\t\toutputs:\t"+String(outputs) );
		return;	
	}


	/* get the total number of filters  */
	if ( filterSettings->getOneByOne() )
		totalFilterCount = irNumber;
	else
		totalFilterCount = inputs*outputs;

	/* calculate the number of IR blocks */
	irBlocksNum = (int) ceil( float(singleIRLength) / K );
	int irLastBlockLen = singleIRLength - (irBlocksNum - 1)*K; // the length of the last IR block
	if (irLastBlockLen == 0) 
		irLastBlockLen = K;

	/* allocate IR FFT buffers */
	if (!mchaRecordPlayer->allocateBuffer(irfft, totalFilterCount*irBlocksNum, L, true, "irfft" ) )
		return;

	/* allocate and clear memory for overlap-and-save buffers */
	resIndex = 0;
	resBufferLen = irBlocksNum + 1;

	if (!mchaRecordPlayer->allocateBuffer( resBuffer, outputs*(irBlocksNum + 1), cbBytes, true, "resBuffer" ) )
		return;

	/* allocate temporary input buffers for IRs */
	float* tempDataIn(nullptr);
	try
	{
		tempDataIn = fftwf_alloc_real(L);
	}
	catch ( std::bad_alloc )
	{					
		mchaRecordPlayer->logError( "\tMemory allocation for tempDataIn failed." );
		return;			
	}
	

	/* define fftw plan */
	irplan = new TransformPlan( L, tempDataIn, irfft[0], FFTW_MEASURE);

	/* do Fourier transform of IRs */
	int64 curIndex = 0;

	if ( filterSettings->getOneByOne() ) // diagonal convolution matrix
	{
		for (int ch = 0; ch < inputs; ch++)
		{
			for (int indBlock =0; indBlock < irBlocksNum - 1; indBlock++) // First (irBlocksNum - 1) blocks of IRs
			{
				curIndex = ch*irLength + indBlock*K ;
				/* zero temporary buffer */
				zeromem(tempDataIn, L*sizeof(float));

				/* copy K samples */
				memcpy(tempDataIn, irData + curIndex, K*sizeof(float));

				/* execute FFT (new-array style) */
				tmpc = irfft[ch*irBlocksNum + indBlock];
				fftwf_execute_dft_r2c( irplan->getPlan(), tempDataIn, tmpc );

				/* normalise irfft by the buffer length L */
				mulBuffer( tmpc, 1.0f/L, tmpc, L);
			}

			/* last block padded with zeros to K samples if required */
			curIndex = ch*irLength + (irBlocksNum-1)*K;
			/* zero temporary buffer */
			zeromem(tempDataIn, L*sizeof(float));
			/* copy irLastBlockLen samples */
			memcpy(tempDataIn, irData + curIndex, irLastBlockLen*sizeof(float));
			
			/* execute FFT (new-array style) */
			tmpc = irfft[ch*irBlocksNum + (irBlocksNum-1)];
			fftwf_execute_dft_r2c( irplan->getPlan(), tempDataIn,  tmpc);	
			
			/* normalise irfft by the buffer length L */
			mulBuffer( tmpc, 1.0f/L, tmpc, L);
		}
	}
	else // full convolution matrix
	{
		for (int indCols = 0; indCols <  inputs; ++indCols)
		{
			
			for (int indRows = 0; indRows < outputs; ++indRows)
			{
			
				for (int indBlock = 0; indBlock < irBlocksNum - 1; ++indBlock) // First (irBlocksNum - 1) blocks of IRs
				{
					curIndex = indCols*irLength + indRows*singleIRLength + indBlock*K ;

					/* zero temporary buffer */
					zeromem(tempDataIn, L*sizeof(float));
					/* copy K samples */
					memcpy(tempDataIn, irData + curIndex, K*sizeof(float));

					/* execute FFT (new-array style) */
					tmpc = irfft[ (indCols*outputs + indRows)*irBlocksNum + indBlock ];
					fftwf_execute_dft_r2c( irplan->getPlan(), tempDataIn, tmpc );

					/* normalise irfft by the buffer length L */
					mulBuffer( tmpc, 1.0f/L, tmpc, L);

				}
				
				/* last block padded with zeros to K samples if required */
				curIndex = indCols*irLength + indRows*singleIRLength + (irBlocksNum-1)*K;
				
				/* zero temporary buffer */
				zeromem(tempDataIn, L*sizeof(float));
				/* copy K samples */
				memcpy(tempDataIn, irData + curIndex, irLastBlockLen*sizeof(float));
				
				/* execute FFT (new-array style) */
				tmpc = irfft[ (indCols*outputs + indRows)*irBlocksNum + (irBlocksNum-1)];
				fftwf_execute_dft_r2c( irplan->getPlan(), tempDataIn, tmpc );	

				/* normalise irfft by the buffer length L */
				mulBuffer( tmpc, 1.0f/L, tmpc, L);

			}
		}
	}

	/* create plan for signal ffts */
	signalPlan = new TransformPlan( L, circularBuffer[0], sigfft, FFTW_MEASURE);

	/* create plan for inverse ffts */
	invPlan = new TransformPlan( L, resBuffer[0], tempDataIn, FFTW_MEASURE);

	/* allocate temporary complex buffer */
	try
	{
		tempData = fftwf_alloc_complex(L);
	}
	catch ( std::bad_alloc )
	{					
		mchaRecordPlayer->logError( "\tMemory allocation for tempData failed." );
		return;			
	}

	/* allocate temporary real buffer */
	try
	{
		tempDataFloat = fftwf_alloc_real(L);
	}
	catch ( std::bad_alloc )
	{					
		mchaRecordPlayer->logError( "\tMemory allocation for tempDataFloat failed." );
		return;			
	}
	
	/* release temporary IR data */
	fftwf_free(irData);
	irData = nullptr;	

	fftwf_free(tempDataIn);

	/* initialise channel mapping array */
	resetChannelsMap(outputs);

}

//==============================================================================
void MchaFIRFilter::releaseResources()
{

	DBG("MchaFIRFilter::releaseResources() ");
	
	if (filterSettings == nullptr) // No memory has been allocated - exit
		return;

	/* Release circular buffer */
	DBG("releaseBuffer(circularBuffer, inputs);");
	mchaRecordPlayer->releaseBuffer(circularBuffer, inputs);

	/* Release sigfft	*/
		DBG("fftwf_free(sigfft);");
	fftwf_free(sigfft);

	/* Release IR FFTs */
		DBG("releaseBuffer( irfft, totalFilterCount*irBlocksNum );");
	mchaRecordPlayer->releaseBuffer( irfft, totalFilterCount*irBlocksNum );

	/* Release overlap-and-save buffer */
	DBG("releaseBuffer( resBuffer,  outputs*(irBlocksNum + 1));");
	mchaRecordPlayer->releaseBuffer( resBuffer,  outputs*(irBlocksNum + 1) );

	/* Release tempIrData */
		DBG("fftwf_free(tempData);");
	fftwf_free(tempData);
		DBG("fftwf_free(tempDataFloat);");
	fftwf_free(tempDataFloat);

	/* release temporary IR data */
	if (irData != nullptr)
	{	
		fftwf_free(irData); 
	}

	/* the plans are released automatically by ScopedPointer*/

	/* clear channels mapping array */
	channelsMap.clear();
}



//==============================================================================
void MchaFIRFilter::processBlock (float** sourceData, float** destinationData, int numSamples)
{
	
	/* Make the code is safe it buffer length exceeds K */
	curBufferLen = jmin(numSamples, K);
	curBufferLenBytes = curBufferLen*sizeof(float);
	if (filterSettings->getOneByOne()) // diagonal convolution matrix
	{
		for (int ch = 0; ch< inputs; ++ch)
		{
			/* swap buffer halves (do circular shift) */
			memcpy(circularBuffer[ch], circularBuffer[ch]+K, KBytes);

			/* copy data to circular buffers and do Fourier Transform */
			memcpy(circularBuffer[ch]+K, sourceData[ch], curBufferLenBytes);

			fftwf_execute_dft_r2c( signalPlan->getPlan(), circularBuffer[ch], sigfft );	

			for (int i = 0; i < irBlocksNum; ++i)
			{
				/* multiply signal and IR FFTs */
				mulBuffer(sigfft, irfft[ch*irBlocksNum + i], tempData, L);
			
				/* add to the overlap-and-add buffer */
				writeIndex= resIndex + i;
				writeIndex = writeIndex % resBufferLen;
				
				tmpc = resBufferPtr( ch, writeIndex );
				addBuffer(tempData, tmpc, tmpc, L);
			}

			/* do inverse FFT and copy the last L-K = K samples to the output buffer.
				This will destoy resBuffer[ch*resBufferLen+resIndex] but it has to be cleared anyway */
			fftwf_execute_dft_c2r(invPlan->getPlan(), resBufferPtr( ch, resIndex ), tempDataFloat);

			/* copy samples to the output buffer */
			memcpy( destinationData[channelsMap[ch]], tempDataFloat + K, curBufferLenBytes );

			/* clear outdated data */
			zeromem( resBufferPtr( ch, resIndex ), cbBytes );

		}
		
		/* increment/wrap resIndex */
		resIndex++; 
		resIndex = resIndex % (resBufferLen);

	}
	else // full convolution matrix
	{
		for (int indCols = 0; indCols <  inputs; ++indCols)
		{
			/* swap buffer halves (do circular shift) */
			memcpy(circularBuffer[indCols], circularBuffer[indCols]+K, KBytes);

			/* copy data to circular buffers and do Fourier Transform */
			memcpy(circularBuffer[indCols]+K, sourceData[indCols], curBufferLenBytes);
	
			fftwf_execute_dft_r2c( signalPlan->getPlan(), circularBuffer[indCols], sigfft );

			/* loop through IR filters for channel indCols */
			for (int indRows =0; indRows < outputs; ++indRows)
			{
				for (int indBlock = 0; indBlock < irBlocksNum; ++indBlock) // First (irBlocksNum - 1) blocks of IRs
				{
				
					/* multiply signal and IR FFTs */
					mulBuffer(sigfft, irfft[(indCols*outputs + indRows)*irBlocksNum + indBlock], tempData, L);

					/* add to the overlap-and-add buffer */
					writeIndex= resIndex + indBlock;
					writeIndex = writeIndex % resBufferLen;
					
					tmpc = resBufferPtr(indRows, writeIndex);
					addBuffer(tempData, tmpc, tmpc, L);
				}
			}
		}


		for (int indRows = 0; indRows < outputs; ++indRows)
		{
				/* do inverse FFT and copy the last L-K = K samples to the output buffer */
				fftwf_execute_dft_c2r(invPlan->getPlan(), resBufferPtr(indRows, resIndex), tempDataFloat);

				/* copy samples to the output buffer */
				memcpy( destinationData[channelsMap[indRows]], tempDataFloat + K, curBufferLenBytes );

				/* clear outdated data */
				zeromem( resBufferPtr(indRows, resIndex), cbBytes );
		}

		/* increment/wrap resIndex */
		resIndex++; 
		resIndex = resIndex % (resBufferLen);

	}

}


//==============================================================================
bool	MchaFIRFilter::convertData()
{
	
	if (irData == NULL)
		return false;

	int		irFilterLength; // length of one filter in multichannel file

	/* check file format */

	if ( filterSettings->getOneByOne() )	/* diagonal convolution matrix */
	{
		/* check size and filter lengths */
		return (irNumber == inputs) && (irNumber == outputs) && ( irLength == filterSettings->getFilterLength() ) ;
	}
	else	/* full matrix */
	{
		if ( irNumber == inputs )
		{
			/* no conversion required */
			irFilterLength = (int) div( irLength, (int64) outputs ).quot; 
			return ( irFilterLength == filterSettings->getFilterLength() ) ;
		}
		else
		{
			/* the matrix has to be converted */			
			irFilterLength = (int) div( irLength, (int64) inputs ).quot; 			
			if (( irNumber == outputs )&& ( irFilterLength == filterSettings->getFilterLength() ))
			{
				/* allocate memory for tmpData */
				float* tmpData(nullptr);
				try
				{
					tmpData = fftwf_alloc_real( (size_t)(irNumber*irLength) );
				}
				catch (std::bad_alloc)
				{
				   mchaRecordPlayer->logError( "mchaFilter::readWAVFile failed. Unable to allocate memory for irData." );
				   return false;	
				}

				/* convert from 'outputs x inputs' to 'inputs x otputs' matrix */
				int indSrc, indDst;
				for (int i=0; i<inputs; i++)
				{	
					for (int j=0; j<outputs; j++)
					{
						indDst = (i*outputs + j)*irFilterLength;
						indSrc = (j*inputs + i)*irFilterLength;
						memcpy(irData + indSrc, tmpData + indDst, irFilterLength*sizeof(float));
					}
				}
				/* copy tmpData to irData */
				memcpy( tmpData, irData, (size_t) (irNumber*irLength*sizeof(float)) );

				/* update size variables */
				irNumber = inputs;
				irLength = outputs*irFilterLength;
				
				/* release memory */
				fftwf_free(tmpData);

				return true;
			}
			else
			{
				/* wrong size or format */
				return false;
			}
		}
	}

}

}


