/*
Copyright (c) 2013 Aerys

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

#include "AssetsLibrary.hpp"

#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/AbstractParser.hpp"
#include "minko/file/EffectParser.hpp"
#include "minko/file/AbstractTextureParser.hpp"
#include "minko/file/AbstractModelParser.hpp"
#include "minko/resource/Texture.hpp"

using namespace minko::render;
using namespace minko::geometry;

AssetsLibrary::AssetsLibrary(std::shared_ptr<AbstractContext> context) :
	_context(context),
	_complete(Signal<Ptr>::create()),
	_defaultOptions(file::Options::create(context))
{
}

AssetsLibrary::GeometryPtr
AssetsLibrary::geometry(const std::string& name)
{
	return _geometries[name];
}

AssetsLibrary::Ptr
AssetsLibrary::geometry(const std::string& name, std::shared_ptr<Geometry> geometry)
{
	_geometries[name] = geometry;

	return shared_from_this();
}

resource::Texture::Ptr
AssetsLibrary::texture(const std::string& name)
{
	return _textures[name];
}

AssetsLibrary::Ptr
AssetsLibrary::texture(const std::string& name, resource::Texture::Ptr texture)
{
	_textures[name] = texture;

	return shared_from_this();
}

scene::Node::Ptr
AssetsLibrary::node(const std::string& name)
{
	return _nodes[name];
}

AssetsLibrary::Ptr
AssetsLibrary::node(const std::string& name, scene::Node::Ptr node)
{
	_nodes[name] = node;

	return shared_from_this();
}

AssetsLibrary::EffectPtr
AssetsLibrary::effect(const std::string& name)
{
	return _effects[name];
}

AssetsLibrary::Ptr
AssetsLibrary::effect(const std::string& name, std::shared_ptr<Effect> effect)
{
	_effects[name] = effect;

	return shared_from_this();
}

const std::vector<unsigned char>&
AssetsLibrary::blob(const std::string& name)
{
	return _blobs[name];
}

AssetsLibrary::Ptr
AssetsLibrary::blob(const std::string& name, const std::vector<unsigned char>& blob)
{
	_blobs[name] = blob;

	return shared_from_this();
}

AssetsLibrary::Ptr
AssetsLibrary::queue(const std::string& filename, std::shared_ptr<file::Options> options)
{
	_filesQueue.push_back(filename);
	_filenameToOptions[filename] = options ? options : _defaultOptions;

	return shared_from_this();
}

AssetsLibrary::Ptr
AssetsLibrary::load(const std::string& filename, std::shared_ptr<file::Options> options)
{
	queue(filename, options);
	load();

	return shared_from_this();
}

AssetsLibrary::Ptr
AssetsLibrary::load()
{
	std::list<std::string> queue = _filesQueue;

	for (auto& filename : queue)
	{
		if (_filenameToLoader.count(filename) == 0)
		{
			auto loader = file::Loader::create();

			_filenameToLoader[filename] = loader;
			_loaderSlots.push_back(loader->error()->connect(std::bind(
				&AssetsLibrary::loaderErrorHandler, shared_from_this(), std::placeholders::_1
			)));
			_loaderSlots.push_back(loader->complete()->connect(std::bind(
				&AssetsLibrary::loaderCompleteHandler, shared_from_this(), std::placeholders::_1
			)));
			loader->load(filename, _defaultOptions);
		}
	}

	return shared_from_this();
}

void
AssetsLibrary::loaderErrorHandler(std::shared_ptr<file::Loader> loader)
{
	throw std::invalid_argument(loader->filename());
}

void
AssetsLibrary::loaderCompleteHandler(std::shared_ptr<file::Loader> loader)
{
	auto filename = loader->filename();
	auto extension = filename.substr(filename.find_last_of('.') + 1);
	
	std::cout << loader->filename() << std::endl << std::flush;

	_filesQueue.erase(std::find(_filesQueue.begin(), _filesQueue.end(), filename));
	_filenameToLoader.erase(filename);

	if (_parsers.count(extension))
	{
		auto parser = _parsers[extension]();
		
		parser->parse(loader->filename(), _filenameToOptions[filename], loader->data(), shared_from_this());

		/*auto fxParser = std::dynamic_pointer_cast<file::EffectParser>(parser);
		if (fxParser)
			effect(fxParser->effectName(), fxParser->effect());
		
		auto textureParser = std::dynamic_pointer_cast<file::AbstractTextureParser>(parser);
		if (textureParser)
			texture(filename, textureParser->texture());

		auto modelParser = std::dynamic_pointer_cast<file::AbstractModelParser>(parser);
		if (modelParser)
			node(filename, modelParser->node());*/
	}
	else
		blob(filename, loader->data());

	if (_filesQueue.size() == 0)
	{
		_loaderSlots.clear();
		_filenameToLoader.clear();
		_filenameToOptions.clear();

		_complete->execute(shared_from_this());
	}
}
