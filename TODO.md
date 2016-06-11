TODO list
=========

May change over time. :P

1.0.8.1
-------

* Model Viewer z-buffer visualizer, etc. (for debugging/visualization)

* Seperate ModelViewer function to smaller files.

* Add way to position light (shift left click?)

* Render light source. (for debugging/visualization)

* Add a button to reload shader.

Further plans
-------------

* In ModelReader make new function material( ) that return Material vector
for use by range-base loop.

* Fix crash when ModelReader read some model that have material in other version.
(cause by out of range or file is in difference version?)

* SSAO

* Self shadowed model shading?

* Draw each model (need when implement map viewer)

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
