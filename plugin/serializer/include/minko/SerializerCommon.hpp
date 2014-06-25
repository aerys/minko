#pragma once

#include "minko/Common.hpp"
#include <queue>

namespace minko
{
	namespace extension
	{
		class SerializerExtension;
		class AbstractExtension;
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
        class SceneWriter;
		class SceneParser;
		class GeometryWriter;
		class GeometryParser;
		class MaterialParser;
		class MaterialWriter;
		class Dependency;
        class WriterOptions;
	}

	namespace serialize
	{
		class TypeSerializer;
		class ComponentSerializer;
	}
}

