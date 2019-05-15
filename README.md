# Synthesizer

C++ Synthesizer 

 * 2 Oscillators with tone variation
 * 4 waveforms - Sine, Triangle, Saw and Square (with variable pulse width)
 * ADSR envelope for volume level
 * full polyphony
 * 4 pole filter with envelope adjustable cutoff and resonance
 * MIDI support and keyboard support
 * built with platform abstraction in mind

## How to build

#### The Synthesizer Project
Synthesizer uses a project generation tool called [GENie](https://github.com/bkaradzic/GENie) to generate project files. Currently only project generation for Visual Studio 2017 has been tested, but support for Linux makefiles should soon be added.
The project comes with packaged lib, dll and include files for all dependancies, compiled with vs2017. gtkmm was obtained through vcpkg. If you want to use a different visual studio version, use Vcpkg to compile gtkmm for that version.
Note you might have to [set the toolset for vcpkg manually](https://github.com/microsoft/vcpkg/blob/master/docs/users/triplets.md#VCPKG_VISUAL_STUDIO_PATH).

In order to build the project, first download the GENie executable. The easiest thing for later use would be installing it in your PATH, but you can also use the executable anywhere else.
Next, open a terminal and navigate to the repository root folder, and execute:

    genie --file=build/genie.lua vs2017

If you didn't install GENie in your path it would look more like

    .\relative\path\to\your\genie.exe --file=build/genie.lua [preferred visual studio version]

Next open the solution that was generated at **build/Synthesizer.sln** in visual studio, select the desired configuration (_Development_ is good for most purposes), and build as usual.

The final executable will appear in **bin/[configuration]_[platform]/bin/Synthesizer.exe**

Adjacent to the internal bin folder there will be share and lib folders where the gtk theme and icons live in


## Third Party

This project uses RtMIDI, RtAudio, GTKmm and the "Sweet" theme for GTK, here are the licenses for them:

### RtMidi and RtAudio

RtMidi is licensed with the [MIT license](http://www.music.mcgill.ca/~gary/rtmidi/index.html#license)

RtAudio is licensed with the [MIT license](http://www.music.mcgill.ca/~gary/rtaudio/license.html)

### GTK

GTKmm is licensed under [GNU LGPL](https://www.gtkmm.org/en/license.html) 

Sweet is licensed under [GNU General Public License v3.0](https://github.com/EliverLara/Sweet/blob/master/LICENSE)