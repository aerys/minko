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

#include "minko/component/SkinningMethod.hpp"
#include "minko/file/EffectParser.hpp"

namespace minko
{
	namespace file
	{
		class Options:
			public std::enable_shared_from_this<Options>
		{
		private:
			typedef std::shared_ptr<AbstractProtocol>						            AbsProtocolPtr;
			typedef std::shared_ptr<data::Provider>										ProviderPtr;
			typedef std::shared_ptr<material::Material>									MaterialPtr;
			typedef std::shared_ptr<geometry::Geometry>									GeomPtr;
			typedef std::shared_ptr<scene::Node>										NodePtr;
			typedef std::shared_ptr<render::Effect>										EffectPtr;
            typedef std::shared_ptr<Loader>                                        LoaderPtr;
            typedef std::shared_ptr<AbstractParser>                                     AbsParserPtr;
            typedef std::function<AbsParserPtr(void)>                                   ParserHandler;
            typedef std::function<AbsProtocolPtr(void)>		                            ProtocolHandler;

		public:
			typedef std::shared_ptr<Options>											Ptr;
			typedef std::function<MaterialPtr(const std::string&, MaterialPtr)>			MaterialFunction;
			typedef std::function<GeomPtr(const std::string&, GeomPtr)> 				GeometryFunction;
			typedef std::function<AbsProtocolPtr(const std::string&)>	                ProtocolFunction;
			typedef std::function<const std::string(const std::string&)>				UriFunction;
			typedef std::function<NodePtr(NodePtr)>										NodeFunction;
			typedef std::function<EffectPtr(EffectPtr)>									EffectFunction;

		private:
			std::shared_ptr<render::AbstractContext>	        _context;
            std::shared_ptr<AssetLibrary>                       _assets;
			std::list<std::string>						        _includePaths;
			std::list<std::string>						        _platforms;
			std::list<std::string>						        _userFlags;

            std::unordered_map<std::string, ParserHandler>	    _parsers;
            std::unordered_map<std::string, ProtocolHandler>    _protocols;

            bool                                                _generateMipMaps;
			bool										        _resizeSmoothly;
			bool										        _isCubeTexture;
			bool										        _startAnimation;
			bool										        _loadAsynchronously;
            bool                                                _disposeIndexBufferAfterLoading;
            bool                                                _disposeVertexBufferAfterLoading;
            bool                                                _disposeTextureAfterLoading;

			unsigned int								        _skinningFramerate;
			component::SkinningMethod					        _skinningMethod;
            std::shared_ptr<render::Effect>                     _effect;
			MaterialPtr									        _material;
			MaterialFunction							        _materialFunction;
			GeometryFunction							        _geometryFunction;
			ProtocolFunction								    _protocolFunction;
			UriFunction									        _uriFunction;
			NodeFunction								        _nodeFunction;
			EffectFunction								        _effectFunction;

		public:
            inline static
			Ptr
			create()
			{
                auto opt = std::shared_ptr<Options>(new Options());

                opt->initializeDefaultFunctions();
                opt->registerParser<file::EffectParser>("effect");
                opt->registerProtocol<FileProtocol>("file");

				return opt;
			}

            inline static
            Ptr
			create(std::shared_ptr<render::AbstractContext> context)
			{
				auto opt = create();

                opt->_context = context;

                return opt;
            }

			inline static
			Ptr
			create(Ptr options)
			{
				auto opt = create();

                opt->_context = options->_context;
                opt->_assets = options->_assets;
                opt->_parsers = options->_parsers;
                opt->_protocols = options->_protocols;
                opt->_includePaths = options->_includePaths;
                opt->_generateMipMaps = options->_generateMipMaps;
                opt->_resizeSmoothly = options->_resizeSmoothly;
                opt->_isCubeTexture = options->_isCubeTexture;
                opt->_startAnimation = options->_startAnimation;
                opt->_disposeIndexBufferAfterLoading = options->_disposeIndexBufferAfterLoading;
                opt->_disposeVertexBufferAfterLoading = options->_disposeVertexBufferAfterLoading;
                opt->_disposeTextureAfterLoading = options->_disposeTextureAfterLoading;
                opt->_skinningFramerate = options->_skinningFramerate;
                opt->_skinningMethod = options->_skinningMethod;
                opt->_effect = options->_effect;
                opt->_materialFunction = options->_materialFunction;
                opt->_geometryFunction = options->_geometryFunction;
                opt->_protocolFunction = options->_protocolFunction;
                opt->_uriFunction = options->_uriFunction;
                opt->_nodeFunction = options->_nodeFunction;
                opt->_loadAsynchronously = options->_loadAsynchronously;

				return opt;
			}

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
			std::list<std::string>&
			includePaths()
			{
				return _includePaths;
			}

			inline
			std::list<std::string>&
			platforms()
			{
				return _platforms;
			}

			inline
			std::list<std::string>&
			userFlags()
			{
				return _userFlags;
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

		private:
			Options();

			void
			initializePlatforms();

			void
			initializeUserFlags();

            std::shared_ptr<AbstractProtocol>
            defaultProtocolFunction(const std::string& filename);

            void
            initializeDefaultFunctions();
		};
	}
}
