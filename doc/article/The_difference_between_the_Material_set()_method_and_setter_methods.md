In this article we will introduce the difference between the `Material::set()` method and hard-coded setter methods (ex: `Material::set("diffuseColor")` VS `BasicMaterial::diffuseColor()`). It should help you understanding how materials works internally, why there is two ways to do the same thing and how each of them might be a better fit for certain situations.

Example
-------

Before we can discuss this matter, we will first see a little example. The two following code snippets introduce two different ways to set the "diffuseColor" material property:

The "static" approach, using a class derived from Material and an actuall `diffuseColor()` setter method:

```cpp
auto basicMaterial = BasicMaterial::create();

basicMaterial->diffuseColor(Vector4::create(1.f, 0.f, 0.f, 1.f)); // red 
```


The "dynamic" approach, using the `Material` generic base class and the `set()` method with the property name as a string argument:

```cpp
auto material = Material::create();

material->set("diffuseColor", Vector4::create(1.f, 0.f, 0.f, 1.f)); // red 
```


The dynamic approach
--------------------

The `Material::set()` method is more dynamic for two reasons:

-   We don't have to know the exact type of our material object to set the property since the `set()` method is defined in the `Material` base class (this method is actually inherited from `[data::Provider`](data::Provider`)).
-   We don't have to make long list of if/else to call the right setter method since `set()` is generic and only expects the property name as a string.

It makes `Material::set()` de facto more suitable for dynamic code such as complex/dynamic assets loading operations.

Because material properties are not actually statically declared in a class but just referenced as string values, it also makes it possible to setup a `Material` object that will declare all the properties expected by any `Effect` without much work. Thus, you can create new rendering effects without having to write any C++ code to plug them to the engine/application.

Yet, this approach is a lot less user friendly if you actually know the name of the properties you want to set at author time. Indeed, developers don't have access to code-hinting since properties are referenced by simple string values. Setting all the required properties of a material is then a lot harder because you have to find their actual name from the documentation or - even worse - read the code of the corresponding `*.effect` file.

The static approach
-------------------

Working with hard-coded `Material`-derived classes and their setter methods is more static because you have to know each property name at runtime. Indeed, dynamic method invokation is pretty complicated and unsafe in C++.

Yet, this approach is a lot easier when:

-   There is an actual `Material`-derived class that declares all the setter methods you need.
-   The material object is typed as this derived class or can be dynamically casted safely (trying to cast a generic `Material` to all the possible derived class you have in an application is not really scalable). In other words you know the actual type of the material at author time.

Indeed, if both those requirements are met then code-hinting can be used at author time to list all setter methods available and even read their documentation directly from the IDE. Setting up a material is then a lot easier as far as the development experience is concerned.

If you actually read the implementation of `Material`-derived classes - such as `BasicMaterial` or `PhongMaterial` - you will notice that the setter methods are declared as simple inlined wrappers of the `Material::set()` method called with the corresponding property name as a string argument.

For example, `BasicMaterial::diffuseColor()` is implemented like this:

```cpp
class BasicMaterial :

 public Material

{

 inline
 Ptr
 diffuseColor(std::shared_ptr<Vector4> color)
 {
   set("diffuseColor", color);

   return std::dynamic_pointer_cast<BasicMaterial>(shared_from_this());
 }

} 
```


Thus, setter methods will actully call `Material::set()` internally.

Manually declaring specialized setter method in `Material`-derived classes is also a good opportunity to provide overloads. Thanks to this mechanism, you can implement data conversion when appropriate. For example, the `BasicMaterial` declares the `diffuseColor(Vector4::Ptr)` method because the `Basic.effect` expects an (x, y, z, w) float 4 tuple value. As colors are also often represented as RGBA integer values, the `BasicMaterial` class also provide the following overload:

```cpp
inline Ptr diffuseColor(
const uint rgba) {

 return diffuseColor(math::Vector4::create(
   ((rgba >> 24) & 0xff) / 255.f,
   ((rgba >> 16) & 0xff) / 255.f,
   ((rgba >> 8) & 0xff) / 255.f,
   (rgba & 0xff) / 255.f
 ));

} 
```


Such mechanism provides the developer with more semantic options for his code. Yet this is still mostly syntaxic sugar since `Material::set()` is always called in the end.

Performance consideration
-------------------------

Since `Material`-derived classes setter methods are just inlined `Material::set()` wrappers, both approaches should be equivalent performance wise. The setter methods defined in `Material`-derived classes should be considered as nothing more but syntaxic sugar to provide a better programming experience through code-hinting.

Conclusion
----------

You should use `Material::set()` when:

-   you cannot know the actual type of the material at author time;
-   there is no specific material class for the effect you are working with.

You should use `Material`-derived class and call their setter methods when:

-   they actually exist;
-   you know the actual type of the material at author time.

Both approaches should be equivalent CPU/memory wise, `Material`-derived classes setter methods are just better for code-hinting.

If you create new effects, you should try to provide the corresponding `Material`-derived class, declare as many setter methods and overloads as possible and document them to ease up the work of the developers.

