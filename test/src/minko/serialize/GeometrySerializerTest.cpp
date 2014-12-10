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

#include "minko/serialize/GeometrySerializerTest.hpp"
#include "minko/serialize/TypeSerializer.hpp"
#include "minko/deserialize/TypeDeserializer.hpp"
#include "minko/file/GeometryParser.hpp"
#include "minko/file/GeometryWriter.hpp"
#include "minko/MinkoTests.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/Dependency.hpp"

using namespace minko;
using namespace minko::serialize;
using namespace minko::deserialize;

TEST_F(GeometrySerializerTest, CubeGeometrySerialization)
{
	auto cubeGeometry		= geometry::CubeGeometry::create(MinkoTests::canvas()->context());
    auto assetLibrary       = file::AssetLibrary::create(MinkoTests::canvas()->context());
	auto geometryWriter		= file::GeometryWriter::create();
    auto outputAssetLibrary = file::AssetLibrary::create(MinkoTests::canvas()->context());
	auto geometryParser		= file::GeometryParser::create();

	std::string	filename = "asset.tmp";

	assetLibrary->geometry("cube", cubeGeometry);
	geometryWriter->data(cubeGeometry);
	geometryWriter->write(
        filename,
        assetLibrary,
        file::Options::create(MinkoTests::canvas()->context()),
        file::WriterOptions::create()
    );

	std::vector<unsigned char>  data;
	auto						flags = std::ios::in | std::ios::ate | std::ios::binary;
	std::fstream				file(filename, flags);
	unsigned int				size = (unsigned int)file.tellg();

	data.resize(size);
	file.seekg(0, std::ios::beg);
	file.read((char*)&data[0], size);
	file.close();

	geometryParser->parse(filename, filename, file::Options::create(MinkoTests::canvas()->context()), data, outputAssetLibrary);

	ASSERT_TRUE(outputAssetLibrary->geometry("cube") != nullptr);
	ASSERT_TRUE(cubeGeometry->equals(outputAssetLibrary->geometry("cube")));
}

TEST_F(GeometrySerializerTest, SphereGeometrySerialization)
{
    auto sphereGeometry = geometry::SphereGeometry::create(MinkoTests::canvas()->context(), 20, 20);
    auto assetLibrary       = file::AssetLibrary::create(MinkoTests::canvas()->context());
	auto geometryWriter		= file::GeometryWriter::create();
    auto outputAssetLibrary = file::AssetLibrary::create(MinkoTests::canvas()->context());
	auto geometryParser		= file::GeometryParser::create();
	std::string	filename	= "asset.tmp";

	assetLibrary->geometry("sphere", sphereGeometry);
	geometryWriter->data(sphereGeometry);
	geometryWriter->write(filename,
                          assetLibrary,
                          file::Options::create(MinkoTests::canvas()->context()),
                          file::WriterOptions::create());

	std::vector<unsigned char>  data;
	auto						flags = std::ios::in | std::ios::ate | std::ios::binary;
	std::fstream				file(filename, flags);
	unsigned int				size = (unsigned int)file.tellg();

	data.resize(size);
	file.seekg(0, std::ios::beg);
	file.read((char*)&data[0], size);
	file.close();

	geometryParser->parse(filename, filename, file::Options::create(MinkoTests::canvas()->context()), data, outputAssetLibrary);

	ASSERT_TRUE(outputAssetLibrary->geometry("sphere") != nullptr);
	ASSERT_TRUE(sphereGeometry->equals(outputAssetLibrary->geometry("sphere")));
}
