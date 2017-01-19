![logo](http://i.stack.imgur.com/gLloD.png)

Minko
=====

[![Join the chat at https://gitter.im/aerys/minko](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/aerys/minko?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

3D. Everywhere.

Minko is a technology to develop and design rich, interactive and universal 3D applications targeting computers, web browsers and mobile platforms.

It provides a powerful editor on top of a robust, extensible and fully documented open source core framework, and is created and maintained by [Aerys](http://aerys.in/).

The repository hosts the source code of ***Minko 3***. Looking for Minko 2? We've moved the [repository](http://github.com/aerys/minko-as3).


Resources
---------
* [Official website](https://minko.io/ "Website")
* [Documentation](doc "Documentation")
* [Community](https://minko.io/community/ "Community")

Get started
===========

For more detailed instructions, please follow our [Getting started with Minko 3](doc/tutorial/Getting_started_with_Minko_3.md) tutorial.

### Windows
* Set the `MINKO_HOME` environment variable to the root of the SDK.
* Open the explorer in `script`
* Run `solution_vs2015_full.bat`
* Open `minko.sln` at the root of the SDK

### OS X
* Make sure you have a recent compiler (tested with GCC 4.8 and Clang 3.3)
* Set the `MINKO_HOME` environment variable to the root of the SDK
* Open a command prompt
* `cd <minko>`
* `script/solution_gmake_full.sh`
* `make config=release config=osx64_release`
* `cd example/cube/bin/osx64/release && ./minko-example-cube`

### Linux
* Make sure you have a recent compiler (tested with GCC 4.9 and Clang 3.4)
* Set the `MINKO_HOME` environment variable to the root of the SDK
* Open a terminal
* `cd <minko>`
* `tool/lin/script/solution_gmake_gcc.sh` (or `solution_gmake_clang.sh`)
* `make config=release config=linux64_release`
* `cd example/cube/bin/linux64/release && ./minko-example-cube`

### iOS / Android / HTML5
* [Getting started with Minko 3](doc/tutorial/Getting_started_with_Minko_3.md)

Contents
========
Framework
---------

Sources for the Minko framework.

Example
-------

Example applications created with the Minko framework and its plugins.

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
* debug
* devil
* fx
* html-overlay
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
* sensors
* serializer
* video-camera
* vr
* zlib

Contribute
----------
`minko` is MIT-licensed. Please contribute!
