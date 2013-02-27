# Automatically generated makefile, created by the Introjucer
# Don't edit this file! Your changes will be overwritten when you re-save the Introjucer project!

ifndef CONFIG
  CONFIG=Debug-i386
endif

ifeq ($(TARGET_ARCH),)
  TARGET_ARCH := -march=native
endif

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifeq ($(CONFIG),Debug-i386)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Debug-i386
  OUTDIR := ../../Bin/MCHA-Debug-Linux-i386
  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I ../../Source -I ../../Source/Filter
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0
  CXXFLAGS += $(CFLAGS) -std=c++0x -fPIC
  LDFLAGS += -L$(BINDIR) -L$(LIBDIR) -L/usr/X11R6/lib/ -lX11 -lXext -lXinerama -lasound -ldl -lfreetype -lpthread -lrt -lfftw3f -lm
  LDDEPS :=
  RESFLAGS :=  -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I ../../Source -I ../../Source/Filter
  TARGET := libmcha.so.1
  BLDCMD = $(CXX) -shared -Wl -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
endif

ifeq ($(CONFIG),Release-i386)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Release-i386
  OUTDIR := ../../Bin/MCHA-Release-Linux-i386
  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "NDEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I ../../Source -I ../../Source/Filter
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -Os
  CXXFLAGS += $(CFLAGS) -std=c++0x -fPIC
  LDFLAGS += -L$(BINDIR) -L$(LIBDIR) -L/usr/X11R6/lib/ -lX11 -lXext -lXinerama -lasound -ldl -lfreetype -lpthread -lrt -lfftw3f -lm
  LDDEPS :=
  RESFLAGS :=  -D "LINUX=1" -D "NDEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode -I ../../Source -I ../../Source/Filter
  TARGET := libmcha.so.1
  BLDCMD = $(CXX) -shared -Wl -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
endif

OBJECTS := \
  $(OBJDIR)/mchaFilter_3be1f2bf.o \
  $(OBJDIR)/mchaFIRFilter_fc5ebdca.o \
  $(OBJDIR)/mchaIIRClass_df801d0d.o \
  $(OBJDIR)/mchaIIRFilter_c7e7d14d.o \
  $(OBJDIR)/AudioDeviceSettings_42b510f0.o \
  $(OBJDIR)/AudioFileConverter_408849d7.o \
  $(OBJDIR)/AudioSampleProcessor_31720edb.o \
  $(OBJDIR)/DataReader_af8c9416.o \
  $(OBJDIR)/DataWriter_89e287c6.o \
  $(OBJDIR)/DiskSamplePlayer_51aca5f1.o \
  $(OBJDIR)/DiskSampleRecorder_748bb28e.o \
  $(OBJDIR)/mcha_f471cdf8.o \
  $(OBJDIR)/mchaRecordPlayer_5da65f2a.o \
  $(OBJDIR)/mchaWavAudioFormat_e6219939.o \
  $(OBJDIR)/MemorySamplePlayer_72dd6855.o \
  $(OBJDIR)/MemorySampleRecorder_c956bf2.o \
  $(OBJDIR)/juce_audio_basics_2442e4ea.o \
  $(OBJDIR)/juce_audio_devices_a4c8a728.o \
  $(OBJDIR)/juce_audio_formats_d349f0c8.o \
  $(OBJDIR)/juce_audio_processors_44a134a2.o \
  $(OBJDIR)/juce_audio_utils_f63b12e8.o \
  $(OBJDIR)/juce_core_aff681cc.o \
  $(OBJDIR)/juce_data_structures_bdd6d488.o \
  $(OBJDIR)/juce_events_79b2840.o \
  $(OBJDIR)/juce_graphics_c8f1e7a4.o \
  $(OBJDIR)/juce_gui_basics_a630dd20.o \
  $(OBJDIR)/juce_gui_extra_7767d6a8.o \

.PHONY: clean

