#pragma once

#include <iostream>
#include <string>

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

class Minko
{
public:
	static
	void log(const std::string& message)
	{
		std::cout << message << std::endl;
	}
};

#include "minko/type/Any.hpp"
#include "minko/render/context/OpenGLESContext.hpp"
#include "minko/type/math/Vector2.hpp"
#include "minko/type/math/Vector3.hpp"
#include "minko/type/math/Vector4.hpp"
#include "minko/type/math/Matrix4x4.hpp"
#include "minko/type/Signal.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/data/DataProvider.hpp"
#include "minko/scene/data/DataBindings.hpp"
#include "minko/scene/controller/AbstractController.hpp"
#include "minko/render/geometry/Geometry.hpp"
#include "minko/render/geometry/CubeGeometry.hpp"
#include "minko/scene/controller/SurfaceController.hpp"
#include "minko/scene/controller/RenderingController.hpp"

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
