Change log
----------

Gw2Browser_2014-01-30
---------------------
Change:
- Fix crash when viewing Luminance DDS file.
- Fix camera not look at center of model.
- Hack fix for reading 126x64 ATEX files.
- Model Reader now use high-res texture, still need to figure out ModelReader to not load LOD models.
- WebP support, can export to png. WebP can open by Chrome browser.
- Bink 2 video detect, to play it you need The RAD Video Tools
but the player will play only 1 second of video and it will crash,
the reason why it crash is unknown. (maybe they use custom Bink 2 format?)
- ID3 (MP3), Embedded OpenType font, TextPack Variant, Map content and Shader Cache files detection.
- Simple Wireframe view.
- Force enable 4x MSAA.

Removed:
- ABFF file support.

Issue:
- Memory leak on viewing WebP files.

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
- ModelViewer camera not look at center of model.
- Some times after the program finish re-index .dat file, some of catalog may missing,
simply restart Gw2Browser will fix this.

Gw2Browser_2014-01-07
---------------------
Change:
- 64 bit build.
