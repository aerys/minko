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

#include "minko/file/EffectParser.hpp"

#include "minko/file/Loader.hpp"
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
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/CubeTexture.hpp"
#include "minko/render/Pass.hpp"
#include "minko/render/Priority.hpp"
#include "minko/file/FileProtocol.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "json/json.h"

using namespace minko;
using namespace minko::data;
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

std::unordered_map<std::string, render::CompareMode> EffectParser::_compareFuncMap = EffectParser::initializeCompareFuncMap();
std::unordered_map<std::string, render::CompareMode>
EffectParser::initializeCompareFuncMap()
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

std::unordered_map<std::string, render::StencilOperation> EffectParser::_stencilOpMap = EffectParser::initializeStencilOperationMap();
std::unordered_map<std::string, render::StencilOperation>
EffectParser::initializeStencilOperationMap()
{
	std::unordered_map<std::string, render::StencilOperation> m;

	m["keep"]			= render::StencilOperation::KEEP;
	m["zero"]			= render::StencilOperation::ZERO;
	m["replace"]		= render::StencilOperation::REPLACE;
	m["incr"]			= render::StencilOperation::INCR;
	m["incr_wrap"]		= render::StencilOperation::INCR_WRAP;
	m["decr"]			= render::StencilOperation::DECR;
	m["decr_wrap"]		= render::StencilOperation::DECR_WRAP;
	m["invert"]			= render::StencilOperation::INVERT;

	return m;
}

std::unordered_map<std::string, float> EffectParser::_priorityMap = EffectParser::initializePriorityMap();
std::unordered_map<std::string, float>
EffectParser::initializePriorityMap()
{
	std::unordered_map<std::string, float> m;

	// The higher the priority, the earlier the drawcall is rendered.
	m["first"]			= Priority::FIRST;
	m["background"]		= Priority::BACKGROUND;
	m["opaque"]			= Priority::OPAQUE;
	m["transparent"]	= Priority::TRANSPARENT;
	m["last"]			= Priority::LAST;

	return m;
}

data::MacroBindingMap
EffectParser::initializeDefaultMacroBindings() const
{
	data::MacroBindingMap m;
	std::string propertyName;

#if MINKO_PLATFORM & MINKO_PLATFORM_WINDOWS
	propertyName = "MINKO_PLATFORM_WINDOWS";
#elif MINKO_PLATFORM & MINKO_PLATFORM_OSX
	propertyName = "MINKO_PLATFORM_OSX";
#elif MINKO_PLATFORM & MINKO_PLATFORM_LINUX
	propertyName = "MINKO_PLATFORM_LINUX";
#elif MINKO_PLATFORM & MINKO_PLATFORM_IOS
	propertyName = "MINKO_PLATFORM_IOS";
#elif MINKO_PLATFORM & MINKO_PLATFORM_ANDROID
	propertyName = "MINKO_PLATFORM_ANDROID";
#elif MINKO_PLATFORM & MINKO_PLATFORM_HTML5
	propertyName = "MINKO_PLATFORM_HTML5";
#else
	return m;
#endif

	MacroBindingDefault&	bindingDefault	= std::get<2>(m[propertyName]);

	bindingDefault.semantic = MacroBindingDefaultValueSemantic::PROPERTY_EXISTS;
	bindingDefault.value.propertyExists = true;

	return m;
}

float
EffectParser::priority(const std::string& name)
{
	auto foundPriorityIt = _priorityMap.find(name);

	return foundPriorityIt != _priorityMap.end()
		? foundPriorityIt->second
		: _priorityMap["opaque"];
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

	if (!reader.parse((const char*)&data[0], (const char*)&data[data.size() - 1], root, false))
		throw file::ParserError(resolvedFilename + ": " + reader.getFormattedErrorMessages());

    int pos	= resolvedFilename.find_last_of("/\\");

	_options = file::Options::create(options);

	if (pos != std::string::npos)
		_options->includePaths().push_front(resolvedFilename.substr(0, pos));

	_filename = filename;
	_resolvedFilename = resolvedFilename;
	_assetLibrary = assetLibrary;
	_effectName	= root.get("name", filename).asString();
	_defaultTechnique = root.get("defaultTechnique", "default").asString();

	auto context = _assetLibrary->context();

	// parse default values for bindings and states
	/*
	auto defaultQueue = root.get("queue", "opaque").asString();
	_defaultStates->priority(priority(defaultQueue));
	*/
	_defaultStates = parseRenderStates(root, context, _globalTargets, _defaultStates, 0.0f);
	_defaultMacroBindings = initializeDefaultMacroBindings();

	parseBindings(
		root,
		_defaultAttributeBindings,
		_defaultUniformBindings,
		_defaultStateBindings,
		_defaultMacroBindings,
		_defaultUniformValues
	);

	// parse a global list of passes
	parsePasses(
		root,
		resolvedFilename,
		_options,
		context,
		_globalPasses,
		_globalTargets,
		_defaultAttributeBindings,
		_defaultUniformBindings,
		_defaultStateBindings,
		_defaultMacroBindings,
		_defaultStates,
		_defaultUniformValues
	);

	// parse a global list of render targets
	auto targetsValue = root.get("targets", 0);

	if (targetsValue.isArray())
		for (auto targetValue : targetsValue)
			parseTarget(targetValue, context, _globalTargets);

	// parse the list of techniques, if no "techniques" directive is found then
	// the global list of passes becomes the "default" techinque
	parseTechniques(root, resolvedFilename, _options, context);

    _effect = render::Effect::create();

	if (_numDependencies == _numLoadedDependencies)
		finalize();
	
	_options->includePaths().pop_front();
}

