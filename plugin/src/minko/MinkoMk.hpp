#pragma once

#include "minko/MkCommon.hpp"
#include "minko/parser/mk/type/HalfEdge.hpp"
#include "minko/parser/mk/type/HalfEdgeCollection.hpp"

class MinkoMk
{
public:
	static
	void log(const std::string& message)
	{
		std::cout << message << std::endl;
	}
};

