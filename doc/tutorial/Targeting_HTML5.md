Step 1: Installing Emscripten
-----------------------------

[Emscripten](https://github.com/kripken/emscripten/) is an LLVM-to-JavaScript compiler. It takes LLVM bitcode - which can be generated from C/C++, using llvm-gcc or clang, or any other language that can be converted into LLVM - and compiles that into JavaScript, which can be run on the web (or anywhere else JavaScript can run).

There are multiple approach to install Emscripten, all listed on the [SDK](https://github.com/kripken/emscripten/wiki/Emscripten-SDK) page of the project. We have made it easy for all platforms.

### Windows

-   Install the full package of the [Emscripten](http://kripken.github.io/emscripten-site/docs/getting_started/downloads.html#windows Emscripten SDK 1.22)
-   Double-click `tool\win\script\install_emscripten.bat`



-   Run **Emscripten Command Prompt** (available in your applications)
-   Type `emsdk install mingw-4.6.2-32bit`
-   Type `emsdk activate mingw-4.6.2-32bit`
-   Type `emsdk install java-7.45-32bit`
-   Type `emsdk activate java-7.45-32bit`
-   Make sure you don't have any `sh.exe` in your `PATH` (msysgit for instance)



### OS X

-   Run `tool/mac/script/install_emscripten.sh` (this will install the [SDK).](https://github.com/kripken/emscripten/wiki/Emscripten-SDK#wiki-downloads Emscripten SDK])

### Linux

-   Run `tool/lin/script/install_emscripten.sh` (this will install Emscripten from the source automatically, tested on Ubuntu 14.04 only)

 The Emscripten SDK installer is not yet compatible with Linux, so you will have to install the components manually. Depending on the platform you're on, the procedure differs, but the components are the same:

-   Clang 3.2+
-   Node.js 0.8+
-   Python 2.7+
-   Emscripten 1.13+

The procedure for Ubuntu 12.10 is detailled [here](https://github.com/kripken/emscripten/wiki/Getting-Started-on-Ubuntu-12.10), and should be fairly similar for other Linux flavors.

Under Ubuntu 13.04+, the procedure is easier:

```bash
sudo apt-get install clang-3.2 sudo apt-get install nodejs export EMSCRIPTEN=/opt/emscripten sudo mkdir -m 777 ${EMSCRIPTEN} git clone <https://github.com/kripken/emscripten> ${EMSCRIPTEN} cd ${EMSCRIPTEN} && git checkout 1.13.0 # Above versions are broken. echo "EMSCRIPTEN=${EMSCRIPTEN}"->> ~/.profile 
```


```bash
sudo apt-get update sudo apt-get install -y python-software-properties python g++ make sudo add-apt-repository ppa:chris-lea/node.js sudo apt-get update 
```


Then you need to **install the latest Emscripten compiler backend based on LLVM aka "fastcomp"**. Just follow the instructions available on this [github account](https://github.com/kripken/emscripten/wiki/LLVM-Backend#getting-fastcomp Getting Fastcomp page of the Emscripten wiki). 

Note: Currently, Minko supports **Emscripten 1.25.0**.

Step 2: Building an application
-------------------------------

Building an HTML5 version of your application requires to open a terminal emulator and use a Makefile. Emscripten, in its current version, is unable to complete a real debug build, therefore **you should never use `html5_debug`** until this is fixed (probably in the coming weeks). Also, unless your computer has an enormous amount of memory, you should not use parallel builds with Emscripten.

### Linux

```bash
script/solution_gmake_gcc.sh make config=html5_release 
```


### OS X

```bash
script/solution_gmake_gcc.sh make config=html5_release 
```


### Windows

On Windows, you will need to have a few programs provided by the Emscripten SDK in your path. We've embed a script, which will set up the Emscripten environment and run the build with the above commands.

```bash
script\build_html5.bat 
```


Or simply double-click on the script.

Step 3: Running an application
------------------------------

Go to `bin/html5/release`, and open the generated HTML page with [Firefox](http://www.mozilla.org/en-US/firefox/new/). You should see the same result as your native application.

In some cases, you will need to access the page through a web server running on your computer. Use your favorite ([Apache](http://httpd.apache.org/), [nginx](http://wiki.nginx.org/Main), [pow](http://pow.cx/), [IIS](http://www.iis.net/)) and set up your document root to point to `bin/html5/release`. You can now reach your application though you local domain as seamlessly as you would on the Internet.

At this point, you should be interested in debugging your application. Have a look at [Debugging HTML5 applications](../tutorial/Debugging_HTML5_applications.md).

