TODO list
=========

* Add a icon for Gw2Browser.

* When close filelist or log window, uncheck the checker menu.

* Seperate ModelViewer function to smaller files.

* Optimize model viewer's loadMeshes function.

* Lighting, normal maping, etc.

* Fix crash when ModelReader read some model that have material in other version.
(cause by out of range, file in difference version?)

* Improve performance of ModelViewer's drawText function.

* Use C++11 std::thread for multi-threading in Task.

* COLLADA, Wavefront .obj model export with materials.

* Some restructuring. Decouple views from their data, for instance. Clean up 
headers, etc.

* "Find by fileId" dialog.

* Allow individual file types to add entries to the context menu. For example,
models could get an *export with textures* option.

* String viewer, sound player.

* Support for model LODs, collision mesh rendering, bone, animation and more.

* Support for text manifest and text voice files.

* Support for paged image files.

* External file name database, for known files (such as the exe and dll files).

* Support for R32f DDS files.

* Support NPOT textures.

* Support map files, map renderer.

* Support Guild Wars (1) files.

* Mac OS X and Linux port
