/*  
	MCHA - Multichannel Audio Playback and Recording Library

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


/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-9 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#include "mchaWavAudioFormat.h"

//==============================================================================
#define wavFormatName                          TRANS("WAV file")
static const char* const wavExtensions[] =    { ".wav", ".bwf", 0 };


//==============================================================================
const char* const MchaWavAudioFormat::bwavDescription      = "bwav description";
const char* const MchaWavAudioFormat::bwavOriginator       = "bwav originator";
const char* const MchaWavAudioFormat::bwavOriginatorRef    = "bwav originator ref";
const char* const MchaWavAudioFormat::bwavOriginationDate  = "bwav origination date";
const char* const MchaWavAudioFormat::bwavOriginationTime  = "bwav origination time";
const char* const MchaWavAudioFormat::bwavTimeReference    = "bwav time reference";
const char* const MchaWavAudioFormat::bwavCodingHistory    = "bwav coding history";

const StringPairArray MchaWavAudioFormat::createBWAVMetadata (const String& description,
                                                          const String& originator,
                                                          const String& originatorRef,
                                                          const Time& date,
                                                          const int64 timeReferenceSamples,
                                                          const String& codingHistory)
{
    StringPairArray m;

    m.set (bwavDescription, description);
    m.set (bwavOriginator, originator);
    m.set (bwavOriginatorRef, originatorRef);
    m.set (bwavOriginationDate, date.formatted ("%Y-%m-%d"));
    m.set (bwavOriginationTime, date.formatted ("%H:%M:%S"));
    m.set (bwavTimeReference, String (timeReferenceSamples));
    m.set (bwavCodingHistory, codingHistory);

    return m;
}


//==============================================================================
#if JUCE_MSVC
  #pragma pack (push, 1)
  #define PACKED
#elif defined (JUCE_GCC)
  #define PACKED __attribute__((packed))
#else
  #define PACKED
#endif

struct BWAVChunk
{
    char description [256];
    char originator [32];
    char originatorRef [32];
    char originationDate [10];
    char originationTime [8];
    uint32 timeRefLow;
    uint32 timeRefHigh;
    uint16 version;
    uint8 umid[64];
    uint8 reserved[190];
    char codingHistory[1];

    void copyTo (StringPairArray& values) const
    {
        values.set (MchaWavAudioFormat::bwavDescription, String::fromUTF8 (description, 256));
        values.set (MchaWavAudioFormat::bwavOriginator, String::fromUTF8 (originator, 32));
        values.set (MchaWavAudioFormat::bwavOriginatorRef, String::fromUTF8 (originatorRef, 32));
        values.set (MchaWavAudioFormat::bwavOriginationDate, String::fromUTF8 (originationDate, 10));
        values.set (MchaWavAudioFormat::bwavOriginationTime, String::fromUTF8 (originationTime, 8));

        const uint32 timeLow = ByteOrder::swapIfBigEndian (timeRefLow);
        const uint32 timeHigh = ByteOrder::swapIfBigEndian (timeRefHigh);
        const int64 time = (((int64)timeHigh) << 32) + timeLow;

        values.set (MchaWavAudioFormat::bwavTimeReference, String (time));
        values.set (MchaWavAudioFormat::bwavCodingHistory, String::fromUTF8 (codingHistory));
    }

    static MemoryBlock createFrom (const StringPairArray& values)
    {
        const size_t sizeNeeded = sizeof (BWAVChunk) + values [WavAudioFormat::bwavCodingHistory].getNumBytesAsUTF8();
        MemoryBlock data ((sizeNeeded + 3) & ~3);
        data.fillWith (0);

        BWAVChunk* b = (BWAVChunk*) data.getData();

        // Allow these calls to overwrite an extra byte at the end, which is fine as long
        // as they get called in the right order..
        values [MchaWavAudioFormat::bwavDescription].copyToUTF8 (b->description, 257);
        values [MchaWavAudioFormat::bwavOriginator].copyToUTF8 (b->originator, 33);
        values [MchaWavAudioFormat::bwavOriginatorRef].copyToUTF8 (b->originatorRef, 33);
        values [MchaWavAudioFormat::bwavOriginationDate].copyToUTF8 (b->originationDate, 11);
        values [MchaWavAudioFormat::bwavOriginationTime].copyToUTF8 (b->originationTime, 9);

        const int64 time = values [MchaWavAudioFormat::bwavTimeReference].getLargeIntValue();
        b->timeRefLow = ByteOrder::swapIfBigEndian ((uint32) (time & 0xffffffff));
        b->timeRefHigh = ByteOrder::swapIfBigEndian ((uint32) (time >> 32));

        values [MchaWavAudioFormat::bwavCodingHistory].copyToUTF8 (b->codingHistory, 0x7fffffff);

        if (b->description[0] != 0
            || b->originator[0] != 0
            || b->originationDate[0] != 0
            || b->originationTime[0] != 0
            || b->codingHistory[0] != 0
            || time != 0)
        {
            return data;
        }

        return MemoryBlock();
    }

} PACKED;


//==============================================================================
struct SMPLChunk
{
    struct SampleLoop
    {
        uint32 identifier;
        uint32 type;
        uint32 start;
        uint32 end;
        uint32 fraction;
        uint32 playCount;
    } PACKED;

    uint32 manufacturer;
    uint32 product;
    uint32 samplePeriod;
    uint32 midiUnityNote;
    uint32 midiPitchFraction;
    uint32 smpteFormat;
    uint32 smpteOffset;
    uint32 numSampleLoops;
    uint32 samplerData;
    SampleLoop loops[1];

    void copyTo (StringPairArray& values, const int totalSize) const
    {
        values.set ("Manufacturer",      String (ByteOrder::swapIfBigEndian (manufacturer)));
        values.set ("Product",           String (ByteOrder::swapIfBigEndian (product)));
        values.set ("SamplePeriod",      String (ByteOrder::swapIfBigEndian (samplePeriod)));
        values.set ("MidiUnityNote",     String (ByteOrder::swapIfBigEndian (midiUnityNote)));
        values.set ("MidiPitchFraction", String (ByteOrder::swapIfBigEndian (midiPitchFraction)));
        values.set ("SmpteFormat",       String (ByteOrder::swapIfBigEndian (smpteFormat)));
        values.set ("SmpteOffset",       String (ByteOrder::swapIfBigEndian (smpteOffset)));
        values.set ("NumSampleLoops",    String (ByteOrder::swapIfBigEndian (numSampleLoops)));
        values.set ("SamplerData",       String (ByteOrder::swapIfBigEndian (samplerData)));

        for (uint32 i = 0; i < numSampleLoops; ++i)
        {
            if ((uint8*) (loops + (i + 1)) > ((uint8*) this) + totalSize)
                break;

            const String prefix ("Loop" + String(i));
            values.set (prefix + "Identifier", String (ByteOrder::swapIfBigEndian (loops[i].identifier)));
            values.set (prefix + "Type",       String (ByteOrder::swapIfBigEndian (loops[i].type)));
            values.set (prefix + "Start",      String (ByteOrder::swapIfBigEndian (loops[i].start)));
            values.set (prefix + "End",        String (ByteOrder::swapIfBigEndian (loops[i].end)));
            values.set (prefix + "Fraction",   String (ByteOrder::swapIfBigEndian (loops[i].fraction)));
            values.set (prefix + "PlayCount",  String (ByteOrder::swapIfBigEndian (loops[i].playCount)));
        }
    }

    static MemoryBlock createFrom (const StringPairArray& values)
    {
        const int numLoops = jmin (64, values.getValue ("NumSampleLoops", "0").getIntValue());

        if (numLoops <= 0)
            return MemoryBlock();

        const size_t sizeNeeded = sizeof (SMPLChunk) + (numLoops - 1) * sizeof (SampleLoop);
        MemoryBlock data ((sizeNeeded + 3) & ~3);
        data.fillWith (0);

        SMPLChunk* s = (SMPLChunk*) data.getData();

        // Allow these calls to overwrite an extra byte at the end, which is fine as long
        // as they get called in the right order..
        s->manufacturer      = ByteOrder::swapIfBigEndian ((uint32) values.getValue ("Manufacturer", "0").getIntValue());
        s->product           = ByteOrder::swapIfBigEndian ((uint32) values.getValue ("Product", "0").getIntValue());
        s->samplePeriod      = ByteOrder::swapIfBigEndian ((uint32) values.getValue ("SamplePeriod", "0").getIntValue());
        s->midiUnityNote     = ByteOrder::swapIfBigEndian ((uint32) values.getValue ("MidiUnityNote", "60").getIntValue());
        s->midiPitchFraction = ByteOrder::swapIfBigEndian ((uint32) values.getValue ("MidiPitchFraction", "0").getIntValue());
        s->smpteFormat       = ByteOrder::swapIfBigEndian ((uint32) values.getValue ("SmpteFormat", "0").getIntValue());
        s->smpteOffset       = ByteOrder::swapIfBigEndian ((uint32) values.getValue ("SmpteOffset", "0").getIntValue());
        s->numSampleLoops    = ByteOrder::swapIfBigEndian ((uint32) numLoops);
        s->samplerData       = ByteOrder::swapIfBigEndian ((uint32) values.getValue ("SamplerData", "0").getIntValue());

        for (int i = 0; i < numLoops; ++i)
        {
            const String prefix ("Loop" + String(i));
            s->loops[i].identifier = ByteOrder::swapIfBigEndian ((uint32) values.getValue (prefix + "Identifier", "0").getIntValue());
            s->loops[i].type       = ByteOrder::swapIfBigEndian ((uint32) values.getValue (prefix + "Type", "0").getIntValue());
            s->loops[i].start      = ByteOrder::swapIfBigEndian ((uint32) values.getValue (prefix + "Start", "0").getIntValue());
            s->loops[i].end        = ByteOrder::swapIfBigEndian ((uint32) values.getValue (prefix + "End", "0").getIntValue());
            s->loops[i].fraction   = ByteOrder::swapIfBigEndian ((uint32) values.getValue (prefix + "Fraction", "0").getIntValue());
            s->loops[i].playCount  = ByteOrder::swapIfBigEndian ((uint32) values.getValue (prefix + "PlayCount", "0").getIntValue());
        }

        return data;
    }
} PACKED;


struct ExtensibleWavSubFormat
{
    uint32 data1;
    uint16 data2;
    uint16 data3;
    uint8  data4[8];
} PACKED;


#if JUCE_MSVC
  #pragma pack (pop)
#endif

#undef PACKED


//==============================================================================
class MchaWavAudioFormatReader  : public AudioFormatReader
{
    int bytesPerFrame;
    int64 dataChunkStart, dataLength;

    static inline int chunkName (const char* const name)   { return (int) ByteOrder::littleEndianInt (name); }

    MchaWavAudioFormatReader (const MchaWavAudioFormatReader&);
    MchaWavAudioFormatReader& operator= (const MchaWavAudioFormatReader&);

public:
    int64 bwavChunkStart, bwavSize;

    //==============================================================================
   MchaWavAudioFormatReader (InputStream* const in)
        : AudioFormatReader (in, TRANS (wavFormatName)),
          dataLength (0),
          bwavChunkStart (0),
          bwavSize (0)
    {
        if (input->readInt() == chunkName ("RIFF"))
        {
            const uint32 len = (uint32) input->readInt();
            const int64 end = input->getPosition() + len;
            bool hasGotType = false;
            bool hasGotData = false;

            if (input->readInt() == chunkName ("WAVE"))
            {
                while (input->getPosition() < end
                        && ! input->isExhausted())
                {
                    const int chunkType = input->readInt();
                    uint32 length = (uint32) input->readInt();
                    const int64 chunkEnd = input->getPosition() + length + (length & 1);

                    if (chunkType == chunkName ("fmt "))
                    {
                        // read the format chunk
                        const unsigned short format = input->readShort();
                        const short numChans = input->readShort();
                        sampleRate = input->readInt();
                        const int bytesPerSec = input->readInt();

                        numChannels = numChans;
                        bytesPerFrame = bytesPerSec / (int)sampleRate;
                        bitsPerSample = 8 * bytesPerFrame / numChans;

                        if (format == 3)
                        {
                            usesFloatingPointData = true;
                        }
                        else if (format == 0xfffe /*WAVE_FORMAT_EXTENSIBLE*/)
                        {
                            if (length < 40) // too short
                            {
                                bytesPerFrame = 0;
                            }
                            else
                            {
                                input->skipNextBytes (12); // skip over blockAlign, bitsPerSample and speakerPosition mask
                                ExtensibleWavSubFormat subFormat;
                                subFormat.data1 = input->readInt();
                                subFormat.data2 = input->readShort();
                                subFormat.data3 = input->readShort();
                                input->read (subFormat.data4, sizeof (subFormat.data4));

                                const ExtensibleWavSubFormat pcmFormat
                                    = { 0x00000001, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

                                if (memcmp (&subFormat, &pcmFormat, sizeof (subFormat)) != 0)
                                {
                                    const ExtensibleWavSubFormat ambisonicFormat
                                        = { 0x00000001, 0x0721, 0x11d3, { 0x86, 0x44, 0xC8, 0xC1, 0xCA, 0x00, 0x00, 0x00 } };

                                    if (memcmp (&subFormat, &ambisonicFormat, sizeof (subFormat)) != 0)
                                        bytesPerFrame = 0;
                                }
                            }
                        }
                        else if (format != 1)
                        {
                            bytesPerFrame = 0;
                        }

                        hasGotType = true;
                    }
                    else if (chunkType == chunkName ("data"))
                    {
                        // get the data chunk's position
                        dataLength = length;
                        dataChunkStart = input->getPosition();
                        lengthInSamples = (bytesPerFrame > 0) ? (dataLength / bytesPerFrame) : 0;

                        hasGotData = true;
                    }
                    else if (chunkType == chunkName ("bext"))
                    {
                        bwavChunkStart = input->getPosition();
                        bwavSize = length;

                        // Broadcast-wav extension chunk..
                        HeapBlock <BWAVChunk> bwav;
                        bwav.calloc (jmax ((size_t) length + 1, sizeof (BWAVChunk)), 1);
                        input->read (bwav, length);
                        bwav->copyTo (metadataValues);
                    }
                    else if (chunkType == chunkName ("smpl"))
                    {
                        HeapBlock <SMPLChunk> smpl;
                        smpl.calloc (jmax ((size_t) length + 1, sizeof (SMPLChunk)), 1);
                        input->read (smpl, length);
                        smpl->copyTo (metadataValues, length);
                    }
                    else if (chunkEnd <= input->getPosition())
                    {
                        break;
                    }

                    input->setPosition (chunkEnd);
                }
            }
        }
    }

    ~MchaWavAudioFormatReader()
    {
    }

    //==============================================================================
    bool readSamples (int** destSamples, int numDestChannels, int startOffsetInDestBuffer,
                      int64 startSampleInFile, int numSamples)
    {
        numSamples = (int) jmin ((int64) numSamples, lengthInSamples - startSampleInFile);

        if (numSamples <= 0)
            return true;

        input->setPosition (dataChunkStart + startSampleInFile * bytesPerFrame);

        const int tempBufSize = 480 * 3 * 4; // (keep this a multiple of 3)
        char tempBuffer [tempBufSize];
//		const int numThisTime = jmin (tempBufSize / bytesPerFrame, numSamples);
		
		/* allocate memory for sample buffers and initialise pointers */
		int** channelInd = new int* [numDestChannels];

		while (numSamples > 0)
        {
			for (int ind = 0; ind < numDestChannels; ind++)
			{	
				channelInd[ind] = destSamples[ind] + startOffsetInDestBuffer;
			}			
			
			const int numThisTime = jmin (tempBufSize / bytesPerFrame, numSamples);
            const int bytesRead = input->read (tempBuffer, numThisTime * bytesPerFrame);

            if (bytesRead < numThisTime * bytesPerFrame)
                zeromem (tempBuffer + bytesRead, numThisTime * bytesPerFrame - bytesRead);

            if (bitsPerSample == 16)
            {
                const short* src = (const short*) tempBuffer;

                for (int i = numThisTime; --i >= 0;)
				{
					for (unsigned int ch = 0; ch < numChannels; ch ++)
					{
						if (channelInd[ch] == 0) // don't want this channel
							src++; // skip sample;
						else
						{
                            *channelInd[ch]++ = (int) ByteOrder::swapIfBigEndian ((unsigned short) *src++) << 16;					
						}
					}
				}
			}
			
			else if (bitsPerSample == 24)
            {
                const char* src = (const char*) tempBuffer;

                for (int i = numThisTime; --i >= 0;)
				{
					for (unsigned int ch = 0; ch < numChannels; ch ++)
					{
						if (channelInd[ch] == 0) // don't want this channel
							src += 3; // skip sample (3 bytes);
						else
						{
                            *channelInd[ch]++ = ByteOrder::littleEndian24Bit (src) << 8;
							src += 3;					
						}
					}
				}
			}
				
            else if (bitsPerSample == 32)
            {
                const unsigned int* src = (const unsigned int*) tempBuffer;
                unsigned int** chan  = new unsigned int* [numChannels];
				for (unsigned int ind = 0; ind < numChannels; ind++)	
					chan[ind] = (unsigned int*) channelInd[ind];

				for (int i = numThisTime; --i >= 0;)
                {
					for (unsigned int ind = 0; ind < numChannels; ind++)
						if ( chan[ind] == 0 )
							++src;
						else
							*chan[ind]++ = ByteOrder::swapIfBigEndian (*src++);
                }
				
				for (unsigned int ind = 0; ind < numChannels; ind++)
				    channelInd[ind] = (int*)chan[ind];
				
				/* delete chan from memory */
				delete [] chan;
			}

            else if (bitsPerSample == 8)
            {
                const unsigned char* src = (const unsigned char*) tempBuffer;

                for (int i = numThisTime; --i >= 0;)
				{
					for (unsigned int ch = 0; ch < numChannels; ch ++)
					{
						if (channelInd[ch] == 0) // don't want this channel
							src++; // skip sample;
						else
                            *channelInd[ch]++ = ((int)*src++ - 128) << 24;					
					}
				}
			}
			
			startOffsetInDestBuffer += numThisTime;
            numSamples -= numThisTime;
		}		

        if (numSamples > 0)
        {
            for (int i = numDestChannels; --i >= 0;)
                if (destSamples[i] != 0)
                    zeromem (destSamples[i] + startOffsetInDestBuffer, sizeof (int) * numSamples);
        }

		
		delete [] channelInd;

		return true;
    }


    juce_UseDebuggingNewOperator
};

