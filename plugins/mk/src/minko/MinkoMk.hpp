#pragma once

#include "minko/Qark.hpp"
#include "minko/data/HalfEdge.hpp"
#include "minko/data/HalfEdgeCollection.hpp"
#include "minko/deserialize/SceneDeserializer.hpp"
#include "minko/deserialize/NodeDeserializer.hpp"
#include "minko/deserialize/MkTypes.hpp"
#include "minko/file/MkOptions.hpp"
#include "minko/file/MkParser.hpp"

class MinkoMk
{
public:
	static
	void log(const std::string& message)
	{
		std::cout << message << std::endl;
	}
};

