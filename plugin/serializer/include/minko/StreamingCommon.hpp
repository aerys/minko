#pragma once

#include "minko/Common.hpp"
#include "minko/SerializerCommon.hpp"
#include "minko/Types.hpp"

namespace minko
{
    struct ProgressiveOrderedMeshLodInfo
    {
        int _level;
        int _precisionLevel;
        int _indexOffset;
        int _indexCount;

    private:
        bool _isValid;

    public:
        inline
        bool
        isValid() const
        {
            return _isValid;
        }

        inline
        ProgressiveOrderedMeshLodInfo() :
            _isValid(false)
        {
        }

        inline
        ProgressiveOrderedMeshLodInfo(int level, int precisionLevel) :
            _level(level),
            _precisionLevel(precisionLevel),
            _indexOffset(0),
            _indexCount(0),
            _isValid(false)
        {
        }

        inline
        ProgressiveOrderedMeshLodInfo(int level,
                                      int precisionLevel,
                                      int indexOffset,
                                      int indexCount) :
            _level(level),
            _precisionLevel(precisionLevel),
            _indexOffset(indexOffset),
            _indexCount(indexCount),
            _isValid(true)
        {
        }
    };

    inline
    bool
    operator==(const ProgressiveOrderedMeshLodInfo& left, const ProgressiveOrderedMeshLodInfo& right)
    {
        return left._level == right._level &&
            left._precisionLevel == right._precisionLevel &&
            left._indexOffset == right._indexOffset &&
            left._indexCount == right._indexCount &&
            left.isValid() == right.isValid();
    }

    class StreamingOptions;

    namespace serialize
    {
        enum StreamingComponentId
        {
            POP_GEOMETRY_LOD_SCHEDULER = COMPONENT_ID_EXTENSION + 1,
            TEXTURE_LOD_SCHEDULER,
        };
    }

    namespace component
    {
        class AbstractLodScheduler;
        class MasterLodScheduler;
        class POPGeometryLodScheduler;
        class TextureLodScheduler;
    }

    namespace deserialize
    {
        class LodSchedulerDeserializer;
    }

	namespace extension
	{
		class StreamingExtension;
	}

    namespace file
    {
        class AbstractStreamedAssetParser;
        class MeshPartitioner;
        class POPGeometryParser;
        class POPGeometryWriter;
        class StreamedTextureParser;
        class StreamedTextureWriter;
    }

	namespace serialize
    {
        class LodSchedulerSerializer;
    }
}
