![logo](https://dl.dropbox.com/s/68w979idplf8j21/256.png)

Minko
=====

3D. Everywhere.

Minko is a technology to develop and design rich, interactive and universal 3D applications targeting computers, web browsers and mobile platforms. 

It provides a powerful editor on top of a robust, extensible and fully documented open source core framework, and is created and maintained by [Aerys](http://aerys.in/).

Get started
===========
Linux
-----
* `premake4 gmake` or `premake4 --platform=clang gmake`
* `make config=release`
* `cd examples && bin/release/minko-example`

OS X
----
* `premake4 gmake` or `premake4 --platform=clang gmake`
* `make config=release`
* `cd examples && bin/release/minko-example`

Windows
-------
* `premake4 vs2012`
* `start minko.sln`

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

`premake_vs2010.bat` - Windows batch script to create a VS2010 solution and perform other necessary operations such as copying required DLL into the output folders

`premake_clean.bat` - Windows batch script to properly clean a VS2010 solution created by premake_vs2010.bat

Templates
---------

This folder contains templates you can use to ease the development of the Minko framework, plugins
or applications.

* `vs` - Visual Studio templates; can be installed by:
	* running the `tools/premake_templates.bat` script
	* or executing `premake4 installTemplates` command line
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
