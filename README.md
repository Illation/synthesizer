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

In order to build the project, first download the GENie executable. The easiest thing for later use would be installing it in your PATH, but you can also use the executable anywhere else.
Next, open a terminal and navigate to the repository root folder, and execute:

    genie --file=build/genie.lua vs2017

If you didn't install GENie in your path it would look more like

    .\relative\path\to\your\genie.exe --file=build/genie.lua [preferred visual studio version]

Next open the solution that was generated at **build/Synthesizer.sln** in visual studio, select the desired configuration (_Development_ is good for most purposes), and build as usual.

The final executable will appear in **bin/[configuration]_[platform]/Synthesizer/Synthesizer.exe**


## Third Party

This project uses PortAudio, RtMIDI and GTK 3+, here are the licenses for them:

#### Portaudio

    PortAudio Portable Real-Time Audio Library
    Copyright (c) 1999-2011 Ross Bencina and Phil Burk

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#### RtMidi

	RtMidi: realtime MIDI i/o C++ classes<BR>
	Copyright (c) 2003-2019 Gary P. Scavone

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation files
	(the "Software"), to deal in the Software without restriction,
	including without limitation the rights to use, copy, modify, merge,
	publish, distribute, sublicense, and/or sell copies of the Software,
	and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	Any person wishing to distribute modifications to the Software is
	asked to send the modifications to the original developer so that
	they can be incorporated into the canonical version.  This is,
	however, not a binding provision of this license.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
	ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
	WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

### GTK

    GTK is licensed under [GNU LGPL 2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html) 