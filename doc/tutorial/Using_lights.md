You can add 4 different type of lights in your scene for this, open the right click menu on the scene tree where you want to add your light, and go in the `Add Light` submenu.

Ambient Light
=============

An ambient light will affect all objects in the scene and light them equally on all surface.

-   `Color`: changes the light color
-   `Ambient`: changes the intensity of the light. All materials can accentuate this intensity with the `Ambient Multiply` parameter.
-   `Emission`: sets the emission mask for the light. Any material with a layer enabled in its `reception` mask will be affected by the light if this same layer is enabled in the `emission` mask

 

-   Ambient = 0

![](../../doc/image/Ambient0.jpg "../../doc/image/Ambient0.jpg")

-   Ambient = 0.5

![](../../doc/image/Ambient0.5.jpg "../../doc/image/Ambient0.5.jpg")

-   Ambient = 1

![](../../doc/image/Ambient1.jpg "../../doc/image/Ambient1.jpg")

A scene with a single `ambient light` with white color and `ambient` set to 1 will look just like it would in `Basic` rendering mode.

Directional Light
=================

Directional lights, as their name suggest, light the nodes from your scene uniformly, in a defined direction.

Transform parameters
--------------------

In the `transform` section, you can see two parameters: `position` and `target position`. The direction of the light will go from the `position` point to the `target position`.

On the scene, the arrow will indicate the direction in which your light points. The position will have no influence on the way the scene is lighted, it will only change how shadows are cast. See the `shadow` section below for more information.

Light parameters
----------------

-   `Color`: changes the light color.
-   `Diffuse`: changes the intensity of the light. All materials can accentuate this intensity with the `Diffuse Multiplier` parameter.
-   `Shininess`: this will affect the spread of the specular spot. A bigger value will result in a smaller spot.
-   `Specular`: this will affect the intensity and radius of the light specular spot. A bigger value will result in a bigger size and intensity.

 

-   Diffuse = 0.1

![](../../doc/image/Dirdiffuse0.1.jpg "../../doc/image/Dirdiffuse0.1.jpg")

-   Diffuse = 0.3

![](../../doc/image/Dirdiffuse0.3.jpg "../../doc/image/Dirdiffuse0.3.jpg")

-   Diffuse = 0.6

![](../../doc/image/Dirdiffuse0.6.jpg "../../doc/image/Dirdiffuse0.6.jpg")

-   Shininess = 1024

![](../../doc/image/Dirshininess1024.jpg "../../doc/image/Dirshininess1024.jpg")

-   Shininess = 128

![](../../doc/image/Dirshininess128.jpg "../../doc/image/Dirshininess128.jpg")

-   Shininess = 40

![](../../doc/image/Dirshininess40.jpg "../../doc/image/Dirshininess40.jpg")

-   Specular = 0

![](../../doc/image/Dirspecular0.jpg "../../doc/image/Dirspecular0.jpg")

-   Specular = 0.3

![](../../doc/image/Dirspecular0.3.jpg "../../doc/image/Dirspecular0.3.jpg")

-   Specular = 0.8

![](../../doc/image/Dirspecular0.8.jpg "../../doc/image/Dirspecular0.8.jpg")

-   Specular = 2

![](../../doc/image/Dirspecular2.jpg "../../doc/image/Dirspecular2.jpg")

Shadows
-------

### Common Parameters

-   `Map Size`: size of the map used to draw shadows to. A bigger size will result in better, cleaner shadows, but a smaller size will be better for performance.
-   `Quality`: changes the way shadows are computed. Higher quality could impact on the performances.
-   `Map width`: width on which the shadows will be computed. Will be represented on screen by the light frustum. The position of this frustum will depend on the light `position`.
-   `Z-far`: maximum distance on which the shadows will be produced. Will be represented on screen by the light frustum.

 

-   Small `map width`: the shadows of the cube and torus are cut.

![](../../doc/image/Dirshadowsmallfrustum.jpg "../../doc/image/Dirshadowsmallfrustum.jpg")

-   Larger `map width`: the shadows of the cube and torus are fully visible.

![](../../doc/image/Dirshadowlargerfrustum.jpg "../../doc/image/Dirshadowlargerfrustum.jpg")

-   `Map size` = `64` `Quality` = Hard

![](../../doc/image/Dirshadowhard64.jpg "../../doc/image/Dirshadowhard64.jpg")

-   `Map size` = `512` `Quality` = Medium

![](../../doc/image/Dirshadowmedium512.jpg "../../doc/image/Dirshadowmedium512.jpg")

### Shadow mapping types

Minko provides two different shadow types.

-   `ESM`: exponential shadow mapping.
-   `PCF `: percentage-closer shadow mapping.

#### ESM

The ESM filters the shadow map by approximating the shadow test with an exponential function.

-   `Exponential factor`: a bigger value will give a better approximation

`ESM` works better with smaller `Z-far` and `Map width`.

