# Synthesizer

C++ Synthesizer using OpenAL

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
