#pragma once

#include <algorithm>
#include <cmath>
#include <exception>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace minko
{
	namespace render
	{
		namespace context
		{
			class AbstractContext;
			class OpenGLESContext;			
		}

		namespace geometry
		{
			class Geometry;
			class CubeGeometry;
		}

		class DrawCall;
	}

	namespace scene
	{
		class Node;

		namespace controller
		{
			class AbstractController;
			class SurfaceController;
			class RenderingController;
		}

		namespace data
		{
			class DataProvider;
			class DataBindings;
		}
	}

	namespace math
	{
		class Vector2;
		class Vector3;
		class Vector4;
		class Matrix4x4;
	}

	template<typename... A>
	class Signal;
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
