# Effect File Format

Effect files (.effect) are JSON formated text files used to describe rendering effects. Minko comes with an extensive set of effects and the following documentation defines how *.effect files are written.

More about creating custom effects:
* [Creating a custom effect](../tutorials/17-Creating_a_custom_effect.md)
* [Creating a simple post-processing effect](../tutorial/22-Creating_a_simple_post-processing_effect.md)

## Techniques

Each effect has one or more techniques. Each technique is (supposed to be) a different implementation of the same rendering effect.
For example, the same effect can provide different techniques for desktop or mobile targets. Multiple techniques can also be defined to provide different quality settings.

```javascript
{
    "techniques" : [
        {
            "name" : "default",
            // default technique definition...
        },
        {
            "name" : "another-technique",
            // second technique definition...
        }
    ]
}
```
The "name" directive is optionnal. The effect technique to use in the app will be specified when creating the `Surface` component:

```cpp
Surface::Ptr s = Surface::create(material, geometry, effect, "my-second-technique");
```
If no technique name is specified, the technique name will be assumed to be "default". Thus, it is recommended to always name at least one technique "default". If your effect as only one technique, it should be named "default".

## Passes

### Definition

Each technique has one or more passes. Each pass declaration defines an actual rendering pass. A rendering pass is defined by:
* a name,
* a vertex shader,
* a fragment shader,
* a set of uniform, attribute, state and macro bindings.

```javascript
"techniques" : [
    {
        "name" : "default",
        // default technique passes...
    },
    {
        // second technique passes...
    }
]
```

The rendering passes order is the same as their declaration order.

### Vertex and fragment shaders

The vertex and fragment shaders are the most important components of a pass since they define the actual rendering code that will be executed on the GPU.

```javascript
"techniques" : [
    {
        "name" : "default",
        "vertexShader" : "
            // vertex shader GLSL code...
        ",
        "fragmentShader" : "
            // fragment shader GLSL code...
        "
    },
]
```

Those shaders are implemented as GLSL programs. The following example declares a fragment shader that simply outputs white pixels:

```javascript
"fragmentShader" : "
    void main()
    {
        gl_FragColor = vec4(1.0);
    }
"
```

In order to ease code reuse, Minko implements an "include" pre-processing directive: `#pragma include`.
This is not an actual pre-processing directive.
Instead, it's evaluated upon loading when the shader is read from the effect file. The corresponding line
is then replaced with the content of the included file:

```glsl
// saturate.function.glsl
vec4 saturate(vec4 v)
{
    return min(max(v, 0.0), 1.0);
}
```

```glsl
#pragma include "test.function.glsl"

uniform vec4 uDiffuseColor;

void main()
{
    gl_FragColor = saturate(uDiffuseColor);
}
```

The `#pragma include` directive can also be used to implement the GLSL shader code outside of the effect file:

```javascript
// Basic.effect
{
    "name" : "basic",
    "techniques" : [
        {
            "name" : "default",
            "vertexShader" : "#pragma include \"Basic.vertex.glsl\"",
            "fragmentShader" : "#pragma include \"Basic.fragment.glsl\""
        }
    ]
}
```

Such GLSL shader can then easily be shared among multiple effects.

More about vertex and fragment shaders:
* [Using external GLSL code in effect files](../tutorial/23-Using_external_GLSL_code_in_effect_files.md)

### Pass inheritance

It is sometimes useful to be able to share a common base definition among multiple passes of
multiple techniques. It's also sometimes useful to be able to extend passes defined by other
effects. In order to share and extend a common base definition, passes can be extended using the
`extends` directive.

#### Extending a "free" base pass

In order to re-use the same base pass declaration among multiple techniques, "free" passes can
be declared aside from actual techniques:

```javascript
{
    "techniques" : [
        {
            "name" : "default",
            "passes" : [
                {
                    "extends" : "base-pass",
                    // overrides base-pass declaration...
                }
            ]
        }
    ],
    "passes" : [
        {
            "name" : "base-pass",
            // base pass declaration...
        }
    ]
}
```

*Attention: such "free" passes are only meant to be extended and do not end up in the loaded Effect object.*
Only passes properly defined in techniques are actually kept and stored in the final Effect object.

#### Extending a base pass from another technique

A pass can also extend a pass from another technique of the same effect:

