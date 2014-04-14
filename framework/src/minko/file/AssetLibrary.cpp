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
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
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

    al->_loader->options()->context(context);
    al->_loader->options()->assetLibrary(al);

    return al;
}

AssetLibrary::AssetLibrary(std::shared_ptr<AbstractContext> context) :
	_context(context),
    _loader(Loader::create())
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

	return std::enable_shared_from_this<AssetLibrary>::shared_from_this();
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

	throw std::logic_error("AssetLibrary does not reference this geometry.");
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

	return std::enable_shared_from_this<AssetLibrary>::shared_from_this();
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

	throw std::logic_error("AssetLibrary does not reference this texture.");
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

	return std::enable_shared_from_this<AssetLibrary>::shared_from_this();
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

	throw std::logic_error("AssetLibrary does not reference this symbol.");
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

	return std::enable_shared_from_this<AssetLibrary>::shared_from_this();
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

	throw std::logic_error("AssetLibrary does not reference this material.");
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

	return std::enable_shared_from_this<AssetLibrary>::shared_from_this();
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

	throw std::logic_error("AssetLibrary does not reference this effect.");
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

	return std::enable_shared_from_this<AssetLibrary>::shared_from_this();
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

    return std::enable_shared_from_this<AssetLibrary>::shared_from_this();
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

	throw std::logic_error("AssetLibrary does not reference this script.");
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

	return std::enable_shared_from_this<AssetLibrary>::shared_from_this();
}
