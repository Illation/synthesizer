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

For visual studio 2017:

    ./dependancies/tools/genie/genie.exe --file=build/genie.lua vs2017

Then build the solution that appears in build/synthesizer.sln

In all other cases check [the build documentation](doc/building.md).

## Third Party

This project uses RtMIDI, RtAudio, RTTR, GTKmm and the "Sweet" theme for GTK, here are the licenses for them:

### RtMidi and RtAudio

RtMidi is licensed with the [MIT license](http://www.music.mcgill.ca/~gary/rtmidi/index.html#license)

RtAudio is licensed with the [MIT license](http://www.music.mcgill.ca/~gary/rtaudio/license.html)

### GTK

GTKmm is licensed under [GNU LGPL](https://www.gtkmm.org/en/license.html) 

Sweet is licensed under [GNU General Public License v3.0](https://github.com/EliverLara/Sweet/blob/master/LICENSE)

### RTTR

RTTR is licensed with the [MIT license](https://github.com/rttrorg/rttr/blob/master/LICENSE.txt)

### Genie

Genie is licensed [here](https://github.com/bkaradzic/GENie/blob/master/LICENSE)