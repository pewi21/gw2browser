TODO list
=========

May change over time. :P

Gw2Browser_201x-xx-xx (1.0.9.1)
-------------------------------

* Add XML exporter for some files format.

* Add a way to config model viewer graphic option.

* Use wxDataView or wxListCtrl with combo box for folder and file list.

* Since lod model use low-res texture, load only hi-res mesh?
figure out how gw2 do it.

* content reader and viewer.

Further plans
-------------

* cmake build system.

* map file reader, renderer, exporter.
see Tyria-3D code for this.

* Scene manager (need when implement map viewer).

* COLLADA model export, use libxml2 for write xml, also export skeleton and animation.

* Deferred shading (need for map viewer with many light)

* Improve performance of ModelViewer's drawText function by using texture atlas.

* SSAO?

* Self shadowed model shading?

* Allow individual file types to add entries to the context menu. For example,
models could get an *export with textures* option.

* Allow to extract raw data with raw texture.

* External file name database, for known files (such as the exe and dll files).

* Support for paged image files.

* Support for R32f DDS files.

* Support NPOT textures.

* Support for model LODs, collision mesh rendering, bone, animation and more.

* Support for text manifest and text voice files.

* Support Guild Wars (1) files.
