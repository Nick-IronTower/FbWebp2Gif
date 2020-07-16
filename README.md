# FbWebp2Gif
 Converts animated webp images to gif.
 Based on https://github.com/init-01/webp2gif, but rewritten for Windows/Visual Studio builds.
 Has more safe checks
 
## Initial differences
 - Fixed resulting GIFs looping only once;
 - Added (optional) basic logging for troubleshooting;
 - More safe checks - no more crashing on short or invalid file paths.

## Installation
 Just download, open solution and build the binary type you need - executable (EXE), dynamic library (DLL) or static library (LIB).
 x64 only at the moment, though it won't be hard to add x86 support.
 
## Usage
 1) Executable accepts 1 argument - full path to webp image you want to convert. Converted gif image is created at the same location.
 2) DLL or LIB - ConvertWebp2Gif function is what you need.

## Credits, dependencies
 - Original conversion to C++ by https://github.com/init-01/, long days and pleasant nights to this person.
 - Gif-h header by https://github.com/charlietangora/
 - libwepb by https://developers.google.com/speed/webp/