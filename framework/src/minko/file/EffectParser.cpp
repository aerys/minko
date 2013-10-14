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

#include "minko/data/Provider.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/Program.hpp"
#include "minko/render/States.hpp"
#include "minko/render/Effect.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/CompareMode.hpp"
#include "minko/render/WrapMode.hpp"
#include "minko/render/TextureFilter.hpp"
#include "minko/render/MipFilter.hpp"
#include "minko/render/TriangleCulling.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/Pass.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "json/json.h"

using namespace minko;
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
    m["dst_src_alpha_saturate"]		= static_cast<uint>(render::Blending::Destination::SRC_ALPHA_SATURATE);
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
	_effect(nullptr),
	_numDependencies(0),
	_numLoadedDependencies(0),
	_defaultStates(render::States::create())
{
}

void
EffectParser::parse(const std::string&				    filename,
				    const std::string&                  resolvedFilename,
                    std::shared_ptr<Options>            options,
				    const std::vector<unsigned char>&	data,
				    std::shared_ptr<AssetLibrary>	    assetLibrary)
{
	Json::Value root;
	Json::Reader reader;

	if (!reader.parse((const char*)&data[0], (const char*)&data[data.size() - 1],	root, false))
    {
        std::cerr << resolvedFilename << ":" << reader.getFormattedErrorMessages() << std::endl;

		throw std::invalid_argument("data");
    }

    _filename = filename;
	_assetLibrary = assetLibrary;
	_effectName = root.get("name", filename).asString();
	_defaultTechnique = root.get("defaultTechnique", "default").asString();

	auto context = _assetLibrary->context();

	// parse default values for bindings and states
	parseRenderStates(root, context, _globalTargets, _defaultStates);
	parseBindings(root, _defaultAttributeBindings, _defaultUniformBindings, _defaultStateBindings, _defaultMacroBindings);

	// parse a global list of passes
	parsePasses(
		root,
		resolvedFilename,
		options,
		context,
		_globalPasses,
		_globalTargets,
		_defaultAttributeBindings,
		_defaultUniformBindings,
		_defaultStateBindings,
		_defaultMacroBindings,
		_defaultStates
	);

	// parse a global list of dependencies
	parseDependencies(root, resolvedFilename, options, _effectIncludes);

	// parse a global list of render targets
	auto targetsValue = root.get("targets", 0);

	if (targetsValue.isArray())
		for (auto targetValue : targetsValue)
			parseTarget(targetValue, context, _globalTargets);

	// parse the list of techniques, if no "techniques" directive is found then
	// the global list of passes becomes the "default" techinque
	parseTechniques(root, resolvedFilename, options, context);

	_effect = render::Effect::create();

	if (_numDependencies == _numLoadedDependencies)
		finalize();
}

render::States::Ptr
EffectParser::parseRenderStates(Json::Value&			root,
								AbstractContext::Ptr	context,
								TexturePtrMap&			targets,
								render::States::Ptr		defaultStates)
{
	auto priority = root.get("priority", defaultStates->priority()).asFloat();
	auto blendSrcFactor	= defaultStates->blendingSourceFactor();
	auto blendDstFactor	= defaultStates->blendingDestinationFactor();
	auto depthMask = defaultStates->depthMask();
	auto depthFunc = defaultStates->depthFunc();
    auto triangleCulling = defaultStates->triangleCulling();
	
	render::Texture::Ptr target = defaultStates->target();
	std::unordered_map<std::string, SamplerState> samplerStates = defaultStates->samplers();

	parseBlendMode(root, blendSrcFactor, blendDstFactor);
	parseDepthTest(root, depthMask, depthFunc);
	parseTriangleCulling(root, triangleCulling);
    parseSamplerStates(root, samplerStates);
	parseTarget(root, context, targets);

	return render::States::create(
		samplerStates,
		priority,
		blendSrcFactor,
		blendDstFactor,
		depthMask,
		depthFunc,
		triangleCulling,
		target
	);
}

