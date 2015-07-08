# Lomoco
Lomoco was created by Alexios Chouchoulas, Andreas Schneider &lt;andreas.schneider@linux-gamers.net>, and  Peter Feuerer &lt;peter.feuerer@linux-gamers.net>
It was designed to allow Logitech Mice to work on linux.

I am trying to revive it to do the following:

1. Disable the DPI increase/decrese buttons on the Logitech g400s (hopefully other logitech mice will be supported)
2. Manually set the DPI of a logitech mouse.
3. Free up the mouse buttons so that all of them can be used.

# Building

You'll need:

  * Build system (compiler, make, etc), either Autotools ([Autoconf](https://www.gnu.org/software/autoconf/)/[Automake](https://www.gnu.org/software/automake/))
    or [CMake](http://www.cmake.org/)
  * [libusb](http://www.libusb.org/)

## Building with Autotools

1. ./autogen.sh
2. make

## Building with CMake

1. Create build dir: `mkdir build && cd build`
2. Configure: `cmake ../` (You can use other generators too like Ninja)
3. Compile: `make`


