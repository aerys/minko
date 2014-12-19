Plugins are projects separate from Minko's SDK. They bring new features by extending the code framework classes or providing external assets such as `*.effect` files. Because plugins provide external C++ code and assets, they imply some modifications on how to compile and link your applications.

Hopefully, Minko's build system provide a very simple and easy way to not only create plugins but also link them with your application project.

Step 1: Update the solution configuration
-----------------------------------------

To make sure our plugin linkage and other required operations are automated by Minko's build system, we will have to update our project solution to enable the plugin we want. To do this, launch your favorite text editor and open the `premake5.lua` file located in the root folder of your application project.

The default solution file looks like this:

```lua
dofile(os.getenv("MINKO_HOME") .. "/sdk.lua")

PROJECT_NAME = path.getname(os.getcwd())

minko.project.solution(PROJECT_NAME)

   minko.project.application(PROJECT_NAME)

       language "c++"
       kind "ConsoleApp"

       files { "src/**.cpp", "src/**.hpp" }
       includedirs { "src" }

       -- plugins
       minko.plugin.enable("sdl")
       --minko.plugin.enable("bullet")
       --minko.plugin.enable("jpeg")
       --minko.plugin.enable("mk")
       --minko.plugin.enable("particles")
       --minko.plugin.enable("png")

```


As you can see, some plugins are referenced by default in the solution file. But they are commented. To enable a plugin, uncomment the corresponding line.

For example, to enable the "jpeg" plugin that provides a JPEG image files parser, uncomment the following line:

```lua
--minko.plugin.enable("jpeg") 
```


To uncomment a line in LUA, simply remove the "--" at the begining of the line. You should then have:

```lua
minko.plugin.enable("jpeg") 
```


If the plugin you want to enable is not in the list, you can call the `minko.plugin.enable()` function and pass the name of the plugin you need:

```lua
minko.plugin.enable("myplugin") 
```


The "name" of a plugin is the name of its folder in the `%MINKO_SDK%/plugins` directory.

You can, of course, enable as many plugins as you need.

Step 2: Generate the solution
-----------------------------

Now that our solution configuration is up to date, we have to re-generate the actual solution file for our IDE/build system. To do this, please refer to the [step 3 of the "Create a new application" tutorial](../tutorial/Create_a_new_application.md#Step_3:_Generate_the_solution_file).

Step 2 (alternative): Enable a plugin in the command line
---------------------------------------------------------

If a plugin is not mandatory but rather just "supported" by the application, you can also chose to enable it in the command line used to generate the solution/project file. It is done by passing an option for each plugin. Each plugin can be enabled using the `--with-${PLUGIN_NAME}` option (where `${PLUGIN_NAME}` has to be replaced with the actual name of the plugin).

For example, if we want to enabled the "jpeg" plugin, we will have to add the following option:

```
 --with-jpeg 
```


The command line itself is documented in the [step 3 of the "Create a new application" tutorial](../tutorial/Create_a_new_application.md#Step_3:_Generate_the_solution_file).

For example, the following Windows command line will enable the "jpeg" plugin when generating the Visual Studio 2013 solution file for our project:

```
 "%MINKO_HOME%"\tools\win\bin\premake5.exe --with-jpeg vs2013 
```


Step 3: Check that the plugin is enabled
----------------------------------------

Each plugin should define a custom dedicated C++ pre-processor macro that will help us enabling/disabling some of the plugin-related features in our code at compile time. Any plugin should define the corresponding `MINKO_PLUGIN_%{PLUGIN_NAME}` where `${PLUGIN_NAME}` should be replaced by the actual name of the plugin. For example, the "jpeg" plugin will define the `MINKO_PLUGIN_JPEG` macro.

In the following code, we will enable the JPEG image files parser only if the `MINKO_PLUGIN_JPEG` is defined (ie only if the corresponding plugin is actually enabled):

```cpp
#ifdef MINKO_PLUGIN_JPEG sceneManager->assets()->registerParser<JPEGParser>("jpg"); #endif 
```


