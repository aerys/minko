![logo](https://dl.dropbox.com/s/68w979idplf8j21/256.png)

Minko
=====

3D. Everywhere.

Minko is a technology to develop and design rich, interactive and universal 3D applications targeting computers, web browsers and mobile platforms. 

It provides a powerful editor on top of a robust, extensible and fully documented open source core framework, and is created and maintained by [Aerys](http://aerys.in/).

The repository hosts the source code of ***Minko 3***. Looking for Minko 2? We've moved the [repository](http://github.com/aerys/minko-as3).

Get started
===========

For more detailed instructions, please follow our [Getting started with Minko 3 beta 1](http://doc.v3.minko.io/wiki/Getting_started_with_Minko_3_beta_1) tutorial.

### Windows
* Set the `MINKO_HOME` environment variable to the root of the SDK.
* Open the explorer in `tool\win\script`
* Run `solution_vs2013.bat`
* Open `minko.sln` at the root of the SDK

### OS X
* Make sure you have a recent compiler (tested with GCC 4.8 and Clang 3.3)
* Set the `MINKO_HOME` environment variable to the root of the SDK
* Open a command prompt
* `cd <minko>`
* `tool/mac/script/solution_gmake_gcc.sh` (or `solution_gmake_clang.sh`)
* `make config=release config=osx64_release`
* `cd example/cube/bin/osx64/release && ./minko-example-cube`

### Linux
* Make sure you have a recent compiler (tested with GCC 4.8 and Clang 3.3)
* Set the `MINKO_HOME` environment variable to the root of the SDK
* Open a command prompt (or start [Vagrant](http://www.vagrantup.com/))
* `cd <minko>`
* `tool/lin/script/solution_gmake_gcc.sh` (or `solution_gmake_clang.sh`)
* `make config=release config=linux64_release`
* `cd example/cube/bin/linux64/release && ./minko-example-cube`


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

`solution_vs2013.bat` - Windows batch script to create a VS2013 solution and perform other necessary operations such as copying required DLL into the output folders

`solution_vs2013ctp.bat` - Windows batch script to create a VS2013 solution (with the Nov. 2013 CTP compiler) and perform other necessary operations such as copying required DLL into the output folders

`solution_gmake.bat` - Windows batch script to create a GNU Make solution (supporting HTML5 compilation)

`clean.bat` - Windows batch script to properly clean the generated solutions and builds

`doc.bat` - Windows batch script to generate the developer reference

`dist.bat` - Windows batch script to make a redistributable Minko SDK

`install_emscripten.bat` - Windows batch script to install Emscripten (needed by the HTML5 target)

You will find similar tools for Linux and OS X.

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
* [doc.v3.minko.io](http://doc.v3.minko.io/ "Documentation")

Contribute
----------
`minko` is MIT-licensed. Please contribute!
