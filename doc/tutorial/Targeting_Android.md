This tutorial requires that you have read [Create a new application](Create_a_new_application.md).

Step 1: Set up the environment
------------------------------

Please read the tutorial [Compiling the SDK for Android](Compiling the SDK for Android) to know how to set up the environment to build for Android.

Step 2: Generate the Makefile
-----------------------------

Minko's SDK uses premake5, which is embed in the SDK, for its build system. The Native Development Kit for Android uses Makefiles, so we can use the default Cygwin GNU make.

To do this, open a command line prompt in the root directory of the SDK and run: `script/solution_gmake_android.sh`

Step 3: Build and run the project
---------------------------------

Now, you should have a Makefile at the root directory of your project. You can build the solution using the following command line:


```bash
$ make config=android_release 
```


If a device is connected, this will automatically deploy the APK on it.

