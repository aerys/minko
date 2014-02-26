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

#include "minko/Signal.hpp"
#include "minko/file/AbstractParser.hpp"
#include "minko/file/EffectParser.hpp"
#include "minko/file/AbstractLoader.hpp"

namespace minko
{
	namespace file
	{
		class AssetLibrary :
			public std::enable_shared_from_this<AssetLibrary>
		{
		public:
			typedef std::shared_ptr<AssetLibrary>				Ptr;

		private:
			typedef std::shared_ptr<render::AbstractContext>	AbsContextPtr;
			typedef std::shared_ptr<render::Effect>				EffectPtr;
			typedef std::shared_ptr<render::AbstractTexture>	AbsTexturePtr;
			typedef std::shared_ptr<geometry::Geometry>			GeometryPtr;
			typedef std::shared_ptr<file::AbstractParser>		AbsParserPtr;
			typedef std::shared_ptr<file::AbstractLoader>		AbsLoaderPtr;
			typedef std::function<AbsParserPtr(void)>			ParserHandler;
			typedef std::function<AbsLoaderPtr(void)>			LoaderHandler;
			typedef std::shared_ptr<scene::Node>				NodePtr;
            typedef std::shared_ptr<component::AbstractScript>  AbsScriptPtr;
			typedef std::shared_ptr<data::Provider>				MaterialPtr;

		private:
			AbsContextPtr															_context;
			std::shared_ptr<file::Options>											_defaultOptions;
			std::unordered_map<std::string, ParserHandler>							_parsers;
			std::unordered_map<std::string, LoaderHandler>							_loaders;

			std::unordered_map<std::string, MaterialPtr>							_materials;
			std::unordered_map<std::string, GeometryPtr>							_geometries;
			std::unordered_map<std::string, EffectPtr>								_effects;
			std::unordered_map<std::string, AbsTexturePtr>							_textures;
			std::unordered_map<std::string, NodePtr>								_symbols;
			std::unordered_map<std::string, std::vector<unsigned char>>				_blobs;
            std::unordered_map<std::string, AbsScriptPtr>                           _scripts;

            std::unordered_map<std::string, uint>									_layouts;

			std::list<std::string>													_filesQueue;
			std::list<std::string>													_loading;
			std::unordered_map<std::string, std::shared_ptr<file::Options>>			_filenameToOptions;
			std::unordered_map<std::string, std::shared_ptr<file::AbstractLoader>>	_filenameToLoader;

			std::vector<Signal<std::shared_ptr<file::AbstractLoader>>::Slot>		_loaderSlots;
			std::vector<Signal<std::shared_ptr<file::AbstractParser>>::Slot>	_parserSlots;

            Signal<Ptr>::Ptr											            _complete;
            Signal<Ptr, std::shared_ptr<AbstractParser>>::Ptr                       _parserError;

		public:
			static
			Ptr
			create(AbsContextPtr context);

			inline
			AbsContextPtr
			context()
			{
				return _context;
			}

			inline
			std::shared_ptr<file::Options>
			defaultOptions()
			{
				return _defaultOptions;
			}

			inline
			Signal<Ptr>::Ptr
			complete()
			{
				return _complete;
			}

            inline
            Signal<Ptr, std::shared_ptr<AbstractParser>>::Ptr
            parserError()
            {
                return _parserError;
            }

			inline
			const std::list<std::string>&
			filesQueue()
			{
				return _filesQueue;
			}

			inline
			bool
			loading() const
			{
				return _filesQueue.size() > 0 || _loading.size() > 0;
			}

			GeometryPtr
			geometry(const std::string& name);

			Ptr
			geometry(const std::string& name, GeometryPtr geometry);

			const std::string&
			geometryName(GeometryPtr geometry);
			
			Ptr
			texture(const std::string& name, AbsTexturePtr texture);

			AbsTexturePtr
			texture(const std::string& name) const;

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

            AbsScriptPtr
            script(const std::string& name);

            AssetLibrary::Ptr
            script(const std::string& name, AbsScriptPtr script);

			const std::string&
			scriptName(AbsScriptPtr script);

			const unsigned int
			layout(const std::string& name);

			Ptr
			layout(const std::string& name, const unsigned int mask);

			template <typename T>
			typename std::enable_if<std::is_base_of<file::AbstractParser, T>::value, Ptr>::type
			registerParser(const std::string& extension)
			{
				std::string ext(extension);

				std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

				_parsers[ext] = T::create;

				return shared_from_this();
			}

			file::AbstractParser::Ptr
			getParser(const std::string& extension)
			{
                return _parsers.count(extension) == 0 ? nullptr : _parsers[extension]();
			}

			template <typename T>
			typename std::enable_if<std::is_base_of<file::AbstractLoader, T>::value, Ptr>::type
			registerProtocol(const std::string& protocol)
			{
				std::string prefix(protocol);

				std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

				_loaders[prefix] = T::create;

				return shared_from_this();
			}

			file::AbstractLoader::Ptr
			getLoader(const std::string& protocol)
			{
                return _loaders.count(protocol) == 0 ? nullptr : _loaders[protocol](); 
			}

			Ptr
			queue(const std::string& filename);

			Ptr
			queue(const std::string& filename, std::shared_ptr<file::Options> options);

			Ptr
			queue(const std::string&				filename,
				  std::shared_ptr<Options>			options,
				  std::shared_ptr<AbstractLoader>	loader);

			/*
			Ptr
			queue(const std::string&				filename,
				  std::shared_ptr<Options>			options = nullptr,
				  std::shared_ptr<AbstractLoader>	loader 	= nullptr);
			*/

			inline
			Ptr
			load(const std::string&					filename,
				 std::shared_ptr<file::Options>		options = nullptr,
				 std::shared_ptr<AbstractLoader>	loader	= nullptr,
				 bool								executeCompleteSignal = true)
			{
				queue(filename, options, loader);
				load(executeCompleteSignal);

				return shared_from_this();
			};

			inline
			Ptr
			load(const char*						filename,
				 std::shared_ptr<file::Options>		options = nullptr,
				 std::shared_ptr<AbstractLoader>	loader	= nullptr,
				 bool								executeCompleteSignal = true)
			{
				return load(std::string(filename), options, loader, executeCompleteSignal);
			};

			Ptr
			load(bool executeCompleteSignal = true);

		private:
			AssetLibrary(AbsContextPtr context);

			void
			loaderErrorHandler(std::shared_ptr<file::AbstractLoader> loader);

			void
			loaderCompleteHandler(std::shared_ptr<file::AbstractLoader> loader);

			void
			finalize(const std::string& filename);
		};
	}
}
