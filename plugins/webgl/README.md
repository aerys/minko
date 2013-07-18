Minko (for HTML5)
=================

Getting started
---------------
### Windows

* Install [Vagrant](http://www.vagrantup.com/)
* Open a command prompt
* `vagrant up`
* `vagrant ssh`
* `cd /vagrant`
* `tools\win\bin\premake4 --platform=emscripten gmake`
* `make config=release`
* `start examples\bin\release\minko-examples.html`

### OS X

* `tools/mac/bin/premake4 --platform=emscripten gmake`
* `make config=release`
* `open examples/bin/release/minko-examples.html`

### Linux

* `tools/lin/bin/premake4 --platform=emscripten gmake`
* `make config=release`
* `firefox examples/bin/release/minko-examples.html`
