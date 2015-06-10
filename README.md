On the break
------------

Due to I have been conscripted to do military services for 2 years (1/5/2014 - 1/5/2016)
I may or may not continue to develop Gw2Browser during that time. If you think waiting
is long and want to continue this project, feel free to folk it ;)

See you later! * waves *

Gw2Browser
==========

Opens a Guild Wars 2 .dat file and allows the user to browse and extract its
files. Currently it supports power-of-two textures, JPEG and WebP Images, models,
126x64 ATEX textures, sound, strings and binary files.

When a .dat file is opened, it will first index the file. This makes it *a lot*
faster to re-open the same .dat. Unfortunately, every time the .dat changes it
will have to be re-indexed (for now).

Keep in mind that if you keep tree items expanded while the indexer is running,
or while an index is being read, the process is *a lot* slower.

The latest binary can always be found [Here](http://goo.gl/4xMRVF)

For people who can't run Gw2Browser, download and Install
[Visual C++ Redistributable Packages for Visual Studio 2013](http://www.microsoft.com/en-us/download/details.aspx?id=40784)
choose vcredist_x64.exe for 64bit and vcredist_x86.exe for 32bit executable.

And if it still didn't run, download and install
[DirectX End-User Runtimes (June 2010)](http://www.microsoft.com/en-us/download/details.aspx?id=8109)

Usage
-----

It can be started with:

    Gw2Browser <input dat>

If `<input dat>` is given, the program will open the file as soon as it starts.

Known issues
------------

* **Model rendering is very experimental.**
Viewing one may crash the browser, or it may not. It might also use the wrong texture,
or it might not. It entirely depends on what model you view, as some models don't have
any references to textures (mostly equipment models). Some models also use the
second UV channel for rendering while the shader in the browser only uses the
first. Loading any model is also *far* from optimized!

* **Non-power-of-two textures are unsupported at the moment.**
Decompressing one produces garbage blocks. well, except 126x64 ATEX files.
Probably they use custom format for NPOT texture.

* **Some times after the finish re-index .dat file, some of catalog is missing**
Restart Gw2Browser will fix this issue.

Libraries and restrictions
--------------------------

The application is written specifically for MSVC10+, as it links with DirectX 9
and gw2DatTools. It also uses some C++11 features available in said compiler,
since gw2DatTools won't compile without those features anyway.

### Required libraries

* [DirectX SDK June 2010](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
* [gw2DatTools](https://github.com/kytulendu/gw2DatTools)
* [gw2formats](https://github.com/kytulendu/gw2formats)
* [wxWidgets 3.0.1](http://wxwidgets.org/)
* [OpenGL Mathematics](http://glm.g-truc.net/) *Included*
* [libwebp](https://code.google.com/p/webp/) *Included*

### Optional libraries

* [Visual Leak Detector](http://vld.codeplex.com/)

If you want to use Visual Leak Detector, remove the "//" at #include <vld.h>
in Gw2Browser.cpp.

How to compile
--------------

Windows:
* Compile libwebp using Visual Studio Native Tools Command Prompt, both 32 and 64 bit
by use these command at extern\libweb directory.

`nmake /f Makefile.vc CFG=debug-static RTLIBCFG=dynamic OBJDIR=obj`

`nmake /f Makefile.vc CFG=release-static RTLIBCFG=dynamic OBJDIR=obj`

* Compile wxWidgets.
* Compile Gw2Browser.

Authors
-------

* [Khral Steelforge](https://github.com/kytulendu)

### Thanks to:

* [Rhoot](https://github.com/rhoot) : Original Gw2Browser
* David Dantowitz : Simple CRC
* Demonsangel : Noesis script fmt_GW2_pf.py
* [hackedd](https://github.com/hackedd) : Idea for fix extraction of uncompressed files larger than 65532 bytes.
* [Loumie](https://github.com/ahom) : gw2DatTools
* [Open Icon Library](http://openiconlibrary.sourceforge.net/) : Icons used in this software
* [Till034](https://github.com/Till034) : OnCompareItems function overload
* [ral](https://sites.google.com/site/jumptovictory/) : Information of files in Gw2.dat
* [Xtridence](http://forum.xentax.com/viewtopic.php?p=72880#p72880) : modified GWUnpacker to work with Gw2.dat

### Special thanks to:

* ArenaNet/NCSoft and the team that make Guild Wars 2!

Contributing
------------

For contributions, please go to [this project's page on GitHub](https://github.com/kytulendu/Gw2Browser).

Legal Disclaimer
----------------

Guild Wars 2 © ArenaNet, LLC. and NC Interactive, LLC. All rights reserved.
Guild Wars, Guild Wars 2, ArenaNet, NCSOFT, the Interlocking NC Logo,
and all associated logos and designs are trademarks or registered trademarks of NCSOFT Corporation.
All other trademarks are the property of their respective owners.

Code License Exceptions
-----------------------

### OpenGL Mathematics (GLM)

-----------------------------------------------------------------------------

extern/glm/*

Copyright (c) 2005 - 2014 G-Truc Creation (www.g-truc.net)

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
