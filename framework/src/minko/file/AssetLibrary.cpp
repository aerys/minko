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

#include "minko/file/AssetLibrary.hpp"

#include "minko/material/Material.hpp"
#include "minko/scene/Node.hpp"
#include "minko/file/AbstractLoader.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/AbstractParser.hpp"
#include "minko/file/EffectParser.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"
#include "minko/render/Effect.hpp"
#include "minko/geometry/Geometry.hpp"

using namespace minko;
using namespace minko::render;
using namespace minko::geometry;
using namespace minko::file;

AssetLibrary::Ptr
AssetLibrary::create(AbsContextPtr context)
{
	auto al = std::shared_ptr<AssetLibrary>(new AssetLibrary(context));

	al->registerParser<file::EffectParser>("effect");

	return al;
}

AssetLibrary::AssetLibrary(std::shared_ptr<AbstractContext> context) :
	_context(context),
	_defaultOptions(file::Options::create(context)),
	_complete(Signal<Ptr>::create())
{
}

AssetLibrary::GeometryPtr
AssetLibrary::geometry(const std::string& name)
{
	return _geometries.count(name) ? _geometries[name] : nullptr;
}

AssetLibrary::Ptr
AssetLibrary::geometry(const std::string& name, std::shared_ptr<Geometry> geometry)
{
	_geometries[name] = geometry;

	return shared_from_this();
}

const std::string&
AssetLibrary::geometryName(GeometryPtr geometry)
{
	auto it = std::find_if(_geometries.begin(), _geometries.end(), [&](std::pair<std::string, GeometryPtr> itr) -> bool
	{
		return itr.second == geometry;
	});

	if (it != _geometries.end())
		return it->first;

	throw new std::logic_error("AssetLibrary does not reference this geometry.");
}

render::AbstractTexture::Ptr
AssetLibrary::texture(const std::string& name) const
{
	const auto foundTextureIt = _textures.find(name);

	return foundTextureIt != _textures.end()
		? foundTextureIt->second
		: nullptr;
}

AssetLibrary::Ptr
AssetLibrary::texture(const std::string& name, render::AbstractTexture::Ptr texture)
{
	_textures[name] = texture;

	return shared_from_this();
}

const std::string&
AssetLibrary::textureName(render::AbstractTexture::Ptr texture)
{
	auto it = std::find_if(
		_textures.begin(), 
		_textures.end(), 
		[&](std::pair<std::string, render::AbstractTexture::Ptr> itr) -> bool
	{
		return itr.second == texture;
	});

	if (it != _textures.end())
		return it->first;

	throw new std::logic_error("AssetLibrary does not reference this texture.");
}

scene::Node::Ptr
AssetLibrary::symbol(const std::string& name)
{
	return _symbols.count(name) ? _symbols[name] : nullptr;
}

AssetLibrary::Ptr
AssetLibrary::symbol(const std::string& name, scene::Node::Ptr node)
{
	_symbols[name] = node;

	return shared_from_this();
}

const std::string&
AssetLibrary::symbolName(NodePtr node)
{
	auto it = std::find_if(_symbols.begin(), _symbols.end(), [&](std::pair<std::string, NodePtr> itr) -> bool
	{
		return itr.second == node;
	});

	if (it != _symbols.end())
		return it->first;

	throw new std::logic_error("AssetLibrary does not reference this symbol.");
}

material::Material::Ptr
AssetLibrary::material(const std::string& name)
{
	return _materials.count(name) ? std::dynamic_pointer_cast<material::Material>(_materials[name]) : nullptr;
}

AssetLibrary::Ptr
AssetLibrary::material(const std::string& name, MaterialPtr material)
{
	material::Material::Ptr mat = std::dynamic_pointer_cast<material::Material>(material);

#ifdef DEBUG
	if (mat == nullptr)
		throw std::invalid_argument("material");
#endif

	_materials[name] = material;

	return shared_from_this();
}

const std::string&
AssetLibrary::materialName(MaterialPtr material)
{
	auto it = std::find_if(_materials.begin(), _materials.end(), [&](std::pair<std::string, MaterialPtr> itr) -> bool
	{
		return itr.second == material;
	});

	if (it != _materials.end())
		return it->first;

	throw new std::logic_error("AssetLibrary does not reference this material.");
}

AssetLibrary::EffectPtr
AssetLibrary::effect(const std::string& name)
{
	return _effects.count(name) ? _effects[name] : nullptr;
}

AssetLibrary::Ptr
AssetLibrary::effect(const std::string& name, std::shared_ptr<Effect> effect)
{
	_effects[name] = effect;

	return shared_from_this();
}

const std::string&
AssetLibrary::effectName(EffectPtr effect)
{
	auto it = std::find_if(_effects.begin(), _effects.end(), [&](std::pair<std::string, EffectPtr> itr) -> bool
	{
		return itr.second == effect;
	});

	if (it != _effects.end())
		return it->first;

	throw new std::logic_error("AssetLibrary does not reference this effect.");
}

const std::vector<unsigned char>&
AssetLibrary::blob(const std::string& name)
{
	if (!_blobs.count(name))
		throw;

	return _blobs[name];
}

AssetLibrary::Ptr
AssetLibrary::blob(const std::string& name, const std::vector<unsigned char>& blob)
{
	_blobs[name] = blob;

	return shared_from_this();
}

