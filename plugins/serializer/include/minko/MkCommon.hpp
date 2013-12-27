#pragma once

#include "minko/Common.hpp"
#include <queue>

namespace minko
{
	class Qark;
	class MkStats;

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
		class WriterOption;
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

typedef std::map<std::shared_ptr<minko::scene::Node>, std::vector<minko::component::AbstractComponent>>	ControllerMap;
typedef std::map<std::shared_ptr<minko::scene::Node>, minko::uint>										NodeMap;
typedef	std::map<std::string, minko::Any>																NodeInfo;
