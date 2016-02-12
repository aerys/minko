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
#include "minko/component/SkinningMethod.hpp"
#include "minko/file/EffectParser.hpp"
#include "minko/render/TextureFormat.hpp"
#include "minko/Hash.hpp"

namespace minko
{
	namespace file
	{
		class Options:
			public std::enable_shared_from_this<Options>
		{
		private:
			typedef std::shared_ptr<AbstractProtocol>						        AbsProtocolPtr;
			typedef std::shared_ptr<data::Provider>									ProviderPtr;
			typedef std::shared_ptr<material::Material>								MaterialPtr;
			typedef std::shared_ptr<geometry::Geometry>								GeomPtr;
			typedef std::shared_ptr<render::AbstractTexture>						AbstractTexturePtr;
			typedef std::shared_ptr<scene::Node>									NodePtr;
			typedef std::shared_ptr<render::Effect>									EffectPtr;
			typedef std::shared_ptr<Loader>                                         LoaderPtr;
			typedef std::shared_ptr<AbstractParser>                                 AbsParserPtr;
			typedef std::function<AbsParserPtr(void)>                               ParserHandler;
			typedef std::function<AbsProtocolPtr(void)>		                        ProtocolHandler;
			typedef Hash<render::TextureFormat>										TextureFormatHash;
			typedef std::unordered_set<render::TextureFormat, TextureFormatHash>	TextureFormatSet;

		public:
			typedef std::shared_ptr<Options>													Ptr;
            typedef std::shared_ptr<File>                                                       FilePtr;
			typedef std::function<MaterialPtr(const std::string&, MaterialPtr)>					MaterialFunction;
			typedef std::function<AbstractTexturePtr(const std::string&, AbstractTexturePtr)>	TextureFunction;
			typedef std::function<GeomPtr(const std::string&, GeomPtr)> 						GeometryFunction;
			typedef std::function<AbsProtocolPtr(const std::string&)>	                		ProtocolFunction;
            typedef std::function<AbsParserPtr(const std::string&)>                     		ParserFunction;
			typedef std::function<const std::string(const std::string&)>						UriFunction;
			typedef std::function<NodePtr(NodePtr)>												NodeFunction;
			typedef std::function<EffectPtr(EffectPtr)>											EffectFunction;
            typedef std::function<render::TextureFormat(const TextureFormatSet&)>				TextureFormatFunction;
            typedef std::function<void(NodePtr, const std::string&, const std::string&)> 		AttributeFunction;

            enum class FileStatus
            {
                Pending,
                Aborted
            };

            typedef std::function<FileStatus(FilePtr, float)>                                   FileStatusFunction;

		private:
			std::shared_ptr<render::AbstractContext>	                _context;
			std::shared_ptr<AssetLibrary>                               _assets;
			std::list<Flyweight<std::string>>			                _includePaths;
			std::list<Flyweight<std::string>>			                _platforms;
			std::list<Flyweight<std::string>>			                _userFlags;

			std::unordered_map<Flyweight<std::string>, ParserHandler>	_parsers;
			std::unordered_map<Flyweight<std::string>, ProtocolHandler> _protocols;

            bool                                                        _optimizeForRendering;
			bool                                                        _generateMipMaps;
			bool												        _parseMipMaps;
			bool										                _resizeSmoothly;
			bool										                _isCubeTexture;
            bool                                                        _isRectangleTexture;
            bool                                                        _generateSmoothNormals;
            float                                                       _normalMaxSmoothingAngle;
            bool                                                        _includeAnimation;
			bool										                _startAnimation;
			bool										                _loadAsynchronously;
            bool                                                        _disposeIndexBufferAfterLoading;
            bool                                                        _disposeVertexBufferAfterLoading;
            bool                                                        _disposeTextureAfterLoading;
            bool                                                        _storeDataIfNotParsed;
            bool                                                        _preserveMaterials;
            bool                                                        _trackAssetDescriptor;
			unsigned int								                _skinningFramerate;
			component::SkinningMethod					                _skinningMethod;
			std::shared_ptr<render::Effect>                             _effect;
			MaterialPtr									                _material;
            std::list<render::TextureFormat>                            _textureFormats;
			MaterialFunction							                _materialFunction;
			TextureFunction							                    _textureFunction;
			GeometryFunction							                _geometryFunction;
			ProtocolFunction								            _protocolFunction;
            ParserFunction                                              _parserFunction;
			UriFunction									                _uriFunction;
			NodeFunction								                _nodeFunction;
			EffectFunction								                _effectFunction;
            TextureFormatFunction                                       _textureFormatFunction;
            AttributeFunction                                           _attributeFunction;
            FileStatusFunction                                          _fileStatusFunction;
            int                                                         _seekingOffset;
            int                                                         _seekedLength;

			static ProtocolFunction								        _defaultProtocolFunction;
            static MaterialPtr                                          _defaultMaterial;

		public:
			inline static
			Ptr
			create()
			{
                auto instance = std::shared_ptr<Options>(new Options());

                instance->initialize();

                return instance;
			}

			inline static
			Ptr
			create(std::shared_ptr<render::AbstractContext> context)
			{
				auto options = create();

				options->_context = context;

				return options;
			}

            virtual
			Ptr
            clone();

			inline
			std::shared_ptr<render::AbstractContext>
			context() const
			{
				return _context;
			}

			inline
			void
			context(std::shared_ptr<render::AbstractContext> context)
			{
				_context = context;
			}

			inline
			std::shared_ptr<AssetLibrary>
			assetLibrary()
			{
				return _assets;
			}

			inline
			void
			assetLibrary(std::shared_ptr<AssetLibrary> assetLibrary)
			{
				_assets = assetLibrary;
			}

			inline
			std::list<Flyweight<std::string>>&
			includePaths()
			{
				return _includePaths;
			}

			inline
			std::list<Flyweight<std::string>>&
			platforms()
			{
				return _platforms;
			}

			inline
			std::list<Flyweight<std::string>>&
			userFlags()
			{
				return _userFlags;
			}

			inline
			bool
			optimizeForRendering() const
			{
				return _optimizeForRendering;
			}

			inline
			Ptr
			optimizeForRendering(bool value)
			{
				_optimizeForRendering = value;

				return shared_from_this();
			}

			inline
			bool
			generateMipmaps() const
			{
				return _generateMipMaps;
			}

			inline
			Ptr
			generateMipmaps(bool generateMipmaps)
			{
				_generateMipMaps = generateMipmaps;

				return shared_from_this();
			}

			inline
			bool
			parseMipMaps() const
			{
				return _parseMipMaps;
			}

			inline
			Ptr
			parseMipMaps(bool parseMipMaps)
			{
				_parseMipMaps = parseMipMaps;

				return shared_from_this();
			}

			inline
			bool
			includeAnimation() const
			{
				return _includeAnimation;
			}

			inline
			Ptr
			includeAnimation(bool value)
			{
				_includeAnimation = value;

				return shared_from_this();
			}

			inline
			bool
			startAnimation() const
			{
				return _startAnimation;
			}

			inline
			Ptr
			startAnimation(bool value)
			{
				_startAnimation = value;

				return shared_from_this();
			}

			inline
			bool
			loadAsynchronously() const
			{
				return _loadAsynchronously;
			}

			inline
			Ptr
			loadAsynchronously(bool value)
			{
				_loadAsynchronously = value;

				return shared_from_this();
			}

			inline
			bool
			resizeSmoothly() const
			{
				return _resizeSmoothly;
			}

			inline
			Ptr
			resizeSmoothly(bool value)
			{
				_resizeSmoothly = value;

				return shared_from_this();
			}

			inline
			bool
			isCubeTexture() const
			{
				return _isCubeTexture;
			}

			inline
			Ptr
			isCubeTexture(bool value)
			{
				_isCubeTexture = value;

				return shared_from_this();
			}

			inline
			bool
            isRectangleTexture() const
            {
                return _isRectangleTexture;
            }

            inline
            Ptr
            isRectangleTexture(bool value)
            {
                _isRectangleTexture = value;

                return shared_from_this();
            }

			inline
			bool
            generateSmoothNormals() const
            {
                return _generateSmoothNormals;
            }

            inline
            Ptr
            generateSmoothNormals(bool value)
            {
                _generateSmoothNormals = value;

                return shared_from_this();
            }

			inline
			float
            normalMaxSmoothingAngle() const
            {
                return _normalMaxSmoothingAngle;
            }

            inline
            Ptr
            normalMaxSmoothingAngle(float value)
            {
                _normalMaxSmoothingAngle = value;

                return shared_from_this();
            }

            inline
            bool
			disposeIndexBufferAfterLoading() const
			{
				return _disposeIndexBufferAfterLoading;
			}

			inline
			Ptr
			disposeIndexBufferAfterLoading(bool value)
			{
				_disposeIndexBufferAfterLoading = value;

				return shared_from_this();
			}

			inline
			bool
			disposeVertexBufferAfterLoading() const
			{
				return _disposeVertexBufferAfterLoading;
			}

			inline
			Ptr
			disposeVertexBufferAfterLoading(bool value)
			{
				_disposeVertexBufferAfterLoading = value;

				return shared_from_this();
			}

			inline
			bool
			disposeTextureAfterLoading() const
			{
				return _disposeTextureAfterLoading;
			}

			inline
			Ptr
			disposeTextureAfterLoading(bool value)
			{
				_disposeTextureAfterLoading = value;

				return shared_from_this();
			}

			inline
            bool
            storeDataIfNotParsed() const
            {
                return _storeDataIfNotParsed;
            }

            inline
            Ptr
            storeDataIfNotParsed(bool value)
            {
                _storeDataIfNotParsed = value;

                return shared_from_this();
            }

			inline
            bool
            preserveMaterials() const
            {
                return _preserveMaterials;
            }

            inline
            Ptr
            preserveMaterials(bool value)
            {
                _preserveMaterials = value;

                return shared_from_this();
            }

            inline
            bool
            trackAssetDescriptor() const
            {
                return _trackAssetDescriptor;
            }

            inline
            Ptr
            trackAssetDescriptor(bool value)
            {
                _trackAssetDescriptor = value;

                return shared_from_this();
            }

            inline
			unsigned int
			skinningFramerate() const
			{
				return _skinningFramerate;
			}

			inline
			Ptr
			skinningFramerate(unsigned int value)
			{
				_skinningFramerate = value;

				return shared_from_this();
			}

			inline
			component::SkinningMethod
			skinningMethod() const
			{
				return _skinningMethod;
			}

			inline
			Ptr
			skinningMethod(component::SkinningMethod value)
			{
				_skinningMethod	= value;

				return shared_from_this();
			}

			inline
			std::shared_ptr<render::Effect>
			effect() const
			{
				return _effect;
			}

			inline
			Ptr
			effect(std::shared_ptr<render::Effect> effect)
			{
				_effect = effect;

				return shared_from_this();
			}

			inline
			MaterialPtr
			material() const
			{
				return _material;
			}

			inline
			Ptr
			material(MaterialPtr material)
			{
				_material = material;

				return shared_from_this();
			}

			inline
            Ptr
            registerTextureFormat(render::TextureFormat textureFormat)
            {
                _textureFormats.push_back(textureFormat);

                return shared_from_this();
            }

            inline
			const ProtocolFunction&
			protocolFunction() const
			{
				return _protocolFunction;
			}

			inline
			Ptr
			protocolFunction(const ProtocolFunction& func)
			{
				_protocolFunction = func;

				return shared_from_this();
			}

            inline
			const ParserFunction&
			parserFunction() const
			{
				return _parserFunction;
			}

			inline
			Ptr
			parserFunction(const ParserFunction& func)
			{
				_parserFunction = func;

				return shared_from_this();
			}

			inline
			const MaterialFunction&
			materialFunction() const
			{
				return _materialFunction;
			}

			inline
			Ptr
			materialFunction(const MaterialFunction& func)
			{
				_materialFunction = func;

				return shared_from_this();
			}

			inline
			const TextureFunction&
			textureFunction() const
			{
				return _textureFunction;
			}

			inline
			Ptr
			textureFunction(const TextureFunction& func)
			{
				_textureFunction = func;

				return shared_from_this();
			}

			inline
			const GeometryFunction&
			geometryFunction() const
			{
				return _geometryFunction;
			}

			inline
			Ptr
			geometryFunction(const GeometryFunction& func)
			{
				_geometryFunction = func;

				return shared_from_this();
			}

			inline
			const UriFunction&
			uriFunction() const
			{
				return _uriFunction;
			}

			inline
			Ptr
			uriFunction(const UriFunction& func)
			{
				_uriFunction = func;

				return shared_from_this();
			}

			inline
			const NodeFunction&
			nodeFunction() const
			{
				return _nodeFunction;
			}

			inline
			Ptr
			nodeFunction(const NodeFunction& func)
			{
				_nodeFunction = func;

				return shared_from_this();
			}

			inline
			const EffectFunction&
			effectFunction() const
			{
				return _effectFunction;
			}

			inline
			Ptr
			effectFunction(const EffectFunction& func)
			{
				_effectFunction = func;

				return shared_from_this();
			}

            inline
            const TextureFormatFunction&
            textureFormatFunction() const
            {
                return _textureFormatFunction;
            }

            inline
            Ptr
            textureFormatFunction(const TextureFormatFunction& func)
            {
                _textureFormatFunction = func;

                return shared_from_this();
            }

            inline
            const AttributeFunction&
            attributeFunction() const
            {
                return _attributeFunction;
            }

            inline
            Ptr
            attributeFunction(const AttributeFunction& func)
            {
                _attributeFunction = func;

                return shared_from_this();
            }

            inline
            const FileStatusFunction&
            fileStatusFunction() const
            {
                return _fileStatusFunction;
            }

            inline
            Ptr
            fileStatusFunction(const FileStatusFunction& func)
            {
                _fileStatusFunction = func;

                return shared_from_this();
            }

            inline
            int
			seekingOffset() const
			{
				return _seekingOffset;
			}

			inline
			Ptr
			seekingOffset(int value)
			{
				_seekingOffset = value;

				return shared_from_this();
			}

            inline
			int
			seekedLength() const
			{
				return _seekedLength;
			}

			inline
			Ptr
			seekedLength(int value)
			{
				_seekedLength = value;

				return shared_from_this();
			}

			template <typename T>
			typename std::enable_if<std::is_base_of<file::AbstractParser, T>::value, Ptr>::type
			registerParser(const std::string& extension)
			{
				std::string ext(extension);

				std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

				_parsers[ext] = T::create;

				return shared_from_this();
			}

			std::shared_ptr<AbstractParser>
			getParser(const std::string& extension);

			template <typename T>
			typename std::enable_if<std::is_base_of<file::AbstractProtocol, T>::value, Ptr>::type
			registerProtocol(const std::string& protocol)
			{
				std::string prefix(protocol);

				std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

				_protocols[prefix] = T::create;

				return shared_from_this();
			}

			AbsProtocolPtr
			getProtocol(const std::string& protocol);

			static
			void
			defaultProtocolFunction(const std::string& filename, const ProtocolFunction& func);

        protected:
			Options();

            Options(const Options& copy);

            virtual
            void
            initialize();

        private:
			void
			initializePlatforms();

			void
			initializeUserFlags();

			void
			initializeDefaultFunctions();

            void
            resetNotInheritedValues();

#if MINKO_PLATFORM & MINKO_PLATFORM_HTML5
            bool
            testUserAgentPlatform(const std::string& platform);
#endif
		};
	}
}
