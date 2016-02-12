/*
Copyright (c) 2014 Aerys

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

#include "minko/Setup.hpp"
#include "minko/system/Platform.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <list>
#include <forward_list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cassert>
#include <ctime>
#include <type_traits>
#include <cfloat>
#include <climits>
#include <cstdint>
#include <future>
#include <thread>
#include <chrono>
#include <bitset>
#include <regex>

#ifndef _MSC_VER
# define GLM_FORCE_CXX11
//# define GLM_FORCE_INLINE
#endif
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/epsilon.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/matrix_interpolation.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/color_space.hpp"
#include "glm/gtx/string_cast.hpp"

#ifndef MINKO_USE_SPARSE_HASH_MAP
# ifndef DEBUG
#  define MINKO_USE_SPARSE_HASH_MAP
# endif
#endif

namespace minko
{
    typedef unsigned int uint;
	typedef std::shared_ptr<std::regex>	RegexPtr;

	class Any;
	template<typename... A>
	class Signal;
    template<typename T>
    class Flyweight;
	class Color;
	enum class CloneOption;
	class AbstractCanvas;
    template<typename T>
    struct Hash;

	namespace render
	{
        class DrawCallPool;
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

        typedef int ResourceId;
		class AbstractResource;
		class Shader;
		class Program;
		class ProgramSignature;
		class VertexFormat;
        class VertexAttribute;
        class VertexBuffer;
		class IndexBuffer;

		enum class TextureType
		{
			Texture2D	= 0,
			CubeTexture	= 1
		};

        enum class TextureFormat;
		class AbstractTexture;
		class Texture;
        class RectangleTexture;
		class CubeTexture;
        struct TextureSampler;

		typedef std::function<std::string(const std::string&)> FormatNameFunction;
		typedef std::list<std::pair<Flyweight<std::string>, Flyweight<std::string>>> EffectVariables;
	}

	namespace scene
	{
		class Node;
		class NodeSet;
	}

	namespace component
	{
		class AbstractComponent;
	    class AbstractRootDataComponent;
	    class SceneManager;
    	class Transform;
		class Surface;
		class Renderer;
		class PerspectiveCamera;
		class Culling;
		class Picking;
		class JobManager;

        class AbstractLight;
        class AmbientLight;
        class AbstractDiscreteLight;
        class DirectionalLight;
		class SpotLight;
		class PointLight;

		class BoundingBox;

		class MousePicking;
		class MouseManager;
        class AbstractScript;
		enum class SkinningMethod;

		class AbstractAnimation;
		class MasterAnimation;
		class Animation;
		class Skinning;

        class Metadata;
	}

	namespace data
	{
		class Provider;
		class Store;
		class AbstractFilter;
        class Collection;

        struct Binding;
        struct MacroBinding;
        //struct BindingMap;
        //struct MacroBindingMap;

		class LightMaskFilter;
	}

	namespace geometry
	{
		class Geometry;
		class CubeGeometry;
		class SphereGeometry;
        class QuadGeometry;
		class TeapotGeometry;
		class LineGeometry;
	}

	namespace animation
	{
		class AbstractTimeline;
		class Matrix4x4Timeline;
	}

	namespace math
	{
		using namespace glm;

		class Ray;
		class AbstractShape;
		class Box;
		class Frustum;
		class OctTree;

		inline
		vec4
		rgba(uint rgba)
		{
			return math::vec4(
		        (float)((rgba >> 24) & 0xff) / 255.f,
		        (float)((rgba >> 16) & 0xff) / 255.f,
		        (float)((rgba >> 8) & 0xff) / 255.f,
		        (float)(rgba & 0xff) / 255.f
		    );
		}

		inline
		bool
		isp2(unsigned int x)
		{
			return x == 0 || (x & (x-1)) == 0;
		}

		inline
		uint
		getp2(unsigned int x)
		{
			unsigned int tmp	= x;
			unsigned int p		= 0;
			while (tmp >>= 1)
				++p;

			return p;
		}

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
        class File;
		class Options;
		class Loader;
        class AbstractProtocol;
		class AbstractParser;
		class EffectParser;
        class AssetLibrary;
        class AssetLocation;
        class AbstractAssetDescriptor;

        class Error : public std::runtime_error
        {
        private:
            std::string _type;

        public:
            explicit
            Error(const std::string& message) :
                std::runtime_error(message),
                _type()
            {
            }

            Error(const std::string& type, const std::string& message) :
                std::runtime_error(message),
                _type(type)
            {
            }

            inline
            const std::string&
            type() const
            {
                return _type;
            }
        };
	}

	namespace material
	{
		class Material;
		class BasicMaterial;
		class PhongMaterial;
	}

	namespace input
	{
		class Mouse;
        class Keyboard;
        class KeyMap;
		class Joystick;
        class Touch;
	}

	namespace async
	{
		class Worker;
	}

    namespace log
    {
        class Logger;
        class ConsoleSink;
    }
}

#include "minko/std.hpp"