```javascript
{
    "techniques" : [
        {
            "name" : "default",
            "passes" : [
                {
                    "name" : "base-pass",
                    // base pass definition...
                },
                {
                    "extends" : {
                        "technique" : "default",
                        "pass" : "base-pass"
                    },
                    // override base-pass definition...
                }
            ]
        }
    ]
}
```

#### Extending a base pass from another effect

A pass can also extend a base pass from a specific technique of another effect:

```javascript
{
    "techniques" : [
        {
            "name" : "default",
            "passes" : [
                {
                    "extends" : {
                        "effect" : "base-effect.effect"
                        "technique" : "base-technique",
                        "pass" : "base-pass"
                    },
                    // overrides base-pass declaration...
                }
            ]
        }
    ]
}
```

## Bindings

### Definition

Bindings are a key concept of effect files. They are used to automagically bind
rendering properties - such as vertex attributes, uniforms or states - to actual
values provided by the application through the data API. When a rendering property
is bound, the engine will make sure it's always set to the actual value
provided by the application code.

Bindings are very important because they allow effect developers to:
* re-use existing shader code from foreign code base without editing them: the expected input values (uniforms, attributes...) just have to be bound properly;
* separate efficiently rendering effects definition from the application business code: rendering properties do not have to be set manually in the application code;
* declare more generic effect files that can easily be shared among multiple projects;
* avoid tedious manual rendering effect properties setup.

### Declaration

A binding declaration has 2 components:
* `property` represents the application data property to be bound to; this very string value that will be used to call  the `data::Store::get` method and must be formatted as such;
* `source` represents the scene data context the bound property should be read from:
  * "surface" means the scene graph node that hosts the `Surface` component using the effect we're declaring;
  * "renderer" means the scene node that hosts the `Renderer` component which is actually performing rendering;
  * "root" means the root scene node of the rendered scene.

Example:

```javascript
"binding" : {
    "property" : "boundPropertyName",
    "source" : "surface"
}
```

By default, the `source` is assumed to be "surface" and the binding declaration can be shortened:

```javascript
"binding" : "boundPropertyName"
```

### Effect variables

Effect variables are a set of values provided by the execution context of the
effect. Their value is set by the engine itself. They are immutable in the scope
of the effect itself. But they are called "variable" because their value for a
specific effect will very from a `Surface` to another.

Effect variables can be used in binding property names in order to make them
more dynamic. For example, let's consider the following (non-working and
educational only) binding example :

```javascript
{
    "property" : "material.diffuseColor", // wrong: material is a collection, so which material?
    "source" : "surface"
}
```

The data API stores properties at the node level in order to be able to share
multiple properties among multiple components. Thus, a node with multiple `Surface`
component will have multiple materials stored in its target node `data::Store`.
It is then impossible to decide which material the "material.diffuseColor" property
refers to.

In order to make sure we will bind a property from the actual material set for
the `Surface` being rendered, we will use the "materialUuid" effect variable:

```javascript
{
    "property" : "material[${materialUuid}].diffuseColor", // right: we target a specific item of the material collection
    "source" : "surface"
}
```

Available variables are:
* materialUuid
* geometryUuid
* surfaceUuid

### Binding attributes

The `attributes` directive is used to bind the (vertex) attributes declared in the shaders with actual data properties
provided by the application.

```javascript
"attributes" : {
    "aPosition" : { "binding" : "geometry[${geometryUuid}].position" }
}
```

Attributes that are not bound will have to be manually set by calling the `Effect::setAttribute()` method.

More about vertex attributes:
* [Working with custom vertex attributes](../tutorial/24-Working_with_custom_vertex_attributes.md)

### Binding uniforms

The `uniforms` directive is used to bind the uniforms declared in the shaders with actual data properties provided
by the application:

```javascript
"uniforms" : {
    "uDiffuseColor" : {
        "binding" : "geometry[${materialUuid}].diffuseColor"
        "default" : [[ 1.0, 0.0, 0.0, 1.0 ]]
    }
}
```

```javascript
"uniforms" : {
    "uDiffuseColor" : "geometry[${materialUuid}].diffuseColor"
}
```

If a uniform is not bound and has no default value, it will have to be set by
calling the `Effect::setUniform()` method.

More about uniform bindings:
* [Binding the model to world transform](../tutorials/19-Binding_the_model_to_world_transform.md)
* [Binding the camera](../tutorials/20-Binding_the_camera.md)

### Binding states

```javascript
"uniforms" : {
    "blendingMode" : {
        "binding" : "material[${materialUuid}].blendingMode",
        "default" : "opaque"
    }
}
```

