This tutorial requires that you have read [Create a new application](Create_a_new_application.md).

Step 1: Set up the environment
------------------------------

Please read the tutorial [Compiling the SDK for Android](Compiling_the_SDK_for_Android.md) to know how to set up the environment to build for Android.

Step 2: Generate the Makefile
-----------------------------

Minko's SDK uses premake5, which is embed in the SDK, for its build system. The Native Development Kit for Android uses Makefiles, so we can use the default Cygwin GNU make.

To do this, open a command line prompt in the root directory of the SDK and run: `script/solution_gmake.sh`

Step 3: Make sure JDK is installed
----------------------------------

Java files need to be compiled to deploy a Minko Android application, so make sure that you have a `${JAVA_HOME}` environment variable that is pointing to the JDK root folder.

If not, you can download the JDK [here](http://www.oracle.com/technetwork/java/javase/downloads/jdk8-downloads-2133151.html) (*Java SE Development Kit 8u101*) and create the `${JAVA_HOME}` variable as said earlier.

Please also make sure that the JDK's `bin` folder is in your `${PATH}` because we need to use the `jarsigner` binary.


Step 4: Build and run the project
---------------------------------

Now, you should have a Makefile at the root directory of your project. You can build the solution using the following command line:


```bash
$ make config=android_release
```


If a device is connected, this will automatically deploy the APK on it.
