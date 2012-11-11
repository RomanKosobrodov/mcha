#ifndef MCHA_WAVAUDIOFORMAT
#define MCHA_WAVAUDIOFORMAT

#include "../JuceLibraryCode/JuceHeader.h"
//#include "..\..\..\juce\src\audio\audio_file_formats\juce_AudioFormat.h"


//==============================================================================
/**
    Reads and Writes WAV format audio files.

    @see AudioFormat
*/


class MchaWavAudioFormat  : public AudioFormat
{
public:
    //==============================================================================
    /** Creates a format object. */
	MchaWavAudioFormat();
	~MchaWavAudioFormat();

    //==============================================================================
    /** Metadata property name used by wav readers and writers for adding
        a BWAV chunk to the file.

        @see AudioFormatReader::metadataValues, createWriterFor
    */
    static const char* const bwavDescription;

    /** Metadata property name used by wav readers and writers for adding
        a BWAV chunk to the file.

        @see AudioFormatReader::metadataValues, createWriterFor
    */
    static const char* const bwavOriginator;

    /** Metadata property name used by wav readers and writers for adding
        a BWAV chunk to the file.

        @see AudioFormatReader::metadataValues, createWriterFor
    */
    static const char* const bwavOriginatorRef;

    /** Metadata property name used by wav readers and writers for adding
        a BWAV chunk to the file.

        Date format is: yyyy-mm-dd

        @see AudioFormatReader::metadataValues, createWriterFor
    */
    static const char* const bwavOriginationDate;

    /** Metadata property name used by wav readers and writers for adding
        a BWAV chunk to the file.

        Time format is: hh-mm-ss

        @see AudioFormatReader::metadataValues, createWriterFor
    */
    static const char* const bwavOriginationTime;

    /** Metadata property name used by wav readers and writers for adding
        a BWAV chunk to the file.

        This is the number of samples from the start of an edit that the
        file is supposed to begin at. Seems like an obvious mistake to
        only allow a file to occur in an edit once, but that's the way
        it is..

        @see AudioFormatReader::metadataValues, createWriterFor
    */
    static const char* const bwavTimeReference;

    /** Metadata property name used by wav readers and writers for adding
        a BWAV chunk to the file.

        This is a

        @see AudioFormatReader::metadataValues, createWriterFor
    */
    static const char* const bwavCodingHistory;

    /** Utility function to fill out the appropriate metadata for a BWAV file.

        This just makes it easier than using the property names directly, and it
        fills out the time and date in the right format.
    */
    static const StringPairArray createBWAVMetadata (const String& description,
                                                     const String& originator,
                                                     const String& originatorRef,
                                                     const Time& dateAndTime,
                                                     const int64 timeReferenceSamples,
                                                     const String& codingHistory);

    //==============================================================================
    Array <int> getPossibleSampleRates();
    Array <int> getPossibleBitDepths();
    bool canDoStereo();
    bool canDoMono();

    //==============================================================================
    AudioFormatReader* createReaderFor (InputStream* sourceStream,
                                        bool deleteStreamIfOpeningFails);

    AudioFormatWriter* createWriterFor (OutputStream* streamToWriteTo,
                                        double sampleRateToUse,
                                        unsigned int numberOfChannels,
                                        int bitsPerSample,
                                        const StringPairArray& metadataValues,
                                        int qualityOptionIndex);

    //==============================================================================
    /** Utility function to replace the metadata in a wav file with a new set of values.

        If possible, this cheats by overwriting just the metadata region of the file, rather
        than by copying the whole file again.
    */
    bool replaceMetadataInFile (const File& wavFile, const StringPairArray& newMetadata);

    //==============================================================================
    juce_UseDebuggingNewOperator
};


#endif   // MCHA_WAVAUDIOFORMAT_JUCEHEADER__
