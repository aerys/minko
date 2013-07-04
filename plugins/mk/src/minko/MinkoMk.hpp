#pragma once

#include "minko/Qark.hpp"
#include "minko/data/HalfEdge.hpp"
#include "minko/data/HalfEdgeCollection.hpp"

namespace minko
{
	namespace data
	{
		class HalfEdge;
		class HalfEdgeCollection;
	}

	class Qark;
}

class MinkoMk
{
public:
	static
	void log(const std::string& message)
	{
		std::cout << message << std::endl;
	}
};

