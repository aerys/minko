Minko (for HTML5)
=================

Getting started
---------------
### Windows / OS X / Linux
* **You need an internet connection**
* Install [Vagrant](http://www.vagrantup.com/)
* Open a command prompt
* `cd <minko-cpp>`
* `vagrant up`
* `vagrant ssh`
* `tools/lin/bin/premake4 --platform=emscripten gmake`
* `make verbose=1 config=release`
* Open `examples/bin/release/minko-examples.html` in Chrome or Firefox
