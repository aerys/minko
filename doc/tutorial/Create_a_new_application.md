In this tutorial, we will **learn how to create an application that will link with the Minko framework and all the necessary plugins in just minutes**. At the end of the tutorial, you'll be able to create a complete solution file for your favorite IDE and then start working on your project.

**This tutorials works for Windows, OS X or Linux**. The overall procedure is pretty much the same for all operating systems. The small differences will always be detailed: make sure you read the instructions carefully for your OS.

**Do not get scared by the relative length of this tutorial**: we only want to make sure that everything is detailed and cover all use cases for all platforms. In the end its a 10-second process with only 2 steps:

-   copy the application "skeleton" directory
-   generate the solution by running a script

It's really fast and simple. But if it's the first time you're doing it, please take 5 minutes to read this tutorial carefully.

Step 1: Create the application directory
----------------------------------------

The SDK provides a directory you can simply copy/paste wherever you want to bootstrap your application. This directory is called "skeleton" and it's located in the root folder of the SDK (`$MINKO_HOME/skeleton`). Just copy paste this folder wherever you want and rename it to have the name of your project.

For instance, if you want your project to be called "Minko Hello World", copy/paste the `$MINKO_HOME/skeleton` directory wherever you want on your file system and then rename it "Minko Hello World". The name of the folder will be used as the name of the solution/project.

Step 2: Enable plugins (optional)
---------------------------------

It's more than likely that your application will actually require some Minko plugins to work properly. To do this, simply open the `premake5.lua` file located in your application folder and uncomment the line for the plugin you want to use. For example, if you want to use a JPEG image file parser, uncomment the following line:

```lua
minko.plugin.enable("jpeg") 
```


If the plugin you want to use is not listed (see [Plugin compatibility matrix](../article/Plugin_compatibility_matrix.md) ), just add it yourself:

```lua
minko.plugin.enable("my-plugin") 
```


where "my-plugin" is a registered plugin of the SDK (typically listed in the `plugins` directory of the SDK). This will add the plugin to the project solution and link it properly with the application.

**The SDL plugin is enabled by default** because it is required to build the default application. Most of the examples in the SDK use the SDL Minko plugin which requires the SDL2 library. Yet, the framework itself, and all of its plugins (except the SDL one of course) should compile just fine without SDL. You are free to use your own context/window manager library, but we recommend to use SDL as it works out of the box on most platforms and allows you to write 100% cross-platform code. If you want to use your own context/window manager, disable it by commenting out the corresponding line.

You can read more about enabling plugins in the [How to enable a plugin](../tutorial/How_to_enable_a_plugin.md) tutorial.

Step 3: Target your platform
----------------------------

The basic procedure here is to generate the solution for your project and your environment. We use a nice tool named Premake to do this job, and we've embedded it nicely in the SDK.

In your application root, you will find a `script` directory containing scripts to call the generator. Typically:

-   `solution_vs2013.bat` to generate a Visual Studio-compatible solution
-   `solution_gmake_gcc.sh` to generate a GNU Make-compatible solution

To learn more about this step, take some time to read the tutorials for each platform:

-   [Targeting Windows](../tutorial/Targeting_Windows.md)
-   [Targeting OS X](../tutorial/Targeting_OS_X.md)
-   [Targeting Linux](../tutorial/Targeting_Linux.md)
-   [Targeting iOS](../tutorial/Targeting_iOS.md)
-   Targeting Android (coming with the beta 2...)
-   [Targeting HTML5](../tutorial/Targeting_HTML5.md)
-   Targeting Flash (coming when Adobe [updates its compiler](https://github.com/adobe-flash/crossbridge/issues/28)...)

This will teach you how to build and run your application. Now, you can start coding in your favorite environment.

Step 4: Go further! (optional)
------------------------------

Check out our application examples. It's a great way to discover and learn the cool new features we've implemented!

