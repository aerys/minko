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
* `tools/mac/bin/premake5.sh gmake` (`--cc=gcc` or `--cc=clang`)
* `make config=release config=osx64_release`
* `cd examples/cube && bin/osx64/release/minko-example-cube`

### Linux
* Make sure you have a recent compiler (tested with GCC 4.8 and Clang 3.3)
* Open a command prompt (or start [Vagrant](http://www.vagrantup.com/))
* `cd <minko>`
* `tools/lin/bin/premake5.sh gmake` (`--cc=gcc` or `--cc=clang`)
* `make config=release config=linux64_release`
* `cd examples/cube && bin/linux64/release/minko-example-cube`

### Windows
* Open a command prompt
* `cd <minko>`
* `tools\win\scripts\premake5 vs2013ctp`
* `start minko.sln`

Or from the explorer:

* Run `tools\win\scripts\premake_vs2013ctp.bat`
* Open `minko.sln`


Contents
========
Framework
---------

Sources for the Minko framework.

Examples
--------

Example applications created with the Minko framework and its plugins.

Tools
-----

Set of scripts to ease the deployment of the Minko SDK without using the command line.

`premake_vs2013ctp.bat` - Windows batch script to create a VS2010 solution and perform other necessary operations such as copying required DLL into the output folders

`premake_clean.bat` - Windows batch script to properly clean a VS2010 solution

Templates
---------

This folder contains templates you can use to ease the development of the Minko framework, plugins
or applications.

* `vs` - Visual Studio templates; can be installed by:
	* running the `tools/premake_templates.bat` script
	* or executing `premake5 installTemplates` command line
	* or by manually copying each template directory into your VS templates directory (default is `$user\Documents\Visual Studio 2012\Templates\ItemTemplates\Projet Visual C++`)

Plugins
-------

Sources for Minko's plugins.

Available plugins are:
* jpeg
* png
* webgl: please follow `webgl/README.md` to learn how to deploy

Resources
---------
* [minko.io](http://minko.io/ "Website")

Contribute
----------
`minko` is MIT-licensed. Please contribute!