void
EffectParser::parsePasses(Json::Value&				root,
						  const std::string&		resolvedFilename,
						  file::Options::Ptr		options,
						  AbstractContext::Ptr		context,
						  std::vector<Pass::Ptr>&	passes,
						  TexturePtrMap&			targets,
						  data::BindingMap&			defaultAttributeBindings,
						  data::BindingMap&			defaultUniformBindings,
						  data::BindingMap&			defaultStateBindings,
						  data::MacroBindingMap&	defaultMacroBindings,
						  render::States::Ptr		defaultStates)
{
	auto passId = 0;

	for (auto passValue : root.get("passes", 0))
	{
		if (passValue.isString())
		{
			auto name = passValue.asString();
			auto pass = std::find_if(
				_globalPasses.begin(),
				_globalPasses.end(),
				[&](Pass::Ptr pass)
				{
					return pass->name() == name;
				}
			);

			if (pass != _globalPasses.end())
				throw std::logic_error("Pass '" + name + "' does not exist.");

			passes.push_back(*pass);
		}

		auto name = passValue.get("name", std::to_string(passId++)).asString();

		// pass bindings
		data::BindingMap		attributeBindings(defaultAttributeBindings);
		data::BindingMap		uniformBindings(defaultUniformBindings);
		data::BindingMap		stateBindings(defaultStateBindings);
		data::MacroBindingMap	macroBindings(defaultMacroBindings);
        
		parseBindings(passValue, attributeBindings, uniformBindings, stateBindings, macroBindings);

		// render states
		auto states = parseRenderStates(passValue, context, targets, defaultStates);

		// program
		auto vertexShaderValue = passValue.get("vertexShader", "");
		auto vertexShader = parseShader(
			vertexShaderValue, resolvedFilename, options, render::Shader::Type::VERTEX_SHADER
		);
		
		auto fragmentShaderValue = passValue.get("fragmentShader", "");
		auto fragmentShader = parseShader(
			fragmentShaderValue, resolvedFilename, options, render::Shader::Type::FRAGMENT_SHADER
		);

        auto target = parseTarget(passValue, options->context(), targets);
		auto pass = render::Pass::create(
			name,
			Program::create(options->context(), vertexShader, fragmentShader),
			attributeBindings,
			uniformBindings,
			stateBindings,
			macroBindings,
            states
		);

        passes.push_back(pass);

		parseDependencies(passValue, resolvedFilename, options, _passIncludes[pass]);
	}
}

render::Shader::Ptr
EffectParser::parseShader(Json::Value& 			shaderNode,
						  const std::string&	resolvedFilename,
						  file::Options::Ptr    options,
						  render::Shader::Type 	type)
{
	if (shaderNode.isObject())
	{
		auto shader = Shader::create(options->context(), type, shaderNode.get("code", "").asString());

		parseDependencies(shaderNode, resolvedFilename, options, _shaderIncludes[shader]);

		return shader;
	}
	else if (shaderNode.isString())
		return Shader::create(options->context(), type, shaderNode.asString());

	throw;
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
EffectParser::parseDepthTest(Json::Value& contextNode, bool& depthMask, render::CompareMode& depthFunc)
{
	auto depthTest	= contextNode.get("depthTest", 0);
	
	if (depthTest.isObject())
	{
        auto depthMaskValue = depthTest.get("depthMask", 0);
        auto depthFuncValue = depthTest.get("depthFunc", 0);

        if (depthMaskValue.isBool())
            depthMask = depthMaskValue.asBool();

        if (depthFuncValue.isString())
    		depthFunc = _depthFuncMap[depthFuncValue.asString()];
	}
    else if (depthTest.isArray())
    {
        depthMask = depthTest[0].asBool();
		depthFunc = _depthFuncMap[depthTest[1].asString()];
    }
}

void
EffectParser::parseTriangleCulling(Json::Value& contextNode, TriangleCulling& triangleCulling)
{
    auto triangleCullingValue   = contextNode.get("triangleCulling", 0);

    if (triangleCullingValue.isString())
    {
        auto triangleCullingString = triangleCullingValue.asString();

        if (triangleCullingString == "back")
            triangleCulling = TriangleCulling::BACK;
        else if (triangleCullingString == "front")
            triangleCulling = TriangleCulling::FRONT;
        else if (triangleCullingString == "both")
            triangleCulling = TriangleCulling::BOTH;
        else if (triangleCullingString == "none")
            triangleCulling = TriangleCulling::NONE;
    }
}

void
EffectParser::parseBindings(Json::Value&			contextNode,
						    data::BindingMap&		attributeBindings,
						    data::BindingMap&		uniformBindings,
						    data::BindingMap&		stateBindings,
							data::MacroBindingMap&	macroBindings)
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

	auto macroBindingsValue = contextNode.get("macroBindings", 0);
	if (macroBindingsValue.isObject())
		for (auto propertyName : macroBindingsValue.getMemberNames())
		{
			auto macroBindingValue = macroBindingsValue.get(propertyName, 0);

			if (macroBindingValue.isString())
				macroBindings[propertyName] = std::tuple<std::string, int, int>(macroBindingValue.asString(), -1, -1);
			else if (macroBindingValue.isObject())
			{
				auto nameValue = macroBindingValue.get("name", 0);
				auto minValue = macroBindingsValue.get("min", -1);
				auto maxValue = macroBindingsValue.get("max", -1);

				if (!nameValue.isString() || !minValue.isInt() ||!maxValue.isInt())
					throw;

				macroBindings[propertyName] = std::tuple<std::string, int, int>(
					nameValue.asString(),
					minValue.asInt(),
					maxValue.asInt()
				);
			}
		}
}

