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

#include "MkParser.hpp"
#include "minko/AssetsLibrary.hpp"
#include "minko/file/Options.hpp"
#include "minko/scene/Node.hpp"

using namespace minko::file;
using namespace minko::math;

std::map<std::string, MkOptions::DeserializeFunction>	MkParser::_pluginEntryToFunction;

void
MkParser::parse(const std::string&					filename,
				std::shared_ptr<Options>			options,
				const std::vector<unsigned char>&	data,
				std::shared_ptr<AssetsLibrary>		assetsLibrary)
{
	std::vector<char> dataCopy(data.begin(), data.end());

	minko::Qark::Object&    obj 			= minko::Qark::decode(dataCopy);

	std::map<std::string, Any>& qarkData = minko::Any::cast<std::map<std::string, Any>&>(obj);

	std::cout << "-------------------------------" << std::endl <<std::endl;

	std::cout << "Magic number   : " << minko::Any::cast<int>(qarkData["magicNumber"]) << std::endl;
	std::cout << "Version        : " << minko::Any::cast<std::string>(qarkData["version"]) << std::endl;
	std::cout << std::flush;

	std::shared_ptr<file::MkOptions> mkOptions = file::MkOptions::create(options, assetsLibrary);

	mkOptions->pluginEntryToFunction(std::make_shared<std::map<std::string, file::MkOptions::DeserializeFunction>>(_pluginEntryToFunction));

	std::shared_ptr<deserialize::SceneDeserializer> sceneDeserializer = deserialize::SceneDeserializer::create(options->context());

	_node = sceneDeserializer->deserializeScene(qarkData["scene"], qarkData["assets"], mkOptions, _controllerMap, _nodeMap);
	
	assetsLibrary->node(filename, _node);

	std::cout << "parse MK" << std::endl << std::flush;
}

void
MkParser::registerController(std::string entryName, DeserializeFunction2 deserializedFunction)
{
	MkParser::_pluginEntryToFunction[entryName] = deserializedFunction;
}