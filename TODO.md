TODO list
=========

May change over time. :P

1.0.8.1
-------

* Seperate ModelViewer function to smaller files.

* Add way to position light (shift left click?)

* Render light source. (for debugging/visualization)

* Re-write camera class?

Further plans
-------------

* In ModelReader make new function material( ) that return Material vector
for use by range-base loop.

* Exporters class, like Reader/Viewer.

* Fix crash when ModelReader read some model.
Related to material in gw2formats, possibly library bug.

Example, error std::invalid_argument at model file 905578.

Possible work around:
 - Don't read material on that files, build exclude file lists.
 - Have option to export model without material/texture.

* Then fix the model exporter, automatically map the materials and model.
(have to disable it, for now)

* Add a button to reload shader.

* Frame Buffer

* SSAO?

* Self shadowed model shading?

* Scene manager (need when implement map viewer)

* Also display current file id when have error on reading files.

* Improve performance of ModelViewer's drawText function.

* Use C++11 std::thread for multi-threading in Task.

* COLLADA model export, use libxml2 for write xml, also export skeleton and animation.

* Some restructuring. Decouple views from their data, for instance. Clean up 
headers, etc.

* "Find by fileId" dialog.

* Allow individual file types to add entries to the context menu. For example,
models could get an *export with textures* option.

* String viewer, sound player.

* External file name database, for known files (such as the exe and dll files).

* Support for paged image files.

* Support for R32f DDS files.

* Support NPOT textures.

* Support map files, map renderer.

* Support for model LODs, collision mesh rendering, bone, animation and more.

* Support for text manifest and text voice files.

* Linux and Mac OS X port.

* Support Guild Wars (1) files.
