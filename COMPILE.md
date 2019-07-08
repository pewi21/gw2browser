Building Gw2Browser
-------------------

* [Building on Windows](#building-on-windows)

    * [Required software](#required-software)

    * [Optional software](#optional-software)

    * [Required libraries](#required-libraries)

    * [Optional libraries](#optional-libraries)

    * [Getting the source code](#getting-the-source-code)

    * [Getting the required library](#getting-the-required-library)

    * Building instructions

        * [Building with Visual Studio](#building-with-visual-studio)

        * [Building with MinGW-w64](#building-with-mingw-w64)

* [Building on Linux](#building-on-linux)

    * [Building instructions](#building-instructions-1)

* [Cross compile for Windows from Linux](#cross-compile-for-windows-from-linux)

---

## Building on Windows

### Required software

* [Git](https://git-scm.com/downloads)

* [CMake](https://cmake.org/)

* Microsoft Visual Studio 2017 or [Visual Studio Community 2017](https://www.visualstudio.com/products/visual-studio-community-vs)
Choose ... also install Windows SDK Version 8.1

  or

* [MinGW-w64](https://sourceforge.net/projects/mingw-w64/)

* [CodeBlocks](http://www.codeblocks.org/)

### Optional software

* [Cppcheck](http://cppcheck.sourceforge.net/) a static code analysis tool for the C and C++.

If using Visual Studio, also download [Visual Studio integration add-in for Cppcheck](https://github.com/VioletGiraffe/cppcheck-vs-addin/releases)

### Required libraries

* [mpg123](https://www.mpg123.de)
* [OpenAL-Soft](http://kcat.strangesoft.net/openal.html)
* [wxWidgets 3.1.2 or higher](http://wxwidgets.org/)
* [FreeType](http://www.freetype.org/) Included
* [gw2dattools](https://github.com/kytulendu/gw2dattools) Included
* [gw2formats](https://github.com/kytulendu/gw2formats) Included
* [libogg](https://github.com/xiph/ogg) Included
* [libvorbis](https://github.com/xiph/vorbis) Included
* [libwebp](https://developers.google.com/speed/webp/download) Included
* [OpenGL Mathematics](http://glm.g-truc.net/) Included
* [The OpenGL Extension Wrangler Library](http://glew.sourceforge.net/) Included
* [TinyXML2](https://github.com/leethomason/tinyxml2) Included

### Optional libraries

* [Visual Leak Detector](http://vld.codeplex.com/) (for use with Visual Studio only)

If you want to use Visual Leak Detector, remove the comment for `#include <vld.h>` in `Gw2Browser.cpp`.

### Getting the source code

* Crate a directory for source code some where, for example, `C:\DEV`.
* Open Git Bash window and change directory to the one you create above.
* Use this command to download the source code.

      git clone --recursive https://github.com/kytulendu/Gw2Browser.git

This will download Gw2Browser and all included library source code.

### Getting the required library

* Download mpg123 binaries from [here](https://www.mpg123.de/), choose the lastest
Win32 or Win64 file and extract it to the same directory that Gw2Browser directory is in.
Or download the source code and build it your self.

* Download OpenAL-Soft binaries from [here](http://kcat.strangesoft.net/openal.html),
extract it to the same directory that Gw2Browser directory is in. Or download the source code
and build it your self.

* Download wxWidgets source code from [here](https://github.com/wxWidgets/wxWidgets/releases),
choose `wxWidgets-3.1.2.zip` or `wxWidgets-3.1.2.7z` and extract it to the same directory
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
         +--wxWidgets-3.1.2
              +--include
              +--src
              +--...

### Building instructions

---

### Building with Visual Studio

#### Compile wxWidgets:

* Use solution file corresponding with your VS version in directory `wxWidgets-3.1.2/build/msw`.
  For example, VS2017 is `wx_vc15.sln`, if there is no corresponding solution file for your newer VS,
  just open the higest version of solution file available.

#### Compile libmpg123:

* Open a `x64 Native Tools Command Prompt for VS 2017` or `x86 Native Tools Command Prompt for VS 2017`
  and change it's directory to mpg123's directory.

* Rename export definition file by using this command.

      ren libmpg123-0.dll.def libmpg123-0.def

* Build the library.

  for 64 bit binary, use this command.

      lib /MACHINE:X64 /DEF:libmpg123-0.def /OUT:libmpg123-0.lib

  for 32 bit binary, use this command.

      lib /DEF:libmpg123-0.def /OUT:libmpg123-0.lib

#### Compile libwebp:

* Open `x64 Native Tools Command Prompt for VS 2017`
  If you want to compile 32 bit library, use `x86 Native Tools Command Prompt for VS 2017` instread.

* Change directory to `Gw2Browser/extern/libweb`

* Use this command to compile libwebp

      nmake /f Makefile.vc CFG=debug-static RTLIBCFG=dynamic OBJDIR=obj
      nmake /f Makefile.vc CFG=release-static RTLIBCFG=dynamic OBJDIR=obj

#### Compile freetype:

* Create vc2017 directory in `Gw2Browser/extern/freetype/builds/windows`

* Copy all content from vc2010 directory to vc2017 directory

* Open `freetype.sln` solution file in `Gw2Browser/extern/freetype/builds/windows/vc2017`
  If it ask for upgrade project file, choose Windows SDK Version to `8.1`,
  then click "Ok" to upgrade it.

* Choose `Debug Static` or `Release Static` configuration and choose to Win32 for 32 bit build or x64 for 64 bit build
  from two dropdown box in Visual Studio toolbar.

* Press F7 or in MenuBar -> Build -> Build Solution to compile freetype.

#### Compile glew:

* Create vc14 directory in `Gw2Browser/extern/glew/build`

* Copy all content from vc12 directory to vc15 directory

* Open `glew.sln` solution file in `Gw2Browser/extern/glew/build/vc15`
  If it ask for upgrade project file, choose Windows SDK Version to `8.1`,
  then click "Ok" to upgrade it.

* Right click "glew_static" project in "Solution Explorer" at the left and choose "Properties".

* In "Configuration:" dropdown box, choose "Debug"
  and in "Platform:" dropdown box, choose "All Platforms"

* In "Configuration Properties" -> "C/C++" -> "Code Generation"
  In "Runtime Library" box, Choose "Multi-threaded Debug DLL (/MDd)"

* Click "Apply" button.

* In "Configuration:" dropdown box, choose "Release"
  and in "Platform:" dropdown box, choose "All Platforms"

* In "Configuration Properties" -> "C/C++" -> "Code Generation"
  In "Runtime Library" box, Choose "Multi-threaded DLL (/MD)"

* Click "Apply" button then click "OK".

* Choose Debug or Release configuration and choose to Win32 for 32 bit build or x64 for 64 bit build
  from two dropdown box in Visual Studio toolbar.

* Press F7 or in MenuBar -> Build -> Build Solution to compile libogg.

#### Compile libogg:

* Create VS2017 directory in `Gw2Browser/extern/libogg/win32`

* Copy all content from VS2015 directory to VS2017 directory

* Open `libogg_static.sln` solution file in `Gw2Browser/extern/libogg/win32/VS2017`
  If it ask for upgrade project file, choose Windows SDK Version to `8.1`,
  then click "Ok" to upgrade it.

* Right click "libogg_static" project in "Solution Explorer" at the left and choose "Properties".

* In "Configuration:" dropdown box, choose "Debug"
  and in "Platform:" dropdown box, choose "All Platforms"

* In "Configuration Properties" -> "C/C++" -> "Code Generation"
  In "Runtime Library" box, Choose "Multi-threaded Debug DLL (/MDd)"

* Click "Apply" button.

* In "Configuration:" dropdown box, choose "Release"
  and in "Platform:" dropdown box, choose "All Platforms"

* In "Configuration Properties" -> "C/C++" -> "Code Generation"
  In "Runtime Library" box, Choose "Multi-threaded DLL (/MD)"

* Click "Apply" button then click "OK".

* Choose Debug or Release configuration and choose to Win32 for 32 bit build or x64 for 64 bit build
  from two dropdown box in Visual Studio toolbar.

* Press F7 or in MenuBar -> Build -> Build Solution to compile libogg.

#### Compile libvorbis:

* Create VS2017 directory in `Gw2Browser/extern/libvorbis/win32`

* Copy all content from VS2010 directory to VS2017 directory

* Open `vorbis_static.sln` solution file in `Gw2Browser/extern/libvorbis/win32/VS2015`
  If it ask for upgrade project file, choose Windows SDK Version to `8.1`,
  then click "Ok" to upgrade it.

* Choose Debug or Release configuration and choose to Win32 for 32 bit build or x64 for 64 bit build
  from two dropdown box in Visual Studio toolbar.

* Press F7 or in MenuBar -> Build -> Build Solution to compile libogg.

#### Compile Gw2Browser:

* Open `Gw2Browser.sln` solution file in `Gw2Browser/prj`.

* Choose Debug or Release configuration and choose to Win32 for 32 bit build or x64 for 64 bit build
  from two dropdown box in Visual Studio toolbar.

* Press F7 or in MenuBar -> Build -> Build Solution to compile Gw2Browser.

#### Copy required binaries:

* Copy `soft_oal.dll` from `openal-soft/bin/Win32` or `openal-soft/bin/Win64` directory to `Gw2Browser/bin` and rename it to `OpenAL32.dll`

* Copy `libmpg123-0.dll` from mpg123 directory to `Gw2Browser/bin`

---

### Building with MinGW-w64

Download MinGW-w64 and install it to any location, for example `C:\mingw-64`,
choose Version to `8.1.0 rev 0`, choose Architecture to `x86_64 or i686`, choose Threads to `POSIX` (important).

You must also add MinGW-w64 to your path.

for example set your path to `%path%;C:\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin`

#### Compile libmpg123:

* Open `libmpg123-0.dll.def` with any text editor, like Notepad++ and insert this to the first line.

      LIBRARY libmpg123-0.dll

  It will look some thing like this.

      LIBRARY libmpg123-0.dll
      EXPORTS
      mpg123_add_string
      ...

* Open Git Bash command line window and change directory to mpg123 directory and use these command.

      dlltool -d libmpg123-0.dll.def -l libmpg123-0.a

#### Compile wxWidgets:

* Open Git Bash command line window and change directory to `wxWidgets-3.1.2/build/msw` and use these command.

      mingw32-make -j 4 -f makefile.gcc BUILD=debug SHARED=1

  If it give error, re-type it again.
  Wait for it to finish, then use these command.

      mingw32-make -j 4 -f makefile.gcc BUILD=release SHARED=1

  This will build wxWidgets in debug and release configuration as a dynamiclink library.
  If you want static library, remove `SHARED=1` from the commandline. But you have to change
  library search path in CodeBlocks project file from `gcc_dll` to `gcc_lib`.

      mingw32-make -j 4 -f makefile.gcc BUILD=debug
      mingw32-make -j 4 -f makefile.gcc BUILD=release

* If you need to rebuild, use "clean" target first.

      mingw32-make -j 4 -f makefile.gcc BUILD=debug SHARED=1 clean
      mingw32-make -j 4 -f makefile.gcc BUILD=release SHARED=1 clean
      mingw32-make -j 4 -f makefile.gcc BUILD=debug clean
      mingw32-make -j 4 -f makefile.gcc BUILD=release clean

#### Compile glew:

* Open Command Prompt and change directory to `Gw2Browser/extern/glew` and use these command

      cmake -G "MinGW Makefiles" ./build/cmake
      mingw32-make

#### Compile libogg:

* Open Command Prompt and change directory to `Gw2Browser/extern/libogg` and use these command

      mkdir build
      cd build
      cmake -G "MinGW Makefiles" .. -DBUILD_SHARED_LIBS=ON
      mingw32-make

#### Compile libvorbis:

* Open Command Prompt and change directory to `Gw2Browser/extern/libvorbis` and use these command

      mkdir build
      cd build
      cmake -G "MinGW Makefiles" .. -DBUILD_SHARED_LIBS=ON -DOGG_INCLUDE_DIRS="E:/DEV/Gw2Browser/extern/libogg/include" -DOGG_LIBRARIES="E:/DEV/Gw2Browser/extern/libogg/build/libogg.dll.a"
      mingw32-make

#### Compile libwebp:

* Open Command Prompt and change directory to `Gw2Browser/extern/libwebp` and use these command

      mkdir build
      cd build
      cmake -G "MinGW Makefiles" .. -DBUILD_SHARED_LIBS=ON
      mingw32-make

#### Compile freetype:

* Open Command Prompt and change directory to `Gw2Browser/extern/freetype` and use these command

      mkdir build
      cd build
      cmake -G "MinGW Makefiles" ..
      mingw32-make

#### Compile tinyxml2:

* Open Command Prompt and change directory to `Gw2Browser/extern/tinyxml2` and use these command

      mkdir build
      cd build
      cmake -G "MinGW Makefiles" ..
      mingw32-make

#### Compile Gw2Browser:

* Open workspace file `Gw2Browser.workspace` in `Gw2Browser/prj` with CodeBlocks.

* Compile Gw2Browser by right click and select Gw2Browser as active project,
  choose debug or release build, then click the gear button in toolbar
  or press CTRL+F9 key or in MenuBar -> Build -> Build.

#### Copy required binaries:

* Copy these dll files from MinGW-w64 install directory to `Gw2Browser/bin`

      libgcc_s_seh-1.dll
      libgomp-1.dll
      libstdc++-6.dll
      libwinpthread-1.dll

* Copy `soft_oal.dll` from `openal-soft/bin/Win32` or `openal-soft/bin/Win64` directory to `Gw2Browser/bin` and rename it to `OpenAL32.dll`

* Copy `libmpg123-0.dll` from mpg123 directory to `Gw2Browser/bin`

* Copy `glew32.dll1 from `Gw2Browser/extern/glew/bin` directory to `Gw2Browser/bin`

* Copy `libtinyxml2.dll` from `Gw2Browser/extern/tinyxml2/build` directory to `Gw2Browser/bin`

* Copy `libogg.dll` from `Gw2Browser/extern/libogg/build` directory to `Gw2Browser/bin`

* Copy `libvorbis.dll` and `libvorbisfile.dll` from `Gw2Browser/extern/libvorbis/build/lib` directory to `Gw2Browser/bin`

* Copy `libwebpdecoder.dll` from `Gw2Browser/extern/libwebp/build` directory to `Gw2Browser/bin`

* Copy following dll files from `wxWidgets/lib/gcc_dll` directory to `Gw2Browser/bin`

      wxbase312u_gcc_custom.dll
      wxmsw312u_adv_gcc_custom.dll
      wxmsw312u_aui_gcc_custom.dll
      wxmsw312u_core_gcc_custom.dll
      wxmsw312u_gl_gcc_custom.dll

  If debug build, also copy these dll files.

      wxbase312ud_gcc_custom.dll
      wxmsw312ud_adv_gcc_custom.dll
      wxmsw312ud_aui_gcc_custom.dll
      wxmsw312ud_core_gcc_custom.dll
      wxmsw312ud_gl_gcc_custom.dll

---

## Building on Linux

For this guide, I was using Kubuntu 18.04, but this guide also work with any Linux distribution that was Debian based.

### Building instructions

#### Getting the required library and tools:

* Download wxWidgets 3.1 source code from [here](https://github.com/wxWidgets/wxWidgets/releases),
choose wxWidgets-3.1.2.tar.bz2 and extract it to somewhere, for example, your home directory.
If your Linux distribution have wxWidgets 3.1 package, you can skip this and use a command like below command to install wxWidgets 3.1.

      sudo apt install libwxbase3.1-dev libwxgtk3.1-dev

  Or download wxWidgets's repository using this command.

      git clone https://github.com/wxWidgets/wxWidgets.git

* Open a terminal window, the directory would be in your home directory.
* Install git by using this command.
  You can skip this step if you have already install git.

      sudo apt install git git-gui

* Use this command to install required library and tools.

      sudo apt install build-essential codeblocks cmake cmake-gui libwebp-dev libglew-dev libopenal-dev libmpg123-dev libvorbis-dev libogg-dev libfreetype6 libfreetype6-dev libtinyxml2-dev

#### Getting the source code:

* Use this command to download the source code.

      git clone --recursive https://github.com/kytulendu/Gw2Browser.git

This will download Gw2Browser and all included library source code, although we won't use most of the library that was included in the repo.

#### Compiling wxWidgets:

* If your system have wxWidgets 3.1 package, you can skip this step.
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

* After that use this command

      sudo ldconfig

#### Compiling gw2dattools:

* Open a terminal window and change directory to extern/gw2dattools source code directory you have extracted.
* Make a directory using this command

      mkdir build

* Change directory to the directory you make previously, in this case, build directory.

      cd build

* Use this command to and wait for it to finish.

      cmake ..

* Use this command to compile gw2dattools source code.

      make

  or

      make -j 4

* When finished, use this command to install gw2dattools

      sudo make install

#### Compiling gw2formats:

* Open a terminal window and change directory to extern/gw2formats source code directory you have extracted.
* Make a directory using this command

      mkdir build

* Change directory to the directory you make previously, in this case, build directory.

      cd build

* Use this command to and wait for it to finish.

      cmake ..

* Use this command to compile gw2formats source code.

      make

  or

      make -j 4

* When finished, use this command to install gw2formats

      sudo make install

#### Compiling Gw2Browser:

* Open a terminal window and change directory to Gw2Browser directory.
* Make a directory using this command

      mkdir build

* Change directory to the directory you make previously, in this case, build directory.

      cd build

* Use this command to and wait for it to finish.

      cmake ..

* Use this command to compile Gw2Browser source code.

      make

  or

      make -j 4

* When finished, use this command to install Gw2Browser

      sudo make install

---

## Cross compile for Windows from Linux

Todo
