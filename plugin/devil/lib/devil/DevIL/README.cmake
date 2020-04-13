Build DevIL with CMake
----------------------

GENERIC
-------

For Linux/Unix/OSX/mingw, using makefiles:

    $ cd devil/DevIL
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ make install

To generate project files for visual studio/xcode/whatever, you
need to tell CMake which generator to use. For example:


See here for list of generators:
http://www.cmake.org/cmake/help/v3.3/manual/cmake-generators.7.html


Debug/Release/etc:

    $ cmake -DCMAKE_BUILD_TYPE=Debug ..


WINDOWS - Visual studio
-----------------------

You can get going without any commandline fiddling. Just install cmake
(from http://cmake.org), and then:

1) Run cmake-gui.

2) Set the "where is the source code:" entry to the dir containing the top-level
CMakeLists.txt, which should be DevIL/DevIL.

3) Set a build dir with the "where to build the binaries:" entry. Generally, this
will be a new directory outside of the DevIL tree (to keep the build all clean
and out-of-tree).

4) click the "configure" button. It should let you specify the generator to use.
eg:
  "Visual Studio 14 2015" (for a 32-buit build)
  "Visual Studio 14 2015 w64" (for a 64-buit build)
It'll spend a little time generating the project files and then you're set to go.

5) In visual studio, navigate to the build directory and open up the new .sln file.

6) build!
Configurations for both Debug and Release (and others) are included in the project.


WINDOWS - msys2
---------------


Run mingw32 or mingw64 shell (for 32bit or 64bit respectively), then follow
the generic instructions, except for:

  $ cmake -G "MSYS Makefiles" ..

To set install prefix (windows default seems to be C:/program files/....):
  $ cmake -G "MSYS Makefiles" -DCMAKE_INSTALL_PREFIX=/usr/local ..




TODO:
- give some hints on installing prerequisites, eg:
    - an apt-get install line for ubuntu
    - brew packages to install for OSX
    - instructions for Msys2 on windows
    - steps for setting up libs for visual studio




