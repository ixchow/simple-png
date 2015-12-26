#Simple PNG
A set of utilities to manipulate PNG images.
Created for use in [Rainbow](http://tchow.com/games/rainbow)'s build scripts, where ImageMagick was being too clever with colorspace transformations.

These utilities are specifically not clever, and pretty much assume linear color and alpha data (not to mention discard all fancy info chunks in your png files).
