# Building this project


### The Synthesizer Project
Synthesizer uses CMake to generate project files. 
Currently the project is being developed with Visual Studio 2017, but it is done in a way that should make it easy to transfer to other platforms by using project generation and crossplatform libraries

The project comes with packaged lib, dll and include files for all dependancies, compiled with vs2017. If you want to build with a different visual studio version, you need to build GTKmm and its dependancies for that compiler.

In order to build the project, you need to generate the project files with CMake. You should have CMake 3.10 or later installed on your computer
Next, open a terminal and navigate to the repository root folder, and execute:

    cmake -G "Visual Studio 15 2017 Win64" -H. -Bbuild

The -H and -B options specify an out of source build in the build folder, which is recommended as the gitignore is set up for this case. You could alternatively do this

    cd build
	cmake -G "Your preferred generator [and architecture]" ..

Next open the solution that was generated at **build/Synthesizer.sln** in visual studio, select the desired configuration (_Development_ is good for most purposes), and build as usual.

The final executable will appear in **bin/[configuration]_[platform]/bin/Synthesizer.exe**

In order to run the built program, you also need to assemble its dependancies - such as dlls and resource files - relative to the executable in the inner 'bin/' folder. To do this, build the 'INSTALL' project in visual studio.


### Building GTKmm for MSVC
Gtkmm was obtained through vcpkg. If you want to use a different visual studio version, use Vcpkg to compile gtkmm for that version.

To get vcpkg, find a directory you want to install it in and do:

    git clone https://github.com/Microsoft/vcpkg
    cd vcpkg
    .\bootstrap-vcpkg.bat

If you have multiple versions of Visual Studio installed on your computer you need to [specify the toolset for vcpkg manually](https://github.com/microsoft/vcpkg/blob/master/docs/users/triplets.md#VCPKG_VISUAL_STUDIO_PATH).
To do so add these two lines to 'x86-windows.cmake' and 'x64-windows.cmake'

    set(VCPKG_PLATFORM_TOOLSET v141)
    set(VCPKG_VISUAL_STUDIO_PATH "C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community")

Then, to get GTKmm, install it for both 64 bit and 32 bit

    vcpkg install gtkmm:x86-windows
    vcpkg install gtkmm:x64-windows

Finally, the various .lib, .dll and .pdb files will appear in 'vcpkg/installed'. These need to be copied over into the synthesizers dependancies folder for gtkmm, under Debug and Release respectively.