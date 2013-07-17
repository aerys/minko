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
#include "minko/Any.hpp"
#include "minko/file/AbstractModelParser.hpp"
#include "minko/Qark.hpp"
#include "minko/deserialize/SceneDeserializer.hpp"
#include "minko/component/AbstractComponent.hpp"
#include "minko/file/MkOptions.hpp"


namespace minko
{
	namespace file
	{
		class MkParser : 
			public AbstractModelParser
		{
		public:
			typedef std::shared_ptr<MkParser>																Ptr;
			typedef std::map<std::shared_ptr<scene::Node>, std::vector<component::AbstractComponent>>		ControllerMap;
			typedef std::map<std::shared_ptr<scene::Node>, uint>											NodeMap;
			typedef std::shared_ptr<component::AbstractComponent> (*DeserializeFunction)(minko::Qark::Map&					nodeInfo,
																						 std::shared_ptr<file::MkOptions>	options,
																						 ControllerMap&						controllerMap,
																						 NodeMap&							nodeMap);
			typedef std::function<std::shared_ptr<component::AbstractComponent>(minko::Qark::Map&, std::shared_ptr<file::MkOptions>, ControllerMap&, NodeMap&)> DeserializeFunction2;

		private:
			static std::map<std::string, DeserializeFunction2>	_pluginEntryToFunction;
			ControllerMap	_controllerMap;
			NodeMap			_nodeMap;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<MkParser>(new MkParser());
			}

			static
			void
			registerController(std::string mkEntry, DeserializeFunction2 deserializeFunction);

			void
			parse(const std::string&				filename,
				  std::shared_ptr<Options>			options,
				  const std::vector<unsigned char>&	data,
				  std::shared_ptr<AssetsLibrary>	assetsLibrary);

		private:
			MkParser()
			{
			}
		};
	}
}
