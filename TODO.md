TODO list
=========

May change over time. :P

Gw2Browser_2017-xx-xx (1.0.9.1)
-------------------------------

* Mesh and Material class (texture, lighting).

* Texture2D class parameters.

* In ModelViewer, Optimize texture loader for faster loading, load DXT texture data directly, if possible?

* Exporters class, like Reader/Viewer.

* "Find by fileId" dialog.

Further plans
-------------

* COLLADA model export, use libxml2 for write xml, also export skeleton and animation.

* Add way to position light (shift left click?) by using gobal variable or access class variable

* Deferred shading (need for map viewer with many light)

* Frame Buffer.

* SSAO?

* Self shadowed model shading?

* Scene manager (need when implement map viewer).

* Improve performance of ModelViewer's drawText function.

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
