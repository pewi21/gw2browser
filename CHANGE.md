Change log
----------

Gw2Browser_2016-xx-xx
---------------------
Change:
- Use wxAUI for interface manager.
- New about box.
- Sound Bank and Eula file support.
- Improve string file support, add support for Chinese language string.
- Wavefront .obj model export improvement.
No need to rotate the exported model file in Blender any more.
It can be import using default blender Wavefront .obj import settings.

Fix:
- More stability, less crash.

Gw2Browser_2016-01-01
---------------------
Change:
- Model viewer now use OpenGL for rendering the model, require OpenGL 3.3 support.
- Faster extraction of uncompressed files.
- Wavefront .obj model export improvement.
No need to rotate the exported model file in Blender, may need to
rotate it to face the camera in Blender tho.
- Improved model viewer's wireframe view.
- Khral is now really confused, since he have taken 25 stack of confusion
during the development of OpenGL model viewer. (really, he's not good at math)
- Change string file reader csv export delimiter from comma (,) to semicolon (;).
since comma some times use in the strings.
- Categorized string files by language.
- Merged Packed Ogg and MP3 reader.
- Merge onTreeExtractConverted and onTreeExtractRaw to onTreeExtractFile.
- Model file now have it's own icon.

Fix:
- Fix some texture not in it's sub category.
- Fix Wavefront .obj formating if the model not have uv.
- Eliminate z-fighting (hopefully).

Gw2Browser_2014-10-06
---------------------
Change:
- String and Sound files support. Strings can be export to CSV,
If open the CSV file in Notepad++, go to Encoding -> Convert to ANSI
and go to Encoding again -> Encode in UTF-8 for the text to display correctly.
- Ogg, Configuration and ARAP file detection.
- Add icon for model files.
- Some change in category name.
- Set Gw2Browser to center of screen.

Fix:
- Fix memory leak on viewing WebP files.
- Fix extraction of uncompressed files larger than 65532 bytes.
- Fix MP3 files doesn't have icon.

Issue:
- Some textures not go to it's sub category. *Fixed in Gw2Browser_2016-01-01*

Gw2Browser_2014-04-24
---------------------
Change:
- Add texture id info used with the model in model viewer, texture file need to be extract manually.

Gw2Browser_2014-03-25
---------------------
Change:
- Hack for read 126x64 ATEX files.
- Model Reader now use high-res texture, still need to figure out ModelReader to not load LOD models.
- WebP support, can export to png. WebP can open by Chrome browser.
- Bink 2 video detect, to play it you need The RAD Video Tools
but the player will play only 1 second of video and it will crash.
*Fixed in Gw2Browser_2014-10-06*
- ID3 (MP3), Embedded OpenType font, TextPack Variant, Map content and Shader Cache files detection.
- Simple Wireframe view.
- Force enable 4x MSAA.

Removed:
- ABFF file support.

Fix:
- Fix crash when viewing Luminance DDS file.
- Fix camera not look at center of model.

Issue:
- Memory leak on viewing WebP files. *Fixed in Gw2Browser_2014-10-06*

Gw2Browser_2014-01-30
---------------------
Change:
- Files will sort as ascending order
- JPEG files support.
- When export as raw, file extension will add automatically, example, export DDS file(s) as raw,
the file(s) will have .dds as extension or export string file(s), the file(s) will have extension .strs.
- Change hex viewer font to easy to read font, display only printable ASCII character.
- Use icons from Open Icon Library.
- Re-catalog files.
- Replace XNA Math with OpenGL Mathematics, doesn't finish yet.

Issue:
- ModelViewer camera not look at center of model. *Fixed in Gw2Browser_2014-03-25*
- Some times after the program finish re-index .dat file, some of catalog may missing,
simply restart Gw2Browser will fix this.

Gw2Browser_2014-01-07
---------------------
Change:
- 64 bit build.
