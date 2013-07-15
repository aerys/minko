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


namespace minko
{
	class AssetsLibrary :
		public std::enable_shared_from_this<AssetsLibrary>
	{
	public:
		typedef std::shared_ptr<AssetsLibrary>				Ptr;

	private:
		typedef std::shared_ptr<render::AbstractContext>	AbsContextPtr;
		typedef std::shared_ptr<render::Effect>				EffectPtr;
		typedef std::shared_ptr<render::Texture>			TexturePtr;
		typedef std::shared_ptr<geometry::Geometry>			GeometryPtr;
		typedef std::shared_ptr<file::AbstractParser>		AbsParserPtr;
		typedef std::function<AbsParserPtr(void)>			Handler;
		typedef std::shared_ptr<scene::Node>				NodePtr;

	private:
		AbsContextPtr													_context;
		std::shared_ptr<file::Options>									_defaultOptions;
		std::unordered_map<std::string, Handler>						_parsers;

		std::unordered_map<std::string, GeometryPtr>					_geometries;
		std::unordered_map<std::string, EffectPtr>						_effects;
		std::unordered_map<std::string, TexturePtr>						_textures;
		std::unordered_map<std::string, NodePtr>						_nodes;
		std::unordered_map<std::string, std::vector<unsigned char>>		_blobs;

		std::list<std::string>											_filesQueue;
		std::unordered_map<std::string, std::shared_ptr<file::Options>>	_filenameToOptions;
		std::unordered_map<std::string, std::shared_ptr<file::Loader>>	_filenameToLoader;

		std::vector<Signal<std::shared_ptr<file::Loader>>::Slot>		_loaderSlots;

		std::shared_ptr<Signal<Ptr>>									_complete;

	public:
		// fixme cyclic reference
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
		std::shared_ptr<Signal<Ptr>>
		complete()
		{
			return _complete;
		}

		GeometryPtr
		geometry(const std::string& name);

		Ptr
		geometry(const std::string& name, GeometryPtr geometry);

		TexturePtr
		texture(const std::string& name);

		Ptr
		texture(const std::string& name, TexturePtr texture);

		NodePtr
		node(const std::string& name);

		Ptr
		node(const std::string& name, NodePtr node);

		EffectPtr
		effect(const std::string& name);

		Ptr
		effect(const std::string& name, EffectPtr effect);

		const std::vector<unsigned char>&
		blob(const std::string& name);

		Ptr
		blob(const std::string& name, const std::vector<unsigned char>& blob);

		template <typename T>
		typename std::enable_if<std::is_base_of<file::AbstractParser, T>::value, Ptr>::type
		registerParser(const std::string& extension)
		{
			_parsers[extension] = T::create;

			return shared_from_this();
		}

		Ptr
		queue(const std::string& filename, std::shared_ptr<file::Options> options = nullptr);

		Ptr
		load(const std::string& filename, std::shared_ptr<file::Options> options = nullptr);

		Ptr
		load();

		AbsParserPtr
		parser(std::string extension);

	private:
		AssetsLibrary(AbsContextPtr context);

		void
		loaderErrorHandler(std::shared_ptr<file::Loader> loader);

		void
		loaderCompleteHandler(std::shared_ptr<file::Loader> loader);
	};
}