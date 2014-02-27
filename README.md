![logo](https://dl.dropbox.com/s/68w979idplf8j21/256.png)

Minko
=====

3D. Everywhere.

Minko is a technology to develop and design rich, interactive and universal 3D applications targeting computers, web browsers and mobile platforms. 

It provides a powerful editor on top of a robust, extensible and fully documented open source core framework, and is created and maintained by [Aerys](http://aerys.in/).

Get started
===========
### OS X
* Make sure you have a recent compiler (tested with GCC 4.8 and Clang 3.3)
* Open a command prompt
* `cd <minko>`
* `tool/mac/bin/premake5.sh gmake` (`--cc=gcc` or `--cc=clang`)
* `make config=release config=osx64_release`
* `cd example/cube/bin/osx64/release && ./minko-example-cube`

### Linux
* Make sure you have a recent compiler (tested with GCC 4.8 and Clang 3.3)
* Open a command prompt (or start [Vagrant](http://www.vagrantup.com/))
* `cd <minko>`
* `tool/lin/bin/premake5.sh gmake` (`--cc=gcc` or `--cc=clang`)
* `make config=release config=linux64_release`
* `cd example/cube/bin/linux64/release && ./minko-example-cube`

### Windows
* Open a command prompt
* `cd <minko>`
* `tool\win\bin\premake5.exe --no-test vs2013`
* `start minko.sln`

Or from the explorer:

* Run `tool\win\script\premake_vs2013.bat`
* Open `minko.sln`


Contents
========
Framework
---------

Sources for the Minko framework.

Example
-------

Example applications created with the Minko framework and its plugins.

Tool
----

Set of scripts to ease the deployment of the Minko SDK without using the command line.

`premake_vs2013.bat` - Windows batch script to create a VS2013 solution and perform other necessary operations such as copying required DLL into the output folders

`premake_vs2013ctp.bat` - Windows batch script to create a VS2013 solution (with the Nov. 2013 CTP compiler) and perform other necessary operations such as copying required DLL into the output folders

`premake_gmake.bat` - Windows batch script to create a GNU Make solution (supporting HTML5 compilation)

`premake_clean.bat` - Windows batch script to properly clean the generated solutions and builds

Template
--------

This folder contains templates you can use to ease the development of the Minko framework, plugins
or applications.

Plugin
------

Sources for Minko's plugins.

Available plugins are:
* angle
* assimp
* bullet
* devil
* fx
* http-loader
* http-worker
* jpeg
* leap
* lua
* oculus
* offscreen
* particles
* png
* sdl
* serializer
* webgl: please follow `webgl/README.md` to learn how to deploy

Resources
---------
* [minko.io](http://minko.io/ "Website")
* [doc.minko.io](http://doc.minko.io/ "Documentation")

Contribute
----------
`minko` is MIT-licensed. Please contribute!
