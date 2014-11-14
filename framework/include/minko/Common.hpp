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

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <exception>
#include <functional>
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

namespace minko
{
    typedef unsigned int uint;
    typedef std::shared_ptr<std::regex>    RegexPtr;

    class Any;
    template<typename... A>
    class Signal;
    class Color;
	enum class CloneOption;
    class AbstractCanvas;

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
        typedef std::tuple<WrapMode, TextureFilter, MipFilter>    SamplerState;

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

        enum class TextureType
        {
            Texture2D    = 0,
            CubeTexture    = 1
        };

        enum class EnvironmentMap2dType
        {
            Unset        = -1,
            Probe        = 0,
            BlinnNewell    = 1
        };

        enum class TextureFormat
        {
            RGB,
            RGBA
        };
        class AbstractTexture;
        class Texture;
        class RectangleTexture;
        class CubeTexture;

        struct ScissorBox
        {
            int        x, y;
            int    width, height;

            inline
            ScissorBox(): x(0), y(0), width(-1), height(-1)
            {
            }
        };

        enum class FogType
        {
            None,
            Linear,
            Exponential,
            Exponential2,
        };

        typedef std::function<std::string(const std::string&)> FormatNameFunction;
    }

    namespace scene
    {
        class Node;
        class NodeSet;
    }

    namespace component
    {
        class AbstractComponent;
		class AbstractRebindableComponent;
        template <class ProviderClass, class Enable = void>
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
    }

    namespace data
    {
        class Provider;
        class ArrayProvider;
        class StructureProvider;
        class ValueBase;
        class Value;
        class Container;
        class AbstractFilter;

        enum class BindingSource
        {
            TARGET,
            RENDERER,
            ROOT
        };

        typedef std::pair<std::string, BindingSource>                        Binding;
        typedef std::unordered_map<std::string, Binding>                    BindingMap;
        typedef std::pair<std::shared_ptr<data::Container>, std::string>    ContainerAndName;


        template<typename T>
        using UniformArray = std::pair<uint, const T*>;

        template<typename T>
        using UniformArrayPtr = std::shared_ptr<UniformArray<T>>;

        enum class MacroBindingDefaultValueSemantic
        {
            UNSET,
            VALUE,
            PROPERTY_EXISTS
        };

        union MacroBindingDefaultValue
        {
            bool    propertyExists;
            int        value;
        };

        struct MacroBindingDefault
        {
            MacroBindingDefaultValueSemantic    semantic;
            MacroBindingDefaultValue            value;
        };

        typedef std::function<bool(const std::string&)> MacroRegexPredicate;
        typedef std::tuple<std::string, BindingSource, MacroBindingDefault, int, int, MacroRegexPredicate>    MacroBinding;

        typedef std::unordered_map<std::string, MacroBinding> MacroBindingMap;

        class ContainerProperty;
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
        class Vector2;
        class Vector3;
        class Vector4;
        class Matrix4x4;
        class Quaternion;
        class Ray;
        class AbstractShape;
        class Box;
        class Frustum;
        class OctTree;

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
            unsigned int tmp    = x;
            unsigned int p        = 0;
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

namespace std
{
    template <class T>
    inline
    void
    hash_combine(size_t & seed, const T& v)
    {
        hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

#ifdef __ANDROID__
    template <typename T>
    inline
    string
    to_string(T v)
    {
        ostringstream oss;
        oss << v;
        return oss.str();
    }
#endif

    template<> struct hash<minko::data::ContainerAndName>
    {
        inline
        size_t
        operator()(const minko::data::ContainerAndName& x) const
        {
            size_t seed = std::hash<std::shared_ptr<minko::data::Container>>()(x.first);

            hash_combine<std::string>(seed, x.second);

            return seed;
        }
    };

    inline
    std::string
    replaceAll(std::string str, const std::string& from, const std::string& to)
    {
        size_t start_pos = 0;
        
        while((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        
        return str;
    }
}
