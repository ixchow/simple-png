#Simple PNG
A set of utilities to manipulate PNG images.
Created for use in [Rainbow](http://tchow.com/games/rainbow)'s build scripts, where ImageMagick was being too clever with colorspace transformations.

These utilities are specifically not clever, and pretty much assume linear color and alpha data (not to mention discard all fancy info chunks in your png files).

#Building

Get ft-jam, then type `jam` in this directory. Utilities will be built into the `dist/` subdirectory.

Or, if you feel like it, just roll your own build process. There's not that many files here and they only need libpng.

#Using

Utilities should all provide a usage blurb when run without arguments.
The image origin is considered to be in the upper left.

Many utilities support special image types when loading.
These special images start with some characters an then a colon.

`blank:WxH` a blank image (all channels zero) of WxH pixels.

`flip:file.png` load and then vertically flip file.png.

`:file.png` just load file.png -- useful if you have some colons in your filenames.
