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
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace minko
{
	namespace render
	{
		namespace context
		{
			class AbstractContext;
			class OpenGLES2Context;			
		}

		namespace stream
		{
			class VertexFormat;
			class VertexStream;
			class VertexAttribute;
			class IndexStream;
		}

		class DrawCall;

		class Effect;
		class GLSLProgram;
		class ShaderProgramInputs;
	}

	namespace scene
	{
		class Node;
		class NodeSet;

		namespace controller
		{
			class AbstractController;
			class TransformController;
			class SurfaceController;
			class RenderingController;
		}

		namespace data
		{
			class DataProvider;
			template <typename P>
			class DataProviderProperty;
			class DataBindings;

			namespace geometry
			{
				class Geometry;
				class CubeGeometry;
			}
		}
	}

	namespace math
	{
		class Vector2;
		class Vector3;
		class Vector4;
		class Matrix4x4;
	}

	namespace file
	{
		class FileLoader;
		class EffectParser;
	}

	class Any;
	template<typename... A>
	class Signal;
	template<typename... A>
	class SignalConnection;
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
	template<typename T>
	string
	to_string(shared_ptr<T> value)
	{
		return to_string(*value);
	}
}