$(OUTDIR)/$(TARGET): $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking mcha
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@$(BLDCMD)
	-@ln -s -f $(OUTDIR)/$(TARGET) /lib/$(TARGET)

clean:
	@echo Cleaning mcha
	-@rm -f $(OUTDIR)/$(TARGET)
	-@rm -rf $(OBJDIR)/*
	-@rm -rf $(OBJDIR)

strip:
	@echo Stripping mcha
	-@strip --strip-unneeded $(OUTDIR)/$(TARGET)

$(OBJDIR)/mchaFilter_3be1f2bf.o: ../../Source/Filter/mchaFilter.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling mchaFilter.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/mchaFIRFilter_fc5ebdca.o: ../../Source/Filter/mchaFIRFilter.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling mchaFIRFilter.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/mchaIIRClass_df801d0d.o: ../../Source/Filter/mchaIIRClass.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling mchaIIRClass.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/mchaIIRFilter_c7e7d14d.o: ../../Source/Filter/mchaIIRFilter.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling mchaIIRFilter.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/AudioDeviceSettings_42b510f0.o: ../../Source/AudioDeviceSettings.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling AudioDeviceSettings.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/AudioFileConverter_408849d7.o: ../../Source/AudioFileConverter.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling AudioFileConverter.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/AudioSampleProcessor_31720edb.o: ../../Source/AudioSampleProcessor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling AudioSampleProcessor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/DataReader_af8c9416.o: ../../Source/DataReader.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling DataReader.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/DataWriter_89e287c6.o: ../../Source/DataWriter.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling DataWriter.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/DiskSamplePlayer_51aca5f1.o: ../../Source/DiskSamplePlayer.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling DiskSamplePlayer.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/DiskSampleRecorder_748bb28e.o: ../../Source/DiskSampleRecorder.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling DiskSampleRecorder.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/mcha_f471cdf8.o: ../../Source/mcha.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling mcha.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/mchaRecordPlayer_5da65f2a.o: ../../Source/mchaRecordPlayer.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling mchaRecordPlayer.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/mchaWavAudioFormat_e6219939.o: ../../Source/mchaWavAudioFormat.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling mchaWavAudioFormat.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/MemorySamplePlayer_72dd6855.o: ../../Source/MemorySamplePlayer.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling MemorySamplePlayer.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/MemorySampleRecorder_c956bf2.o: ../../Source/MemorySampleRecorder.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling MemorySampleRecorder.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_basics_2442e4ea.o: ../../JuceLibraryCode/modules/juce_audio_basics/juce_audio_basics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_basics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_devices_a4c8a728.o: ../../JuceLibraryCode/modules/juce_audio_devices/juce_audio_devices.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_devices.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_formats_d349f0c8.o: ../../JuceLibraryCode/modules/juce_audio_formats/juce_audio_formats.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_formats.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_processors_44a134a2.o: ../../JuceLibraryCode/modules/juce_audio_processors/juce_audio_processors.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_processors.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_utils_f63b12e8.o: ../../JuceLibraryCode/modules/juce_audio_utils/juce_audio_utils.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_utils.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_core_aff681cc.o: ../../JuceLibraryCode/modules/juce_core/juce_core.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_core.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_data_structures_bdd6d488.o: ../../JuceLibraryCode/modules/juce_data_structures/juce_data_structures.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_data_structures.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_events_79b2840.o: ../../JuceLibraryCode/modules/juce_events/juce_events.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_events.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_graphics_c8f1e7a4.o: ../../JuceLibraryCode/modules/juce_graphics/juce_graphics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_graphics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_gui_basics_a630dd20.o: ../../JuceLibraryCode/modules/juce_gui_basics/juce_gui_basics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_gui_basics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_gui_extra_7767d6a8.o: ../../JuceLibraryCode/modules/juce_gui_extra/juce_gui_extra.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_gui_extra.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
