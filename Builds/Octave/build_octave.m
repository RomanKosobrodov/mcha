function build_octave

clc;

disp("m_playRecord.oct")
mkoctfile -L../../Bin/Win32/Debug -lmcha-Win32 -o ../../Bin/Octave/Win32/m_playRecord.oct  ../../Source/octave/playRecord.cc 

disp("m_play.oct")
mkoctfile -L../../Bin/Win32/Debug -lmcha-Win32 -o ../../Bin/Octave/Win32/m_play.oct  ../../Source/octave/play.cc 

disp("m_setGain.oct")
mkoctfile -L../../Bin/Win32/Debug -lmcha-Win32 -o ../../Bin/Octave/Win32/m_setGain.oct  ../../Source/octave/setGain.cc 

disp("m_getData.oct")
mkoctfile -L../../Bin/Win32/Debug -lmcha-Win32 -o ../../Bin/Octave/Win32/m_getData.oct ../../Source/octave/getData.cc

disp("m_record.oct")
mkoctfile -L../../Bin/Win32/Debug -lmcha-Win32 -o ../../Bin/Octave/Win32/m_record.oct  ../../Source/octave/record.cc 

disp("m_getVersion.oct")
mkoctfile -L../../Bin/Win32/Debug -lmcha-Win32 -o ../../Bin/Octave/Win32/m_getVersion.oct  ../../Source/octave/getVersion.cc 

disp("m_running.oct")
mkoctfile -L../../Bin/Win32/Debug -lmcha-Win32 -o ../../Bin/Octave/Win32/m_running.oct  ../../Source/octave/running.cc 

disp("m_stop.oct")
mkoctfile -L../../Bin/Win32/Debug -lmcha-Win32 -o ../../Bin/Octave/Win32/m_stop.oct   ../../Source/octave/stop.cc 

disp("m_getPosition.oct")
mkoctfile -L../../Bin/Win32/Debug -lmcha-Win32 -o ../../Bin/Octave/Win32/m_getPosition.oct  ../../Source/octave/getPosition.cc 

disp("m_setPosition.oct")
mkoctfile -L../../Bin/Win32/Debug -lmcha-Win32 -o ../../Bin/Octave/Win32/m_setPosition.oct  ../../Source/octave/setPosition.cc 

disp("m_getError.oct")
mkoctfile -L../../Bin/Win32/Debug -lmcha-Win32 -o ../../Bin/Octave/Win32/m_getError.oct  ../../Source/octave/getError.cc 

disp("m_init.oct")
mkoctfile -L../../Bin/Win32/Debug -lmcha-Win32 -o ../../Bin/Octave/Win32/m_init.oct  ../../Source/octave/init.cc 

copyfile( "../../Source/octave/m_wait.m",  "../../Bin/Octave/Win32/m_wait.m", "f");
copyfile( "../../Bin/Win32/Debug/mcha-Win32.dll",  "../../Bin/Octave/Win32/mcha-Win32.dll", "f");

endfunction