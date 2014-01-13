#pragma once

#include "minko/file/AbstractWriter.hpp"

#include "minko/file/SceneWriter.hpp"
#include "minko/file/SceneParser.hpp"
#include "minko/file/GeometryWriter.hpp"
#include "minko/file/GeometryParser.hpp"
#include "minko/file/MaterialParser.hpp"
#include "minko/file/MaterialWriter.hpp"


class MinkoMk
{
public:
	static
	void log(const std::string& message)
	{
		std::cout << message << std::endl;
	}
};