render::States::Ptr
EffectParser::parseRenderStates(const Json::Value&		root,
								AbstractContext::Ptr	context,
								TexturePtrMap&			targets,
								render::States::Ptr		defaultStates,
								float					priorityOffset)
{
	auto zSorted			= defaultStates->zSorted();
	auto blendSrcFactor		= defaultStates->blendingSourceFactor();
	auto blendDstFactor		= defaultStates->blendingDestinationFactor();
	auto colorMask			= defaultStates->colorMask();
	auto depthMask			= defaultStates->depthMask();
	auto depthFunc			= defaultStates->depthFunc();
    auto triangleCulling	= defaultStates->triangleCulling();
	auto stencilFunc		= defaultStates->stencilFunction();
	auto stencilRef			= defaultStates->stencilReference();
	auto stencilMask		= defaultStates->stencilMask();
	auto stencilFailOp		= defaultStates->stencilFailOperation();
	auto stencilZFailOp		= defaultStates->stencilDepthFailOperation();
	auto stencilZPassOp		= defaultStates->stencilDepthPassOperation();
	auto scissorTest		= defaultStates->scissorTest();
	auto scissorBox			= defaultStates->scissorBox();

	AbstractTexture::Ptr target = defaultStates->target();
	std::unordered_map<std::string, SamplerState> samplerStates = defaultStates->samplers();

	const float priority = parsePriority(root, defaultStates->priority() + priorityOffset);
	parseZSort(root, zSorted);
	parseBlendMode(root, blendSrcFactor, blendDstFactor);
	parseColorMask(root, colorMask);
	parseDepthTest(root, depthMask, depthFunc);
	parseTriangleCulling(root, triangleCulling);
    parseSamplerStates(root, samplerStates);
	parseStencilState(root, stencilFunc, stencilRef, stencilMask, stencilFailOp, stencilZFailOp, stencilZPassOp);
	parseScissorTest(root, scissorTest, scissorBox);
	target = parseTarget(root, context, targets);

	return render::States::create(
		samplerStates,
		(float)priority,
		zSorted,
		blendSrcFactor,
		blendDstFactor,
		colorMask,
		depthMask,
		depthFunc,
		triangleCulling,
		stencilFunc,
		stencilRef,
		stencilMask,
		stencilFailOp,
		stencilZFailOp,
		stencilZPassOp,
		scissorTest,
		scissorBox,
		target
	);
}

void
EffectParser::parsePasses(const Json::Value&		root,
						  const std::string&		resolvedFilename,
						  file::Options::Ptr		options,
						  AbstractContext::Ptr		context,
						  std::vector<Pass::Ptr>&	passes,
						  TexturePtrMap&			targets,
						  BindingMap&			    defaultAttributeBindings,
						  BindingMap&			    defaultUniformBindings,
						  BindingMap&			    defaultStateBindings,
						  MacroBindingMap&	        defaultMacroBindings,
						  render::States::Ptr		defaultStates,
						  UniformValues&			defaultUniformDefaultValues)
{
	auto passId				= 0;
	auto passesValue		= root.get("passes", 0);

    for (auto passValue : passesValue)
    {
        passId++;
		const auto priorityOffset = (float)(passesValue.size() - passId);

        if (passValue.isString())
        {
            auto name = passValue.asString();
            auto passIt = std::find_if(
                _globalPasses.begin(),
                _globalPasses.end(),
                [&](Pass::Ptr pass)
	            {
	                return pass->name() == name;
	            }
	        );

            if (passIt == _globalPasses.end())
                throw file::ParserError("Pass '" + name + "' does not exist.");

            auto pass = *passIt;
            auto passCopy = Pass::create(pass, true);

			passCopy->states()->priority(defaultStates->priority() + priorityOffset);
			_glslBlocks[passCopy->program()->vertexShader()] = _glslBlocks[pass->program()->vertexShader()];
			_glslBlocks[passCopy->program()->fragmentShader()] = _glslBlocks[pass->program()->fragmentShader()];

            // set uniform default values
            for (auto& nameAndValues : defaultUniformDefaultValues)
                setUniformDefaultValueOnPass(
                    passCopy,
                    nameAndValues.first,
                    nameAndValues.second.first,
                    nameAndValues.second.second
                );

            passes.push_back(passCopy);
        }
        else
        {
            if (!parseConfiguration(passValue))
                continue;

            auto	name		= passValue.get("name", std::to_string(passId)).asString();
			auto	fallback	= passValue.get("fallback", std::string()).asString();

            // pass bindings
            BindingMap		attributeBindings(defaultAttributeBindings);
            BindingMap		uniformBindings(defaultUniformBindings);
            BindingMap		stateBindings(defaultStateBindings);
            MacroBindingMap	macroBindings(defaultMacroBindings);
            UniformValues	uniformDefaultValues(defaultUniformDefaultValues);

            parseBindings(
                passValue,
                attributeBindings,
                uniformBindings,
                stateBindings,
                macroBindings,
                uniformDefaultValues
            );

            // render states
            auto states = parseRenderStates(passValue, context, targets, defaultStates, priorityOffset);

            // program
            auto vertexShaderValue = passValue.get("vertexShader", 0);
            auto vertexShader = parseShader(
                vertexShaderValue, resolvedFilename, options, render::Shader::Type::VERTEX_SHADER
            );

            if (!vertexShader)
            	throw file::ParserError("Missing vertex shader for pass '" + name + "'");

            auto fragmentShaderValue = passValue.get("fragmentShader", 0);
            auto fragmentShader = parseShader(
                fragmentShaderValue, resolvedFilename, options, render::Shader::Type::FRAGMENT_SHADER
            );

            if (!fragmentShader)
				throw file::ParserError("Missing fragment shader for pass '" + name + "'");

            auto pass = render::Pass::create(
                name,
                Program::create(options->context(), vertexShader, fragmentShader),
                attributeBindings,
                uniformBindings,
                stateBindings,
                macroBindings,
                states,
				fallback
            );

			// set uniform default values
            for (auto& nameAndValues : uniformDefaultValues)
                setUniformDefaultValueOnPass(
                    pass,
                    nameAndValues.first,
                    nameAndValues.second.first,
                    nameAndValues.second.second
                );

            passes.push_back(pass);
        }
	}
}