void
EffectParser::parseSamplerStates(Json::Value&                                           contextNode,
                                 std::unordered_map<std::string, render::SamplerState>& samplerStates)
{
    auto samplerStatesValue = contextNode.get("samplerStates", 0);

    if (samplerStatesValue.isObject())
        for (auto propertyName : samplerStatesValue.getMemberNames())
        {
            auto samplerStateValue = samplerStatesValue.get(propertyName, 0);

            if (samplerStateValue.isObject())
            {
                auto wrapModeStr        = samplerStateValue.get("wrapMode", "clamp").asString();
                auto textureFilterStr   = samplerStateValue.get("textureFilter", "nearest").asString();
                auto mipFilterStr       = samplerStateValue.get("mipFilter", "none").asString();
                auto wrapMode           = wrapModeStr == "repeat" ? WrapMode::REPEAT : WrapMode::CLAMP;
                auto textureFilter      = textureFilterStr == "linear"
                    ? TextureFilter::LINEAR
                    : TextureFilter::NEAREST;
                auto mipFilter          = mipFilterStr == "linear"
                    ? MipFilter::LINEAR
                    : (mipFilterStr == "nearest" ? MipFilter::NEAREST : MipFilter::NONE);

                samplerStates[propertyName] = SamplerState(wrapMode, textureFilter, mipFilter);
            }
        }
}

std::shared_ptr<render::Texture>
EffectParser::parseTarget(Json::Value&                      contextNode,
                          std::shared_ptr<AbstractContext>  context,
                          TexturePtrMap&                    targets)
{
    auto targetValue = contextNode.get("target", 0);

	std::shared_ptr<render::Texture> target;
	std::string targetName;

    if (targetValue.isObject())
    {
        auto nameValue  = targetValue.get("name", 0);

        if (nameValue.isString())
			targetName = nameValue.asString();

        auto sizeValue  = targetValue.get("size", 0);
        auto width      = 0;
        auto height     = 0;

        if (!sizeValue.empty())
            width = height = sizeValue.asUInt();
        else
        {
            width = targetValue.get("width", 0).asUInt();
            height = targetValue.get("height", 0).asUInt();
        }

        target = render::Texture::create(context, width, height, true);

		if (targetName.length())
	        _assetLibrary->texture(targetName, target);

        return target;
    }
	else if (targetValue.isString())
	{
		targetName = targetValue.asString();
		target = _assetLibrary->texture(targetName);
	}

	if (target && targetName.length())
		targets[targetName] = target;

    return target;
}

