#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <list>
#include <vector>
#include <functional>
#include <map>
#include <exception>
#include <stdexcept>

class OpenGLESContext;

class Vector2;
class Vector3;
class Vector4;
class Matrix4x4;

template<typename... A>
class Signal;

class Node;
class Geometry;
class DrawCall;

class DataProvider;
class DataBindings;

class AbstractController;
class SurfaceController;
class RenderingController;

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

namespace std
{
	template<typename T>
	string
	to_string(shared_ptr<T> value)
	{
		return to_string(*value);
	}
}