//==============================================================================
class MchaWavAudioFormatWriter  : public AudioFormatWriter
{
    MemoryBlock tempBlock, bwavChunk;
    uint32 lengthInSamples, bytesWritten;
    int64 headerPosition;
    bool writeFailed;

    static inline int chunkName (const char* const name)   { return (int) ByteOrder::littleEndianInt (name); }

    MchaWavAudioFormatWriter (const MchaWavAudioFormatWriter&);
    MchaWavAudioFormatWriter& operator= (const MchaWavAudioFormatWriter&);

    void writeHeader()
    {
        const bool seekedOk = output->setPosition (headerPosition);
        (void) seekedOk;

        // if this fails, you've given it an output stream that can't seek! It needs
        // to be able to seek back to write the header
        jassert (seekedOk);

        const int bytesPerFrame = numChannels * bitsPerSample / 8;
        output->writeInt (chunkName ("RIFF"));
        output->writeInt (lengthInSamples * bytesPerFrame
                            + ((bwavChunk.getSize() > 0) ? (44 + bwavChunk.getSize()) : 36));

        output->writeInt (chunkName ("WAVE"));
        output->writeInt (chunkName ("fmt "));
        output->writeInt (16);
        output->writeShort ((bitsPerSample < 32) ? (short) 1 /*WAVE_FORMAT_PCM*/
                                                 : (short) 3 /*WAVE_FORMAT_IEEE_FLOAT*/);
        output->writeShort ((short) numChannels);
        output->writeInt ((int) sampleRate);
        output->writeInt (bytesPerFrame * (int) sampleRate);
        output->writeShort ((short) bytesPerFrame);
        output->writeShort ((short) bitsPerSample);

        if (bwavChunk.getSize() > 0)
        {
            output->writeInt (chunkName ("bext"));
            output->writeInt (bwavChunk.getSize());
            output->write (bwavChunk.getData(), bwavChunk.getSize());
        }

        output->writeInt (chunkName ("data"));
        output->writeInt (lengthInSamples * bytesPerFrame);

        usesFloatingPointData = (bitsPerSample == 32);
    }

public:
    //==============================================================================
    MchaWavAudioFormatWriter (OutputStream* const out,
                          const double sampleRate,
                          const unsigned int numChannels_,
                          const int bits,
                          const StringPairArray& metadataValues)
        : AudioFormatWriter (out,
                             wavFormatName,
                             sampleRate,
                             numChannels_,
                             bits),
          lengthInSamples (0),
          bytesWritten (0),
          writeFailed (false)
    {
        if (metadataValues.size() > 0)
            bwavChunk = BWAVChunk::createFrom (metadataValues);

        headerPosition = out->getPosition();
        writeHeader();
    }