```javascript
"states" : {
    "blendingMode" : "opaque"
}
```

#### Available states

|Key|Default Value|
|-|-|
|`"priority"`|`Priority::OPAQUE`|
|`"zSorted"`|`false`|
|`"blendingMode"`|`BlendingMode::OPAQUE`|
|`"blendingSource"`|`Blending::Source::ONE`|
|`"blendingDestination"`|`Blending::Destination::ZERO`|
|`"colorMask"`|`true`|
|`"depthMask"`|`true`|
|`"depthFunction"`|`CompareMode::LESS`|
|`"triangleCulling"`|`TriangleCulling::BACK`|
|`"stencilFunction"`|`CompareMode::ALWAYS`|
|`"stencilReference"`|`0`|
|`"stencilMask"`|`1`|
|`"stencilFailOperation"`|`StencilOperation::KEEP`|
|`"stencilZFailOperation"`|`StencilOperation::KEEP`|
|`"stencilZPassOperation"`|`StencilOperation::KEEP`|
|`"scissorTest"`|`false`|
|`"scissorBox"`|`math::vec4(0, 0, 1, 1)`|
|`"target"`|none|

### Binding macros

A GLSL macro can be used to write shaders that can have different behaviors depending on the value of that
macro or whether it's set or not. Such shaders are called "über-shaders". Über-shaders are rendering programs
that can handle many different rendering scenarios. For example, a lighting über-shader will be able to handle
many different counts and types of lights. An even simpler scenario is whether we want to render using the
vertex color, a solid color or a texture.

```c
#ifdef SOME_OPTION
    // do something...
#else
    // do something else...
#endif
```

The question is then how to automagically set the value of the `SOME_OPTION` macro according to actual values
provided by the application. In order to do this, we will once again use bindings.

The behavior of a macro binding is described in the following pseudo-code:

```
defineString = ""

if propertyExists(propertyName) then
 if isInteger(data[propertyName]) then
   defineString = "#define " + propertyName + " " + data[propertyName] // #define MACRO_NAME propertyValue
 else
   defineString = "#define " + propertyName // #define MACRO_NAME

// else no #define
```

If a macro is expected to have an actual value, the type of that value must be specified. Available types are:
* `int`
* `int2`
* `int3`
* `int4`
* `bool`
* `bool2`
* `bool3`
* `bool4`
* `float`
* `float2`
* `float3`
* `float4`
* `float9`
* `float16`

The following example will bnd the `NUM_LIGHTS` macro to the "directionalLight.length" data property:

```javascript
"uniforms" : {
    "NUM_LIGHTS" : {
        "binding" : "directionalLight.length",
        "type" : "int",
        "default" : 0
    }
}
```

The following example can be used to have a shader that behaves differently when per-vertex normals are available:

```javascript
"uniforms" : {
    "HAS_NORMALS" : "geometry[${geometryUuid}].position"
}
```

Macros that are not bound can be set using the `Effect::define()` method.

More about macro bindings:
* [Authoring über-shaders](../tutorial/21-Authoring_uber-shaders.md)

### Scope inheritance

Multiple passes of the same techniques and multiple techniques of the same effect are likely to share
common definitions. For example all the passes of all the techniques of an effect are likely to use the
same attribute bindings.

Instead of re-declaring attributes, uniforms, states and macros, one can define them in the upper scope.
Passes will inherit all definition from their technique, and techniques will inherit from the global root
scope of the effect.

For example, the attributes declared globally in the effect will be inherited by all techniques. In the code
below, the "aPosition" attribute is declared and bound in the root scope because all passes of all techniques
will need it:

```javascript
{
    "attributes" : {
        "aPosition" : "geometry[${geometryUuid}].position"
    },
    "techniques" : [
        // ...
    ]
}
```
Inheritance also works at the technique scope level. In the following example, all passes of the "default"
technique will inherit the "aPosition" attribute binding:

```javascript
{
    "techniques" : [
        {
            "name" : "default",
            "attributes" : {
                "aPosition" : "geometry[${geometryUuid}].position"
            },
        },
        {
            "name" : "second-technique",
            // ...
        }
    ]
}
```

but passes of the "second-technique" technique will not have that binding.

### Default values

#### Declaration

In addition to bindings, effect developers can also provide default values for uiforms, states and macros.
Default values can have the following types:

