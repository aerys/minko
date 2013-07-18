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
* `tools/lin/bin/premake4 --platform=emscripten gmake`
* `make config=release verbose=1`
* `exit`
* `start examples\bin\release\minko-examples.html`

### OS X

* `cd /vagrant`
* `tools/mac/bin/premake4 --platform=emscripten gmake`
* `make config=release verbose=1`
* `open examples/bin/release/minko-examples.html`

### Linux

* `cd /vagrant`
* `tools/lin/bin/premake4 --platform=emscripten gmake`
* `make config=release verbose=1`
* `firefox examples/bin/release/minko-examples.html`
