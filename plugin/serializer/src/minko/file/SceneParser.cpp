/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/file/AssetLibrary.hpp"
#include "minko/file/SceneParser.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/TextureParser.hpp"
#include "minko/Types.hpp"
#include "minko/component/Transform.hpp"
#include "minko/component/JobManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/BoundingBox.hpp"
#include "minko/component/MasterAnimation.hpp"
#include "minko/component/Metadata.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/deserialize/Unpacker.hpp"

#include <stack>

using namespace minko;
using namespace minko::file;
using namespace minko::deserialize;

std::unordered_map<std::int8_t, SceneParser::ComponentReadFunction> SceneParser::_componentIdToReadFunction;

SceneParser::SceneParser()
{
    _dependency = Dependency::create();

    _geometryParser = file::GeometryParser::create();
    _materialParser = file::MaterialParser::create();
    _textureParser = file::TextureParser::create();

    registerComponent(serialize::PROJECTION_CAMERA,
        std::bind(&deserialize::ComponentDeserializer::deserializeProjectionCamera,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    registerComponent(serialize::CAMERA,
        std::bind(&deserialize::ComponentDeserializer::deserializeCamera,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    registerComponent(serialize::TRANSFORM,
        std::bind(&deserialize::ComponentDeserializer::deserializeTransform,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
		std::placeholders::_4));

    registerComponent(serialize::IMAGE_BASED_LIGHT,
        std::bind(&deserialize::ComponentDeserializer::deserializeImageBasedLight,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    registerComponent(serialize::AMBIENT_LIGHT,
        std::bind(&deserialize::ComponentDeserializer::deserializeAmbientLight,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    registerComponent(serialize::DIRECTIONAL_LIGHT,
        std::bind(&deserialize::ComponentDeserializer::deserializeDirectionalLight,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    registerComponent(serialize::SPOT_LIGHT,
        std::bind(&deserialize::ComponentDeserializer::deserializeSpotLight,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    registerComponent(serialize::POINT_LIGHT,
        std::bind(&deserialize::ComponentDeserializer::deserializePointLight,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    registerComponent(serialize::SURFACE,
        std::bind(&deserialize::ComponentDeserializer::deserializeSurface,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    registerComponent(serialize::RENDERER,
        std::bind(&deserialize::ComponentDeserializer::deserializeRenderer,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    registerComponent(serialize::MASTER_ANIMATION,
        std::bind(&deserialize::ComponentDeserializer::deserializeMasterAnimation,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    registerComponent(serialize::ANIMATION,
        std::bind(&deserialize::ComponentDeserializer::deserializeAnimation,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    registerComponent(serialize::SKINNING,
        std::bind(&deserialize::ComponentDeserializer::deserializeSkinning,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    registerComponent(serialize::BOUNDINGBOX,
        std::bind(&deserialize::ComponentDeserializer::deserializeBoundingBox,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    registerComponent(serialize::METADATA,
        std::bind(&deserialize::ComponentDeserializer::deserializeMetadata,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));

    registerComponent(serialize::ASCII_TEXT,
        std::bind(&deserialize::ComponentDeserializer::deserializeASCIIText,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3,
        std::placeholders::_4));
}

void
SceneParser::registerComponent(std::int8_t				componentId,
                               ComponentReadFunction    readFunction)
{
    _componentIdToReadFunction[componentId] = readFunction;
}

void
SceneParser::parse(const std::string&                   filename,
                   const std::string&                   resolvedFilename,
                   std::shared_ptr<Options>             options,
                   const std::vector<unsigned char>&    data,
                   AssetLibraryPtr                      assetLibrary)
{
    AbstractSerializerParser::parse(filename, resolvedFilename, options, data, assetLibrary);

    const auto scenePath = File::extractPrefixPathFromFilename(resolvedFilename);

    auto& includePaths = options->includePaths();

    auto includePathIt = std::find_if(
        includePaths.begin(),
        includePaths.end(),
        [&](const std::string& includePath) -> bool { return includePath == scenePath; }
    );

    if (includePathIt == includePaths.end())
        includePaths.push_front(scenePath);

    _dependency = Dependency::create();

	_dependency->options(options);

    parseHeader(filename, resolvedFilename, options, data, assetLibrary);
}

void
SceneParser::parseHeader(const std::string&					filename,
				         const std::string&					resolvedFilename,
				         std::shared_ptr<Options>			options,
				         const std::vector<unsigned char>&	data,
				         AssetLibraryPtr					assetLibrary)
{
    if (!readHeader(filename, data))
    {
        _error->execute(shared_from_this(), Error("SceneParsingError", "Failed to parse header: " + filename));

        return;
    }

    const auto embedContentOffset = this->embedContentOffset();
    const auto embedContentLength = this->embedContentLength();

    if (data.size() >= embedContentOffset + embedContentLength)
    {
        const auto embedContentDataBegin = data.begin() + embedContentOffset;
        const auto embedContentDataEnd = embedContentDataBegin + embedContentLength;

        parseEmbedContent(
            filename,
            resolvedFilename,
            options,
            std::vector<unsigned char>(embedContentDataBegin, embedContentDataEnd),
            assetLibrary
        );

        return;
    }

    auto embedContentLoader = Loader::create();
    auto embedContentOptions = options->clone();

    embedContentLoader->options(embedContentOptions);

    embedContentOptions
        ->seekingOffset(embedContentOffset)
        ->seekedLength(embedContentLength)
        ->storeDataIfNotParsed(false)
        ->parserFunction([](const std::string& extension) -> AbstractParser::Ptr
        {
            return nullptr;
        });

    _embedContentLoaderCompleteSlot = embedContentLoader->complete()->connect(
        [=](Loader::Ptr embedContentLoaderThis) -> void
        {
            _embedContentLoaderCompleteSlot = nullptr;

            parseEmbedContent(
                filename,
                resolvedFilename,
                embedContentOptions,
                embedContentLoaderThis->files().at(filename)->data(),
                assetLibrary
            );
        }
    );

    embedContentLoader
        ->queue(filename)
        ->load();
}

void
SceneParser::parseEmbedContent(const std::string&					filename,
				               const std::string&					resolvedFilename,
				               std::shared_ptr<Options>				options,
				               const std::vector<unsigned char>&	data,
				               AssetLibraryPtr					    assetLibrary)
{
	std::string 		folderPath = extractFolderPath(resolvedFilename);

    msgpack::type::tuple<std::vector<std::string>, std::vector<SerializedNode>> dst;

    extractDependencies(assetLibrary, data, 0, _dependencySize, options, folderPath);

    unpack(dst, data, _sceneDataSize, _dependencySize);

    assetLibrary->symbol(filename, parseNode(dst.get<1>(), dst.get<0>(), assetLibrary, options));

    if (_jobList.size() > 0)
    {
        auto jobManager = component::JobManager::create(30);

        for (auto it = _jobList.begin(); it != _jobList.end(); ++it)
            jobManager->pushJob(*it);

        assetLibrary->symbol(filename)->addComponent(jobManager);
    }

    complete()->execute(shared_from_this());
}

scene::Node::Ptr
SceneParser::parseNode(std::vector<SerializedNode>&            nodePack,
                       std::vector<std::string>&            componentPack,
                       AssetLibraryPtr                        assetLibrary,
                       Options::Ptr                            options)
{
    scene::Node::Ptr                                    root;
    std::queue<std::tuple<scene::Node::Ptr, uint>>        nodeStack;
    std::map<int, std::vector<scene::Node::Ptr>>        componentIdToNodes;
    std::map<scene::Node::Ptr, scene::Node::Ptr>        nodeToParentMap;

    for (uint i = 0; i < nodePack.size(); ++i)
    {
        uint                layouts            = nodePack[i].get<1>();
        uint                numChildren        = nodePack[i].get<2>();
        std::vector<uint>    componentsId    = nodePack[i].get<3>();
        std::string            uuid            = nodePack[i].get<4>();

        scene::Node::Ptr    newNode            = scene::Node::create(uuid, "");

		newNode->layout(layouts);
		newNode->name(nodePack[i].get<0>());

        for (uint componentId : componentsId)
            componentIdToNodes[componentId].push_back(newNode);

        if (nodeStack.size() == 0)
        {
            root = newNode;

            nodeToParentMap.insert(std::make_pair(root, nullptr));
        }
        else
        {
            scene::Node::Ptr parent = std::get<0>(nodeStack.front());

            std::get<1>(nodeStack.front())--;

            if (std::get<1>(nodeStack.front()) == 0)
                nodeStack.pop();

            nodeToParentMap.insert(std::make_pair(newNode, parent));
        }

        if (numChildren > 0)
            nodeStack.push(std::make_tuple(newNode, numChildren));
    }

    for (auto nodeToParentPair : nodeToParentMap)
    {
        auto node = nodeToParentPair.first;
        auto parent = nodeToParentPair.second;

        if (parent != nullptr)
            parent->addChild(node);
    }

	_dependency->loadedRoot(root);

    std::set<uint> markedComponent;

    for (uint componentIndex = 0; componentIndex < componentPack.size(); ++componentIndex)
    {
        int8_t            dst = componentPack[componentIndex].at(componentPack[componentIndex].size() - 1);

        if (dst == serialize::SKINNING || dst == serialize::MASTER_ANIMATION)
            markedComponent.insert(componentIndex);
        else
        {
            if (_componentIdToReadFunction.find(dst) != _componentIdToReadFunction.end())
            {
                std::shared_ptr<component::AbstractComponent> newComponent = _componentIdToReadFunction[dst](_version, componentPack[componentIndex], assetLibrary, _dependency);

                for (scene::Node::Ptr node : componentIdToNodes[componentIndex])
                {
                    node->addComponent(newComponent);

                    if (std::dynamic_pointer_cast<component::Metadata>(newComponent) != nullptr)
                        handleMetadata(node, std::dynamic_pointer_cast<component::Metadata>(newComponent), options);
                }
            }
        }
    }

    bool isSkinningFree = true; // FIXME

	for (auto componentIndex : markedComponent)
	{
        if (_version.major <= 0 && _version.minor < 3)
            continue;

        int8_t dst = componentPack[componentIndex].at(componentPack[componentIndex].size() - 1);

		isSkinningFree = false;
        std::shared_ptr<component::AbstractComponent> newComponent = _componentIdToReadFunction[dst](_version, componentPack[componentIndex], assetLibrary, _dependency);

        for (scene::Node::Ptr node : componentIdToNodes[componentIndex])
		{
            node->addComponent(newComponent);

            if (!node->hasComponent<component::MasterAnimation>())
			    node->addComponent(component::MasterAnimation::create());

            if (std::dynamic_pointer_cast<component::Metadata>(newComponent) != nullptr)
                handleMetadata(node, std::dynamic_pointer_cast<component::Metadata>(newComponent), options);
		}
    }

    if (isSkinningFree)
    {
        auto nodeSet = scene::NodeSet::create(root)->descendants(true)->where([](scene::Node::Ptr n){ return !n->components<component::Surface>().empty() && n->components<component::BoundingBox>().empty(); });

        for (auto n : nodeSet->nodes())
        {
            n->addComponent(component::BoundingBox::create());
        }
    }

    for (auto nodeToParentPair : nodeToParentMap)
    {
        auto node = nodeToParentPair.first;

        auto newNode = options->nodeFunction()(node);

        if (newNode != node)
        {
            auto parent = node->parent();
            parent->removeChild(node);
            parent->addChild(newNode);
        }
    }

    return root;
}

void
SceneParser::handleMetadata(std::shared_ptr<scene::Node> node,
                            std::shared_ptr<component::Metadata> metadata,
                            std::shared_ptr<file::Options> options)
{
    auto data = metadata->data();

    for (auto it = data.begin(); it != data.end(); ++it)
    {
        auto metadataName = it->first;
        auto metadataValue = it->second;

        options->attributeFunction()(node, metadataName, metadataValue);
    }
}
