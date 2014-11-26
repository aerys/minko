This tutorial requires that your have read [Create a new application](../tutorial/Create_a_new_application.md). We'll suppose your application is named `my-project`.

Step 1: Installing the toolchain
--------------------------------

Linux usually doesn't come with a C++ compiler, we'll have to install it by ourselves. Minko requires a C++11-compliant compiler and therefore supports two compilers on Linux:

-   GCC 4.8+
-   Clang 3.4+ (recommended)

We'll document the procedure for Ubuntu. Please refer to your Linux distribution documentation to install these packages for other flavors.

You can install either GCC:

```bash
sudo apt-get install gcc-4.8 g++-4.8 sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50 sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50 
```


Or Clang:

```bash
sudo apt-get install clang-3.4 
```


For older (< 13.10) versions of Ubuntu, you may have to add some PPAs first:

```bash
sudo apt-get install python-software-properties sudo add-apt-repository ppa:ubuntu-toolchain-r/test # GCC 4.8 wget -O - <http://llvm.org/apt/llvm-snapshot.gpg.key> | sudo apt-key add - # Clang sudo apt-get update 
```


Optionally, you can install `gcc-multilib` to allow cross-compilation between 32 and 64-bit Linux:

```bash
sudo apt-get install gcc-multilib g++-multilib 
```


Step 2: Install libraries
-------------------------

No library is necessary to run a basic Minko application. However, most applications will use plugins, which may require external libraries. Under Linux, the following libraries are used by included plugins:

-   SDL 2
-   OpenGL
-   USB communication
-   GTK+
-   Multiscreen support
-   probably a proprietary driver for your graphics card (refer to you graphics chip vendor)

Install them:

```bash
# Install the PPA for libsdl2 sudo apt-get install software-properties-common sudo add-apt-repository ppa:zoogie/sdl2-snapshots sudo apt-get update

# SDL2 sudo apt-get install libgl1-mesa-dev libglu1-mesa-dev libglew-dev libsdl2-dev

# HTTP (optional) sudo apt-get install libcurl4-openssl-dev

# Oculus (optional) sudo apt-get install libudev-dev libxinerama-dev

# HTML overlay (optional) sudo apt-get install libgtk2.0-dev libgtkglext1-dev libudevpath=$([ \uname -m\ = x86_64 ] && echo "/lib/x86_64-linux-gnu" || echo "/lib/i386-linux-gnu") sudo test -e $libudevpath/libudev.so.0 || sudo ln -s $libudevpath/libudev.so.1 $libudevpath/libudev.so.0 
```


If you need to do offscreen rendering (available through the `offscreen` plugin), you should also install another bunch of libraries:

```bash
sudo apt-get install libosmesa6-dev 
```


Step 3: Generate the solution
-----------------------------

On Linux, only `Makefile`s are supported. A script is provided to generate a GNU Make-compatible solution with default options:

```bash
script/solution_gmake_gcc.sh 
```


Or, if we prefer Clang:

```bash
script/solution_gmake_clang.sh 
```


If we want to customise our solution, we can call `premake` directly. For instance, when selecting the compiler, the `cc` option is passed (supported values are `gcc` and `clang`):

```bash
${MINKO_HOME}/tool/lin/script/premake5.sh --cc=clang gmake 
```


To learn more about premake commands, run:

```bash
${MINKO_HOME}/tool/lin/script/premake5.sh help 
```


Step 4: Build the solution
--------------------------

To target native command line applications, run:

```bash
make config=linux32_release 
```


There are 4 supported Linux configuration:

-   `linux32_release`
-   `linux32_debug`
-   `linux64_release`
-   `linux64_debug`

As expected, they allow you to select optimized or debuggable binaries, along with the target architecture (cross-compiling might require additional packages, see [MultiArch](https://help.ubuntu.com/community/MultiArch)).

We can get more information about the building process by setting the `verbose` variable:

```bash
make config=linux32_release verbose=1 
```


To leverage multi-core systems, you can also use `make -j`. The following example will use 4 cores and will compile much faster as a result:

```bash
make -j4 config=linux32_release verbose=1 
```


Step 5: Run one of the examples
-------------------------------

Let's run the application under a 32-bit Linux. Open a terminal in the application directory and type:

```bash
cd bin/linux32/release ./my-project 
```


That should open a rendering window with your application running inside.

Step 6: Clean the solution (optional)
-------------------------------------

To clean the build, pass the `clean` target to `make`. For instance:

```bash
make config=linux32_release clean 
```


This will basically remove any target file (`bin` and `obj` folders) for the specific configuration.

If you also want to erase generated solution files (`Makefile`), you can use a stronger command which will erase any ignored file (files matched by a pattern in `.gitignore`):

```bash
script/clean.sh 
```


Step 7: Support more targets (optional)
---------------------------------------

Your application should now target OS X in one click. You can also [turn your native application into an HTML5 one](../tutorial/Targeting_HTML5.md)!