void
EffectParser::setUniformDefaultValueOnPass(render::Pass::Ptr	pass,
										   const std::string&	name,
										   UniformType			type,
										   UniformValue&		value)
{
	if (type == UniformType::INT)
	{
		auto& nv = value.numericValue;

		if (nv.size() == 1)
			pass->setUniform(name, nv[0].intValue);
		else if (nv.size() == 2)
			pass->setUniform(name, nv[0].intValue, nv[1].intValue);
		else if (nv.size() == 3)
			pass->setUniform(name, nv[0].intValue, nv[1].intValue, nv[2].intValue);
		else if (nv.size() == 4)
			pass->setUniform(name, nv[0].intValue, nv[1].intValue, nv[2].intValue, nv[3].intValue);
	}
	else if (type == UniformType::FLOAT)
	{
		auto& nv = value.numericValue;

		if (nv.size() == 1)
			pass->setUniform(name, nv[0].floatValue);
		else if (nv.size() == 2)
			pass->setUniform(name, nv[0].floatValue, nv[1].floatValue);
		else if (nv.size() == 3)
			pass->setUniform(name, nv[0].floatValue, nv[1].floatValue, nv[2].floatValue);
		else if (nv.size() == 4)
			pass->setUniform(name, nv[0].floatValue, nv[1].floatValue, nv[2].floatValue, nv[3].floatValue);
	}
	else if (type == UniformType::TEXTURE)
	{
		pass->setUniform(name, value.textureValue);
	}
}

render::Shader::Ptr
EffectParser::parseShader(const Json::Value& 		shaderNode,
						  const std::string&		resolvedFilename,
						  file::Options::Ptr    	options,
						  render::Shader::Type 		type)
{
	std::string glsl;

	if (shaderNode.isString())
		glsl = shaderNode.asString();
	else
		return nullptr;

	auto shader = Shader::create(options->context(), type, glsl);
	auto blocks = std::shared_ptr<GLSLBlockList>(new GLSLBlockList());

	blocks->push_front(GLSLBlock(GLSLBlockType::TEXT, ""));
	_glslBlocks[shader] = blocks;
	parseGLSL(glsl, options, blocks, blocks->begin());

	return shader;
}

void
EffectParser::parseGLSL(std::string 			glsl,
						file::Options::Ptr 		options,
						GLSLBlockListPtr		blocks,
						GLSLBlockList::iterator	insertIt)
{
	std::string line;
	std::stringstream stream(glsl);
	auto i = 0;
	auto lastBlockEnd = 0;
	auto numIncludes = 0;

	while (std::getline(stream, line))
	{
		auto firstSharpPos = line.find_first_of('#');
		if (firstSharpPos != std::string::npos && line.substr(firstSharpPos, 16) == "#pragma include("
			&& (line[firstSharpPos + 16] == '"' || line[firstSharpPos + 16] == '\''))
		{
			auto filename = line.substr(firstSharpPos + 17, line.find_last_of(line[firstSharpPos + 16]) - 17 - firstSharpPos);

			if (lastBlockEnd != i)
				insertIt = blocks->insert_after(insertIt, GLSLBlock(GLSLBlockType::TEXT, glsl.substr(lastBlockEnd, i - lastBlockEnd)));
			insertIt = blocks->insert_after(insertIt, GLSLBlock(GLSLBlockType::FILE, filename));

			lastBlockEnd = i + line.size() + 1;

			++numIncludes;
		}
		i += line.size() + 1;
	}

	if (i != lastBlockEnd)
		insertIt = blocks->insert_after(insertIt, GLSLBlock(GLSLBlockType::TEXT, glsl.substr(lastBlockEnd)));

	if (numIncludes)
		loadGLSLDependencies(blocks, options);
}

