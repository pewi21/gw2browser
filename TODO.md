TODO list
=========

May change over time. :P

Gw2Browser_2017-xx-xx (1.0.9.1)
-------------------------------

* Re-add AA that work with frame buffer.

* Seperate sound decoder class.

* map file reader, renderer, exporter.

* Add way to position light (shift left click?) by using gobal variable or access class variable?

* Support for paged image files.

* Material class (texture, lighting).

* Exporters class, like Reader/Viewer.

* Add a way to config model viewer graphic option.

Further plans
-------------

* COLLADA model export, use libxml2 for write xml, also export skeleton and animation.

* Deferred shading (need for map viewer with many light)

* SSAO?

* Self shadowed model shading?

* Scene manager (need when implement map viewer).

* Improve performance of ModelViewer's drawText function by using texture atlas.

* Allow individual file types to add entries to the context menu. For example,
models could get an *export with textures* option.

* External file name database, for known files (such as the exe and dll files).

* Support for R32f DDS files.

* Support NPOT textures.

* Support for model LODs, collision mesh rendering, bone, animation and more.

* Support for text manifest and text voice files.

* Linux port.

* Support Guild Wars (1) files.
