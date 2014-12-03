#pragma once

#include "minko/Common.hpp"
#include <queue>

#define MINKO_SCENE_MAGIC_NUMBER    0x4D4B0300 // MK30 last byte reserved for extensions (material, geometry...)

#define MINKO_SCENE_HEADER_SIZE        30

#define MINKO_SCENE_VERSION_HI		0
#define MINKO_SCENE_VERSION_LO		2
#define MINKO_SCENE_VERSION_BUILD       3

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
        template <typename T>
        class AbstractWriter;
        template <typename T>
        class AbstractWriterPreprocessor;
        class SceneWriter;
		class SceneParser;
		class GeometryWriter;
		class GeometryParser;
		class MaterialParser;
		class MaterialWriter;
		class Dependency;
        class TextureParser;
        class TextureWriter;
        class WriterOptions;
	}

	namespace serialize
	{
		class TypeSerializer;
		class ComponentSerializer;
	}
}

