Building Gw2Browser
-------------------

## Building on Windows

### Required software

* [Git](https://git-scm.com/downloads) for download source code. It also come with Git Bash
for use as command prompt to compile some required libraries if using GCC to compile.

* Microsoft Visual Studio 2012 or better. Express version will work too.
or use [Visual Studio Community 2015](https://www.visualstudio.com/products/visual-studio-community-vs).

        or

* [Git](https://git-scm.com/downloads)

* [TDM-GCC 5.1.0](http://tdm-gcc.tdragon.net/)

* [CMake](https://cmake.org/)

* [CodeBlocks](http://www.codeblocks.org/) for use to open Gw2Browser's CodeBlocks solution file.

### Optional software

* [Cppcheck](http://cppcheck.sourceforge.net/) a static code analysis tool for the C and C++.

If using Visual Studio, also download [Visual Studio integration add-in for Cppcheck](https://github.com/VioletGiraffe/cppcheck-vs-addin/releases)

### Required libraries

* [mpg123](https://www.mpg123.de)
* [OpenAL-Soft](http://kcat.strangesoft.net/openal.html)
* [wxWidgets 3.1.0 or higher](http://wxwidgets.org/)
* [FreeType](http://www.freetype.org/) *Included*
* [gw2DatTools](https://github.com/kytulendu/gw2DatTools) *Included*
* [gw2formats](https://github.com/kytulendu/gw2formats) *Included*
* [libogg](https://github.com/xiph/ogg) *Included*
* [libvorbis](https://github.com/xiph/vorbis) *Included*
* [libwebp](https://developers.google.com/speed/webp/download) *Included*
* [OpenGL Mathematics](http://glm.g-truc.net/) *Included*
* [The OpenGL Extension Wrangler Library](http://glew.sourceforge.net/) *Included*

### Optional libraries

* [Visual Leak Detector](http://vld.codeplex.com/) (for use with Visual Studio only)

If you want to use Visual Leak Detector, remove the "//" at #include <vld.h>
in Gw2Browser.cpp.

### Building instructions

#### Getting the source code:

* Crate a directory for source code some where, for example, C:\DEV.
* Open Git Bash window and change directory to the one you create above.
* Use this command to download the source code.

        git clone --recursive https://github.com/kytulendu/Gw2Browser.git

This will download Gw2Browser and all included library source code.

#### Getting the required library:

* Download mpg123 binaries from [here](https://www.mpg123.de/), choose the lastest
Win32 or Win64 file and extract it to the same directory that Gw2Browser directory is in.
Or download the source code and build it your self.

* Download OpenAL-Soft binaries from [here](http://kcat.strangesoft.net/openal.html),
extract it to the same directory that Gw2Browser directory is in. Or download the source code
and build it your self.

* Download wxWidgets source code from [here](https://github.com/wxWidgets/wxWidgets/releases),
choose wxWidgets-3.1.0.zip or wxWidgets-3.1.0.7z and extract it to the same directory
that Gw2Browser directory is in. Or download and use the binaries.

**Note:** The library and source code directory must be in directory like this.
The ROOT is the directory you created in Getting the source code.

        [ROOT]
         |
         +--Gw2Browser
         |    |
         |    +--src
         |    +--...
         |
         +--mpg123
         |
         +--openal-soft
         |    |
         |    +--bin
         |    +--include
         |    +--lib
         |    +--...
         |
         +--wxWidgets-3.1.0
              +--include
              +--src
              +--...

#### Windows, building with Visual Studio:

* Compile wxWidgets using solution file corresponding with your VS version in directory
wxWidgets-3.1.0/build/msw. For example, VS2012 is wx_vc11.sln, VS2013 is wx_vc12.sln
and VS2015 is wx_vc14.sln, if there is no corresponding solution file for your newer VS,
just open the higest version of solution file available.

* Compile libwebp using Visual Studio Native Tools Command Prompt, both 32 and 64 bit
by use these command at Gw2Browser/extern/libweb directory.

        nmake /f Makefile.vc CFG=debug-static RTLIBCFG=dynamic OBJDIR=obj
        nmake /f Makefile.vc CFG=release-static RTLIBCFG=dynamic OBJDIR=obj

If you use Visual Studio 2015, use "Developer Command Prompt for VS2015",
the default command line will compile 64 bit library (if you use 64 bit OS).
If you want to compile 32 bit library, use this command.

        cd vc

The directory will change to C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC
then use this command before compile library by above command.

        vcvarsall amd64_x86

For 32 bit library.

        vcvarsall amd64

For 64 bit library.

* Build mpg123 library.

1. "Visual Studio Native Tools Command Prompt" or "Developer Command Prompt for VS2015"
and change it's directory to mpg123's directory.

2. Rename export definition file by using this command.

        ren libmpg123-0.dll.def libmpg123-0.def

3. Build the library.

for 32 bit binary, use this command.

        lib /DEF:libmpg123-0.def /OUT:libmpg123-0.lib

for 64 bit binary, use this command.

        lib /MACHINE:X64 /DEF:libmpg123-0.def /OUT:libmpg123-0.lib

* Compile libogg using libogg_static.sln solution file in Gw2Browser/extern/libogg/win32/VS2015.

* Compile libvorbis using vorbis_static.sln solution file in Gw2Browser/extern/libvorbis/win32/VS2015.

* Compile Gw2Browser using Gw2Browser.sln solution file in Gw2Browser/prj.

#### Windows, building with TDM-GCC

* You must add TDM-GCC to your path.

* Open Git Bash command line window.

* Build mpg123 library.

1. Open libmpg123-0.dll.def with any text editor, like Notepad++ and insert this to the first line.

        LIBRARY libmpg123-0.dll

It will look some thing like this.

        LIBRARY libmpg123-0.dll
        EXPORTS
        mpg123_add_string
        ...

2. Change directory of Git Bash window to mpg123 directory and use these command.

        dlltool -d libmpg123-0.dll.def -l libmpg123-0.a

* Build wxWidgets.

1. Change directory of Git Bash window to wxWidgets-3.1.0/build/msw and use these command.

        mingw32-make -j 4 -f makefile.gcc CXXFLAGS="-std=c++11" BUILD=debug

   If it give error, re-type it again.
   Wait for it to finish, then use these command

        mingw32-make -j 4 -f makefile.gcc CXXFLAGS="-std=c++11" BUILD=release

   This will build wxWidgets in debug and release configuration as a static
   library, or add "SHARED=1" for building dynamiclink library.

        mingw32-make -j 4 -f makefile.gcc CXXFLAGS="-std=c++11" BUILD=debug SHARED=1
        mingw32-make -j 4 -f makefile.gcc CXXFLAGS="-std=c++11" BUILD=release SHARED=1

2. If you need to rebuild, use "clean" target first.

        mingw32-make -j 4 -f makefile.gcc BUILD=debug clean
        mingw32-make -j 4 -f makefile.gcc BUILD=release clean

* Build libwebp.

1. Change directory of Git Bash window to Gw2Browser/extern/libwebp and use these command

		mingw32-make -f makefile.unix "EXTRA_FLAGS=" "DWEBP_LIBS=" "CWEBP_LIBS=" "GIF_LIBS="

* Build glew.

1. Use CMake-gui to generate glew's MinGW makefile.

        In "Where is the source code:" box, enter Gw2Browser/extern/glew
        In "Where to build the binaries:" box, enter Gw2Browser/extern/glew
        Click "Configure", then click "Generate".

2. Open CMakeCache.txt in Gw2Browser/extern/glew directory and change these line

        CMAKE_C_FLAGS:STRING=
        to
        CMAKE_C_FLAGS:STRING=-DGLEW_STATIC

        CMAKE_CXX_FLAGS:STRING=
        to
        CMAKE_CXX_FLAGS:STRING=-DGLEW_STATIC

3. Change directory of Git Bash window to Gw2Browser/extern/glew and use these command

        mingw32-make

* Build libogg

1. Use CMake-gui to generate libogg's MinGW makefile.

        In "Where is the source code:" box, enter Gw2Browser/extern/libogg
        In "Where to build the binaries:" box, enter Gw2Browser/extern/libogg
        Click "Configure", then click "Generate".

2. Change directory of Git Bash window to Gw2Browser/extern/libogg and use these command

        mingw32-make

* Build libvorbis

1. Use CMake-gui to generate libvorbis's MinGW makefile.

        In "Where is the source code:" box, enter Gw2Browser/extern/libvorbis
        In "Where to build the binaries:" box, enter Gw2Browser/extern/libvorbis
        Click "Configure",
        Set "OGG_INCLUDE_DIRS" to "Gw2Browser/extern/libogg/include"
        Set "OGG_LIBRARIES" to "Gw2Browser/extern/libogg.a"
        Then, click "Generate".

2. Change directory of Git Bash window to Gw2Browser/extern/libvorbis and use these command

        mingw32-make

* Build freetype

1. Open Command Prompt window (cmd.exe).

2. Change directory to Gw2Browser/extern/freetype and use these command.

        mkdir objs
        mingw32-make

* Build Gw2Browser

   Open solution file Gw2Browser.workspace in Gw2Browser/prj/. Then, compile gw2DatTools and gw2formats
   by right click and select as active project and build it, both debug and release build. Then proceed
   to compile Gw2Browser.

* Copy libgomp-1.dll or libgomp_64-1.dll from TDM-GCC install directory to Gw2Browser/bin

* Copy soft_oal.dll from openal-soft/bin/Win32 or openal-soft/bin/Win64 directory to Gw2Browser/bin and rename it to OpenAL32.dll

* Copy libmpg123-0.dll from mpg123 directory to Gw2Browser/bin

## Building on Linux

For this guide, I was using Kubuntu, but this guide will work with any Linux distribution that was Debian based too.

### Building instructions

#### Getting the required library and tools:

* Download wxWidgets 3.1 source code from [here](https://github.com/wxWidgets/wxWidgets/releases),
choose wxWidgets-3.1.0.zip or wxWidgets-3.1.0.7z and extract it to somewhere, for example, your home directory. If your system have wxWidgets 3.1 package, you can skip this and use a command like below command to install wxWidgets 3.1.

        sudo apt install libwxbase3.1-dev libwxgtk3.1-dev

* Open a terminal window, the directory would be in your home directory.
* Use these command to install the required package.

Incase you haven't have git installed use this command.

        sudo apt install git git-gui

Use this command to install required library and tools.

        sudo apt install build-essential codeblocks libwebp-dev libglew-dev libglew2.0 libopenal-dev libmpg123-dev libvorbis-dev libogg-dev libfreetype6 libfreetype6-dev

#### Getting the source code:

* Use this command to download the source code.

        git clone --recursive https://github.com/kytulendu/Gw2Browser.git

This will download Gw2Browser and all included library source code, although we won't use most of the library that was included in the repo.

#### Compiling wxWidgets:

* Open a terminal window and change directory to wxWidgets source code directory you have extracted.
* Make a directory using this command

        mkdir gtk-build

* Change directory to the directory you make previously, in this case, gtk-build directory.

        cd gtk-build

* Use this command to config the build script and wait for it to finish.

        ../configure --enable-unicode --enable-debug --with-opengl

* Use this command to compile wxWidgets source code.

        make

or

        make -j 4

* When finished, use this command to install wxWidgets

        sudo make install

after that use this command

        sudo ldconfig

#### Compiling the source:

* Open solution file Gw2Browser-linux.workspace in Gw2Browser/prj/ with CodeBlocks. Then, compile it.