AssetLibrary::AbsScriptPtr
AssetLibrary::script(const std::string& name)
{
    return _scripts.count(name) ? _scripts[name] : nullptr;
}

AssetLibrary::Ptr
AssetLibrary::script(const std::string& name, AbsScriptPtr script)
{
    _scripts[name] = script;

    return shared_from_this();
}

const std::string&
AssetLibrary::scriptName(AbsScriptPtr script)
{
	auto it = std::find_if(_scripts.begin(), _scripts.end(), [&](std::pair<std::string, AbsScriptPtr> itr) -> bool
	{
		return itr.second == script;
	});

	if (it != _scripts.end())
		return it->first;

	throw new std::logic_error("AssetLibrary does not reference this script.");
}

const unsigned int
AssetLibrary::layout(const std::string& name)
{
	if (_layouts.count(name) == 0)
	{
		unsigned int existingMask = 0;

		for (auto layout : _layouts)
			existingMask |= layout.second;

		auto mask = 1;
		for (auto i = 0; i < 32 && (existingMask & mask); ++i, mask <<= 1)
			continue;

		if (mask == 0)
			throw;

		_layouts[name] = mask;
	}

	return _layouts[name];
}

AssetLibrary::Ptr
AssetLibrary::layout(const std::string& name, const unsigned int mask)
{
	_layouts[name] = mask;

	return shared_from_this();
}

AssetLibrary::Ptr
AssetLibrary::queue(const std::string& filename)
{
	return queue(filename, nullptr, nullptr);
}

AssetLibrary::Ptr
AssetLibrary::queue(const std::string& filename, std::shared_ptr<file::Options> options)
{
	return queue(filename, options, nullptr);
}

AssetLibrary::Ptr
AssetLibrary::queue(const std::string&						filename,
				    std::shared_ptr<file::Options>			options,
					std::shared_ptr<file::AbstractLoader>	loader)
{
	_filesQueue.push_back(filename);

	if (options)
		_filenameToOptions[filename] = options;

	if (loader)
		_filenameToLoader[filename] = loader;

	return shared_from_this();
}


AssetLibrary::Ptr
AssetLibrary::load(bool	executeCompleteSignal)
{
	std::list<std::string> queue(_filesQueue);

	if (queue.empty())
	{
		if (executeCompleteSignal)
			_complete->execute(shared_from_this());
	}
	else
	{
		for (auto& filename : queue)
		{
			auto options = _filenameToOptions.count(filename)
				? _filenameToOptions[filename]
				: _filenameToOptions[filename] = _defaultOptions;
			auto loader = _filenameToLoader.count(filename)
				? _filenameToLoader[filename]
				: _filenameToLoader[filename] = options->loaderFunction()(filename);

			_filesQueue.erase(std::find(_filesQueue.begin(), _filesQueue.end(), filename));
			_loading.push_back(filename);

			_loaderSlots.push_back(loader->error()->connect(std::bind(
				&AssetLibrary::loaderErrorHandler, shared_from_this(), std::placeholders::_1
			)));
			_loaderSlots.push_back(loader->complete()->connect(std::bind(
				&AssetLibrary::loaderCompleteHandler, shared_from_this(), std::placeholders::_1
			)));
			loader->load(filename, options);
		}
	}

	return shared_from_this();
}

void
AssetLibrary::loaderErrorHandler(std::shared_ptr<file::AbstractLoader> loader)
{
	auto filename = loader->filename();

	std::cerr << "error: AssetLibrary::loaderErrorHandler(): " << filename << std::endl;

	throw std::invalid_argument(filename);
}

void
AssetLibrary::loaderCompleteHandler(std::shared_ptr<file::AbstractLoader> loader)
{
	auto filename = loader->filename();
	auto extension = filename.substr(filename.find_last_of('.') + 1);

	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	if (_parsers.count(extension))
	{
		auto parser = _parsers[extension]();
		auto completeSlot = parser->complete()->connect([=](AbstractParser::Ptr)
		{
			loader->parserComplete()->execute(loader, parser, shared_from_this());

			finalize(filename);
		});

		parser->parse(
            loader->filename(),
            loader->resolvedFilename(),
            _filenameToOptions[filename],
            loader->data(),
            shared_from_this()
        );
	}
	else
	{
		std::cerr << "warning: no parser found for file extesntion '" << extension << "'" << std::endl;
		blob(filename, loader->data());
		finalize(filename);
	}
}

void
AssetLibrary::finalize(const std::string& filename)
{
	_loading.erase(std::find(_loading.begin(), _loading.end(), filename));
	_filenameToLoader.erase(filename);
	_filenameToOptions.erase(filename);

	if (_loading.size() == 0 && _filesQueue.size() == 0)
	{
		_loaderSlots.clear();
		_filenameToLoader.clear();
		_filenameToOptions.clear();

		_complete->execute(shared_from_this());
	}
}

AssetLibrary::AbsParserPtr
AssetLibrary::parser(std::string extension)
{
	/*
	if ()
		throw std::invalid_argument("No parser found for extension '" + extension + "'");
	*/

	return _parsers.count(extension) == 0 ? nullptr : _parsers[extension]();
}
