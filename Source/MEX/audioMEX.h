/*  
	MCHA - Multichannel Audio Playback and Recording Library
    Copyright (C) 2011-2013  Roman Kosobrodov

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
#ifndef MCHA_AUDIOMEX_H
#define MCHA_AUDIOMEX_H

#if ( defined (LINUX) || defined (__linux__) )
	#include <dlfcn.h>
#endif

#if  (defined (_WIN32) || defined (_WIN64))
	#include <windows.h>
#endif

#include "mex.h"
#include <math.h> // for ceil
#include <sstream>

class Mcha
{
public:
	Mcha() 
		: lib_handle(NULL)
	{
		loadAndCheck("");
	};

	Mcha(const char* mchaPath)
		: lib_handle(NULL)
	{
		loadAndCheck( mchaPath);
	};

	~Mcha()
	{
		#if ( defined (LINUX) || defined (__linux__) )
			if (lib_handle)
				dlclose( lib_handle ); // release the handle
		#endif

		#if (defined (_WIN32) || defined (_WIN64))
			if (lib_handle)
				FreeLibrary( lib_handle ); // release the handle			
		#endif
	}

	const char* getErrorStr() const	{ return error.str().c_str(); };
	bool	noError() const				{ return error.str().empty(); };

	// function identification 
	enum FunctionID   { d0d0 = 0x00, d0d1 = 0x01, d1d0 = 0x04, d1d1 = 0x05,
						d0m0 = 0x02, d0m1 = 0x03, d1m0 = 0x06, d1m1 = 0x07,
						m0d0 = 0x08, m0d1 = 0x09, m1d0 = 0x0C, m1d1 = 0x0D,
						m0m0 = 0x0A, m0m1 = 0x0B, m1m0 = 0x0E, m1m1 = 0x0F,
						errorId = 0xFF };

	// public function pointers
	typedef 	bool (*fptrInitAudioDevice)();
	fptrInitAudioDevice		initAudioDevice;

	typedef 	bool (*fptrInitAudioDeviceFile)(const char* );
	fptrInitAudioDeviceFile	initAudioDeviceFile;

	typedef 	bool (*fptrSetDebugMode)(const char* );
	fptrSetDebugMode	setDebugMode;

	typedef 	const char* (*fptrGetLastError)();
	fptrGetLastError	getLastError;

	typedef 	const char* (*fptrGetVersion)();
	fptrGetVersion	getVersion;

	typedef 	bool (*fptrAddFilter)(const char* , bool );
	fptrAddFilter	addFilter;

	typedef 	bool (*fptrPlayFiles) (const char** , const int , const int *, const int );
	fptrPlayFiles	playFiles;

	typedef 	bool (*fptrPlayData_s) (const float** , const int , const size_t , const int *, const int );
	fptrPlayData_s	playData_s;

	typedef 	bool (*fptrPlayData_d) (const double** , const int , const size_t , const int *, const int );
	fptrPlayData_d	playData_d;

	typedef 	bool (*fptrRecordFiles) (const char* , const int , const float , const int *, const int );
	fptrRecordFiles	recordFiles;

	typedef 	bool (*fptrRecordData) (float** , const int , const size_t , const int *, const int );
	fptrRecordData	recordData;

	typedef 	bool (*fptrPlayRecordDataMM_s) ( float** , const int , const size_t , const int *, const int ,
								 const float** , const int , const size_t , const int* , const int );
	fptrPlayRecordDataMM_s	playRecordDataMM_s;

	typedef 	bool (*fptrPlayRecordDataMM_d) ( float** , const int , const size_t , const int *, const int ,
								 const double** , const int , const size_t , const int* , const int  );
	fptrPlayRecordDataMM_d	playRecordDataMM_d;

	typedef 	bool (*fptrPlayRecordDataMD_s) (   const char* , const int , const float , const int* , const int , 
									const float** , const int , const size_t , const int* , const int );
	fptrPlayRecordDataMD_s	playRecordDataMD_s;

	typedef 	bool (*fptrPlayRecordDataMD_d) (   const char* , const int , const float , const int* , const int , 
									const double** , const int , const size_t , const int* , const int );
	fptrPlayRecordDataMD_d	playRecordDataMD_d;

	typedef 	bool (*fptrPlayRecordDataDM) ( float** , const int , const size_t , const int *, const int ,
							const char**  , const int , const int* , const int );
	fptrPlayRecordDataDM	playRecordDataDM;

	typedef 	bool (*fptrPlayRecordDataDD) ( const char* , const int , const float , const int* , const int , 
							const char**  , const int , const int* , const int );
	fptrPlayRecordDataDD	playRecordDataDD;

	typedef 	int	(*fptrGetRecordedChannelsNum) ();
	fptrGetRecordedChannelsNum	getRecordedChannelsNum;

	typedef 	size_t	(*fptrGetRecordedSamplesNum) ();
	fptrGetRecordedSamplesNum	getRecordedSamplesNum;

	typedef  bool	(*fptrGetData_s) (float**	,  const int* , const int, size_t, size_t);
	fptrGetData_s	getData_s;

	typedef  bool	(*fptrGetData_d) (double**	,  const int* , const int , size_t , size_t );
	fptrGetData_d	getData_d;

	typedef  	bool (*fptrSetGain) (int *, int , float *);
	fptrSetGain	setGain;

	typedef  bool (*fptrStopAudio)();
	fptrStopAudio	stopAudio;

	typedef  bool (*fptrIsRunning)();
	fptrIsRunning	isRunning;

	typedef  double (*fptrGetCurrentPosition)();
	fptrGetCurrentPosition	getCurrentPosition;

	typedef  bool (*fptrSetPosition) (double );
	fptrSetPosition	setPosition;

	typedef  bool (*fptrGetDeviceSettings)(double& , int& , int& , int& , int& );
	fptrGetDeviceSettings	getDeviceSettings;

	typedef  void (*fptrGetFilterSettings)(bool , int& , int& );
	fptrGetFilterSettings	getFilterSettings;

	typedef  void	(*fptrLogError)(const char* );
	fptrLogError	logError;

	// helper mex functions
	// ---------------------------------------------------------------------------------	
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

	// ---------------------------------------------------------------------------------	
	bool prepareMemoryPlay( const mxArray *mem, bool &isSingle, void** &memData, 
							size_t &samplesCount, size_t &channelCount)
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

	// ---------------------------------------------------------------------------------	
	void releaseMemoryBuffer( void** memData)
	{
		if (memData == NULL)
			return;
	
		delete [] memData;
		memData = NULL;
	}

	// ---------------------------------------------------------------------------------	
	float getDuration(const mxArray* duration)
	{
		double* pData = mxGetPr(duration);
		return (float)*pData;
	}

	// ---------------------------------------------------------------------------------	
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

	// ---------------------------------------------------------------------------------	
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

	// ---------------------------------------------------------------------------------	
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
				std::stringstream buffer;
				buffer << "Wrong number of filter inputs or outputs: filterInputs = " << filterInputs;
				buffer << "; filterOutputs = " << filterOutputs;
				mexErrMsgTxt( buffer.str().c_str() );
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

	// ---------------------------------------------------------------------------------	
	bool playDisk(const mxArray *mxFiles, const mxArray *mxChannels, const mxArray* mxFilterSettings)
	{

		char**  audioFiles = NULL;
		size_t	filesNumber(0);
		int*	channels = NULL;
		int		outChannelsCount(0);
		bool	res(false);
		bool	isInputFilter(false);
		int		filterOutputs(-1);
	
		/* Prepare playback channels */
		prepareChannels( mxChannels, channels, outChannelsCount );

		/* Prepare playback file list */
		prepareFileList( mxFiles, audioFiles, filesNumber );

		/* Prepare filters */
		res = prepareFilter( mxFilterSettings, isInputFilter, filterOutputs);
		if (!res)
		{
			delete [] audioFiles;
			delete [] channels;		
			return false;
		}

		/* call PlayRecord function */
		res = playFiles (	(const char**) audioFiles, (const int) filesNumber, (const int*) channels, 
							(const int) outChannelsCount);

		delete [] audioFiles;
		delete [] channels;
	
		return res;
	}


	// ---------------------------------------------------------------------------------	
	bool playMemory(const mxArray *mxData,  const mxArray *mxChannels, const mxArray* mxFilterSettings)
	{

		void**		outputData = NULL;
		size_t		outputChanNumber = 0; 
		size_t		outputSamplesNumber = 0;
		int*		outChannels = NULL;
		int			outChannelsCount = 0;
		bool		res = false;
		bool		isInputFilter = false;
		int			filterOutputs = -1;
		bool		isDataSingle = false;


		/* Check input channels */
		if (!mxIsNumeric (mxChannels))
		{
			mexErrMsgTxt("Input channels should be an array of numeric values.");
			return false;
		}

		/* Prepare playback channels */
		prepareChannels( mxChannels, outChannels, outChannelsCount);
	
		/* Prepare playback memory variables */

		res = prepareMemoryPlay( mxData, isDataSingle, outputData, outputSamplesNumber, outputChanNumber );
		if (!res)
		{	
			delete [] outChannels;			
			mexErrMsgTxt("Input data should be in single-precision floating-point format. Use  single  for conversion.");
			return false;
		}
	
		/* Prepare filters */
		res = prepareFilter( mxFilterSettings, isInputFilter, filterOutputs);
		if (!res)
		{
			delete [] outputData;
			delete [] outChannels;	
			return false;
		}

		/* call PlayRecord function */
		if (isDataSingle)
		{
			res = playData_s ((const float**) outputData, (const int) outputChanNumber, 
							(const size_t) outputSamplesNumber, (const int*) outChannels, (const int) outChannelsCount);
		}
		else
		{
			res = playData_d ((const double**) outputData, (const int) outputChanNumber, 
							(const size_t) outputSamplesNumber, (const int*) outChannels, (const int) outChannelsCount);	
		}


		delete [] outputData;
		delete [] outChannels;

		return res;
	}

	// ---------------------------------------------------------------------------------	
	bool recordMemory( const mxArray *inputChannels,  const mxArray *recDuration,  const mxArray *filterSettings )
	{
	
		float**		inputData = NULL;
		int			inputChanNumber = 0; 
		size_t		inputSamplesNumber = 0;
		int*		inChannels = NULL;
		int			inChannelsCount = 0;
		mxArray*	memDestination = NULL;
		bool		res;
		bool		isInputFilter = true;
		int			filterOutputs = -1;

		/* Check input duration */
		if (!mxIsNumeric (recDuration))
		{
			logError("MEX record error: Record duration should be a positive number.");
			return false;
		}

		/* Check input channels */
		if (!mxIsNumeric (inputChannels))
		{
			logError("MEX record error: Input channels should be an array of integer values.");
			return false;
		}

		/* Get audio device parameters */
		double		sampleRate = -1;
		int			bufSize = -1;
		int			bitDepth = -1;
		int			inChan = -1;
		int			outChan = -1;

		if ( !getDeviceSettings(sampleRate, bufSize, bitDepth, inChan, outChan) )
			return false;
	
		/* get duration and calculate the number of samples */
		float	duration = getDuration(recDuration);
	
		if (duration <= 0)
		{	
			logError("MEX record error: Record duration should be a positive number.");
			return false;
		}
		else
			inputSamplesNumber = static_cast <const size_t> ( ceil( duration*sampleRate ) );

		/* Prepare recording channels */
		prepareChannels( inputChannels, inChannels, inChannelsCount );
		inputChanNumber = inChannelsCount;

		/* Prepare filters */
		res = prepareFilter( filterSettings, isInputFilter, filterOutputs);
		if (!res)
		{
			delete [] inChannels;
			return false;
		}
	
		if (filterOutputs != -1)
			inputChanNumber = filterOutputs;

		/* Call record the function */
		res = recordData ( (float **)NULL, (const int) inputChanNumber, inputSamplesNumber, inChannels, inChannelsCount);
	
		/* release memory */
		delete [] inChannels;

		return res;
	}


	// ---------------------------------------------------------------------------------	
	bool recordDisk( const mxArray *recFolder, const mxArray *inputChannels, const mxArray *recDuration,  
					 const mxArray *filterSettings )
	{
		char*		recordDir = NULL;
		int			inputChanNumber = 0;
		int*		inChannels = NULL;
		int			inChannelsCount = 0; 
		float		duration = 0;
		bool		res;
		bool		isInputFilter = true;
		int			filterOutputs = -1;


		/* Check output folder */
		if (!mxIsChar(recFolder))
		{
			logError("MEX record error: The first argument should be a string containing output directory.");
			return false; 			
		}

		/* Check input duration */
		if (!mxIsNumeric (recDuration))
		{
			logError("MEX record error: Record duration should be a positive number.");
			return false; 
		}

		/* Check input channels */
		if (!mxIsNumeric (inputChannels))
		{
			logError("MEX record error: Input channels should be an array of integer values.");
			return false; 
		}

		/* get duration */
		duration = getDuration(recDuration);

		/* Prepare recording channels */
		prepareChannels( inputChannels, inChannels, inChannelsCount );
		inputChanNumber = inChannelsCount;

		/* Prepare filters */
		res = prepareFilter( filterSettings, isInputFilter, filterOutputs);
		if (!res)
		{
			delete [] inChannels;
			return false;
		}
	
		if (filterOutputs != -1)
			inputChanNumber = filterOutputs;

		/* Create char string with the disk folder */
		recordDir = mxArrayToString(recFolder);

		/* Call the function */
		res = recordFiles(  (const char*) recordDir, (const int) inputChanNumber, (const float)duration, 
							(const int*) inChannels, (const int) inChannelsCount );
	
		/* release memory */
		mxFree(recordDir);
		delete [] inChannels;

		return res;
	}


	// ---------------------------------------------------------------------------------	
	FunctionID getFunctionId(int nlhs,	mxArray *plhs[], int nrhs, const mxArray *prhs[])
	{
		if (( nrhs < 4 )||(nlhs > 1))
		{
			return errorId;		
		}

		if (  mxIsNumeric (prhs[0]) )  /* recording to internal memory */
		{
			switch (nrhs)
			{
			/* no filters */
			case 4: if ( mxIsNumeric(prhs[2]) )	/* playback from memory */
					{
						return m0m0;
					}
					else if ( mxIsCell(prhs[2]) ) /* playback from disk */
					{
						return m0d0;
					}
					else
					{
						return errorId;
					}
					break;
			/* one filter */
			case 5: if (  mxIsChar(prhs[2]) && mxIsNumeric(prhs[3]) )	/* playback from memory */
					{
						return m1m0;				
					}
					else if (  mxIsChar(prhs[2]) && mxIsCell(prhs[3]) )  /* playback from disk */
					{
						return m1d0;					
					}				
					else if (  mxIsChar(prhs[4]) && mxIsNumeric(prhs[3]) )  /* playback from memory */
					{
						return m0m1;					
					}				
					else if (  mxIsChar(prhs[4]) && mxIsCell(prhs[3]) )  /* playback from disk */
					{
						return m0d1;					
					}
					else
					{
						return errorId;			
					}
					break;
			/* two filters */
			case 6: if (  mxIsChar(prhs[2]) &&  mxIsChar(prhs[5]) && mxIsNumeric(prhs[3]) )  /* playback from memory */
					{
						return m1m1;				
					}
					else if (  mxIsChar(prhs[2])&&  mxIsChar(prhs[5]) && mxIsCell(prhs[3]) )  /* playback from disk */
					{
						return m1d1;					
					}
					else
					{
						return errorId;			
					}
					break;
			default:
				return errorId;
			}
		}
		else if ( mxIsChar(prhs[0]) ) /* recording to disk */
		{
			switch (nrhs)
			{
			/* no filters */
			case 5: if ( mxIsNumeric(prhs[3]) )	/* playback from memory */
					{
						return d0m0;
					}
					else if ( mxIsCell(prhs[3]) ) /* playback from disk */
					{
						return d0d0;
					}
					else
					{
						return errorId;
					}
					break;
			/* one filter */
			case 6: if (  mxIsChar(prhs[3]) && mxIsNumeric(prhs[4]) )	/* playback from memory */
					{
						return d1m0;				
					}
					else if (  mxIsChar(prhs[3]) && mxIsCell(prhs[4]) )  /* playback from disk */
					{
						return d1d0;					
					}
					else if (  mxIsChar(prhs[5]) && mxIsNumeric(prhs[4]) )  /* playback from memory */
					{
						return d0m1;					
					}				
					else if (  mxIsChar(prhs[5]) && mxIsCell(prhs[4]) )  /* playback from disk */
					{
						return d0d1;					
					}				
					else
					{
						return errorId;			
					}
					break;
			/* two filters */
			case 7: if (  mxIsChar(prhs[3]) &&  mxIsChar(prhs[6]) && mxIsNumeric(prhs[4]) )  /* playback from memory */
					{
						return d1m1;				
					}
					else if (  mxIsChar(prhs[3])&&  mxIsChar(prhs[6]) && mxIsCell(prhs[4]) )  /* playback from disk */
					{
						return d1d1;					
					}
					else
					{
						return errorId;			
					}
					break;
			default:
				return errorId;
			}
		}
		else
		{
			return errorId;		
		}
	}


	// ---------------------------------------------------------------------------------	
	bool callPlayRecord(FunctionID fid, const mxArray** params)
	{
		/* return on error */
		if (fid == errorId)
			return false;	
	
		/* initialise variables */
		char**	audioFiles = NULL;
		size_t	filesCount(0);
	
		bool	isSingle(false);
		void**	memPlay = NULL;
		size_t	memSamplesCount(0);
		size_t	memChanCount(0);

		int*	playChans = NULL;
		int		playChanCount(0);

		char*	recDir = NULL;
		int*	recChans = NULL;
		int		recChanCount(0);
		float	recDuration(0.0);

		int		inputChanNumber(0);
		size_t	inputSamplesNumber(0);

		int		outputChanNumber(0);
		size_t	outputSamplesNumber(0);

		bool	res(false);

		/* get function type */		
		bool	isMemRec = ((fid & 0x08) > 0 );
		bool	isRecFilter  = ((fid & 0x04) > 0 );
		bool	isMemPlay = ((fid & 0x02) > 0);	
		bool	isPlayFilter = ((fid & 0x01) > 0);

		unsigned int parNumber(0);
	
		/* ---- Prepare parameters ---- */

		if (!isMemRec) /* saving to disk. Get the first parameter: folder name */
		{
			size_t recDirLen =  mxGetM(params[parNumber])*mxGetN(params[parNumber]); /* get the length of path string */

			recDir = mxArrayToString( params[parNumber++] );
			if (recDir == NULL)
			{
				mexErrMsgTxt( "Unable to convert recording directory to string." );
				return false;
			}
		}
	
		prepareChannels( params[parNumber++], recChans, recChanCount);
	
		/* record duration (in seconds for files and in samples for memory recording) */
		recDuration = getDuration( params[parNumber++] );
		if (isMemRec)
		{
			double samplingRate;
			int bufSize, bDepth, inChannels, outChannels;
			res = getDeviceSettings(samplingRate, bufSize, bDepth, inChannels, outChannels);
			if (res)
				inputSamplesNumber = static_cast <size_t> (ceil(recDuration*samplingRate));
			else
				return false;
		}

		/* initialise recording filter */
		if (isRecFilter)
		{
			res = prepareFilter(params[parNumber++], true, inputChanNumber);
			if (!res)
				return false;
		}
		else
		{
			inputChanNumber = recChanCount;
		}
		
		if (isMemPlay)
		{	
			res = prepareMemoryPlay(params[parNumber++], isSingle, memPlay, memSamplesCount, memChanCount);
			if (!res)
				return false;		
		}
		else
		{
			res = prepareFileList( params[parNumber++], audioFiles, filesCount);
			if (!res)
				return false;	
		}
	
		prepareChannels( params[parNumber++], playChans, playChanCount);

		/* initialise playback filter */
		if (isPlayFilter)
		{
			res = prepareFilter(params[parNumber], false, outputChanNumber);
			if (!res)
				return false;		
		}
		else
		{
			outputChanNumber = playChanCount;
		}

		/* ---- Call the playback/record function ---- */
		if (isMemRec) /* recording into (internal) memory */
		{
			float** dummyDst = NULL;
			if (isMemPlay)
			{
			
				if (isSingle) /* single precision playback */
				{
					res = playRecordDataMM_s( dummyDst, inputChanNumber, inputSamplesNumber, 
											  const_cast <const int*> (recChans), recChanCount,
											  (const float**) memPlay, memChanCount, memSamplesCount, 
											  const_cast <const int*> (playChans), playChanCount );
				}
				else /* double precision playback */
				{
					res = playRecordDataMM_d( dummyDst, inputChanNumber, inputSamplesNumber, 
											  const_cast <const int*> (recChans), recChanCount,
											  (const double**) memPlay, memChanCount, memSamplesCount, 
											  const_cast <const int*> (playChans), playChanCount );
				}
			}
			else
			{
				res = playRecordDataDM( dummyDst, inputChanNumber, inputSamplesNumber, 
										const_cast <const int*> (recChans), recChanCount,
										const_cast <const char**> (audioFiles), filesCount, 
										const_cast <const int*> (playChans), playChanCount);
			}
		}
		else /* recording to the disk */
		{
			if (isMemPlay)
			{
				if (isSingle) /* single precision playback */
				{
					res = playRecordDataMD_s( const_cast<const char*> (recDir), inputChanNumber, recDuration, 
											  const_cast <const int*> (recChans), recChanCount,
											  (const float**) memPlay, memChanCount, memSamplesCount, 
											  const_cast <const int*> (playChans), playChanCount );
				}
				else /* double precision playback */
				{
					res = playRecordDataMD_d(  const_cast<const char*> (recDir), inputChanNumber, recDuration, 
												const_cast <const int*> (recChans), recChanCount, 
											  (const double**) memPlay, memChanCount, memSamplesCount, 
												const_cast <const int*> (playChans), playChanCount );
				}
			}
			else /* playing from the disk */
			{
				res = playRecordDataDD( const_cast<const char*> (recDir), inputChanNumber, recDuration, 
										const_cast <const int*> (recChans), recChanCount, 
										const_cast <const char**> (audioFiles), filesCount, 
										const_cast <const int*> (playChans), playChanCount );
			}
		}

		/* release resources */
		delete [] recChans;
		delete [] playChans;	
		
		if (isMemPlay)
		{
			releaseMemoryBuffer(memPlay);
		}
		else
		{
			releaseFileList(audioFiles, filesCount);	
		}
	
		if (!isMemRec)
		{
			mxFree(recDir);
		}
	
		return res;
	}

