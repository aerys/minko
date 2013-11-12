/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <algorithm>
#include <cmath>
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#define PI 3.1415926535897932384626433832795

namespace minko
{
    typedef unsigned int uint;

	class Any;
	template<typename... A>
	class Signal;
	class Color;
    
	namespace render
	{
		class AbstractContext;
		class OpenGLES2Context;
        class Blending;
		enum class CompareMode;
        enum class TriangleCulling;

        enum class WrapMode;
        enum class TextureFilter;
        enum class MipFilter;
		enum class StencilOperation;
        typedef std::tuple<WrapMode, TextureFilter, MipFilter>	SamplerState;

        class States;
		class DrawCall;
		class Pass;
		class Effect;
		class ProgramInputs;

		class AbstractResource;
		class Shader;
		class Program;
		class ProgramSignature;
		class VertexFormat;
		class VertexBuffer;
		class IndexBuffer;
		class Texture;
	}

	namespace scene
	{
		class Node;
		class NodeSet;
	}

	namespace component
	{
		class AbstractComponent;
		template <class ProviderClass, class Enable = void>
	    class AbstractRootDataComponent;
	    class SceneManager;
    	class Transform;
		class Surface;
		class Renderer;
		class PerspectiveCamera;

        class LightManager;
        class AbstractLight;
        class AmbientLight;
        class AbstractDiscreteLight;
        class DirectionalLight;
		class SpotLight;
		class PointLight;

		class BoundingBox;
	}

	namespace data
	{
		class Provider;
		class ArrayProvider;
		class StructureProvider;
		class ValueBase;
		class Value;
		class Container;
        typedef std::unordered_map<std::string, std::string> BindingMap;

		enum class MacroBindingDefaultValueSemantic
		{
			UNSET,
			VALUE,
			PROPERTY_EXISTS
		};

		union MacroBindingDefaultValue
		{
			bool propertyExists;
			int value;
		};

		struct MacroBindingDefault
		{
			MacroBindingDefaultValueSemantic semantic;
			MacroBindingDefaultValue value;
		};

		typedef std::tuple<std::string, MacroBindingDefault, int, int>	MacroBinding;

		typedef std::unordered_map<std::string, MacroBinding> MacroBindingMap;
	}

	namespace geometry
	{
		class Geometry;
		class CubeGeometry;
		class SphereGeometry;
        class QuadGeometry;
		class TeapotGeometry;
	}

	namespace math
	{
		class Vector2;
		class Vector3;
		class Vector4;
		class Matrix4x4;
		class Quaternion;
		class Ray;
		class AbstractShape;
		class Box;

		inline
		unsigned int
		flp2(unsigned int x)
		{
			x = x | (x >> 1);
			x = x | (x >> 2);
			x = x | (x >> 4);
			x = x | (x >> 8);
			x = x | (x >> 16);

			return x - (x >> 1);
		}

		inline
		unsigned int
		clp2(unsigned int x)
		{
			x = x - 1;
			x = x | (x >> 1);
			x = x | (x >> 2);
			x = x | (x >> 4);
			x = x | (x >> 8);
			x = x | (x >> 16);

			return x + 1;
		}
	}

	namespace file
	{
		class Options;
		class Loader;
		class AbstractLoader;
		class AbstractParser;
		class EffectParser;
        class AssetLibrary;
	}

	namespace material
	{
		class Material;
		class BasicMaterial;
	}
}

template<typename T>
std::shared_ptr<T>
operator*(std::shared_ptr<T> a, float b)
{
	return (*a) * b;
}

template<typename T>
std::shared_ptr<T>
operator*(std::shared_ptr<T> a, std::shared_ptr<T> b)
{
	return *a * b;
}

template<typename T>
std::shared_ptr<T>
operator-(std::shared_ptr<T> a, std::shared_ptr<T> b)
{
	return *a - b;
}

template<typename T>
std::shared_ptr<T>
operator+(std::shared_ptr<T> a, std::shared_ptr<T> b)
{
	return *a + b;
}

template<typename T>
std::shared_ptr<T>
operator/(std::shared_ptr<T> a, std::shared_ptr<T> b)
{
	return *a / b;
}

template<typename T>
std::shared_ptr<T>
operator*=(std::shared_ptr<T> a, std::shared_ptr<T> b)
{
	return *a *= b;
}

template<typename T>
std::shared_ptr<T>
operator+=(std::shared_ptr<T> a, std::shared_ptr<T> b)
{
	return *a += b;
}

template<typename T>
std::shared_ptr<T>
operator-=(std::shared_ptr<T> a, std::shared_ptr<T> b)
{
	return *a -= b;
}

//using namespace minko;