    ~MchaWavAudioFormatWriter()
    {
        writeHeader();
    }

    //==============================================================================
    bool write (const int** data, int numSamples)
    {
        if (writeFailed)
            return false;

        const int bytes = numChannels * numSamples * bitsPerSample / 8;
        tempBlock.ensureSize (bytes, false);
        char* buffer = (char*) tempBlock.getData();

        const int* left = data[0];
        const int* right = data[1];
        if (right == 0)
            right = left;

        if (bitsPerSample == 16)
        {
            short* b = (short*) buffer;

            if (numChannels > 1)
            {
                for (int i = numSamples; --i >= 0;)
                {
                    *b++ = (short) ByteOrder::swapIfBigEndian ((unsigned short) (*left++ >> 16));
                    *b++ = (short) ByteOrder::swapIfBigEndian ((unsigned short) (*right++ >> 16));
                }
            }
            else
            {
                for (int i = numSamples; --i >= 0;)
                {
                    *b++ = (short) ByteOrder::swapIfBigEndian ((unsigned short) (*left++ >> 16));
                }
            }
        }
        else if (bitsPerSample == 24)
        {
            char* b = (char*) buffer;

            if (numChannels > 1)
            {
                for (int i = numSamples; --i >= 0;)
                {
                    ByteOrder::littleEndian24BitToChars ((*left++) >> 8, b);
                    b += 3;
                    ByteOrder::littleEndian24BitToChars ((*right++) >> 8, b);
                    b += 3;
                }
            }
            else
            {
                for (int i = numSamples; --i >= 0;)
                {
                    ByteOrder::littleEndian24BitToChars ((*left++) >> 8, b);
                    b += 3;
                }
            }
        }
        else if (bitsPerSample == 32)
        {
            unsigned int* b = (unsigned int*) buffer;

            if (numChannels > 1)
            {
                for (int i = numSamples; --i >= 0;)
                {
                    *b++ = ByteOrder::swapIfBigEndian ((unsigned int) *left++);
                    *b++ = ByteOrder::swapIfBigEndian ((unsigned int) *right++);
                }
            }
            else
            {
                for (int i = numSamples; --i >= 0;)
                {
                    *b++ = ByteOrder::swapIfBigEndian ((unsigned int) *left++);
                }
            }
        }
        else if (bitsPerSample == 8)
        {
            unsigned char* b = (unsigned char*) buffer;

            if (numChannels > 1)
            {
                for (int i = numSamples; --i >= 0;)
                {
                    *b++ = (unsigned char) (128 + (*left++ >> 24));
                    *b++ = (unsigned char) (128 + (*right++ >> 24));
                }
            }
            else
            {
                for (int i = numSamples; --i >= 0;)
                {
                    *b++ = (unsigned char) (128 + (*left++ >> 24));
                }
            }
        }

        if (bytesWritten + bytes >= (uint32) 0xfff00000
             || ! output->write (buffer, bytes))
        {
            // failed to write to disk, so let's try writing the header.
            // If it's just run out of disk space, then if it does manage
            // to write the header, we'll still have a useable file..
            writeHeader();
            writeFailed = true;
            return false;
        }
        else
        {
            bytesWritten += bytes;
            lengthInSamples += numSamples;

            return true;
        }
    }