void
EffectParser::parseDependencies(Json::Value& 				root,
								const std::string& 			filename,
								file::Options::Ptr 			options,
								std::vector<LoaderPtr>& 	store)
{
	auto includes	= root.get("includes", 0);
	int pos			= filename.find_last_of("/");

	if (pos > 0)
	{
		options = file::Options::create(options);
		options->includePaths().insert(filename.substr(0, pos));
	}

	if (includes.isArray())
	{
		_numDependencies += includes.size();

		for (auto include : includes)
		{
			auto loader = Loader::create();

			_loaderCompleteSlots[loader] = loader->complete()->connect(std::bind(
				&EffectParser::dependencyCompleteHandler, shared_from_this(), std::placeholders::_1
			));
			_loaderErrorSlots[loader] = loader->error()->connect(std::bind(
				&EffectParser::dependencyErrorHandler, shared_from_this(), std::placeholders::_1
			));

			store.push_back(loader);
			loader->load(include.asString(), options);
		}
	}
}

void
EffectParser::parseTechniques(Json::Value&						root,
							  const std::string&				filename,
							  std::shared_ptr<file::Options>	options,
							  render::AbstractContext::Ptr		context)
{
	auto techniquesValues = root.get("techniques", 0);

	if (techniquesValues.isArray())
	{
		for (auto techniqueValue : techniquesValues)
		{
			if (techniqueValue.isObject())
			{
				auto techniqueName	= techniqueValue.get("name", "default").asString();
				auto& targets		= _techniqueTargets[techniqueName];
				auto& passes		= _techniquePasses[techniqueName];

				data::BindingMap		attributeBindings(_defaultAttributeBindings);
				data::BindingMap		uniformBindings(_defaultUniformBindings);
				data::BindingMap		stateBindings(_defaultStateBindings);
				data::MacroBindingMap	macroBindings(_defaultMacroBindings);
        
				// bindings
				parseBindings(techniqueValue, _defaultAttributeBindings, _defaultUniformBindings, _defaultStateBindings, _defaultMacroBindings);

				// render states
				auto states = parseRenderStates(techniqueValue, context, _globalTargets, _defaultStates);

				parsePasses(
					techniqueValue,
					filename,
					options,
					context,
					passes,
					targets,
					attributeBindings,
					uniformBindings,
					stateBindings,
					macroBindings,
					states
				);
			}
		}
	}
	else
	{
		_techniquePasses["default"] = _globalPasses;
		_techniqueTargets["default"] = _globalTargets;
	}
}

void
EffectParser::dependencyCompleteHandler(std::shared_ptr<AbstractLoader> loader)
{
	++_numLoadedDependencies;

	if (_numDependencies == _numLoadedDependencies && _effect)
		finalize();
}

void
EffectParser::dependencyErrorHandler(std::shared_ptr<AbstractLoader> loader)
{
	std::cerr << "Unable to load dependency '" << loader->filename() << "'" << std::endl;
	throw;
}

std::string
EffectParser::concatenateIncludes(std::vector<LoaderPtr>& store)
{
	std::string code = "";

	for (auto loader : store)
		code += std::string((char*)&loader->data()[0], loader->data().size()) + "\r\n";

	return code;
}

void
EffectParser::finalize()
{
	auto effectIncludes = concatenateIncludes(_effectIncludes);

	for (auto& technique : _techniquePasses)
    {
		auto passes = technique.second;

		for (auto& pass : passes)
		{
			auto program = pass->program();
			auto passIncludes = concatenateIncludes(_passIncludes[pass]);

			program->vertexShader()->source(
				"#define VERTEX_SHADER\r\n"
				+ effectIncludes
				+ passIncludes
				+ concatenateIncludes(_shaderIncludes[program->vertexShader()])
				+ program->vertexShader()->source()
			);
			program->fragmentShader()->source(
				"#define FRAGMENT_SHADER\r\n"
				+ effectIncludes
				+ passIncludes
				+ concatenateIncludes(_shaderIncludes[program->fragmentShader()])
				+ program->fragmentShader()->source()
			);
		}

		_effect->addTechnique(technique.first, passes);
    }

	_effect->technique(_defaultTechnique);
	for (auto& targets : _techniqueTargets)
		for (auto& target : targets.second)
			_effect->setUniform(target.first, target.second);

	_assetLibrary->effect(_effectName, _effect);
    _assetLibrary->effect(_filename, _effect);

	_complete->execute(shared_from_this());
}
