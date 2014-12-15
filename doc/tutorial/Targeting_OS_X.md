This tutorial requires that your have read [Create a new application](../tutorial/Create_a_new_application.md). We'll suppose your application is named `my-project`.

Step 1: Installing the toolchain
--------------------------------

OS X doesn't come with a C++ compiler, we'll have to install it by ourselves. Minko requires a C++11-compliant compiler and therefore supports two compilers on OS X:

-   GCC 4.8+
-   Clang 3.2+ (recommended)

### Install Xcode Command Line Tools

Even though Minko doesn't support Xcode projects at the moment, we need to install [Xcode Command Line Tools](https://developer.apple.com/downloads/index.action), which is a fairly up-to-date toolchain for building native applications. *Note that you don't need the Xcode IDE.*

### Install recent compilers (optional)

The Unix-based toolchain provided by the Command Line Tools includes Clang 3.2, which is enough to build Minko, we strongly recommend to install a more recent compiler. The most straightforward way to do it is to use a package manager ([Homebrew](http://brew.sh/), [MacPort](http://www.macports.org/)). With MacPort installed, simply run:

```bash
# Clang sudo port install clang-3.4 sudo port select --set clang mp-clang-3.4

# GCC sudo port install gcc48 sudo port select --set gcc mp-gcc48 
```


Step 2: Generate the solution
-----------------------------

While Xcode project support is part of the short-term roadmap (it's already available for [iOS](../tutorial/Targetting_iOS.md)), only \Makefile\s are supported at the moment. A script is provided to generate a GNU Make-compatible solution with default options:

```bash
script/solution_gmake_gcc.sh 
```


Or, if we prefer Clang":

```bash
script/solution_gmake_clang.sh 
```


If we want to customise our solution, we can call `premake` directly. For instance, when selecting the compiler, the `cc` option is passed (supported values are `gcc` and `clang`):

```bash
${MINKO_HOME}/tool/mac/script/premake5.sh --cc=clang gmake 
```


To learn more about premake commands, run:

```bash
${MINKO_HOME}/tool/mac/script/premake5.sh help 
```


Note that by default a simple binary will be built. No .app is generated. We can create a simple .app by switch the application `kind` to `WindowedApp` in the `premake5.lua` file of the project:

```lua
include "script"

PROJECT_NAME = path.getname(os.getcwd())

minko.project.solution(PROJECT_NAME)

   minko.project.application(PROJECT_NAME)

       kind "WindowedApp"
       -- rest of the project file...

```


Step 3: Build the solution
--------------------------

For a while now, Macs have only been 64-bit stations, so Minko does not support any old generation Mac (PPC or 32-bit Intel). To target native command line applications, run:

```bash
make config=osx64_release 
```


or

```bash
make config=osx64_debug 
```


We can get more information about the building process by setting the `verbose` variable:

```bash
make config=osx64_release verbose=1 
```


Step 4: Run the application
---------------------------

Let's run the application. Open a terminal in the application directory and type:

```bash
cd bin/osx64/release ./my-project 
```


That should open a rendering window with your application running inside.

Step 5: Clean the solution (optional)
-------------------------------------

To clean the build, run:

```bash
make config=osx64_release clean 
```


This will basically remove any target file (`bin` and `obj` folders).

If you also want to erase generated solution files (`Makefile`s), you can use a stronger command which will erase any ignored file (files matched by a pattern in `.gitignore`:

```bash
script/clean.sh 
```


Step 6: Support more targets (optional)
---------------------------------------

Your application should now target OS X in one click. You can also [turn your native application into an HTML5 one](../tutorial/Targeting_HTML5.md)!