    juce_UseDebuggingNewOperator
};


//==============================================================================
MchaWavAudioFormat::MchaWavAudioFormat()
		: AudioFormat (TRANS (wavFormatName), StringArray (wavExtensions))
{
}

MchaWavAudioFormat::~MchaWavAudioFormat() 
{
}

Array <int> MchaWavAudioFormat::getPossibleSampleRates()
{
    const int rates[] = { 22050, 32000, 44100, 48000, 88200, 96000, 176400, 192000, 0 };
    return Array <int> (rates);
}

Array <int> MchaWavAudioFormat::getPossibleBitDepths()
{
    const int depths[] = { 8, 16, 24, 32, 0 };
    return Array <int> (depths);
}

bool MchaWavAudioFormat::canDoStereo()
{
    return true;
}

bool MchaWavAudioFormat::canDoMono()
{
    return true;
}

AudioFormatReader* MchaWavAudioFormat::createReaderFor (InputStream* sourceStream,
                                                    const bool deleteStreamIfOpeningFails)
{
    MchaWavAudioFormatReader* r = new MchaWavAudioFormatReader (sourceStream);

    if (r->sampleRate == 0)
    {
        if (! deleteStreamIfOpeningFails)
            r->input = 0;

        deleteAndZero (r);
    }

    return r;
}

