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

#include "EffectParser.hpp"

#include "minko/render/Program.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/CompareMode.hpp"
#include "minko/render/Pass.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "json/json.h"

using namespace minko::file;
using namespace minko::render;

std::unordered_map<std::string, unsigned int> EffectParser::_blendFactorMap = EffectParser::initializeBlendFactorMap();
std::unordered_map<std::string, unsigned int>
EffectParser::initializeBlendFactorMap()
{
	std::unordered_map<std::string, unsigned int> m;

	m["src_zero"]					= static_cast<uint>(render::Blending::Source::ZERO);
    m["src_one"]					= static_cast<uint>(render::Blending::Source::ONE);
    m["src_color"]					= static_cast<uint>(render::Blending::Source::SRC_COLOR);
    m["src_one_minus_src_color"]	= static_cast<uint>(render::Blending::Source::ONE_MINUS_SRC_COLOR);
    m["src_src_alpha"]				= static_cast<uint>(render::Blending::Source::SRC_ALPHA);
    m["src_one_minus_src_alpha"]	= static_cast<uint>(render::Blending::Source::ONE_MINUS_SRC_ALPHA);
    m["src_dst_alpha"]				= static_cast<uint>(render::Blending::Source::DST_ALPHA);
    m["src_one_minus_dst_alpha"]	= static_cast<uint>(render::Blending::Source::ONE_MINUS_DST_ALPHA);

    m["dst_zero"]					= static_cast<uint>(render::Blending::Destination::ZERO);
    m["dst_one"]					= static_cast<uint>(render::Blending::Destination::ONE);
	m["dst_dst_color"]				= static_cast<uint>(render::Blending::Destination::DST_COLOR);
    m["dst_one_minus_dst_color"]	= static_cast<uint>(render::Blending::Destination::ONE_MINUS_DST_COLOR);
    m["dst_one_minus_alpha"]		= static_cast<uint>(render::Blending::Destination::ONE_MINUS_DST_ALPHA);
    m["dst_one_minus_src_alpha"]	= static_cast<uint>(render::Blending::Destination::ONE_MINUS_SRC_ALPHA);
    m["dst_dst_alpha"]				= static_cast<uint>(render::Blending::Destination::DST_ALPHA);
    m["dst_one_minus_dst_alpha"]	= static_cast<uint>(render::Blending::Destination::ONE_MINUS_DST_ALPHA);

	m["default"]					= static_cast<uint>(render::Blending::Mode::DEFAULT);
	m["alpha"]						= static_cast<uint>(render::Blending::Mode::ALPHA);
	m["additive"]					= static_cast<uint>(render::Blending::Mode::ADDITIVE);

	return m;
}

std::unordered_map<std::string, render::CompareMode> EffectParser::_depthFuncMap = EffectParser::initializeDepthFuncMap();
std::unordered_map<std::string, render::CompareMode>
EffectParser::initializeDepthFuncMap()
{
	std::unordered_map<std::string, render::CompareMode> m;

	m["always"]			= render::CompareMode::ALWAYS;
	m["equal"]			= render::CompareMode::EQUAL;
	m["greater"]		= render::CompareMode::GREATER;
	m["greater_equal"]	= render::CompareMode::GREATER_EQUAL;
	m["less"]			= render::CompareMode::LESS;
	m["less_equal"]		= render::CompareMode::LESS_EQUAL;
	m["never"]			= render::CompareMode::NEVER;
	m["not_equal"]		= render::CompareMode::NOT_EQUAL;

	return m;
}

EffectParser::EffectParser() :
	_numDependencies(0),
	_numLoadedDependencies(0)
{
}

void
EffectParser::parse(const std::string&					filename,
					std::shared_ptr<Options>			options,
					const std::vector<unsigned char>&	data)
{
	Json::Value root;
	Json::Reader reader;

	if (!reader.parse((const char*)&data[0], (const char*)&data[data.size() - 1], root, false))
		throw std::invalid_argument("data");

	_effectName = root.get("name", filename).asString();

	_defaultPriority = root.get("priority", 0.f).asFloat();
	parseDefaultValues(root);
	parseDependencies(root, options);
	parsePasses(root, options);
	
	if (_numDependencies == 0)
		finalize();
}

void
EffectParser::parseDefaultValues(Json::Value& root)
{
	parseBindings(root, _defaultAttributeBindings, _defaultUniformBindings, _defaultStateBindings);
	parseBlendMode(root, _defaultBlendSrcFactor, _defaultBlendDstFactor);
	parseDepthTest(root, _defaultDepthMask, _defaultDepthFunc);
}

