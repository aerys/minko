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

#pragma once

#include "minko/Common.hpp"
#include "msgpack.hpp"

namespace minko
{
	namespace file
	{
		class Dependency
		{
		public:
			typedef std::shared_ptr<Dependency> Ptr;
			typedef std::shared_ptr<render::AbstractTexture> AbsTexturePtr;

		private:
			typedef msgpack::type::tuple<unsigned int, short, std::string> SerializedAsset;
			typedef std::function<SerializedAsset(std::shared_ptr<file::AssetLibrary>, std::shared_ptr<geometry::Geometry>, uint, std::shared_ptr<file::Options>)>		GeometryWriterFunction;
			typedef std::function<SerializedAsset(std::shared_ptr<file::AssetLibrary>, std::shared_ptr<render::AbstractTexture>, uint,std::shared_ptr<file::Options>)>	TextureWriterFunction;
			typedef std::function<SerializedAsset(std::shared_ptr<file::AssetLibrary>, std::shared_ptr<data::Provider>, uint,std::shared_ptr<file::Options>)>		MaterialWriterFunction;

		private:
			std::unordered_map<AbsTexturePtr, uint>							_textureDependencies;
			std::unordered_map<std::shared_ptr<data::Provider>, uint>		_materialDependencies;
			std::unordered_map<std::shared_ptr<scene::Node>, uint>			_subSceneDependencies;
			std::unordered_map<std::shared_ptr<geometry::Geometry>, uint>	_geometryDependencies;
			std::unordered_map<std::shared_ptr<render::Effect>, uint>		_effectDependencies;

			std::unordered_map<uint, AbsTexturePtr>							_textureReferences;
			std::unordered_map<uint, std::shared_ptr<data::Provider>>		_materialReferences;
			std::unordered_map<uint, std::shared_ptr<scene::Node>>			_subSceneReferences;
			std::unordered_map<uint, std::shared_ptr<geometry::Geometry>>	_geometryReferences;
			std::unordered_map<uint, std::shared_ptr<render::Effect>>		_effectReferences;

			uint															_currentId;
			std::shared_ptr<Options>										_options;
			std::shared_ptr<scene::Node>									_loadedRoot;

			static GeometryWriterFunction		_geometryWriteFunction;
			static TextureWriterFunction		_textureWriteFunction;
			static MaterialWriterFunction		_materialWriteFunction;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<Dependency>(new Dependency());
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

			uint
			registerDependency(std::shared_ptr<geometry::Geometry> geometry);

			bool
			hasDependency(std::shared_ptr<data::Provider> material);

			uint
			registerDependency(std::shared_ptr<data::Provider> material);

			bool
			hasDependency(AbsTexturePtr texture);

			uint
			registerDependency(AbsTexturePtr texture);

			bool
			hasDependency(std::shared_ptr<scene::Node> subScene);

			uint
			registerDependency(std::shared_ptr<scene::Node> subScene);

			bool
			hasDependency(std::shared_ptr<render::Effect> effect);

			uint
			registerDependency(std::shared_ptr<render::Effect> effect);

			std::shared_ptr<geometry::Geometry>
			getGeometryReference(uint geometryId);

			void
			registerReference(uint referenceId, std::shared_ptr<geometry::Geometry> geometry);

			std::shared_ptr<data::Provider>
			getMaterialReference(uint materialId);

			void
			registerReference(uint referenceId, std::shared_ptr<data::Provider> material);

			AbsTexturePtr
			getTextureReference(uint textureId);

			void
			registerReference(uint referenceId, AbsTexturePtr texture);

			std::shared_ptr<scene::Node>
			getSubsceneReference(uint subSceneId);

			void
			registerReference(uint referenceId, std::shared_ptr<render::Effect> effect);

			std::shared_ptr<render::Effect>
			getEffectReference(uint effectId);

			void
			registerReference(uint referenceId, std::shared_ptr<scene::Node> subScene);

			std::vector<msgpack::type::tuple<unsigned int, short, std::string>>
			serialize(std::shared_ptr<file::AssetLibrary>	assetLibrary,
					  std::shared_ptr<file::Options>		options);

			static
			SerializedAsset
			serializeGeometry(std::shared_ptr<file::AssetLibrary> assetLibrary, std::shared_ptr<geometry::Geometry> geometry, uint resourceId, std::shared_ptr<file::Options> options);

			static
			SerializedAsset
			serializeTexture(std::shared_ptr<file::AssetLibrary> assetLibrary, std::shared_ptr<render::AbstractTexture> texture, uint resourceId, std::shared_ptr<file::Options> options);

			static
			SerializedAsset
			serializeMaterial(std::shared_ptr<file::AssetLibrary> assetLibrary, std::shared_ptr<data::Provider> material, uint resourceId, std::shared_ptr<file::Options> options);

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
			setGeometryFunction(GeometryWriterFunction geometryFunc)
			{
				_geometryWriteFunction = geometryFunc;
			}

		private:

            void
            copyEffectDependency(std::shared_ptr<AssetLibrary>          assets,
                                 std::shared_ptr<Options>               options,
                                 const std::ifstream&                   source,
                                 std::shared_ptr<render::Effect>        effect,
                                 SerializedAsset&                       result);
			Dependency();
		};
	}
}