AudioFormatWriter* MchaWavAudioFormat::createWriterFor (OutputStream* out,
                                                    double sampleRate,
                                                    unsigned int numChannels,
                                                    int bitsPerSample,
                                                    const StringPairArray& metadataValues,
                                                    int /*qualityOptionIndex*/)
{
    if (getPossibleBitDepths().contains (bitsPerSample))
    {
        return new MchaWavAudioFormatWriter (out,
                                         sampleRate,
                                         numChannels,
                                         bitsPerSample,
                                         metadataValues);
    }

    return 0;
}

static bool mcha_slowCopyOfWavFileWithNewMetadata (const File& file, const StringPairArray& metadata)
{
    bool ok = false;
   MchaWavAudioFormat wav;

    const File dest (file.getNonexistentSibling());

    OutputStream* outStream = dest.createOutputStream();

    if (outStream != 0)
    {
        AudioFormatReader* reader = wav.createReaderFor (file.createInputStream(), true);

        if (reader != 0)
        {
            AudioFormatWriter* writer = wav.createWriterFor (outStream, reader->sampleRate,
                                                             reader->numChannels, reader->bitsPerSample,
                                                             metadata, 0);

            if (writer != 0)
            {
                ok = writer->writeFromAudioReader (*reader, 0, -1);

                outStream = 0;
                delete writer;
            }

            delete reader;
        }

        delete outStream;
    }

    if (ok)
        ok = dest.moveFileTo (file);

    if (! ok)
        dest.deleteFile();

    return ok;
}

bool MchaWavAudioFormat::replaceMetadataInFile (const File& wavFile, const StringPairArray& newMetadata)
{
    MchaWavAudioFormatReader* reader = (MchaWavAudioFormatReader*) createReaderFor (wavFile.createInputStream(), true);

    if (reader != 0)
    {
        const int64 bwavPos = reader->bwavChunkStart;
        const int64 bwavSize = reader->bwavSize;
        delete reader;

        if (bwavSize > 0)
        {
            MemoryBlock chunk = BWAVChunk::createFrom (newMetadata);

            if (chunk.getSize() <= bwavSize)
            {
                // the new one will fit in the space available, so write it directly..
                const int64 oldSize = wavFile.getSize();

                FileOutputStream* out = wavFile.createOutputStream();
                out->setPosition (bwavPos);
                out->write (chunk.getData(), chunk.getSize());
                out->setPosition (oldSize);
                delete out;

                jassert (wavFile.getSize() == oldSize);

                return true;
            }
        }
    }

    return mcha_slowCopyOfWavFileWithNewMetadata (wavFile, newMetadata);
}


