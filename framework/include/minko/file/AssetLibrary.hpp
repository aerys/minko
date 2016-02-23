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

#pragma once

#include "minko/Common.hpp"

#include "minko/file/Loader.hpp"
#include "minko/Signal.hpp"
#include "minko/file/AbstractParser.hpp"
#include "minko/file/EffectParser.hpp"
#include "minko/scene/Layout.hpp"
#include "minko/audio/Sound.hpp"

namespace minko
{
    namespace file
    {
        class AssetLibrary :
            public std::enable_shared_from_this<AssetLibrary>
        {
        public:
            typedef std::shared_ptr<AssetLibrary>                           Ptr;

        private:
            typedef std::shared_ptr<render::AbstractContext>                AbsContextPtr;
            typedef std::shared_ptr<render::Effect>                         EffectPtr;
            typedef std::shared_ptr<render::AbstractTexture>                AbsTexturePtr;
            typedef std::shared_ptr<render::Texture>                        TexturePtr;
            typedef std::shared_ptr<render::CubeTexture>                    CubeTexturePtr;
            typedef std::shared_ptr<render::RectangleTexture>               RectangleTexturePtr;
            typedef std::shared_ptr<geometry::Geometry>                     GeometryPtr;
            typedef std::shared_ptr<file::AbstractParser>                   AbsParserPtr;
            typedef std::shared_ptr<file::Loader>                           LoaderPtr;
            typedef std::shared_ptr<scene::Node>                            NodePtr;
            typedef std::shared_ptr<component::AbstractScript>              AbsScriptPtr;
			typedef std::shared_ptr<material::Material>		                MaterialPtr;
            typedef std::shared_ptr<audio::Sound>                           SoundPtr;
            typedef std::shared_ptr<file::AbstractAssetDescriptor>          AbstractAssetDescriptorPtr;

        private:
            AbsContextPtr                                                   _context;
            std::shared_ptr<Loader>                                         _loader;

            std::unordered_map<std::string, MaterialPtr>                    _materials;
            std::unordered_map<std::string, GeometryPtr>                    _geometries;
            std::unordered_map<std::string, EffectPtr>                      _effects;
            std::unordered_map<std::string, TexturePtr>                     _textures;
            std::unordered_map<std::string, CubeTexturePtr>                 _cubeTextures;
            std::unordered_map<std::string, RectangleTexturePtr>            _rectangleTextures;
            std::unordered_map<std::string, NodePtr>                        _symbols;
            std::unordered_map<std::string, std::vector<unsigned char>>     _blobs;
            std::unordered_map<std::string, AbsScriptPtr>                   _scripts;
            std::unordered_map<std::string, scene::Layout>				    _layouts;
            std::unordered_map<std::string, SoundPtr>                       _sounds;
            std::unordered_map<std::string, AbstractAssetDescriptorPtr>     _assetDescriptors;

            Signal<Ptr, std::shared_ptr<AbstractParser>>::Ptr               _parserError;
            Signal<Ptr>::Ptr                                                _ready;

        public:
            static
            Ptr
            create(AbsContextPtr context);

            static
            Ptr
            create(Ptr);

            inline
            uint
            numGeometries()
            {
                return _geometries.size();
            }

            inline
            uint
            numMaterials()
            {
                return _materials.size();
            }

            inline
            uint
			numEffects()
			{
				return _effects.size();
			}

			inline
			uint
            numTextures()
            {
                return _textures.size();
            }

            inline
            AbsContextPtr
            context()
            {
                return _context;
            }

            inline
            std::shared_ptr<Loader>
            loader()
            {
                return _loader;
            }

            void
            disposeLoader();

            GeometryPtr
            geometry(const std::string& name);

            Ptr
            geometry(const std::string& name, GeometryPtr geometry);

            const std::string&
            geometryName(GeometryPtr geometry);

            Ptr
            texture(const std::string& name, TexturePtr texture);

            TexturePtr
            texture(const std::string& name) const;

            std::shared_ptr<render::AbstractTexture>
            getTextureByUuid(const std::string& uuid, bool failIfNotReady = true);

            Ptr
            cubeTexture(const std::string& name, CubeTexturePtr texture);

            RectangleTexturePtr
            rectangleTexture(const std::string& name) const;

            Ptr
            rectangleTexture(const std::string& name, RectangleTexturePtr texture);

            CubeTexturePtr
            cubeTexture(const std::string& name) const;

            const std::string&
            textureName(AbsTexturePtr texture);

            std::shared_ptr<material::Material>
            material(const std::string& name);

            Ptr
            material(const std::string& name, MaterialPtr material);

            const std::string&
            materialName(MaterialPtr material);

            NodePtr
            symbol(const std::string& name);

            Ptr
            symbol(const std::string& name, NodePtr symbol);

            const std::string&
            symbolName(NodePtr node);

            EffectPtr
            effect(const std::string& name);

            Ptr
            effect(const std::string& name, EffectPtr effect);

            const std::string&
            effectName(EffectPtr effect);

            const std::vector<unsigned char>&
            blob(const std::string& name);

            Ptr
            blob(const std::string& name, const std::vector<unsigned char>& blob);

            inline
            bool
            hasBlob(const std::string& name)
            {
                return _blobs.count(name) != 0;
            }

            AbsScriptPtr
            script(const std::string& name);

            AssetLibrary::Ptr
            script(const std::string& name, AbsScriptPtr script);

            const std::string&
            scriptName(AbsScriptPtr script);

            scene::Layout
            layout(const std::string& name);

            Ptr
			layout(const std::string& name, scene::Layout);

            audio::Sound::Ptr
            sound(const std::string& name);

            AssetLibrary::Ptr
            sound(const std::string& name, audio::Sound::Ptr sound);

            template <typename T>
            typename std::enable_if<std::is_base_of<AbstractAssetDescriptor, T>::value, std::shared_ptr<T>>::type
            assetDescriptor(const std::string& name)
            {
                auto assetDescriptorIt = _assetDescriptors.find(name);

                if (assetDescriptorIt == _assetDescriptors.end())
                    return nullptr;

                return std::static_pointer_cast<T>(assetDescriptorIt->second);
            }

            Ptr
            assetDescriptor(const std::string& name, AbstractAssetDescriptorPtr assetDescriptor);

        private:
            AssetLibrary(AbsContextPtr context);
        };
    }
}
