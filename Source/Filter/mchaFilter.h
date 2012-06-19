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

#ifndef MCHA_FILTERBASE
#define  MCHA_FILTERBASE

#include "fftw3.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "../mchaRecordPlayer.h"
#include "mchaIIRClass.h"


namespace mcha
{

const float denormalsTreshold = 1.1754943E-38f; //The smallest nonzero denormalized value for type float

class	MchaRecordPlayer;	// forward declaration

//==============================================================================
class FilterSettings
{
public:
	enum FilterType {NONE, FIR, IIR};
	
	FilterSettings();
	FilterSettings(const char* settingsFilename);
	FilterSettings (const FilterSettings& fSettings); // Copy constructor

	~FilterSettings();
	
	const FilterSettings& operator= (const FilterSettings& fSettings); // Deep copy of FilterSettings
	
	bool readSettings(const char* settingsFilename);
	bool saveSettings(const char* settingsFileName);

	void setFilterName(String newName) {filterName = newName;}
	void setFilterName(const char* newName) {filterName = String(newName);} // overloaded version
	void setInputs(int inputNum) {inputs = inputNum;}
	void setOutputs(int outputNum) {outputs = outputNum;}
	void setFilterLength(int filterLen) {filterLength = filterLen;}
	void setSamplingFrequency(double freq) {samplingFrequency = freq;}
	void setOneByOne(bool isOneByOne) {oneByOne = isOneByOne;}
	void setFileName(const char* fileStr) {fileName = String(fileStr);}

	const char* getFilterName()	{ return filterName.toUTF8(); }
	int		getInputs()		{ return inputs; }
	int		getOutputs()	{ return outputs; }
	int		getFilterLength()	{ return (int)filterLength; }
	double	getSamplingFrequency()	{ return samplingFrequency; }
	bool	getOneByOne()	{ return oneByOne; }
	const char* getFileName()		{ return fileName.toUTF8(); }
	const FilterType getFilterType() { return typeOfFilter; }
	int		getCascades()	{ return cascades; }

	juce_UseDebuggingNewOperator
private:
	int		cascades;
	int		inputs;
	int		outputs;
	int64	filterLength;
	double	samplingFrequency;
	bool	oneByOne;
	String	fileName;
	String	filterName;
	FilterType typeOfFilter;

	MchaRecordPlayer* mchaRecordPlayer;
};

//==============================================================================
// Helper class to implement RAII on FFTW transform plans
class TransformPlan
{
public:
	
	// Real-data DFT plan
	TransformPlan( int n0, float *in, fftwf_complex *out, unsigned flags)
	{
		plan = fftwf_plan_dft_r2c_1d(n0, in, out, flags);
	};

	// Complex to real iDFT plan
	TransformPlan( int n0, fftwf_complex *in, float *out, unsigned flags)
	{
		plan = fftwf_plan_dft_c2r_1d(n0, in, out, flags);
	}
	
	~TransformPlan() { fftwf_destroy_plan(plan); } ;

	const fftwf_plan getPlan() { return plan; };

private:
	fftwf_plan plan;
};

//==============================================================================
class MchaFilter
{
public:
	MchaFilter();
	virtual ~MchaFilter();

	/* filter initialisation */
	virtual void prepareToPlay (double sampleRate, int samplesPerBlock) = 0;
	/* clean up code */
	virtual void releaseResources() = 0;
	/* signal processing */
	virtual void processBlock (float** sourceData, float** destinationData, int numSamples) = 0;

	/* get filter name from filterSettings */
	const String getName() const 
	{
		if ( filterSettings != NULL )
			return String(filterSettings->getFilterName());
		else
			return String::empty;
	};
	
	/* applies settings provided by filterSettings */
	void applySettings(FilterSettings* fSettings)
	{
		if (filterSettings != NULL)
			*filterSettings = *fSettings; // Deep-copy the object
		else
			filterSettings = new FilterSettings(*fSettings);
	}
	
	virtual Array<int>*	getChannelsMap() { return &channelsMap; }
	virtual void resetChannelsMap(int channelCount)
	{
		channelsMap.clear();
		for (int ch=0; ch<channelCount; ch++)
			channelsMap.add(ch);
	}
	virtual void setChannelsMap(int* chMap, int channelCount) 
	{
		channelsMap.clear();
		channelsMap.insertArray(0, chMap, channelCount);
	}
	virtual void setChannelsMap(Array<int>* srcArray)
	{
		channelsMap.clear();
		channelsMap.addArray(*srcArray, 0, -1);
	}

	int  getInputsNumber();
	int  getOutputsNumber();	

	juce_UseDebuggingNewOperator

protected:
	MchaRecordPlayer* mchaRecordPlayer;
	FilterSettings*		filterSettings;
	
	int		inputs;
	int		outputs;

	int		filterCount; // the total number of filters
	bool	oneByOneFilters; // true if a diagonal matrix of filters is provided
	
	Array<int>	channelsMap;

private:
	MchaFilter (const MchaFilter&);
	const MchaFilter& operator= (const MchaFilter&);
};


//==============================================================================
class MchaIIRFilter : public MchaFilter 
{
public:
	MchaIIRFilter();
	virtual ~MchaIIRFilter();

	/* filter initialisation */
	void prepareToPlay (double sampleRate, int samplesPerBlock);
	/* clean up code */
	void releaseResources() ;
	/* signal processing */
	void processBlock (float** sourceData, float** destinationData, int numSamples);

