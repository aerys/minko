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
#include "minko/Flyweight.hpp"
#include "minko/SerializerCommon.hpp"
#include "msgpack.hpp"

namespace minko
{
	namespace file
	{
		class Dependency :
			public std::enable_shared_from_this<Dependency>
		{
		public:
			typedef std::shared_ptr<Dependency>					    Ptr;
            typedef std::weak_ptr<Dependency>                       WeakPtr;
			typedef std::shared_ptr<render::AbstractTexture>	    AbsTexturePtr;
			typedef std::shared_ptr<geometry::Geometry>			    GeometryPtr;
			typedef std::shared_ptr<material::Material>			    MaterialPtr;
            typedef std::shared_ptr<data::Provider>                 ProviderPtr;

			typedef msgpack::type::tuple<uint, DependencyId, std::string>   SerializedAsset;

            struct TextureDependency
            {
                DependencyId            dependencyId;
                AbsTexturePtr    		texture;
                Flyweight<std::string>  textureType;

                TextureDependency() :
                	dependencyId(0u),
                	texture(),
                	textureType()
            	{
            	}
            };

            struct TextureReference
            {
            	AbsTexturePtr 				    texture;
            	Flyweight<std::string> 		    textureType;
            	std::unordered_set<ProviderPtr>	dependentMaterialDataSet;

            	TextureReference() :
            		texture(),
            		textureType(),
            		dependentMaterialDataSet()
        		{
        		}
            };

		private:
			typedef std::function<SerializedAsset(std::shared_ptr<file::Dependency>, std::shared_ptr<file::AssetLibrary>, std::shared_ptr<geometry::Geometry>, uint, std::shared_ptr<file::Options>, std::shared_ptr<file::WriterOptions>, std::vector<Dependency::SerializedAsset>&)>		GeometryWriterFunction;
            typedef std::function<SerializedAsset(std::shared_ptr<file::Dependency>, std::shared_ptr<file::AssetLibrary>, const TextureDependency&, std::shared_ptr<file::Options>, std::shared_ptr<file::WriterOptions>)>	TextureWriterFunction;
            typedef std::function<SerializedAsset(std::shared_ptr<file::Dependency>, std::shared_ptr<file::AssetLibrary>, std::shared_ptr<material::Material>, uint, std::shared_ptr<file::Options>, std::shared_ptr<file::WriterOptions>)>			MaterialWriterFunction;
			typedef std::function<bool(std::shared_ptr<geometry::Geometry>)>			GeometryTestFunc;

		private:
            WeakPtr                                                                 _parent;

			std::unordered_map<AbsTexturePtr, TextureDependency>			        _textureDependencies;
			std::unordered_map<std::shared_ptr<material::Material>, DependencyId>	_materialDependencies;
			std::unordered_map<std::shared_ptr<scene::Node>, DependencyId>			_subSceneDependencies;
			std::unordered_map<std::shared_ptr<geometry::Geometry>, DependencyId>	_geometryDependencies;
			std::unordered_map<std::shared_ptr<render::Effect>, DependencyId>		_effectDependencies;
			std::unordered_map<std::shared_ptr<LinkedAsset>, DependencyId>		    _linkedAssetDependencies;

			std::unordered_map<DependencyId, TextureReference>						_textureReferences;
			std::unordered_map<DependencyId, std::shared_ptr<material::Material>>	_materialReferences;
			std::unordered_map<DependencyId, std::shared_ptr<scene::Node>>			_subSceneReferences;
			std::unordered_map<DependencyId, std::shared_ptr<geometry::Geometry>>	_geometryReferences;
			std::unordered_map<DependencyId, std::shared_ptr<render::Effect>>		_effectReferences;
			std::unordered_map<DependencyId, std::shared_ptr<LinkedAsset>>          _linkedAssetReferences;

			DependencyId															_currentId;
			std::shared_ptr<Options>										        _options;
			std::shared_ptr<scene::Node>									        _loadedRoot;

			static std::unordered_map<uint, GeometryWriterFunction>			        _geometryWriteFunctions;
			static std::unordered_map<uint, GeometryTestFunc>				        _geometryTestFunctions;

			static TextureWriterFunction									        _textureWriteFunction;
			static MaterialWriterFunction									        _materialWriteFunction;

		public:
            static
			Ptr
			create()
			{
				return std::shared_ptr<Dependency>(new Dependency());
			}

            static
            Ptr
            create(Ptr parent)
            {
                auto instance = create();

                instance->_parent = parent;

                return instance;
            }

			inline
			std::shared_ptr<scene::Node>
			loadedRoot()
			{
				return _loadedRoot;
			}

			inline
			void
			loadedRoot(std::shared_ptr<scene::Node> value)
			{
				_loadedRoot = value;
			}

			inline
			std::shared_ptr<Options>
			options()
			{
				return _options;
			}

			inline
			void
			options(std::shared_ptr<Options> value)
			{
				_options = value;
			}

			bool
			hasDependency(std::shared_ptr<geometry::Geometry> geometry);

			DependencyId
			registerDependency(std::shared_ptr<geometry::Geometry> geometry);

			bool
			hasDependency(std::shared_ptr<material::Material> material);

			DependencyId
			registerDependency(std::shared_ptr<material::Material> material);

			bool
			hasDependency(AbsTexturePtr texture);

			DependencyId
			registerDependency(AbsTexturePtr texture, const std::string& textureType);

			bool
			hasDependency(std::shared_ptr<scene::Node> subScene);

			DependencyId
			registerDependency(std::shared_ptr<scene::Node> subScene);

			bool
			hasDependency(std::shared_ptr<render::Effect> effect);

            bool
			hasDependency(std::shared_ptr<LinkedAsset> linkedAsset);

			DependencyId
			registerDependency(std::shared_ptr<render::Effect> effect);

            DependencyId
            registerDependency(std::shared_ptr<LinkedAsset> linkedAsset);

			std::shared_ptr<geometry::Geometry>
			getGeometryReference(DependencyId geometryId);

			void
			registerReference(DependencyId referenceId, std::shared_ptr<geometry::Geometry> geometry);

            std::shared_ptr<material::Material>
			getMaterialReference(DependencyId materialId);

			void
            registerReference(DependencyId referenceId, std::shared_ptr<material::Material> material);

			TextureReference*
			getTextureReference(DependencyId textureId);

			void
			registerReference(DependencyId referenceId, AbsTexturePtr texture);

			std::shared_ptr<scene::Node>
			getSubsceneReference(DependencyId subSceneId);

			void
			registerReference(DependencyId referenceId, std::shared_ptr<render::Effect> effect);

			std::shared_ptr<render::Effect>
			getEffectReference(DependencyId effectId);

			void
			registerReference(DependencyId referenceId, std::shared_ptr<scene::Node> subScene);

            void
            registerReference(DependencyId referenceId, std::shared_ptr<LinkedAsset> linkedAsset);

            std::shared_ptr<LinkedAsset>
            getLinkedAssetReference(DependencyId id);

			std::vector<SerializedAsset>
			serialize(const std::string&                        parentFilename,
                      std::shared_ptr<file::AssetLibrary>       assetLibrary,
					  std::shared_ptr<file::Options>            options,
                      std::shared_ptr<file::WriterOptions>      writerOptions,
                      std::vector<std::vector<unsigned char>>&  internalLinkedAssets);

			static
			SerializedAsset
			serializeGeometry(std::shared_ptr<file::Dependency>			dependencies,
							  std::shared_ptr<file::AssetLibrary>		assetLibrary,
							  std::shared_ptr<geometry::Geometry>		geometry,
							  DependencyId								resourceId,
							  std::shared_ptr<file::Options>			options,
                              std::shared_ptr<file::WriterOptions>		writerOptions,
							  std::vector<Dependency::SerializedAsset>&	includeDependencies);

			static
			SerializedAsset
			serializeTexture(std::shared_ptr<file::Dependency>			dependecies,
							 std::shared_ptr<file::AssetLibrary>		assetLibrary,
							 const TextureDependency&                   textureDependency,
							 std::shared_ptr<file::Options>				options,
                             std::shared_ptr<file::WriterOptions>       writerOptions);

			static
			SerializedAsset
			serializeMaterial(std::shared_ptr<file::Dependency>		dependecies,
							  std::shared_ptr<file::AssetLibrary>	assetLibrary,
                              std::shared_ptr<material::Material>	material,
							  DependencyId						    resourceId,
							  std::shared_ptr<file::Options>		options,
                              std::shared_ptr<file::WriterOptions>  writerOptions);

            static
            SerializedAsset
            serializeEffect(std::shared_ptr<file::Dependency>	    dependecies,
                            std::shared_ptr<file::AssetLibrary>	    assetLibrary,
                            std::shared_ptr<render::Effect>	        effect,
                            DependencyId							resourceId,
                            std::shared_ptr<file::Options>		    options,
                            std::shared_ptr<file::WriterOptions>    writerOptions);

			static
			void
			setMaterialFunction(MaterialWriterFunction materialFunc)
			{
				_materialWriteFunction = materialFunc;
			}

			static
			void
			setTextureFunction(TextureWriterFunction textureFunc)
			{
				_textureWriteFunction = textureFunc;
			}

			static
			void
			setGeometryFunction(GeometryWriterFunction geometryFunc, GeometryTestFunc testFunc, uint priority)
			{
				_geometryTestFunctions[priority]	= testFunc;
				_geometryWriteFunctions[priority]	= geometryFunc;
			}

		private:

			Dependency();
		};
	}
}