|Type|Examples|
|-|-|
|`bool`|`true` or `false`|
|`int`|`0`, `1`, `2`, `3`...|
|`math::ivec2`|`[[ 23, 42 ]]`|
|`math::ivec3`|`[[ 23, 42, 13 ]]`|
|`math::ivec4`|`[[ 23, 42, 13, 17 ]]`|
|`float`| `0.0`, `1.0`, `2.0`, `3.0`...|
|`math::vec2`|`[[ 23.0, 42.0 ]]`|
|`math::vec3`|`[[ 23.0, 42.0, 13.0 ]]`|
|`math::vec4`|`[[ 23.0, 42.0, 13.0, 17.0 ]]`|
|`render::Texture`|`"path/to/texture.jpg"`|

The following extra default values can be used for states:

|Type|Examples|
|-|-|
|priority| a `float` or one of the pre-defined priorities: `"first"`, `"background"`, `"opaque"`, `"transparent"`, `"last"` or a pre-defined priority with an offset: `["first", 3.0]`, `["opaque", 42.2]`|
|zSorted|`bool`|
|blendingMode|`"opaque"`, `"alpha"` or `"additive"`|
|blendingSource|`"zero"`, `"one"`, `"color"`, `"one_minus_src_color"`, `"src_alpha"`, `"one_minus_src_alpha"`, `"dst_alpha"`, `"one_minus_dst_alpha"`|
|blendingDestination|`"zero"`, `"one"`, `"dst_color"`, `"one_minus_dst_color"`, `"src_alpha_saturate"`, `"one_minus_src_alpha"`, `"dst_alpha"`, `"one_minus_dst_alpha"`|
|colorMask|`bool`|
|depthMask|`bool`|
|depthFunction|`"always"`, `"equal"`, `"greater"`, `"greater_equal"`, `"less"`, `"less_equal"`, `"never"`, `"not_equal"`|
|triangleCulling|`"none"`, `"front"`, `"back"`, `"both"`|
|stencilFunction|`"always"`, `"equal"`, `"greater"`, `"greater_equal"`, `"less"`, `"less_equal"`, `"never"`, `"not_equal"`|
|stencilReference|`int`|
|stencilMask|`bool`|
|stencilFailOperation|`"keep"`, `"zero"`, `"replace"`, `"incr"`, `"incr_wrap"`, `"decr"`, `"decr_wrap"`, `"invert"`|
|stencilZFailOperation|`"keep"`, `"zero"`, `"replace"`, `"incr"`, `"incr_wrap"`, `"decr"`, `"decr_wrap"`, `"invert"`|
|stencilZPassOperation|`"keep"`, `"zero"`, `"replace"`, `"incr"`, `"incr_wrap"`, `"decr"`, `"decr_wrap"`, `"invert"`|
|scissorTest|`bool`|
|scissorBox|`math::ivec4`|
|target|a `string` with the name of an existing texture in the `AssetLibrary` or a target object `{ "name": "target-texture-name", "size" : 1024 }`|


By default attributes, uniforms and macros will interprete literal values as bindings but states will interpret them as default values:

```javascript
"attributes" : {
    "aPosition" : "geometry[${geometryUuid}].position" // binding
},
"states" : {
    "blendingMode" : "alpha" // default value
}
```

#### Examples

```javascript
"uniforms" : {
    "uDiffuseColor" : {
        "binding" : "material[${materialUuid}].diffuseColor",
        "default" : [[ 1.0, 1.0, 1.0, 1.0 ]]
    }
}
```

#### Properly initializing materials

For a matter of consistency, all the rendering default values are to be declared in the effect files as described above.
When instanciating a new material, we must make sure it is properly initialized with those default values.
In order to do so, we call the `Effect::initializeMaterial()` method:

```cpp
Material::Ptr m = fx->initializeMaterial(Material::create());
```

`Effect::initializeMaterial()` will make sure that any default value set for a binding referring to the `materialUuid`
effect variable will actually be set on the specified material object.

```javascript
"uniforms" : {
    "uDiffuseColor" : {
        "binding" : "material[${materialUuid}].diffuseColor",
        "default" : [[ 1.0, 1.0, 1.0, 1.0 ]]
    }
}
```

```cpp
Material::Ptr m = fx->initializeMaterial(Material::create());

std::cout << (m->get<math::vec4>("diffuseColor") == math::vec4(1.0)) << std::endl; // true
```

Inconsistent default values can lead to broken execution/rendering of the corresponding effect.
Thus, it is considered a best practice to always directly initialize materials upon creation using `Effect::initializeMaterial()`.