void
EffectParser::loadGLSLDependencies(GLSLBlockListPtr		blocks,
								   file::Options::Ptr 	options)
{
	for (auto blockIt = blocks->begin(); blockIt != blocks->end(); blockIt++)
	{
		auto& block = *blockIt;

		if (block.first == GLSLBlockType::FILE)
		{
			if (options->assetLibrary()->hasBlob(block.second))
			{
				auto& data = options->assetLibrary()->blob(block.second);

				block.first = GLSLBlockType::TEXT;
#ifdef DEBUG
				block.second = "//#pragma include(\"" + block.second + "\")\n";
#else
				block.second = "\n";
#endif
				parseGLSL(std::string((const char*)&data[0], data.size()), options, blocks, blockIt);
			}
			else
			{
				auto loader = Loader::create(options);

				++_numDependencies;

				_loaderCompleteSlots[loader] = loader->complete()->connect(std::bind(
				    &EffectParser::glslIncludeCompleteHandler,
				    std::static_pointer_cast<EffectParser>(shared_from_this()),
					std::placeholders::_1,
					blocks,
					blockIt,
					block.second
				));

				_loaderErrorSlots[loader] = loader->error()->connect(std::bind(
				    &EffectParser::dependencyErrorHandler,
					std::static_pointer_cast<EffectParser>(shared_from_this()),
					std::placeholders::_1,
					std::placeholders::_2,
					block.second
			    ));

				loader->queue(block.second)->load();
			}
		}
	}

	if (_numDependencies == _numLoadedDependencies && _effect)
		finalize();
}

void
EffectParser::glslIncludeCompleteHandler(LoaderPtr 			        loader,
										 GLSLBlockListPtr 			blocks,
	 								     GLSLBlockList::iterator 	blockIt,
                                         const std::string&         filename)
{
	auto& block = *blockIt;

	block.first = GLSLBlockType::TEXT;
#ifdef DEBUG
	block.second = "//#pragma include(\"" + filename + "\")\n";
#else
	block.second = "\n";
#endif

	++_numLoadedDependencies;

    auto file = loader->files().at(filename);
    auto resolvedFilename = file->resolvedFilename();
	auto options = Options::create(_options);
	auto pos = resolvedFilename.find_last_of("/\\");

    //options->includePaths().clear();
	if (pos != std::string::npos)
	{
		options = file::Options::create(options);
        options->includePaths().push_back(resolvedFilename.substr(0, pos));
	}

    parseGLSL(std::string((const char*)&file->data()[0], file->data().size()), options, blocks, blockIt);

	if (_numDependencies == _numLoadedDependencies && _effect)
		finalize();
}

void
EffectParser::dependencyErrorHandler(std::shared_ptr<Loader> loader, const ParserError& error, const std::string& filename)
{
#ifdef DEBUG
	std::cerr << "Unable to load dependency '" << filename << "', included paths are:" << std::endl;
	for (auto& path : loader->options()->includePaths())
		std::cerr << path << std::endl;
#endif // defined(DEBUG)

	throw file::ParserError("Unable to load dependencies.");
}

