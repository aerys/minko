The `Geometry` section of the `Properties` panel provides a set of operations you can perform on geometries.

Flip normals
============

This will invert all normals of your geometry. Normals will impact the light effects on a surface.

-   Default normals

![Default normals](../../doc/image/Flip_normals1.jpg "Default normals")

-   Flipped normals

![Flipped normals](../../doc/image/Flip_normals2.jpg "Flipped normals")

Flip Tangents
=============

This will invert the tangents of your geometry. Tangents are used with the normal maps of your materials.

-   Default tangents

![](../../doc/image/Flip_tangents1.jpg "../../doc/image/Flip_tangents1.jpg")

-   Flipped tangents

![](../../doc/image/Flip_tangents2.jpg "../../doc/image/Flip_tangents2.jpg")

Invert winding
==============

This will invert the winding of each triangle of the geometry.

-   Default winding

![](../../doc/image/Windings1.jpg "../../doc/image/Windings1.jpg")

-   Inverted winding

![](../../doc/image/Windings2.jpg "../../doc/image/Windings2.jpg")

Compute Normals
===============

This will automatically compute the normals of the geometry.

Compute Tangents
================

This will automatically compute the tangents of the geometry.

Apply Transform
===============

If you select a mesh node, you can apply its transform to the geometry it uses. It will duplicate the geometry and apply the mesh transform to it. The selected mesh will then use this new geometry and its transform will be set to identity.

-   Default cube geometry

![](../../doc/image/Applytransform1.jpg "../../doc/image/Applytransform1.jpg")

-   Cube geometry with the mesh tranform applied to it

![](../../doc/image/Applytransform2.jpg "../../doc/image/Applytransform2.jpg")

Center Geometry
===============

This will set the geometry origin to the center of its bounding box. If a mesh uses this geometry, its transform will be changed so that the geometry keeps its place in space.

-   Offseted geometry

![](../../doc/image/Centergeometry1.jpg "../../doc/image/Centergeometry1.jpg")

-   Centered geometry

![](../../doc/image/Centergeometry2.jpg "../../doc/image/Centergeometry2.jpg")

