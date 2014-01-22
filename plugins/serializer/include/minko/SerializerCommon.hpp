#pragma once

#include "minko/Common.hpp"
#include <queue>

namespace minko
{
	namespace extention
	{
		class SerializerExtention;
		class AbstractExtention;
	}

	namespace data
	{
		class HalfEdge;
		class HalfEdgeCollection;
	}

	namespace deserialize
	{
		class ComponentDeserializer;
		class TypeDeserializer;
	}

	namespace file
	{
		class AbstractMkParser;
		class AbtractWriter;
		class SceneParser;
		class GeometryWriter;
		class GeometryParser;
		class MaterialParser;
		class MaterialWriter;
		class Dependency;
	}

	namespace serialize
	{
		class TypeSerializer;
		class ComponentSerializer;
	}
}

