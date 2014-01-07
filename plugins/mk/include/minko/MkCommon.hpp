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

typedef std::map<std::shared_ptr<minko::scene::Node>, minko::uint>										NodeMap;
typedef std::shared_ptr<minko::file::MkOptions>															OptionsPtr;
typedef	std::map<std::string, minko::Any>																NodeInfo;
