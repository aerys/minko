#pragma once

#include <iostream>
#include <string>

class OpenGLESContext;

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

class Minko
{
public:
	static
	void log(const std::string& message)
	{
		std::cout << message << std::endl;
	}
};

#include "Any.hpp"
#include "OpenGLESContext.hpp"
#include "Matrix4x4.hpp"
#include "Signal.hpp"
#include "Node.hpp"
#include "DataProvider.hpp"
#include "DataBindings.hpp"
#include "AbstractController.hpp"
#include "Geometry.hpp"
#include "CubeGeometry.hpp"
#include "SurfaceController.hpp"
#include "RenderingController.hpp"

template<typename T>
std::shared_ptr<T>
operator*(std::shared_ptr<T> a, std::shared_ptr<T> b)
{
	return std::make_shared<T>(*a * *b);
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
