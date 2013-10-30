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
		typedef std::unordered_map<std::string, std::tuple<std::string, int, int>> MacroBindingMap;
	}

	namespace geometry
	{
		class Geometry;
		class CubeGeometry;
		class SphereGeometry;
        class QuadGeometry;
	}

	namespace math
	{
		class Vector2;
		class Vector3;
		class Vector4;
		class Matrix4x4;
		class Quaternion;
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