-   `Exponential factor` = `1.5`, the two cube shadows have a slightly different color

![](../../doc/image/Esmexp1.5.jpg "../../doc/image/Esmexp1.5.jpg")

-   `Exponential factor` = `10`, the two cube shadows have the same color and blend in an uniform shadow

![](../../doc/image/Esmexp10.jpg "../../doc/image/Esmexp10.jpg")

#### PCF

`PCF` will filter the shadow map by calculating how far from the light the shadow is, producing sharper edges closer to the light and smoother ones further from the light.

-   `Bias`
-   `Spread`: how much the shadow will spread when it is far from the light

 

-   `Spread` = `0`, the shadow edges are sharp.

![](../../doc/image/Pcfspread.jpg "../../doc/image/Pcfspread.jpg")

-   `Spread` = `0`, the shadow edges are smoothed.

![](../../doc/image/Pcfspread15.jpg "../../doc/image/Pcfspread15.jpg")

Spot Light
==========

The light from a `spot light` originates from a point and is diffused in a cone.

Transform properties
--------------------

-   `Position`: position from which the `spot light` will emit.
-   `Target Position`: point to which the cone of the `spot light` will point.

Light properties
----------------

-   `Color</color>: changes the light color.
-   `Attenuation Distance`: distance at which the light intensity will fade out.
-   `Inner radius`, `Outer radius`: the `outer radius` gives the radius of the cone, the light will be more intense inside of the `inner radius` and will fade from the `inner radius` to the `outer radius`.
-   `Diffuse`: changes the intensity of the light. All materials can accentuate this intensity with the `Diffuse Multiplier` parameter.
-   `Shininess`: this will affect the spread of the specular spot. A bigger value will result in a smaller spot.
-   `Specular`: this will affect the intensity and radius of the light specular spot. A bigger value will result in a bigger size and intensity.

For `Shininess` and `Specular` examples, see above in the `Directional Light` section.

-   `Diffuse` = `0.5`

![](../../doc/image/Spotdif0.5.jpg "../../doc/image/Spotdif0.5.jpg")

-   `Diffuse` = `1`

![](../../doc/image/Spotdif1.jpg "../../doc/image/Spotdif1.jpg")

-   `Diffuse` = `5`

![](../../doc/image/Spotdif5.jpg "../../doc/image/Spotdif5.jpg")

-   Small `outer radius` and `inner radius`

![](../../doc/image/Spotsmallradius.jpg "../../doc/image/Spotsmallradius.jpg")

-   Bigger `outer radius`

![](../../doc/image/Spotmediumradius.jpg "../../doc/image/Spotmediumradius.jpg")

-   Bigger `outer radius` and `inner radius` close to the `outer radius`

![](../../doc/image/Spotbigradius.jpg "../../doc/image/Spotbigradius.jpg")

-   Small `Attenuation distance`

![](../../doc/image/Spotsmallattenuation.jpg "../../doc/image/Spotsmallattenuation.jpg")

-   Medium `Attenuation distance`

![](../../doc/image/Spotmediumattenuation.jpg "../../doc/image/Spotmediumattenuation.jpg")

-   Larger `Attenuation distance`

![](../../doc/image/Spotbigattenuation.jpg "../../doc/image/Spotbigattenuation.jpg")

Point Light
===========

Point lights send light in all directions.

Transform properties
--------------------

-   `Position`: position of the light source.

Light properties
----------------

-   `Color`: changes the light color.
-   `Attenuation Distance`: distance at which the light intensity will fade out.
-   `Diffuse`: changes the intensity of the light. All materials can accentuate this intensity with the `Diffuse Multiplier` parameter.
-   `Shininess`: this will affect the spread of the specular spot. A bigger value will result in a smaller spot.
-   `Specular`: this will affect the intensity and radius of the light specular spot. A bigger value will result in a bigger size and intensity.

For `Shininess` and `Specular` examples, see above in the `Directional Light` section.

-   `Diffuse` = `0.2`

![](../../doc/image/Point_diffuse_0.2.jpg "../../doc/image/Point_diffuse_0.2.jpg")

-   `Diffuse` = `0.5`

![](../../doc/image/Pointdiffuse0.5.jpg "../../doc/image/Pointdiffuse0.5.jpg")

-   `Diffuse` = `1`

![](../../doc/image/Pointdiffuse1.jpg "../../doc/image/Pointdiffuse1.jpg")

-   `Diffuse` = `2`

![](../../doc/image/Pointdiffuse2.jpg "../../doc/image/Pointdiffuse2.jpg")

-   `Attenuation distance` = `5`

![](../../doc/image/Pointattenuation5.jpg "../../doc/image/Pointattenuation5.jpg")

-   `Attenuation distance` = `10`

![](../../doc/image/Pointattenuation10.jpg "../../doc/image/Pointattenuation10.jpg")

-   `Attenuation distance` = `18`

![](../../doc/image/Pointattenuation18.jpg "../../doc/image/Pointattenuation18.jpg")

