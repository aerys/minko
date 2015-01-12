Platforms Compatibility for Minko 3 BETA 1
==========================================

The platforms compatibility matrix provides the current state of support for each platform officially targeted by the Minko Framework. you can consult the [plugins compatibility matrix](../article/Plugin_compatibility_matrix.md) to find out more about the plugins support for each platform.

Platforms officially targeted
-----------------------------

|    Premake configuration name    |      Available     | 
|---------------------------------------------------------------------------------------------------------|------------------------------------------------------|
| ![](../../doc/image/Html5_min.png "fig:../../doc/image/Html5_min.png") HTML5                 | ![](../../doc/image/Checked.png "../../doc/image/Checked.png") | html5                            |
| ![](../../doc/image/Winmini.png "fig:../../doc/image/Winmini.png") Windows                   | ![](../../doc/image/Checked.png "../../doc/image/Checked.png") | windows32<br/>windows64        |
| ![](../../doc/image/Mac_min.png "fig:../../doc/image/Mac_min.png") OS X                      | ![](../../doc/image/Checked.png "../../doc/image/Checked.png") | osx64                            |
| ![](../../doc/image/Linux_min.png "fig:../../doc/image/Linux_min.png") Linux                 | ![](../../doc/image/Checked.png "../../doc/image/Checked.png") | linux32<br/>linux64            |
| ![](../../doc/image/Androidmini.png "fig:../../doc/image/Androidmini.png") Android           | ![](../../doc/image/Help_16.png "../../doc/image/Help_16.png") | N/A (expected for the beta 2...) |
| ![](../../doc/image/Iso7mini.png "fig:../../doc/image/Iso7mini.png") iOS                     | ![](../../doc/image/Checked.png "../../doc/image/Checked.png") | ios                              |
| ![](../../doc/image/Flashmini.png "fig:../../doc/image/Flashmini.png") Flash                 | ![](../../doc/image/Help_16.png "../../doc/image/Help_16.png") | N/A                              |
| ![](../../doc/image/Windows_phone.png "fig:../../doc/image/Windows_phone.png") Windows Phone | ![](../../doc/image/Help_16.png "../../doc/image/Help_16.png") | N/A                              |

Target compatibility matrices
-----------------------------

For each platform, support may vary depending of the host version, the possible states for each host are :

-   ![](../../doc/image/Checked.png "fig:../../doc/image/Checked.png"): Fully supported
-   ![](../../doc/image/Warning.png "fig:../../doc/image/Warning.png"): Partially supported
-   ![](../../doc/image/Error.png "fig:../../doc/image/Error.png"): Not supported
-   ![](../../doc/image/Help_16.png "fig:../../doc/image/Help_16.png"): Not supported yet

### ![](../../doc/image/Html5_min.png "fig:../../doc/image/Html5_min.png") HTML5

Minko for HTML5 relies only on WebGL, the HTML5 standard API to create 3D web applications. For more details about WebGL availability, please consult the corresponding [Can I use...](http://caniuse.com/#search=webgl ) page).

| Host        | Support                                                                                                                                                     |
|-------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Chrome 20+  | ![](../../doc/image/Checked.png "../../doc/image/Checked.png")                                                                                              |
| Firefox 20+ | ![](../../doc/image/Checked.png "../../doc/image/Checked.png")                                                                                              |
| IE 6-10     | ![](../../doc/image/Error.png "fig:../../doc/image/Error.png") Not supported as those versions of Internet explorer do NOT support the WebGL fonctionality. |
| IE 11       | ![](../../doc/image/Warning.png "fig:../../doc/image/Warning.png") Some functionalities might not be fully supported                                        |
| Opera 18+   | ![](../../doc/image/Checked.png "../../doc/image/Checked.png")                                                                                              |

### ![](../../doc/image/Winmini.png "fig:../../doc/image/Winmini.png") Microsoft Windows

| Host          | Support                                                                                                                                                                                        |
|---------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Windows XP    | ![](../../doc/image/Error.png "fig:../../doc/image/Error.png") Windows XP cannot be targeted as the Visual Studio 2013 C++ compiler have ended support for this particular version of Windows. |
| Windows Vista | ![](../../doc/image/Checked.png "../../doc/image/Checked.png")                                                                                                                                 |
| Windows Seven | ![](../../doc/image/Checked.png "../../doc/image/Checked.png")                                                                                                                                 |
| Windows 8+    | ![](../../doc/image/Checked.png "../../doc/image/Checked.png")                                                                                                                                 |

### ![](../../doc/image/Mac_min.png "fig:../../doc/image/Mac_min.png") Apple OS X

| Host                 | Support                                                                                                                                                                                            |
|----------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 10.6 "Snow Leopard"  | ![](../../doc/image/Error.png "fig:../../doc/image/Error.png") Even if targeting MacOS 10.6 is technically feasible with some hacking, this version is no more supported by Apple Xcode toolchain. |
| 10.7 "Lion"          | ![](../../doc/image/Checked.png "../../doc/image/Checked.png")                                                                                                                                     |
| 10.8 "Mountain Lion" | ![](../../doc/image/Checked.png "../../doc/image/Checked.png")                                                                                                                                     |
| 10.9 "Mavericks"     | ![](../../doc/image/Checked.png "../../doc/image/Checked.png")                                                                                                                                     |

### ![](../../doc/image/Linux_min.png "fig:../../doc/image/Linux_min.png") GNU/Linux

| Host                       | Support                                                        |
|----------------------------|----------------------------------------------------------------|
| X86 or X86_64 Kernel 2.6+ | ![](../../doc/image/Checked.png "../../doc/image/Checked.png") |
| ARM - kernel 2.6+          | ![](../../doc/image/Checked.png "../../doc/image/Checked.png") |

### ![](../../doc/image/Androidmini.png "fig:../../doc/image/Androidmini.png") Android

| Host         | Support                                                                                                         |
|--------------|-----------------------------------------------------------------------------------------------------------------|
| Android 2.5+ | ![](../../doc/image/Help_16.png "fig:../../doc/image/Help_16.png") We're working on it ! Expected for beta 2... |

### ![](../../doc/image/Iso7mini.png "fig:../../doc/image/Iso7mini.png") Apple iOS

| Host     | Support                                                        |
|----------|----------------------------------------------------------------|
| iOS 4.0+ | ![](../../doc/image/Checked.png "../../doc/image/Checked.png") |

### ![](../../doc/image/Flashmini.png "fig:../../doc/image/Flashmini.png") Adobe Flash

| Host        | Support                                                                                 |
|-------------|-----------------------------------------------------------------------------------------|
| Flash 11.2+ | ![](../../doc/image/Help_16.png "fig:../../doc/image/Help_16.png") We're working on it! |

### ![](../../doc/image/Windows_phone.png "fig:../../doc/image/Windows_phone.png") Windows Phone

| Host               | Support                                                                                 |
|--------------------|-----------------------------------------------------------------------------------------|
| Windows Phone 8.1+ | ![](../../doc/image/Help_16.png "fig:../../doc/image/Help_16.png") We're working on it! |


