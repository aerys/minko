Minko (for HTML5)
=================

Getting started
---------------
### 2 ways
### Windows / OS X / Linux
* **You need an internet connection**
* Install [Vagrant](http://www.vagrantup.com/)
* Open a command prompt
* `cd <minko>`
* `vagrant up` (this may take up to 10 minutes)
* `vagrant ssh`
* `tool/lin/script/premake5.sh gmake`
* `make verbose=1 config=html5_release`
* Open `examples/cube/bin/html5/release/minko-example-cube.html` in Chrome or Firefox

You can also install the Emscripten SDK on your machine. In that case, you should look at the [documentation](http://doc.minko.io/wiki/v3:Targeting_HTML5).