void
EffectParser::parsePasses(Json::Value& root, file::Options::Ptr options)
{
	std::vector<std::shared_ptr<render::Pass>> passes;
	auto passId = 0;

	for (auto pass : root.get("passes", 0))
	{
		auto name = pass.get("name", std::to_string(passId)).asString();

		++passId;

		// pass bindings
		std::unordered_map<std::string, std::string>	attributeBindings(_defaultAttributeBindings);
		std::unordered_map<std::string, std::string>	uniformBindings(_defaultUniformBindings);
		std::unordered_map<std::string, std::string>	stateBindings(_defaultStateBindings);

		parseBindings(pass, attributeBindings, uniformBindings, stateBindings);

		// pass priority
		auto priority = pass.get("priority", _defaultPriority).asFloat();

		// blendMode
		auto blendSrcFactor	= _defaultBlendSrcFactor;
		auto blendDstFactor	= _defaultBlendDstFactor;

		parseBlendMode(pass, blendSrcFactor, blendDstFactor);

		// depthTest
		auto depthMask	= _defaultDepthMask;
		auto depthFunc	= _defaultDepthFunc;

		parseDepthTest(pass, depthMask, depthFunc);

		// program
		auto vertexShaderSource		= pass.get("vertexShader", 0).asString();
		auto fragmentShaderSource	= pass.get("fragmentShader", 0).asString();

		passes.push_back(render::Pass::create(
			name,
			Program::create(options->context(), vertexShaderSource, fragmentShaderSource),
			attributeBindings,
			uniformBindings,
			stateBindings,
			priority,
			blendSrcFactor,
			blendDstFactor,
			depthMask,
			depthFunc
		));

		_effect = render::Effect::create(passes);
	}
}

void
EffectParser::parseBlendMode(Json::Value&					contextNode,
						     render::Blending::Source&		srcFactor,
						     render::Blending::Destination&	dstFactor)
{
	auto blendModeArray	= contextNode.get("blendMode", 0);
	
	if (blendModeArray.isArray())
	{
		auto blendSrcFactorString = "src_" + blendModeArray[0].asString();
		if (_blendFactorMap.count(blendSrcFactorString))
			srcFactor = static_cast<render::Blending::Source>(_blendFactorMap[blendSrcFactorString]);

		auto blendDstFactorString = "dst_" + blendModeArray[1].asString();
		if (_blendFactorMap.count(blendDstFactorString))
			dstFactor = static_cast<render::Blending::Destination>(_blendFactorMap[blendDstFactorString]);
	}
	else if (blendModeArray.isString())
	{
		auto blendModeString = blendModeArray.asString();

		if (_blendFactorMap.count(blendModeString))
		{
			auto blendMode = _blendFactorMap[blendModeString];

			srcFactor = static_cast<render::Blending::Source>(blendMode & 0x00ff);
			dstFactor = static_cast<render::Blending::Destination>(blendMode & 0xff00);
		}
	}
}

void
EffectParser::parseDepthTest(Json::Value&			contextNode,
							 bool&					depthMask,
							 render::CompareMode&	depthFunc)
{
	auto depthTest	= contextNode.get("depthTest", 0);
	
	if (depthTest.isArray())
	{
		auto depthFuncString = depthTest[1].asString();

		depthMask = depthTest[0].asBool();
		if (_depthFuncMap.count(depthFuncString))
			depthFunc = _depthFuncMap[depthFuncString];
	}
}

void
EffectParser::parseBindings(Json::Value&									contextNode,
						    std::unordered_map<std::string, std::string>&	attributeBindings,
						    std::unordered_map<std::string, std::string>&	uniformBindings,
						    std::unordered_map<std::string, std::string>&	stateBindings)
{
	auto attributeBindingsValue = contextNode.get("attributeBindings", 0);
	if (attributeBindingsValue.isObject())
		for (auto propertyName : attributeBindingsValue.getMemberNames())
			attributeBindings[propertyName] = attributeBindingsValue.get(propertyName, 0).asString();

	auto uniformBindingsValue = contextNode.get("uniformBindings", 0);
	if (uniformBindingsValue.isObject())
		for (auto propertyName : uniformBindingsValue.getMemberNames())
			uniformBindings[propertyName] = uniformBindingsValue.get(propertyName, 0).asString();

	auto stateBindingsValue = contextNode.get("stateBindings", 0);
	if (stateBindingsValue.isObject())
		for (auto propertyName : stateBindingsValue.getMemberNames())
			stateBindings[propertyName] = stateBindingsValue.get(propertyName, 0).asString();
}

void
EffectParser::parseDependencies(Json::Value& root, file::Options::Ptr options)
{
	auto require = root.get("includes", 0);
	if (require.isArray())
	{
		_numDependencies = require.size();

		for (unsigned int requireId = 0; requireId < _numDependencies; requireId++)
		{
			auto loader = Loader::create();

			_loaderCompleteSlots[loader] = loader->complete()->connect(std::bind(
				&EffectParser::dependencyCompleteHandler, shared_from_this(), std::placeholders::_1
			));
			_loaderErrorSlots[loader] = loader->error()->connect(std::bind(
				&EffectParser::dependencyErrorHandler, shared_from_this(), std::placeholders::_1
			));

			loader->load(require[requireId].asString(), options);
		}
	}
}

void
EffectParser::dependencyCompleteHandler(std::shared_ptr<Loader> loader)
{
	++_numLoadedDependencies;

	_dependenciesCode += std::string((char*)&loader->data()[0], loader->data().size()) + "\r\n";

	if (_numDependencies == _numLoadedDependencies)
		finalize();
}

void
EffectParser::dependencyErrorHandler(std::shared_ptr<Loader> loader)
{
	std::cout << "error" << std::endl;
}

void
EffectParser::finalize()
{
	for (auto& pass : _effect->passes())
    {
		auto program = pass->program();

		program->vertexShaderSource(_dependenciesCode + program->vertexShaderSource());
		program->fragmentShaderSource(_dependenciesCode + program->fragmentShaderSource());
        program->upload();
    }

	_complete->execute(shared_from_this());
}