private:
	void *lib_handle; // equivalent to HINSTANCE/HANDLE/PVOID
	std::stringstream error;

	void loadAndCheck( const char* libpath)
	{
		std::string libPathString(libpath);

		if ( libPathString.empty() )
		{
			#if ( defined (LINUX) || defined (__linux__) )
				libPathString = "libmcha.so";
			#endif

			#if ( defined (_WIN32) )
				libPathString = "mcha-Win32.dll";		
			#endif

			#if ( defined (_WIN64))
				libPathString = "mcha-x64.dll";		
			#endif
		}
			
		// loading the library dynamically 
		#if ( defined (LINUX) || defined (__linux__) )
			lib_handle = dlopen( libPathString.c_str(), RTLD_LAZY );
			if (!lib_handle) 
			{
				error << dlerror();
			}
			else // resolve functions
			{
				resolveAll();
			}
		#endif

		#if ( defined (_WIN32) || defined (_WIN64) )
			lib_handle = LoadLibrary( TEXT( libPathString.c_str() ) );
			if (!lib_handle) 
			{
				error << GetLastError();
			}
			else // resolve functions
			{
				resolveAll();
			}
		#endif
	}

	template <typename T> bool resolveFunction(T* f, const char* fname)
	{
		if ( NULL == lib_handle )	return false;

		#if ( defined (LINUX) || defined (__linux__) )
			*(void **)(f) =  dlsym( lib_handle, fname );
			error << dlerror();
		#endif

		#if ( defined (_WIN32) || defined (_WIN64) )
			*(void **)(f) =  GetProcAddress( lib_handle, fname );
			error << GetLastError();
		#endif

		if ( error.str().empty() )  
			return true;
		else
			return false;	
	}

	void resolveAll()
	{
		if ( !resolveFunction( &initAudioDevice, "initAudioDevice" ) )  return;
		if ( !resolveFunction( &initAudioDeviceFile, "initAudioDeviceFile" ) )  return;
		if ( !resolveFunction( &setDebugMode, "setDebugMode" ) )  return;
		if ( !resolveFunction( &getLastError, "getLastError" ) )  return;

		if ( !resolveFunction( &getVersion, "getVersion" ) )  return;
		if ( !resolveFunction( &addFilter, "addFilter" ) )  return;
		if ( !resolveFunction( &playFiles, "playFiles" ) )  return;
		if ( !resolveFunction( &playData_s, "playData_s" ) )  return;

		if ( !resolveFunction( &playData_d, "playData_d" ) )  return;
		if ( !resolveFunction( &recordFiles, "recordFiles" ) )  return;
		if ( !resolveFunction( &recordData, "recordData" ) )  return;
		if ( !resolveFunction( &playRecordDataMM_s, "playRecordDataMM_s" ) )  return;

		if ( !resolveFunction( &playRecordDataMM_d, "playRecordDataMM_d" ) )  return;
		if ( !resolveFunction( &playRecordDataMD_s, "playRecordDataMD_s" ) )  return;
		if ( !resolveFunction( &playRecordDataMD_d, "playRecordDataMD_d" ) )  return;
		if ( !resolveFunction( &playRecordDataDM, "playRecordDataDM" ) )  return;

		if ( !resolveFunction( &playRecordDataDD, "playRecordDataDD" ) )  return;
		if ( !resolveFunction( &getRecordedChannelsNum, "getRecordedChannelsNum" ) )  return;
		if ( !resolveFunction( &getRecordedSamplesNum, "getRecordedSamplesNum" ) )  return;
		if ( !resolveFunction( &getData_s, "getData_s" ) )  return;

		if ( !resolveFunction( &getData_d, "getData_d" ) )  return;
		if ( !resolveFunction( &setGain, "setGain" ) )  return;
		if ( !resolveFunction( &stopAudio, "stopAudio" ) )  return;
		if ( !resolveFunction( &isRunning, "isRunning" ) )  return;

		if ( !resolveFunction( &getCurrentPosition, "getCurrentPosition" ) )  return;
		if ( !resolveFunction( &setPosition, "setPosition" ) )  return;
		if ( !resolveFunction( &getDeviceSettings, "getDeviceSettings" ) )  return;
		if ( !resolveFunction( &getFilterSettings, "getFilterSettings" ) )  return;
		if ( !resolveFunction( &logError, "logError" ) )  return;

	};

};

#endif


#if (defined (_WIN32) || defined (_WIN64))
	#define MCHAIMPORT extern "C" __declspec(dllimport)
#endif




