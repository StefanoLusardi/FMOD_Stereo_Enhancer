# FMOD_Stereo_Enhancer
### Stefano Lusardi 2017
FMOD Stereo Enhancer Audio Plugin C++

Simple Stereo Enhancer audio plugin developed for the FMOD Studio platform by Firelight Technologies.  
The DSP involves a simple **M-S stereo widening** technique with a fully smoothed widening parameter.  
The plugin features a true **bypass** parameter that can be automated to prevent real time processing and save CPU (this is not natively supported by FMOD Studio).  
Compiled with *Visual Studio 2015*.  
Tested on *FMOD Studio version 1.08.14*.

## Build and install
1. First you need to download and install [FMOD Studio] (http://www.fmod.com/)
2. Open FMOD_Stereo_Enhancer.sln
3. Under Project -> FMOD_Stereo_Enhancer Properties... -> Build Events -> Post-Build Event -> Command Line -> 
  you can specify to copy the dll from the output folder to the *FMOD Studio* plugins folder. 
4. Under Project -> FMOD_Stereo_Enhancer Properties... -> Configuration Properties -> Debugging -> Command -> 
  you can specify the path to your FMOD Studio executable. This will launch *FMOD Studio.exe* after build and run (F5).
5. Build the entire solution.
6. Alternatively you can ignore points 3 and 4 and after the build manually copy the dll into the FMOD plugins folder, then you can open *FMOD Studio.exe*.
