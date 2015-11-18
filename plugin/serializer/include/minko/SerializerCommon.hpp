#pragma once

#include "minko/Common.hpp"
#include <queue>

#define MINKO_SCENE_MAGIC_NUMBER    0x4D4B0300 // MK30 last byte reserved for extensions (material, geometry...)

#define MINKO_SCENE_HEADER_SIZE     30

#define MINKO_SCENE_VERSION_MAJOR   0
#define MINKO_SCENE_VERSION_MINOR	3
#define MINKO_SCENE_VERSION_PATCH   1

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
        template <typename T>
        class AbstractWriter;
        template <typename T>
        class AbstractWriterPreprocessor;
		class Dependency;
		class GeometryParser;
		class GeometryWriter;
        class LinkedAsset;
		class MaterialParser;
		class MaterialWriter;
		class MeshPartitioner;
		class SceneParser;
		class SceneTreeFlattener;
        struct SceneVersion;
        class SceneWriter;
        class SurfaceOperator;
        class TextureParser;
        class TextureWriter;
        class VertexWelder;
        class WriterOptions;
	}

    namespace math
    {
        template <typename T>
        class SpatialIndex;
    }

	namespace serialize
	{
		class TypeSerializer;
		class ComponentSerializer;
	}
}

