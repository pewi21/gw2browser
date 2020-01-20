Gw2Browser
==========

Opens a Guild Wars 2 .dat file and allows the user to browse and extract its
files. Currently it supports power-of-two textures, JPEG and WebP images, models,
126x64 ATEX textures, sound, sound bank, bitmap font, eula, strings, text and binary files.

When a .dat file is opened, it will first index the file. This makes it *a lot*
faster to re-open the same .dat. Unfortunately, every time the .dat changes it
will have to be re-indexed (for now).

Keep in mind that if you keep tree items expanded while the indexer is running,
or while an index is being read, the process is *a lot* slower.

Also, when you first use find by file id function, the browser will appear to freeze.
This is normal since it will need to expand all file tree in oder to search for
given file id.

The modelviewer required GPU with OpenGL 3.3 support.

The latest binary can always be found [Here](https://github.com/kytulendu/gw2browser/releases)

For Linux binary (Ubuntu 18.04), use this command to get required library.

    sudo apt install libwebp libglew2.0 libopenal libmpg123 libvorbis libogg libfreetype6

For other Linux distribution, you have to compile it your self.

For instructions to compile Gw2Browser, see [COMPILE.md](../master/COMPILE.md) for detail.

Usage
-----

It can be started with:

    gw2browser <input dat>

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

* **Most sound data in bank files and text data in string files are unsupported.**
As the data are encrypted, each entry/files have difference encryption keys that received from the game server.

Authors
-------

* [Khralkatorrix](https://github.com/kytulendu)

### Thanks to:

* [Rhoot](https://github.com/rhoot) : Original Gw2Browser and gw2formats
* [Antoine Hom](https://github.com/ahom) : gw2DatTools
* BoyC : For AFNT reader code.
* David Dantowitz : Simple CRC
* Demonsangel : Noesis script fmt_GW2_pf.py
* [DubyaDubyaDubyaDot](https://stackoverflow.com/questions/13437422/libvorbis-audio-decode-from-memory-in-c) : How to read Oggs from memory using custom ogg callbacks.
* [hackedd](https://github.com/hackedd) : Idea for fix extraction of uncompressed files larger than 65532 bytes.
* [Inigo Quilez](http://www.iquilezles.org/www/articles/normals/normals.htm) : clever normalization of mesh
* [Joey de Vries](http://learnopengl.com) and [opengl-tutorial.org](http://www.opengl-tutorial.org) : Great OpenGL 3.3 tutorial
* [John Hable](http://filmicworlds.com/blog/filmic-tonemapping-operators/) : Uncharted 2 tone maping function
* [Liberation Sans Regular](https://fedorahosted.org/liberation-fonts/) : Font used by the model viewer
* [Open Icon Library](http://openiconlibrary.sourceforge.net/) : Icons used in this software
* [ral](https://sites.google.com/site/jumptovictory/) : Information of files in Gw2.dat
* Rengyr : Hide GLCanvas while loading index file or scanning dat file patch.
* [Romain Dura](https://mouaif.wordpress.com/2009/01/28/levels-control-shader/) : Levels control shader
* [Till034](https://github.com/Till034) : OnCompareItems function overload
* [Xtridence](http://forum.xentax.com/viewtopic.php?p=72880#p72880) : modified GWUnpacker to work with Gw2.dat

### Special thanks to:

* ArenaNet/NCSoft and the team that make Guild Wars 2!

Contributing
------------

For contributions, please go to [this project's page on GitHub](https://github.com/kytulendu/gw2browser).

Legal Disclaimer
----------------

Guild Wars 2 © 2010-2019 ArenaNet, LLC. All rights reserved.
Guild Wars, Guild Wars 2, Heart of Thorns, Guild Wars 2: Path of Fire,
ArenaNet, NCSOFT, the Interlocking NC Logo, and all associated logos
and designs are trademarks or registered trademarks of NCSOFT Corporation.
All other trademarks are the property of their respective owners.

License
-------

    Copyright © 2014-2019 Khralkatorrix <https://github.com/kytulendu>
    Copyright © 2019 BoyC <https://twitter.com/BoyCcns>
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
