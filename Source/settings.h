#ifndef MCHA_SETTINGS_H
#define MCHA_SETTINGS_H

// Version number
#define MCHA_VERSION_NUMBER	"0.0.3"

#define XML_SETTINGS_FILE	"defaultsettings.xml"

// The size of audio file header in bytes used for reserving disk space
#define MAX_FORMAT_HEADER	256	// ... should be enough ...

// Max size of a character string
#define MCHA_MAX_STRING_SIZE	512

// Circular buffer length
const int CIRCULAR_BUFFER_LENGTH	=	16;

// Device configuration constants
const int MIN_AUDIO_INPUT_CHANNELS		= 0;
const int MAX_AUDIO_INPUT_CHANNELS		= 64;
const int MIN_AUDIO_OUTPUT_CHANNELS		= 0;
const int MAX_AUDIO_OUTPUT_CHANNELS		= 64;
const bool SHOW_MIDI_INPUT_OPTIONS		= false;
const bool SHOW_MIDI_OUTPUT_SELECTOR	= false;
const bool SHOW_STEREO_PAIRS			= false;
const bool HIDE_ADVANCED_OPTIONS		= false;

#endif
