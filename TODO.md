TODO list
=========

May change over time. :P

Further plans
-------------

* In ModelReader make new function material( ) that return Material vector
for use by range-base loop.

* Add way to position light (shift left click?) by using gobal variable or access class variable

* In ModelViewer, Optimize texture loader for faster loading, load DXT texture data directly, if possible?

* Exporters class, like Reader/Viewer.

* Material class (texture, lighting).

* Deferred shading (need for map viewer with many light)?

* Frame Buffer.

* SSAO?

* Self shadowed model shading?

* Texture2D class parameters.

* Scene manager (need when implement map viewer).

* Improve performance of ModelViewer's drawText function.

* COLLADA model export, use libxml2 for write xml, also export skeleton and animation.

* "Find by fileId" dialog.

* Allow individual file types to add entries to the context menu. For example,
models could get an *export with textures* option.

* External file name database, for known files (such as the exe and dll files).

* Support for paged image files.

* Support for R32f DDS files.

* Support NPOT textures.

* Support map files, map renderer.

* Support for model LODs, collision mesh rendering, bone, animation and more.

* Support for text manifest and text voice files.

* Linux and Mac OS X port.

* Support Guild Wars (1) files.
