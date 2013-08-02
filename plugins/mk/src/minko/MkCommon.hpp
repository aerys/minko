#pragma once

#include "minko/Common.hpp"
#include <queue>

namespace minko
{
	class Qark;

	namespace data
	{
		class HalfEdge;
		class HalfEdgeCollection;
	}

	namespace deserialize
	{
		class AssetsDeserializer;
		class GeometryDeserializer;
		class NameConverter;
		class NodeDeserializer;
		class SceneDeserializer;
		class TypeDeserializer;
	}

	namespace file
	{
		class MkOptions;
		class MkParser;
	}
}