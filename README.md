Gw2Browser
==========

Opens a Guild Wars 2 .dat file and allows the user to browse and extract its
files. Currently it supports power-of-two textures, JPEG and WebP Images, models,
126x64 ATEX textures, sound, sound bank, eula, strings and binary files.

When a .dat file is opened, it will first index the file. This makes it *a lot*
faster to re-open the same .dat. Unfortunately, every time the .dat changes it
will have to be re-indexed (for now).

Keep in mind that if you keep tree items expanded while the indexer is running,
or while an index is being read, the process is *a lot* slower.

The latest binary can always be found [Here](http://goo.gl/4xMRVF)

For people who can't run Gw2Browser, download and Install
[Visual C++ Redistributable Packages for Visual Studio 2013](http://www.microsoft.com/en-us/download/details.aspx?id=40784)
choose vcredist_x64.exe for 64bit and vcredist_x86.exe for 32bit executable.

The modelviewer required GPU with OpenGL 3.3 support.

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
Decompressing one produces garbage blocks. Well, except 126x64 ATEX files.
Probably they use custom format for NPOT texture.

* **Some sound data in bank file(s) is unsupported.**
The data seems to be compressed or encrypted.

* **Gw2Browser not export some entry in string file(s).**
Some string entry are either empty or encrypted.

* **Some times after the finish re-index .dat file, some of catalog is missing**
Restart Gw2Browser will fix this issue.

* **The code architect is messy**
Need to work on it someday once I understand about code architect thing.

Libraries and restrictions
--------------------------

The application used some C++11 features, so it required C++11 capable
compiler in order to compile it. And if you use Visual Studio, since
Gw2Browser use ranged-base for loop in some area, you need Visual Studio 2012
or newer in order to compile it.

### Required libraries

* [gw2DatTools](https://github.com/kytulendu/gw2DatTools)
* [gw2formats](https://github.com/kytulendu/gw2formats)
* [wxWidgets 3.0.2](http://wxwidgets.org/)
* [OpenGL Mathematics](http://glm.g-truc.net/) *Included*
* [The OpenGL Extension Wrangler Library](http://glew.sourceforge.net/) *Included*
* [libwebp](https://developers.google.com/speed/webp/download) *Included*
* [FreeType](http://www.freetype.org/) *Included*

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
* [Antoine Hom](https://github.com/ahom) : gw2DatTools
* [Open Icon Library](http://openiconlibrary.sourceforge.net/) : Icons used in this software
* [Liberation Sans Regular](https://fedorahosted.org/liberation-fonts/) : Font used by the model viewer
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

Guild Wars 2 © 2010-2015 ArenaNet, LLC. All rights reserved.
Guild Wars, Guild Wars 2, Guild Wars 2: Heart of Thorns, ArenaNet, NCSOFT,
the Interlocking NC Logo, and all associated logos and designs are trademarks
or registered trademarks of NCSOFT Corporation.
All other trademarks are the property of their respective owners.

License
-------

Copyright © 2014-2016 Khral Steelforge <https://github.com/kytulendu>
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

Copyright (c) 2005 - 2015 G-Truc Creation

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

    Portions of this software are copyright (C) 2015 The FreeType
    Project (www.freetype.org).  All rights reserved.

See extern/freetype/docs/FTL.TXT for more details.