	juce_UseDebuggingNewOperator

private:	
	OwnedArray<MchaIIRClass>	iirFilter; // array of pointers to IIR filter classes
	float*						tempData; // temporary data buffer
	MchaRecordPlayer*			mchaRecordPlayer;

	int		cascades;	// the number of cascades in cascaded filters
	int		cascade1filterCount; // the total number of filters in the first cascade 

	/* Read one line from CSV file and convert it to array of Strings */
	bool readLine(FileInputStream* inStream, StringArray& valueStrings);
	
	/* Reads data from CSV file and stores it in the irData array */
	bool readCSVFile(String csvFilename);

	/* Sum of two  arrays of float*/
	void	addBuffer(float* srcdst, float* src, int n)
	{
		for (int i=0; i<n; ++i)
		{
			srcdst[i] += src[i];
		}
	};


	//==============================================================================
	MchaIIRFilter (const MchaIIRFilter&);
	const MchaIIRFilter& operator= (const MchaIIRFilter&);
};

//==============================================================================
class MchaFIRFilter : public MchaFilter 
{
public:
    
    MchaFIRFilter();
    virtual ~MchaFIRFilter();

	/* filter initialisation */
	void prepareToPlay (double sampleRate, int samplesPerBlock);
	/* clean up code */
	void releaseResources();
	/* signal processing */
	void processBlock (float** sourceData, float** destinationData, int numSamples);

	juce_UseDebuggingNewOperator

private:	
	int		K;		// the length of audio buffer in samples
	int		KBytes;	// size of K samples in bytes
	int		Kfft;	// FFT data length >= K
	int		L;		// the length of fft buffer (L=2*Kfft)
	int		cbBytes; // size of cblen in bytes

	bool		swap;			// indicates if circular shift of the buffer is required
	float**		circularBuffer;	// circular buffer to store current and previous audio buffers
	float*		irData;			// temporary storage for IR data

	ScopedPointer<TransformPlan>	irplan;	// fftw plan for IRs
	fftwf_complex*		tempData;		// temporary buffer for IRs and FFTs 
	fftwf_complex*		sigfft;			// temporary buffer to store signal FFTs 	
	fftwf_complex**		resBuffer;		// overlap-and-save buffer
	float*				tempDataFloat;  // temporary float data buffer
	fftwf_complex*		tmpc;			// temporary pointer to fftwf_complex
	
	int			resIndex;		// current index of resBuffer ready for output
	int			writeIndex;		// temporary index
	int			resBufferLen;	// the length of the resBuffer equal to irBlocksNum+1

	ScopedPointer<TransformPlan>	signalPlan;	// FFT specifications
	ScopedPointer<TransformPlan>	invPlan; // inverse FFT plan

	int						irBlocksNum; // The number of IR blocks in partitioned convolution
	fftwf_complex**			irfft;		// Array of pointers to ir block FFTs
	int						irNumber;	// Number of data columns in IR file
	int64					irLength;	// Total length of data in one column
	int						totalFilterCount; // Total number of filters (either irNumber or inputs*outputs)

	int		curBufferLen;			// the length of the audio buffer in processBlock method (could be different from K);
	int		curBufferLenBytes;		// size of curBufferLen in bytes

	/* Reads data from CSV file and stores it in the irData array. 
	   It is the caller's responsibility to free this array */
	bool readCSVFile(String csvFilename, int &irNumber, int64 &irLength);

	/* Reads data from (multichannel) WAV file and stores it in the irData array.  
	   It is the caller's responsibility to free this array */
	bool readWAVFile(String audioFilename, int &irNumber, int64 &irLength);

	/* Returns true if the audio file has one of the supported formats */
	bool isAudioFile(String audioFilename);

	/* Converts data from rows to columns format */
	bool convertData();

	/* Product of two complex arrays */
	void	mulBuffer(fftwf_complex* src1, fftwf_complex* src2, fftwf_complex* dst, int n)
	{
		// ( a, b) * ( c, d) = ( ac - bd, ad + bc) 
		for (int i=0; i<n; ++i)
		{
			dst[i][0] = src1[i][0]*src2[i][0] - src1[i][1]*src2[i][1];
			dst[i][1] = src1[i][0]*src2[i][1] + src1[i][1]*src2[i][0];
		}
	}
	
	/* Product of a complex array and a float number */
	void	mulBuffer(fftwf_complex* src1, float src2, fftwf_complex* dst, int n)
	{
		for (int i=0; i<n; ++i)
		{
			dst[i][0] = src1[i][0]*src2;
			dst[i][1] = src1[i][1]*src2;
		}	
	}

	/* Sum of two complex arrays */
	void	addBuffer(fftwf_complex* src1, fftwf_complex* src2, fftwf_complex* dst, int n)
	{
		for (int i=0; i<n; ++i)
		{
			dst[i][0] = src1[i][0] + src2[i][0];
			dst[i][1] = src1[i][1] + src2[i][1];
		}
	};

	fftwf_complex*	resBufferPtr(int chanNumber, int bufferNumber)
	{
		return resBuffer[ chanNumber*resBufferLen + bufferNumber];
	}

	//==============================================================================
	MchaFIRFilter (const MchaFIRFilter&);
	const MchaFIRFilter& operator= (const MchaFIRFilter&);
};

}

#endif 
