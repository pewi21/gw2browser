Gw2Browser
==========

Opens a Guild Wars 2 .dat file and allows the user to browse and extract its
files. Currently it supports power-of-two textures, JPEG and WebP images, models,
126x64 ATEX textures, sound, sound bank, eula, strings, text and binary files.

When a .dat file is opened, it will first index the file. This makes it *a lot*
faster to re-open the same .dat. Unfortunately, every time the .dat changes it
will have to be re-indexed (for now).

Keep in mind that if you keep tree items expanded while the indexer is running,
or while an index is being read, the process is *a lot* slower.

Also,when you first use find by file id function, the browser will appear to freeze.
This is normal since it will need to expand all file tree in oder to search for
given file id.

The latest binary can always be found [Here](https://github.com/kytulendu/Gw2Browser/releases) and git binary build [Here](http://goo.gl/4xMRVF)

For people who can't run Gw2Browser, download and Install
[Microsoft Visual C++ 2015 Redistributable Update 3](https://www.microsoft.com/en-us/download/details.aspx?id=52982)
choose vcredist.x64.exe for 64bit and vcredist.x86.exe for 32bit executable.

The modelviewer required GPU with OpenGL 3.3 support.

Usage
-----

It can be started with:

	Gw2Browser <input dat>

If `<input dat>` is given, the program will open the file as soon as it starts.

Known issues
------------

* **Viewing or exporting some model may crash the browser.**
This possibly cause by the file is in difference version and gw2formats library
doesn't support it, or more likely, a bug in gw2formats library. This issue
might fixed on next Gw2Browser release.

* **Model rendering is still experimental stage.**
Some of the model may use the wrong texture, or it might not. It entirely
depends on what model you view, as some models don't have any references to textures
(mostly equipment, player character parts models). Some models also use the
second UV channel for rendering while the shader in the browser only uses the
first. Loading any model is also *far* from optimized!

* **Non-power-of-two textures are unsupported at the moment.**
Decompressing one produces garbage blocks. Well, except 126x64 ATEX files.
Probably they use custom format for NPOT texture.

* **Some sound data in bank file(s) and text data in string file(s) are unsupported.**
The data are encrypted, each entry/files have difference encryption keys.

Building Gw2Browser
-------------------

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
* [wxWidgets 3.1.0](http://wxwidgets.org/)
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

* Download wxWidgets source code from [here](http://wxwidgets.org/), extract it to the same directory
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
and VS2015 is wx_vc14.sln.

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

		mingw32-make -j 4 -f makefile.gcc CXXFLAGS="-std=gnu++11" BUILD=debug

   If it give error, re-type it again.
   Wait for it to finish, then use these command

		mingw32-make -j 4 -f makefile.gcc CXXFLAGS="-std=gnu++11" BUILD=release

   This will build wxWidgets in debug and release configuration as a static
   library, or add "SHARED=1" for building dynamiclink library.

		mingw32-make -j 4 -f makefile.gcc CXXFLAGS="-std=gnu++11" BUILD=debug SHARED=1
		mingw32-make -j 4 -f makefile.gcc CXXFLAGS="-std=gnu++11" BUILD=release SHARED=1

2. If you need to rebuild, use "clean" target first.

		mingw32-make -j 4 -f makefile.gcc CXXFLAGS="-std=gnu++11" BUILD=debug clean
		mingw32-make -j 4 -f makefile.gcc CXXFLAGS="-std=gnu++11" BUILD=release clean

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

#### Linux, building with GCC

* I haven't try to build Gw2Browser on Linux yet, but it probably build :P

Authors
-------

* [Khral Steelforge](https://github.com/kytulendu)

### Thanks to:

* [Rhoot](https://github.com/rhoot) : Original Gw2Browser and gw2formats
* [Antoine Hom](https://github.com/ahom) : gw2DatTools
* David Dantowitz : Simple CRC
* Demonsangel : Noesis script fmt_GW2_pf.py
* [DubyaDubyaDubyaDot](https://stackoverflow.com/questions/13437422/libvorbis-audio-decode-from-memory-in-c) : How to read Oggs from memory using custom ogg callbacks.
* [hackedd](https://github.com/hackedd) : Idea for fix extraction of uncompressed files larger than 65532 bytes.
* [Inigo Quilez](http://www.iquilezles.org/www/articles/normals/normals.htm) : clever normalization of mesh
* [Joey de Vries](http://learnopengl.com) and [opengl-tutorial.org](http://www.opengl-tutorial.org) : Great OpenGL 3.3 tutorial
* [Liberation Sans Regular](https://fedorahosted.org/liberation-fonts/) : Font used by the model viewer
* [Open Icon Library](http://openiconlibrary.sourceforge.net/) : Icons used in this software
* [ral](https://sites.google.com/site/jumptovictory/) : Information of files in Gw2.dat
* [Romain Dura](https://mouaif.wordpress.com/2009/01/28/levels-control-shader/) : Levels control shader
* [Till034](https://github.com/Till034) : OnCompareItems function overload
* [Xtridence](http://forum.xentax.com/viewtopic.php?p=72880#p72880) : modified GWUnpacker to work with Gw2.dat

### Special thanks to:

* ArenaNet/NCSoft and the team that make Guild Wars 2!

Contributing
------------

For contributions, please go to [this project's page on GitHub](https://github.com/kytulendu/Gw2Browser).

Legal Disclaimer
----------------

Guild Wars 2 © 2010-2017 ArenaNet, LLC. All rights reserved.
Guild Wars, Guild Wars 2, Guild Wars 2: Heart of Thorns, ArenaNet, NCSOFT,
the Interlocking NC Logo, and all associated logos and designs are trademarks
or registered trademarks of NCSOFT Corporation.
All other trademarks are the property of their respective owners.

License
-------

	Copyright © 2014-2017 Khral Steelforge <https://github.com/kytulendu>
	Copyright © 2013 Till034 <https://github.com/Till034>
	Copyright © 2012 Rhoot <https://github.com/rhoot>

Gw2Browser is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Code License Exceptions
-----------------------

### OpenGL Mathematics (GLM)

-----------------------------------------------------------------------------

extern/glm/*

	Copyright (c) 2005 - 2016 G-Truc Creation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

### OpenGL Extension Wrangler Library (GLEW)

-----------------------------------------------------------------------------

	The OpenGL Extension Wrangler Library
	Copyright (C) 2002-2007, Milan Ikits <milan ikits[]ieee org>
	Copyright (C) 2002-2007, Marcelo E. Magallon <mmagallo[]debian org>
	Copyright (C) 2002, Lev Povalahev
	All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
* The name of the author may be used to endorse or promote products
  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.


Mesa 3-D graphics library
Version:  7.0

	Copyright (C) 1999-2007  Brian Paul   All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


	Copyright (c) 2007 The Khronos Group Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and/or associated documentation files (the
"Materials"), to deal in the Materials without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Materials, and to
permit persons to whom the Materials are furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Materials.

THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.

### libwebp

-----------------------------------------------------------------------------

extern/libwebp/*

	Copyright (c) 2010, Google Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

  * Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in
	the documentation and/or other materials provided with the
	distribution.

  * Neither the name of Google nor the names of its contributors may
	be used to endorse or promote products derived from this software
	without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

### FreeType

-----------------------------------------------------------------------------

extern/freetype/*

	Portions of this software are copyright (C) 2016 The FreeType
	Project (www.freetype.org).  All rights reserved.

See extern/freetype/docs/FTL.TXT for more details.

### libogg

-----------------------------------------------------------------------------

Copyright (c) 2002, Xiph.org Foundation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

- Neither the name of the Xiph.org Foundation nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

### libvorbis

-----------------------------------------------------------------------------

Copyright (c) 2002-2008 Xiph.org Foundation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

- Neither the name of the Xiph.org Foundation nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

### mpg123

-----------------------------------------------------------------------------

	Copyright (c) 1995-2013 by Michael Hipp and others,
	free software under the terms of the LGPL v2.1

### OpenAL Soft

-----------------------------------------------------------------------------

	Software OpenAL implementation.
	Copyright (C) <year>  <name of author>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