void
EffectParser::parseBlendMode(const Json::Value&				contextNode,
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
EffectParser::parseZSort(const Json::Value&	contextNode,
						 bool& zSorted) const
{
	auto zsortedValue	= contextNode.get("zSort", 0);

	if (zsortedValue.isBool())
		zSorted = zsortedValue.asBool();
}

void
EffectParser::parseColorMask(const Json::Value&	contextNode,
						     bool& colorMask) const
{
	auto colorMaskValue	= contextNode.get("colorMask", 0);

	if (colorMaskValue.isBool())
		colorMask = colorMaskValue.asBool();
}

void
EffectParser::parseDepthTest(const Json::Value& contextNode,
							 bool& depthMask,
							 render::CompareMode& depthFunc)
{
	auto depthTest	= contextNode.get("depthTest", 0);

	if (depthTest.isObject())
	{
        auto depthMaskValue = depthTest.get("depthMask", 0);
        auto depthFuncValue = depthTest.get("depthFunc", 0);

        if (depthMaskValue.isBool())
            depthMask = depthMaskValue.asBool();

        if (depthFuncValue.isString())
    		depthFunc = _compareFuncMap[depthFuncValue.asString()];
	}
    else if (depthTest.isArray())
    {
        depthMask = depthTest[0].asBool();
		depthFunc = _compareFuncMap[depthTest[1].asString()];
    }
    else
    {
    	auto depthMaskValue = contextNode.get("depthMask", 0);
    	auto depthFuncValue = contextNode.get("depthFunc", 0);

    	if (depthMaskValue.isBool())
    		depthMask = depthMaskValue.asBool();
    	if (depthFuncValue.isString())
    		depthFunc = _compareFuncMap[depthFuncValue.asString()];
    }
}

void
EffectParser::parseTriangleCulling(const Json::Value& contextNode,
								   TriangleCulling& triangleCulling)
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

float
EffectParser::parsePriority(const Json::Value& contextNode,
							float defaultPriority)
{
	auto	priorityNode	= contextNode.get("priority", defaultPriority);
	float	ret				= defaultPriority;

	if (!priorityNode.isNull())
	{
		if (priorityNode.isInt())
			ret = (float)priorityNode.asInt();
		else if (priorityNode.isDouble())
			ret = (float)priorityNode.asDouble();
		else if (priorityNode.isString())
			ret = priority(priorityNode.asString());
		else if (priorityNode.isArray())
		{
			if (priorityNode[0].isString() && priorityNode[1].isDouble())
				ret = priority(priorityNode[0].asString()) + (float)priorityNode[1].asDouble();
		}
	}

	return ret;
}

void
EffectParser::parseBindingNameAndSource(const Json::Value&	contextNode, 
										std::string&		propertyName, 
										BindingSource&		source,
										RegexPtr&			regexp)
{
	source	= BindingSource::TARGET;
	regexp	= nullptr;
	if (contextNode.isString())
		propertyName = contextNode.asString();
	else if (contextNode.isObject())
	{
		auto propertyValue = contextNode.get("property", 0);
		auto sourceValue = contextNode.get("source", 0);

		if (propertyValue.isString())
			propertyName = propertyValue.asString();

		if (sourceValue.isString())
		{
			auto sourceString = sourceValue.asString();

			if (sourceString == "target")
				source = BindingSource::TARGET;
			else if (sourceString == "renderer")
				source = BindingSource::RENDERER;
			else if (sourceString == "root")
				source = BindingSource::ROOT;
		}
	}

	if (!propertyName.empty())
	{
		const auto withVariableRegex	= std::regex(".*\\[\\$\\{.*\\}\\].*");
		const auto variableRegex		= std::regex("(\\$\\{)(.*)\\}");
	
		if (std::regex_match(propertyName, withVariableRegex))
		{
			// generate regex to recognize the macro when coming from filtered out data provider
	
			auto regexString	= std::regex_replace(propertyName, variableRegex, std::string("\\d"));
			regexString			= std::regex_replace(regexString, std::regex("(\\[|\\.|\\])"), std::string("\\$&"));

			regexp				= std::shared_ptr<std::regex>(new std::regex(regexString));
		}
	}
}

void
EffectParser::parseBindings(const Json::Value&	contextNode,
						    BindingMap&			attributeBindings,
						    BindingMap&			uniformBindings,
						    BindingMap&			stateBindings,
							MacroBindingMap&	macroBindings,
							UniformValues&		uniformDefaultValues)
{
	auto attributeBindingsValue = contextNode.get("attributeBindings", 0);
	if (attributeBindingsValue.isObject())
		for (auto propertyName : attributeBindingsValue.getMemberNames())
			parseBindingNameAndSource(
				attributeBindingsValue.get(propertyName, 0),
				attributeBindings[propertyName].propertyName,
				attributeBindings[propertyName].source
			);

	parseUniformBindings(contextNode, uniformBindings, uniformDefaultValues);

	auto stateBindingsValue = contextNode.get("stateBindings", 0);
	if (stateBindingsValue.isObject())
		for (auto propertyName : stateBindingsValue.getMemberNames())
			parseBindingNameAndSource(
				stateBindingsValue.get(propertyName, 0),
				stateBindings[propertyName].propertyName,
				stateBindings[propertyName].source
			);

	parseMacroBindings(contextNode, macroBindings);
}

void
EffectParser::parseMacroBindings(const Json::Value&	contextNode, MacroBindingMap&	macroBindings)
{
	auto macroBindingsValue = contextNode.get("macroBindings", 0);

	if (macroBindingsValue.isObject())
	{
		for (auto propertyName : macroBindingsValue.getMemberNames())
		{
			auto macroBindingValue = macroBindingsValue.get(propertyName, 0);

			MacroBindingDefault&	bindingDefault	= std::get<2>(macroBindings[propertyName]);
			auto&					min				= std::get<3>(macroBindings[propertyName]);
			auto&					max				= std::get<4>(macroBindings[propertyName]);

			bindingDefault.semantic = MacroBindingDefaultValueSemantic::UNSET;
			min						= -INT_MAX;
			max						= INT_MAX;

			parseBindingNameAndSource(
				macroBindingValue,
				std::get<0>(macroBindings[propertyName]),
				std::get<1>(macroBindings[propertyName]),
				std::get<5>(macroBindings[propertyName])
			);

			if (macroBindingValue.isObject())
			{
				auto nameValue		= macroBindingValue.get("property", 0);
				auto minValue		= macroBindingValue.get("min", -INT_MAX);
				auto maxValue		= macroBindingValue.get("max", INT_MAX);
				auto defaultValue	= macroBindingValue.get("default", "");

				if (defaultValue.isInt())
				{
					bindingDefault.semantic = MacroBindingDefaultValueSemantic::VALUE;
					bindingDefault.value.value = defaultValue.asInt();
				}
				else if (defaultValue.isBool())
				{
					bindingDefault.semantic = MacroBindingDefaultValueSemantic::PROPERTY_EXISTS;
					bindingDefault.value.propertyExists = defaultValue.asBool();
				}

				min = minValue.asInt();
				max = maxValue.asInt();
			}
			else if (macroBindingValue.isInt())
			{
				bindingDefault.semantic = MacroBindingDefaultValueSemantic::VALUE;
				bindingDefault.value.value = macroBindingValue.asInt();
			}
			else if (macroBindingValue.isBool())
			{
				bindingDefault.semantic = MacroBindingDefaultValueSemantic::PROPERTY_EXISTS;
				bindingDefault.value.propertyExists = macroBindingValue.asBool();
			}
		}
	}
}

void
EffectParser::parseUniformBindings(const Json::Value&	contextNode,
								   BindingMap&	uniformBindings,
								   UniformValues&		uniformDefaultValues)
{
	auto uniformBindingsValue = contextNode.get("uniformBindings", 0);
	if (uniformBindingsValue.isObject())
		for (auto propertyName : uniformBindingsValue.getMemberNames())
		{
			auto uniformBindingValue = uniformBindingsValue.get(propertyName, 0);

			parseBindingNameAndSource(
				uniformBindingValue,
				uniformBindings[propertyName].propertyName,
				uniformBindings[propertyName].source
			);

			if (uniformBindingValue.isObject())
			{
				auto nameValue = uniformBindingValue.get("property", 0);
				auto defaultValue = uniformBindingValue.get("default", "");

				if (defaultValue.isArray() || defaultValue.isNumeric()
					|| (defaultValue.isString() && !defaultValue.asString().empty()))
				{
					parseUniformDefaultValues(defaultValue, uniformDefaultValues[propertyName]);
				}
			}
			else if (uniformBindingValue.isArray() || uniformBindingValue.isNumeric())
				parseUniformDefaultValues(uniformBindingValue, uniformDefaultValues[propertyName]);
		}
}

void
EffectParser::parseUniformDefaultValues(const Json::Value&		contextNode,
										UniformTypeAndValue&	uniformTypeAndValue)
{
	if (contextNode.isArray())
	{
		for (auto value : contextNode)
			parseUniformDefaultValues(value, uniformTypeAndValue);

		return;
	}

	UniformValue& v = uniformTypeAndValue.second;

	if (contextNode.isNumeric())
	{
		UniformNumericValue nv;

		if (contextNode.isDouble())
		{
			nv.floatValue = contextNode.asFloat();
			uniformTypeAndValue.first = UniformType::FLOAT;
		}
		else if (contextNode.isInt())
		{
			nv.intValue = contextNode.asInt();
			uniformTypeAndValue.first = UniformType::INT;
		}

		v.numericValue.push_back(nv);
	}
	else if (contextNode.isString())
	{
		auto textureFilename = contextNode.asString();
		int pos = _resolvedFilename.find_last_of("/\\");
		auto options = _options;

		uniformTypeAndValue.first = UniformType::TEXTURE;
		uniformTypeAndValue.second.textureValue = nullptr;

		if (pos > 0)
		{
			options = file::Options::create(_options);
			options->includePaths().push_back(_resolvedFilename.substr(0, pos));
		}

		uniformTypeAndValue.second.textureValue = _assetLibrary->texture(textureFilename);
		for (auto& path : options->includePaths())
		{
			uniformTypeAndValue.second.textureValue = _assetLibrary->texture(path + "/" + textureFilename);
			if (uniformTypeAndValue.second.textureValue)
				break;
		}

		if (!uniformTypeAndValue.second.textureValue)
			loadTexture(textureFilename, uniformTypeAndValue, options);
	}
}

void
EffectParser::loadTexture(const std::string&	textureFilename,
						  UniformTypeAndValue&	uniformTypeAndValue,
						  Options::Ptr			options)
{
	Options::Ptr o = Options::create(options);
    auto loader = Loader::create(o);

	o->loadAsynchronously(false);

	_numDependencies++;

	_loaderCompleteSlots[loader] = loader->complete()->connect([&](file::Loader::Ptr loader)
	{
		uniformTypeAndValue.second.textureValue = _assetLibrary->texture(textureFilename);
		uniformTypeAndValue.second.textureValue->upload();

        ++_numLoadedDependencies;
	});

	_loaderErrorSlots[loader] = loader->error()->connect(std::bind(
		&EffectParser::dependencyErrorHandler,
		std::static_pointer_cast<EffectParser>(shared_from_this()),
        std::placeholders::_1,
		std::placeholders::_2,
        textureFilename
	));

	loader->queue(textureFilename)->load();
}

void
EffectParser::parseSamplerStates(const Json::Value&                                     contextNode,
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

void
EffectParser::parseStencilState(const Json::Value& contextNode,
								CompareMode& stencilFunc,
								int& stencilRef,
								uint& stencilMask,
								StencilOperation& stencilFailOp,
								StencilOperation& stencilZFailOp,
								StencilOperation& stencilZPassOp) const
{
	auto stencilTest	= contextNode.get("stencilTest", 0);

	if (stencilTest.isObject())
	{
        auto stencilFuncValue	= stencilTest.get("stencilFunc", 0);
		auto stencilRefValue	= stencilTest.get("stencilRef", 0);
		auto stencilMaskValue	= stencilTest.get("stencilMask", 0);
		auto stencilOpsValue	= stencilTest.get("stencilOps", 0);

		if (stencilFuncValue.isString())
			stencilFunc	= _compareFuncMap[stencilFuncValue.asString()];
		if (stencilRefValue.isInt())
			stencilRef	= stencilRefValue.asInt();
		if (stencilMaskValue.isUInt())
			stencilMask	= stencilMaskValue.asUInt();
		parseStencilOperations(stencilOpsValue, stencilFailOp, stencilZFailOp, stencilZPassOp);
	}
    else if (stencilTest.isArray())
    {
		stencilFunc = _compareFuncMap[stencilTest[0].asString()];
		stencilRef	= stencilTest[1].asInt();
		stencilMask	= stencilTest[2].asUInt();
		parseStencilOperations(stencilTest[3], stencilFailOp, stencilZFailOp, stencilZPassOp);
    }
}

void
EffectParser::parseScissorTest(const Json::Value& contextNode,
								bool&			  scissorTest,
								ScissorBox&	      scissorBox) const
{
	auto scissorTestNode		= contextNode.get("scissorTest", 0);

	if (!scissorTestNode.isNull() && scissorTestNode.isBool())
		scissorTest = scissorTestNode.asBool();

	auto scissorBoxNode			= contextNode.get("scissorBox", 0);

	if (!scissorBoxNode.isNull() && scissorBoxNode.isArray())
	{
		if (scissorBoxNode[0].isInt())
			scissorBox.x		= scissorBoxNode[0].asInt();
		if (scissorBoxNode[1].isInt())
			scissorBox.y		= scissorBoxNode[1].asInt();
		if (scissorBoxNode[2].isInt())
			scissorBox.width	= scissorBoxNode[2].asInt();
		if (scissorBoxNode[3].isInt())
			scissorBox.height	= scissorBoxNode[3].asInt();
	}
}

void
EffectParser::parseStencilOperations(const Json::Value& contextNode,
									 StencilOperation& 	stencilFailOp,
									 StencilOperation& 	stencilZFailOp,
									 StencilOperation& 	stencilZPassOp) const
{
	if (contextNode.isArray())
	{
		if (contextNode[0].isString())
			stencilFailOp = _stencilOpMap[contextNode[0].asString()];
		if (contextNode[1].isString())
			stencilZFailOp = _stencilOpMap[contextNode[1].asString()];
		if (contextNode[2].isString())
			stencilZPassOp = _stencilOpMap[contextNode[2].asString()];
	}
	else
	{
		auto failValue	= contextNode.get("fail", 0);
		auto zfailValue	= contextNode.get("zfail", 0);
		auto zpassValue	= contextNode.get("zpass", 0);

		if (failValue.isString())
			stencilFailOp = _stencilOpMap[failValue.asString()];
		if (zfailValue.isString())
			stencilZFailOp = _stencilOpMap[zfailValue.asString()];
		if (zpassValue.isString())
			stencilZPassOp = _stencilOpMap[zpassValue.asString()];
	}
}

AbstractTexture::Ptr
EffectParser::parseTarget(const Json::Value&                contextNode,
                          std::shared_ptr<AbstractContext>  context,
                          TexturePtrMap&                    targets)
{
    auto targetValue = contextNode.get("target", 0);

	AbstractTexture::Ptr   target;
	std::string            targetName;

    if (targetValue.isObject())
    {
        auto nameValue  = targetValue.get("name", 0);

        if (nameValue.isString())
			targetName = nameValue.asString();

        auto sizeValue  = targetValue.get("size", 0);
        auto width      = 0;
        auto height     = 0;

        if (sizeValue.asUInt() != 0)
            width = height = sizeValue.asUInt();
        else
        {
            width = targetValue.get("width", 0).asUInt();
            height = targetValue.get("height", 0).asUInt();
        }

		const bool isCubeTexture = targetValue.get("isCube", 0).isBool()
			? targetValue.get("isCube", 0).asBool()
			: false;

		if (!isCubeTexture)
        {
            auto texture = Texture::create(context, width, height, false, true);

            if (targetName.length())
                _assetLibrary->texture(targetName, texture);
            target = texture;
        }
		else
        {
            auto cubeTexture = CubeTexture::create(context, width, height, false, true);

            if (targetName.length())
                _assetLibrary->cubeTexture(targetName, cubeTexture);
            target = cubeTexture;
        }

		target->upload();
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
EffectParser::parseTechniques(const Json::Value&				root,
							  const std::string&				filename,
							  std::shared_ptr<file::Options>	options,
							  render::AbstractContext::Ptr		context)
{
	auto techniquesValues = root.get("techniques", 0);

	if (techniquesValues.isArray())
	{
		for (auto techniqueValue : techniquesValues)
		{
			if (!parseConfiguration(techniqueValue))
				continue;

			if (techniqueValue.isObject())
			{
				auto techniqueName	= techniqueValue.get("name", "default").asString();
				auto& targets		= _techniqueTargets[techniqueName];
				auto& passes		= _techniquePasses[techniqueName];
				auto fallbackValue	= techniqueValue.get("fallback", 0);

				if (fallbackValue.isString() && fallbackValue.asString().length())
					_techniqueFallback[techniqueName] = fallbackValue.asString();

				BindingMap		attributeBindings(_defaultAttributeBindings);
				BindingMap		uniformBindings(_defaultUniformBindings);
				BindingMap		stateBindings(_defaultStateBindings);
				MacroBindingMap	macroBindings(_defaultMacroBindings);
				UniformValues	uniformDefaultValues(_defaultUniformValues);
        
				// bindings
				parseBindings(
					techniqueValue,
					attributeBindings,
					uniformBindings,
					stateBindings,
					macroBindings,
					uniformDefaultValues
				);

				// render states
				auto states = parseRenderStates(techniqueValue, context, _globalTargets, _defaultStates, 0.0f);

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
					states,
					uniformDefaultValues
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

bool
EffectParser::parseConfiguration(const Json::Value&	root)
{
	auto confValue	= root.get("configuration", 0);
	auto platforms	= _options->platforms();
	auto userFlags	= _options->userFlags();
	auto r = false;

	if (confValue.isArray())
	{
		for (auto value : confValue)
		{
			// if the config. token is a string and we can find it in the list of platforms,
			// then the configuration is ok and we return true
			if (value.isString() &&
				(std::find(platforms.begin(), platforms.end(), value.asString()) != platforms.end() ||
				std::find(userFlags.begin(), userFlags.end(), value.asString()) != userFlags.end()))
				return true;
			else if (value.isArray())
			{
				// if the config. token is an array, we check that *all* the string tokens are in
				// the platforms list; if a single of them is not there then the config. token
				// is considered to be false
				for (auto str : value)
				if (str.isString() &&
					(std::find(platforms.begin(), platforms.end(), str.asString()) == platforms.end() ||
					std::find(userFlags.begin(), userFlags.end(), str.asString()) != userFlags.end()))
				{
					r = r || false;
					break;
				}
			}
		}
	}
	else
		return true;

	return r;
}

std::string
EffectParser::concatenateGLSLBlocks(GLSLBlockListPtr blocks)
{
	std::string glsl = "";

	for (auto& block : *blocks)
		glsl += block.second;

	return glsl;
}

void
EffectParser::finalize()
{
	for (auto& technique : _techniquePasses)
    {
    	auto techniqueName = technique.first;
		auto passes = technique.second;

		for (auto& pass : passes)
		{
			auto program = pass->program();

			program->vertexShader()->source(
				"#define VERTEX_SHADER\r\n"
				+ concatenateGLSLBlocks(_glslBlocks[program->vertexShader()])
			);

			program->fragmentShader()->source(
				"#define FRAGMENT_SHADER\r\n"
				+ concatenateGLSLBlocks(_glslBlocks[program->fragmentShader()])
			);
		}

		if (_techniqueFallback.count(techniqueName))
		{
			_effect->addTechnique(techniqueName, passes, _techniqueFallback[techniqueName]);
			if (techniqueName != "default" && techniqueName == _defaultTechnique)
				_effect->addTechnique("default", passes, _techniqueFallback[techniqueName]);
		}
		else
		{
			_effect->addTechnique(techniqueName, passes);
			if (techniqueName != "default" && techniqueName == _defaultTechnique)
				_effect->addTechnique("default", passes);
		}
    }

	if (!_effect->techniques().empty() && _effect->techniques().count("default") == 0)
	{
		// FIXME
		const std::string&		viableTechniqueName = _effect->techniques().begin()->first;
		std::vector<Pass::Ptr>	viableTechnique		(_effect->technique(viableTechniqueName));

		if (_effect->hasFallback(viableTechniqueName))
			_effect->addTechnique("default", viableTechnique, _effect->fallback(viableTechniqueName));
		else
			_effect->addTechnique("default", viableTechnique);

#ifdef DEBUG
		std::cerr << "Warning:\tEffect '" << _effectName << "' does not provide achievable default technique ('" << _defaultTechnique << "'), switched to '" << viableTechniqueName << "'" << std::endl;
#endif // DEBUG
	}

#ifdef DEBUG_FALLBACK
	std::cout << "\nfinalize '" << _effectName << "'" << std::endl;
	for (auto& technique : _effect->techniques())
	{
		std::cout << "\t- technique '" << technique.first << "'";
		if (_effect->hasFallback(technique.first))
			std::cout << "\t-> '" << _effect->fallback(technique.first) << "'" << std::endl;
		else
			std::cout << std::endl;

		for (auto& pass : technique.second)
		{
			std::cout << "\t\t- pass '" << pass->name() << "'\tpriority = " << pass->states()->priority();
			if (!pass->fallback().empty())
				std::cout << "\t-> '" << pass->fallback() << "'" << std::endl;
			else
				std::cout << std::endl;
		}
	}
#endif // DEBUG_FALLBACK

	for (auto& techniqueNameAndTargets : _techniqueTargets)
		for (auto& target : techniqueNameAndTargets.second)
			_effect->data()->set(target.first, target.second);

	for (auto& targetNameAndPtr : _globalTargets)
		_effect->data()->set(targetNameAndPtr.first, targetNameAndPtr.second);

	_assetLibrary->effect(_effectName, _effect);
    _assetLibrary->effect(_filename, _effect);

	_complete->execute(shared_from_this());
}
