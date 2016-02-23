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

#include "minko/material/Material.hpp"
#include "minko/scene/Node.hpp"
#include "minko/file/AbstractAssetDescriptor.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"
#include "minko/render/RectangleTexture.hpp"
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

AssetLibrary::Ptr
AssetLibrary::create(AssetLibrary::Ptr original)
{
    auto al = create(original->_context);
        
    for (auto it = original->_materials.begin(); it != original->_materials.end(); ++it)
        al->_materials[it->first] = it->second;

    for (auto it = original->_geometries.begin(); it != original->_geometries.end(); ++it)
        al->_geometries[it->first] = it->second;

    for (auto it = original->_effects.begin(); it != original->_effects.end(); ++it)
        al->_effects[it->first] = it->second;

    for (auto it = original->_textures.begin(); it != original->_textures.end(); ++it)
        al->_textures[it->first] = it->second;
    
    for (auto it = original->_cubeTextures.begin(); it != original->_cubeTextures.end(); ++it)
        al->_cubeTextures[it->first] = it->second;

    for (auto it = original->_rectangleTextures.begin(); it != original->_rectangleTextures.end(); ++it)
        al->_rectangleTextures[it->first] = it->second;

    for (auto it = original->_symbols.begin(); it != original->_symbols.end(); ++it)
        al->_symbols[it->first] = it->second;

    for (auto it = original->_blobs.begin(); it != original->_blobs.end(); ++it)
        al->_blobs[it->first] = it->second;

    for (auto it = original->_scripts.begin(); it != original->_scripts.end(); ++it)
        al->_scripts[it->first] = it->second;

    for (auto it = original->_layouts.begin(); it != original->_layouts.end(); ++it)
        al->_layouts[it->first] = it->second;

    for (const auto& assetDescriptor : original->_assetDescriptors)
        al->_assetDescriptors.emplace(assetDescriptor.first, assetDescriptor.second);

    return al;
}

AssetLibrary::AssetLibrary(std::shared_ptr<AbstractContext> context) :
    _context(context),
    _loader(Loader::create())
{
}

void
AssetLibrary::disposeLoader()
{
    _loader = nullptr;
}

AssetLibrary::GeometryPtr
AssetLibrary::geometry(const std::string& name)
{
    return _geometries.count(name) ? _geometries[name] : nullptr;
}

AssetLibrary::Ptr
AssetLibrary::geometry(const std::string& name, std::shared_ptr<Geometry> geometry)
{
    std::string tempname = name;
    if (_geometries[tempname])
    {
        std::stringstream ss; 
        ss << _geometries.size();
        tempname = tempname + "_" + ss.str();
    }

    _geometries[tempname] = geometry;

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

render::Texture::Ptr
AssetLibrary::texture(const std::string& name) const
{
    const auto foundTextureIt = _textures.find(name);

    return foundTextureIt != _textures.end()
        ? foundTextureIt->second
        : nullptr;
}

AssetLibrary::Ptr
AssetLibrary::texture(const std::string& name, render::Texture::Ptr texture)
{
    _textures[name] = texture;

    return std::enable_shared_from_this<AssetLibrary>::shared_from_this();
}

std::shared_ptr<render::AbstractTexture>
AssetLibrary::getTextureByUuid(const std::string& uuid, bool failIfNotReady)
{
    auto it = std::find_if(_textures.begin(), _textures.end(), [&](const std::pair<std::string, TexturePtr>& t)
    {
        return t.second->uuid() == uuid && (!failIfNotReady || t.second->isReady());
    });

    if (it == _textures.end())
        return nullptr;

    return it->second;
}

render::CubeTexture::Ptr
AssetLibrary::cubeTexture(const std::string& name) const
{
    const auto foundTextureIt = _cubeTextures.find(name);

    return foundTextureIt != _cubeTextures.end()
        ? foundTextureIt->second
        : nullptr;
}

AssetLibrary::Ptr
AssetLibrary::cubeTexture(const std::string& name, render::CubeTexture::Ptr texture)
{
    _cubeTextures[name] = texture;

    return std::enable_shared_from_this<AssetLibrary>::shared_from_this();
}

render::RectangleTexture::Ptr
AssetLibrary::rectangleTexture(const std::string& name) const
{
    const auto foundTextureIt = _rectangleTextures.find(name);

    return foundTextureIt != _rectangleTextures.end()
        ? foundTextureIt->second
        : nullptr;
}

AssetLibrary::Ptr
AssetLibrary::rectangleTexture(const std::string& name, render::RectangleTexture::Ptr texture)
{
    _rectangleTextures[name] = texture;

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

scene::Layout
AssetLibrary::layout(const std::string& name)
{
    if (_layouts.count(name) == 0)
    {
        scene::Layout existingMask = 0;

        for (auto layout : _layouts)
            existingMask |= layout.second;

        scene::Layout mask = 1;
        for (auto i = 0; i < 32 && (existingMask & mask); ++i, mask <<= 1)
            continue;

        if (mask == 0)
            throw;

        _layouts[name] = mask;
    }

    return _layouts[name];
}

AssetLibrary::Ptr
AssetLibrary::layout(const std::string& name, scene::Layout mask)
{
    _layouts[name] = mask;

    return std::enable_shared_from_this<AssetLibrary>::shared_from_this();
}

audio::Sound::Ptr
AssetLibrary::sound(const std::string& name)
{
    return _sounds[name];
}

AssetLibrary::Ptr
AssetLibrary::sound(const std::string& name, audio::Sound::Ptr sound)
{
    _sounds[name] = sound;

    return std::enable_shared_from_this<AssetLibrary>::shared_from_this();
}

AssetLibrary::Ptr
AssetLibrary::assetDescriptor(const std::string& name, AbstractAssetDescriptor::Ptr assetDescriptor)
{
    _assetDescriptors[name] = assetDescriptor;

    return std::enable_shared_from_this<AssetLibrary>::shared_from_this();
}
