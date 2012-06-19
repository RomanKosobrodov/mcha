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

namespace mcha
{

//==============================================================================
FilterSettings::FilterSettings():
		inputs(0),
		outputs(0),
		filterLength(0),
		samplingFrequency(0),
		oneByOne(false),
		fileName(String::empty),		
		filterName(String::empty),
		typeOfFilter(NONE),
		cascades(0)
{
	mchaRecordPlayer = MchaRecordPlayer::getInstance();
}

//==============================================================================
FilterSettings::FilterSettings(const char* settingsFilename):
		inputs(0),
		outputs(0),
		filterLength(0),
		samplingFrequency(0),
		oneByOne(false),
		fileName(String::empty),		
		filterName(String::empty),
		typeOfFilter(NONE),
		cascades(0)
{
	mchaRecordPlayer = MchaRecordPlayer::getInstance();
	readSettings(settingsFilename);
}

//==============================================================================
FilterSettings::FilterSettings (const FilterSettings& fSettings): 
		inputs(fSettings.inputs),
		outputs(fSettings.outputs),
		filterLength(fSettings.filterLength),
		samplingFrequency(fSettings.samplingFrequency),
		oneByOne(fSettings.oneByOne),
		typeOfFilter(fSettings.typeOfFilter),
		cascades(fSettings.cascades)
{
	fileName = fSettings.fileName;
	filterName = fSettings.filterName;

	mchaRecordPlayer = MchaRecordPlayer::getInstance();
}

//==============================================================================
FilterSettings::~FilterSettings ()
{
}

//==============================================================================
const FilterSettings& FilterSettings::operator= (const FilterSettings& fSettings) // Deep copy of FilterSettings
{
	inputs = fSettings.inputs;
	outputs = fSettings.outputs;
	filterLength = fSettings.filterLength;
	samplingFrequency = fSettings.samplingFrequency;
	oneByOne = fSettings.oneByOne;
	typeOfFilter = fSettings.typeOfFilter;
	fileName = fSettings.fileName;
	filterName = fSettings.filterName;
	cascades = fSettings.cascades;
	return *this;
}

//==============================================================================
bool FilterSettings::readSettings(const char* settingsFilename)
{
	mchaRecordPlayer->clearError();
	mchaRecordPlayer->dbgOut( "FilterSettings::readSettings " );	

	String xmlFileName(settingsFilename);

	File fs(xmlFileName);
	mchaRecordPlayer->dbgOut("\tOpening settings file:\t" + xmlFileName);

	if ( !fs.existsAsFile() )
	{
		mchaRecordPlayer->logError( "\tFailed to open settings file:\t" + xmlFileName );
		return false;
	}
		
	XmlDocument myDocument (fs);
	XmlElement* xmlSettings = myDocument.getDocumentElement();

	if (xmlSettings == NULL)
	{
		mchaRecordPlayer->logError("\tFailed to load filter settings XML file\t"+ xmlFileName);
		return false;
	}
	else
	{
		String wrong = "\tWrong format of filter settings file. The following field is missing or invalid: ";
		
		/* filter type */
		String typeStr;
		typeStr = xmlSettings->getStringAttribute( "filterType", String::empty );
		typeStr = typeStr.toUpperCase();
		if (typeStr == "FIR")
			typeOfFilter = FIR;
		else if (typeStr == "IIR")
			typeOfFilter = IIR;
		else
		{	
			typeOfFilter = NONE;
			mchaRecordPlayer->logError( wrong + "filterType" );		
		}
		
		filterName = xmlSettings->getStringAttribute( "filterName", String::empty );
		
		inputs	= xmlSettings->getIntAttribute( "inputs", -1 );
		if (inputs == -1)
		{
			mchaRecordPlayer->logError( wrong + "inputs" );
		}

		outputs	= xmlSettings->getIntAttribute( "outputs", -1 );
		if (outputs == -1)
		{
			mchaRecordPlayer->logError( wrong + "outputs" );
		}			
		
		if (typeOfFilter == FIR)
		{
			filterLength = xmlSettings->getIntAttribute( "filterLength", -1 );
			if (filterLength == -1)
			{
				mchaRecordPlayer->logError( wrong + "filterLength" );
			}

			samplingFrequency = xmlSettings->getDoubleAttribute( "samplingFrequency", -1.0 );
			if (samplingFrequency == -1.0)
			{
				mchaRecordPlayer->logError( wrong + "samplingFrequency" );
			}
		}
		else if (typeOfFilter == IIR)
		{
			cascades = xmlSettings->getIntAttribute( "cascades", -1 );
			if (cascades == -1)
			{
				mchaRecordPlayer->logError( wrong + "cascades" );
			}
		
		}

		oneByOne = xmlSettings->getBoolAttribute( "oneByOne", false );

		fileName = xmlSettings->getStringAttribute( "fileName", String::empty );
		if (fileName.isEmpty())
		{
			mchaRecordPlayer->logError( wrong + "fileName" );
		}
		
		/* Check if the file exists */
		String fullPathStr = fs.getFullPathName(); // Full path to the settings file
		String fileStr = fs.getFileName(); // settings file without the path
		fullPathStr = fullPathStr.substring( 0, fullPathStr.length() - fileStr.length() ); // replaceCharacters( fileStr, String::empty ); // the path

		File dirFile(fullPathStr); // directory containing irFile
		File irf = dirFile.getChildFile(fileName); // full path to irFile
		if (!irf.existsAsFile())
		{
			mchaRecordPlayer->logError( "\tUnable to find data file: " + fileName );
		}

		/* Update the filename to include full path */
		fileName = irf.getFullPathName();

	}
	delete xmlSettings;

	if (mchaRecordPlayer->getLastErrorStr().isEmpty())
		return true;
	else
		return false;
}

//==============================================================================
bool FilterSettings::saveSettings(const char* settingsFilename)
{
	mchaRecordPlayer->clearError();
	mchaRecordPlayer->dbgOut( "FilterSettings::saveSettings " );	

	String xmlFileName(settingsFilename);

	File fs(xmlFileName);
	mchaRecordPlayer->dbgOut("\tSaving settings file:\t" + xmlFileName);

	if (fs == File::nonexistent)
	{
		mchaRecordPlayer->logError( "\tUnable to create settings file:\t" + xmlFileName );
		return false;	
	}
	
	XmlElement* xmlSettings = new XmlElement("FILTERSETTINGS");

	if (typeOfFilter == FIR)
		xmlSettings->setAttribute("filterType", "FIR");
	else if (typeOfFilter == IIR)
		xmlSettings->setAttribute("filterType", "IIR");
	else
	{
		mchaRecordPlayer->logError( "\tFilter type is not defined (NONE)" );
		delete xmlSettings;
		return false;	
	}

	xmlSettings->setAttribute("filterName", filterName);
	xmlSettings->setAttribute("inputs", inputs);
	xmlSettings->setAttribute("outputs", outputs);
	if (typeOfFilter == FIR)
	{
		xmlSettings->setAttribute("filterLength", (double)filterLength);
		xmlSettings->setAttribute("samplingFrequency", samplingFrequency);
	}
	else
	{
		xmlSettings->setAttribute("cascades", cascades);
	}
	xmlSettings->setAttribute("oneByOne", oneByOne);
	xmlSettings->setAttribute("fileName", fileName);

	xmlSettings->writeToFile(fs, String::empty);

	delete xmlSettings;

	return true;
}

//==============================================================================
MchaFilter::MchaFilter():
				filterSettings(NULL),
				filterCount(0),
				oneByOneFilters(false)
{
	mchaRecordPlayer = MchaRecordPlayer::getInstance();
}

//==============================================================================	
MchaFilter::~MchaFilter()
{
	if (filterSettings != NULL)
	{
		delete filterSettings;
		filterSettings = NULL;
	}	
}

//==============================================================================	
int  MchaFilter::getInputsNumber()
{
	if (filterSettings != NULL)
		return filterSettings->getInputs();
	else
		return 0;
}
//==============================================================================	
int  MchaFilter::getOutputsNumber()
{
	if (filterSettings != NULL)
		return filterSettings->getOutputs();
	else
		return 0;
}

}
