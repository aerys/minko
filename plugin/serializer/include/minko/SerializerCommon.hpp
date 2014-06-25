#pragma once

#include "minko/Common.hpp"
#include <queue>

#define MINKO_SCENE_VERSION_HI		0
#define MINKO_SCENE_VERSION_LO		1
#define MINKO_SCENE_VERSION_BUILD	3

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

