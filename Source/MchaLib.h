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
#ifndef MCHA_LIBRARY_LOADER_H
#define MCHA_LIBRARY_LOADER_H

#include <string>

#if ( defined (LINUX) || defined (__linux__) )
	#include <dlfcn.h>
#endif

#if  (defined (_WIN32) || defined (_WIN64))
	#include <Windows.h>
#endif


class MchaLib
{
public:
	MchaLib () 
		: lib_handle(NULL)
	{
		loadAndCheck("");
	};

	MchaLib (const char* mchaPath)
		: lib_handle(NULL)
	{
		loadAndCheck (mchaPath);
	};

	~MchaLib ()
	{
		#if ( defined (LINUX) || defined (__linux__) )
			if (lib_handle)
				dlclose( lib_handle ); // release the handle
		#endif

		#if (defined (_WIN32) || defined (_WIN64))
			if (lib_handle)
				FreeLibrary( (HMODULE) lib_handle ); // release the handle			
		#endif
	}

	const char* getErrorStr() const		{ return error.c_str(); };
	bool	noError() const				{ return error.empty(); };

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



private:
	void *lib_handle; // equivalent to HINSTANCE/HANDLE/PVOID
	std::string error;

	void loadAndCheck( const char* libpath)
	{
		std::string libPathString(libpath);

		// define platform-dependent default name		
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
				error.assign( dlerror() );
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
				error.assign( "Unable to load MCHA library" );
			}
			else // resolve functions
			{
				resolveAll();
			}
		#endif

	}

	template <typename T> bool resolveFunction(T* f, const char* fname)
	{
		if ( NULL == lib_handle )
		{	
			error.assign("MCHA Library is not loaded");			
			return false;
		}

		#if ( defined (LINUX) || defined (__linux__) )
			*(void **)(f) =  dlsym( lib_handle, fname );
			
			if ( !f )
			{
				error = std::string( dlerror() );
				return false;
			}
			else
				return true;  

		#endif

		#if ( defined (_WIN32) || defined (_WIN64) )
			*(void **)(f) =  GetProcAddress( (HMODULE)lib_handle, fname ) ;		
			if ( *f == NULL )
			{
				error.assign("Unable to resolve function ");
				error += fname;
				return false;			
			}
			else
			{
				return true;
			}

		#endif
				
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

	}

};

#endif





