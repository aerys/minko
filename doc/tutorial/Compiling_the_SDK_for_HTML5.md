This tutorial will guide through the few steps to compile the Minko SDK targeting HTML5.

Step 1: Get the sources
-----------------------

Make sure you have the source code of Minko on your filesystem. You can get them from our repository: [Installing the Minko SDK sources](Installing_the_SDK_sources.md#step-1-install-a-git-client).

Step 2: Installing Emscripten
-----------------------------

[Emscripten](https://github.com/kripken/emscripten/) is an LLVM-to-JavaScript compiler. It takes LLVM bitcode - which can be generated from C/C++, using llvm-gcc or clang, or any other language that can be converted into LLVM - and compiles that into JavaScript, which can be run on the web (or anywhere else JavaScript can run).

There are multiple approach to install Emscripten, all listed on the [SDK](https://github.com/kripken/emscripten/wiki/Emscripten-SDK) page of the project. We have made it easy for all platforms.

### Windows

-   Install the full package of the [Emscripten](https://github.com/kripken/emscripten/wiki/Emscripten-SDK#wiki-downloads Emscripten SDK 1.16)
-   Double-click `tool\win\script\install_emscripten.bat`



-   Run **Emscripten Command Prompt** (available in your applications)
-   Type `emsdk install mingw-4.6.2-32bit`
-   Type `emsdk activate mingw-4.6.2-32bit`
-   Type `emsdk install java-7.45-32bit`
-   Type `emsdk activate java-7.45-32bit`
-   Make sure you don't have any `sh.exe` in your `PATH` (msysgit for instance)



### OS X

-   Run `tool/mac/script/install_emscripten.sh` (this will install the [<https://github.com/kripken/emscripten/wiki/Emscripten-SDK>#wiki-downloads Emscripten SDK])

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


Then you need to **install the latest Emscripten compiler backend based on LLVM aka "fastcomp"**. Just follow the instructions available on the [<https://github.com/kripken/emscripten/wiki/LLVM-Backend>#getting-fastcomp Getting Fastcomp page of the Emscripten wiki]. 

Note: Currently, Minko supports **Emscripten 1.16.0**.

Step 2: Building the SDK
------------------------

### Windows

To make things easy, just run the following batch script:

```bash
> %MINKO_HOME%/tool/win/script/build_html5.bat 
```


It will generate the required Makefiles and run Emscripten for you. By default, it will build the framework, the plugins, the examples and the tutorials. It might take some time, so feel free to customize this script to generate solution files that will exclude some projects (like the examples or the tutorials) that you don't need.

For example, to compile only the framework, the plugins and the examples, you can run the following script:

```bash
> %MINKO_HOME%/tool/win/script/build_html5_no_tutorial.bat 
```


### Linux / OSX

For all the command lines below, `$MINKO_BIN` is either:

-   `$MINKO_HOME/tool/lin/bin/` if you are using Linux;
-   or `$MINKO_HOME/tool/mac/bin/` if you are using OSX.

The first step is to create the required solution/project Makefiles:

```bash
$ $MINKO_BIN/premake5.sh gmake 
```


To learn more about the available options and how to disable the compilation of the examples or the tutorials to save some time, just read the help:

```bash
$ $MINKO_BIN/premake5.sh gmake --help 
```


If you just want to compile the SDK to link your own applications, you don't need to compile the examples or the tutorials. The following command line will create the solution files excluding the tutorials and the examples:

```bash
$ $MINKO_BIN/premake5.sh gmake --no-tutorial --no-example 
```


When you're done, you should have a bunch of Makefiles. You can build the solution using the following command line:

```bash
$ make config=html5_release 
```


If you want to leverage multicore processors, you can use the following command line (replace '4' by your actual number of cores):

```bash
$ make -j 4 config=html5_release 
```


It should speed up the compilation process significantly.

Step 5: Enjoy!
--------------

If you have compiled the examples/tutorials, you can try to launch them by opening the corresponding HTML file that will be located in the `bin/html5/release` directory of each project. **Make sure you open those files from an actual web server and not directly from your file system (do not use "<file:///>").**

You can now use your SDK to [Create a new application](../tutorial/Create_a_new_application.md).

