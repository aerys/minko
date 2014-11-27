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
#include "minko/data/Store.hpp"

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

std::array<std::string, 14> EffectParser::_stateNames = EffectParser::initializeStateNames();
std::array<std::string, 14>
EffectParser::initializeStateNames()
{
    std::array<std::string, 14> names = {
        "blendMode",
        "colorMask",
        "depthMask",
        "depthFunc",
        "triangleCulling",
        "stencilFunc",
        "stencilRef",
        "stencilMask",
        "stencilFailOp",
        "stencilZFailOp",
        "stencilZPassOp",
        "scissorBox",
        "priority",
        "zSort"
    };

    return names;
}

float
EffectParser::getPriorityValue(const std::string& name)
{
	auto foundPriorityIt = _priorityMap.find(name);

	return foundPriorityIt != _priorityMap.end()
		? foundPriorityIt->second
		: _priorityMap["opaque"];
}

EffectParser::EffectParser() :
	_effect(nullptr),
	_numDependencies(0),
	_numLoadedDependencies(0)
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
    {
        _options->includePaths().clear();
		_options->includePaths().push_front(resolvedFilename.substr(0, pos));
    }

	_filename = filename;
	_resolvedFilename = resolvedFilename;
	_assetLibrary = assetLibrary;
	_effectName	= root.get("name", filename).asString();

    parseGlobalScope(root, _globalScope);

    _effect = render::Effect::create(_effectName);
    if (_numDependencies == _numLoadedDependencies)
        finalize();
}

void
EffectParser::parseGlobalScope(const Json::Value& node, Scope& scope)
{
    parseAttributes(node, scope, scope.attributes);
    parseUniforms(node, scope, scope.uniforms);
    parseMacros(node, scope, scope.macros);
    parseStates(node, scope, scope.states);
    parsePasses(node, scope, scope.passes);
    parseTechniques(node, scope, scope.techniques);
}

bool
EffectParser::parseConfiguration(const Json::Value& node)
{
    auto confValue = node.get("configuration", 0);
    auto platforms = _options->platforms();
    auto userFlags = _options->userFlags();
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
            {
                return true;
            }
            else if (value.isArray())
            {
                // if the config. token is an array, we check that *all* the string tokens are in
                // the platforms list; if a single of them is not there then the config. token
                // is considered to be false
                for (auto str : value)
                {
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
    }
    else
        return true;

    return r;
}

void
EffectParser::parseTechniques(const Json::Value& node, Scope& scope, Techniques& techniques)
{
    auto techniquesNode = node.get("techniques", 0);
    auto firstTechnique = true;

    if (techniquesNode.isArray())
    {
        for (auto techniqueNode : techniquesNode)
        {
            // FIXME: switch to fallback instead of ignoring
            if (!parseConfiguration(techniqueNode))
                continue;

            auto techniqueNameNode = techniqueNode.get("name", 0);
            auto techniqueName = techniqueNameNode.isString()
                ? techniqueNameNode.asString()
                : firstTechnique ? "default" : "technique" + std::to_string(techniques.size());

            Scope techniqueScope(scope, scope);

            parseAttributes(techniqueNode, techniqueScope, techniqueScope.attributes);
            parseUniforms(techniqueNode, techniqueScope, techniqueScope.uniforms);
            parseMacros(techniqueNode, techniqueScope, techniqueScope.macros);
            parseStates(techniqueNode, techniqueScope, techniqueScope.states);
            parsePasses(techniqueNode, techniqueScope, techniques[techniqueName]);

            if (firstTechnique)
            {
                firstTechnique = false;
                techniques["default"] = techniques[techniqueName];
            }
        }
    }
    // FIXME: throw otherwise
}

void
EffectParser::parsePasses(const Json::Value& node, Scope& scope, std::vector<PassPtr>& passes)
{
    auto passesNode = node.get("passes", 0);

    if (passesNode.isArray())
    {
        for (auto passNode : passesNode)
        {
            // FIXME: switch to fallback instead of ignoring
            if (!parseConfiguration(passNode))
                continue;

            parsePass(passNode, scope, passes);
        }
    }
    // FIXME: throw otherwise
}

void
EffectParser::parsePass(const Json::Value& node, Scope& scope, std::vector<PassPtr>& passes)
{
    // If the pass is just a string, we assume it's referencing an existing pass defined in an
    // ancestor scope. Thus, we loop up to the root global scope to find the pass by its name.
    if (node.isString())
    {
        auto passName = node.asString();
        const Scope* searchScope = &scope;
        Pass::Ptr pass = nullptr;

        do
        {
            auto passIt = std::find_if(searchScope->passes.begin(), searchScope->passes.end(), [&](PassPtr p)
            {
                return p->name() == passName;
            });

            if (passIt != searchScope->passes.end())
                pass = *passIt;
            else
                searchScope = searchScope->parent;
        }
        while (searchScope != nullptr && pass == nullptr);

        if (pass == nullptr)
            throw std::runtime_error("Undefined pass with name '" + passName + "'.");

        passes.push_back(pass);
    }
    else
    {
        // If the pass is an actual pass object, we parse all its data, create the correspondin
        // Pass object and add it to the vector.

        Scope passScope(scope, scope);

        parseAttributes(node, passScope, passScope.attributes);
        parseUniforms(node, passScope, passScope.uniforms);
        parseMacros(node, passScope, passScope.macros);
        parseStates(node, passScope, passScope.states);

        auto passName = "pass" + std::to_string(scope.passes.size());
        auto nameNode = node.get("name", 0);
        if (nameNode.isString())
            passName = nameNode.asString();
        // FIXME: throw otherwise

        auto vertexShader = parseShader(node.get("vertexShader", 0), passScope, Shader::Type::VERTEX_SHADER);
        auto fragmentShader = parseShader(node.get("fragmentShader", 0), passScope, Shader::Type::FRAGMENT_SHADER);

        passes.push_back(Pass::create(
            passName,
            Program::create(_options->context(), vertexShader, fragmentShader),
            passScope.attributes.bindings,
            passScope.uniforms.bindings,
            passScope.states.bindings,
            passScope.macros.bindings,
            States() // FIXME
        ));
    }
}

//void
//EffectParser::parseDefaultValue(const Json::Value&  node,
//                                const Scope&        scope,
//                                const std::string&  valueName,
//                                Json::ValueType     expectedType,
//                                data::Provider::Ptr defaultValues)
//{
//    if (!node.isObject())
//        return ;
//
//    auto defaultValueNode = node.get("default", 0);
//
//    if (defaultValueNode.isObject()
//        && defaultValueNode[defaultValueNode.getMemberNames()[0]].type() != expectedType)
//        throw;
//    else if (defaultValueNode.isArray() && defaultValueNode[0].type() != expectedType)
//        throw;
//    else if (defaultValueNode.type() != expectedType)
//        throw;
//
//    parseDefaultValue(node, scope, valueName, defaultValues);
//}

void
EffectParser::parseDefaultValue(const Json::Value&  node,
                                const Scope&        scope,
                                const std::string&  valueName,
                                data::Provider::Ptr defaultValues)
{
    if (!node.isObject())
        return;

    auto memberNames = node.getMemberNames();
    if (std::find(memberNames.begin(), memberNames.end(), "default") == memberNames.end())
        return;

    auto defaultValueNode = node.get("default", 0);

    if (defaultValueNode.isObject())
        parseDefaultValueObject(defaultValueNode, scope, valueName, defaultValues);
    else if (defaultValueNode.isArray())
    {
        if (defaultValueNode.size() == 1 && defaultValueNode[0].isArray())
            parseDefaultValueVector(defaultValueNode[0], scope, valueName, defaultValues);
        else
            throw; // FIXME: support array default values
    }
    else if (defaultValueNode.isBool())
        defaultValues->set(valueName, defaultValueNode.asBool());
    else if (defaultValueNode.isInt())
        defaultValues->set(valueName, defaultValueNode.asInt());
    else if (defaultValueNode.isDouble())
        defaultValues->set(valueName, defaultValueNode.asFloat());
    else if (defaultValueNode.isString())
        loadTexture(defaultValueNode.asString(), valueName, defaultValues);
}

void
EffectParser::parseDefaultValueVector(const Json::Value&    defaultValueNode,
                                      const Scope&          scope,
                                      const std::string&    valueName,
                                      data::Provider::Ptr   defaultValues)
{
    auto size = defaultValueNode.size();
    auto type = defaultValueNode[0].type();

    if (type == Json::ValueType::intValue)
    {
        std::vector<int> value(size);
        for (auto i = 0u; i < size; ++i)
            value[i] = defaultValueNode[i].asInt();
        if (size == 2)
            defaultValues->set(valueName, math::make_vec2<int>(&value[0]));
        else if (size == 3)
            defaultValues->set(valueName, math::make_vec3<int>(&value[0]));
        else if (size == 4)
            defaultValues->set(valueName, math::make_vec4<int>(&value[0]));
    }
    else if (type == Json::ValueType::realValue)
    {
        std::vector<float> value(size);
        for (auto i = 0u; i < size; ++i)
            value[i] = defaultValueNode[i].asFloat();
        if (size == 2)
            defaultValues->set(valueName, math::make_vec2<float>(&value[0]));
        else if (size == 3)
            defaultValues->set(valueName, math::make_vec3<float>(&value[0]));
        else if (size == 4)
            defaultValues->set(valueName, math::make_vec4<float>(&value[0]));
    }
    else if (type == Json::ValueType::booleanValue)
    {
        std::vector<bool> value(size);
        for (auto i = 0u; i < size; ++i)
            value[i] = defaultValueNode[i].asBool();

        // std::vector<bool> is not an actual vector of bool:
        // https://stackoverflow.com/questions/6485496/how-to-get-stdvector-pointer-to-the-raw-data
        // thus we cannot use &value[0] to get a bool* to use with math::make_vec
        if (size == 2)
            defaultValues->set(valueName, math::bvec2(value[0], value[1]));
        else if (size == 3)
            defaultValues->set(valueName, math::bvec3(value[0], value[1], value[2]));
        else if (size == 4)
            defaultValues->set(valueName, math::bvec4(value[0], value[1], value[2], value[3]));
    }
}

void
EffectParser::parseDefaultValueObject(const Json::Value&    defaultValueNode,
                                      const Scope&          scope,
                                      const std::string&    valueName,
                                      data::Provider::Ptr   defaultValues)
{
    auto memberNames = defaultValueNode.getMemberNames();
    auto size = memberNames.size();
    auto type = defaultValueNode[memberNames[0]].type();
    std::vector<std::string> offsets = { "x", "y", "z", "w" };

    if (type == Json::ValueType::intValue)
    {
        std::vector<int> value(size);
        for (auto i = 0u; i < size; ++i)
            value[i] = defaultValueNode[offsets[i]].asInt();
        if (size == 2)
            defaultValues->set(valueName, math::make_vec2<int>(&value[0]));
        else if (size == 3)
            defaultValues->set(valueName, math::make_vec3<int>(&value[0]));
        else if (size == 4)
            defaultValues->set(valueName, math::make_vec4<int>(&value[0]));
    }
    else if (type == Json::ValueType::realValue)
    {
        std::vector<float> value(size);
        for (auto i = 0u; i < size; ++i)
            value[i] = defaultValueNode[offsets[i]].asFloat();
        if (size == 2)
            defaultValues->set(valueName, math::make_vec2<float>(&value[0]));
        else if (size == 3)
            defaultValues->set(valueName, math::make_vec3<float>(&value[0]));
        else if (size == 4)
            defaultValues->set(valueName, math::make_vec4<float>(&value[0]));
    }
    else if (type == Json::ValueType::booleanValue)
    {
        std::vector<bool> value(size);
        for (auto i = 0u; i < size; ++i)
            value[i] = defaultValueNode[offsets[i]].asBool();

        // std::vector<bool> is not an actual vector of bool:
        // https://stackoverflow.com/questions/6485496/how-to-get-stdvector-pointer-to-the-raw-data
        // thus we cannot use &value[0] to get a bool* to use with math::make_vec
        if (size == 2)
            defaultValues->set(valueName, math::bvec2(value[0], value[1]));
        else if (size == 3)
            defaultValues->set(valueName, math::bvec3(value[0], value[1], value[2]));
        else if (size == 4)
            defaultValues->set(valueName, math::bvec4(value[0], value[1], value[2], value[3]));
    }
}

void
EffectParser::parseAttributes(const Json::Value& node, const Scope& scope, AttributeBlock& attributes)
{
    auto attributesNode = node.get("attributes", 0);

    if (attributesNode.isObject())
    {
        auto defaultValuesProvider = data::Provider::create();

        attributes.bindings.defaultValues.addProvider(defaultValuesProvider);

        for (auto attributeName : attributesNode.getMemberNames())
        {
            auto attributeNode = attributesNode[attributeName];

            parseBinding(attributeNode, scope, attributes.bindings.bindings[attributeName]);

            /*if (!attributeNode.get("default", 0).empty())
                throw ParserError("Default values are not yet supported for attributes.");*/

            // FIXME: support default values for vertex attributes
            /*
            parseDefaultValue(
                attributeNode,
                scope,
                attributeName,
                //Json::ValueType::realValue,
                defaultValuesProvider
            );
            */
        }
    }
    // FIXME: throw otherwise
}

void
EffectParser::parseUniforms(const Json::Value& node, const Scope& scope, UniformBlock& uniforms)
{
    auto uniformsNode = node.get("uniforms", 0);

    if (uniformsNode.isObject())
    {
        auto defaultValuesProvider = data::Provider::create();

        uniforms.bindings.defaultValues.addProvider(defaultValuesProvider);

        for (auto uniformName : uniformsNode.getMemberNames())
        {
            auto uniformNode = uniformsNode[uniformName];

            parseBinding(uniformNode, scope, uniforms.bindings.bindings[uniformName]);
            parseDefaultValue(uniformNode, scope, uniformName, defaultValuesProvider);
        }
    }
    // FIXME: throw otherwise
}

void
EffectParser::parseMacros(const Json::Value& node, const Scope& scope, MacroBlock& macros)
{
    auto macrosNode = node.get("macros", 0);

    if (macrosNode.isObject())
    {
        auto defaultValuesProvider = data::Provider::create();

        macros.bindings.defaultValues.addProvider(defaultValuesProvider);

        for (auto macroName : macrosNode.getMemberNames())
        {
            auto macroNode = macrosNode[macroName];

            parseBinding(macroNode, scope, macros.bindings.bindings[macroName]);
            parseMacroBinding(macroNode, scope, macros.bindings.bindings[macroName]);
            parseDefaultValue(macroNode, scope, macroName, defaultValuesProvider);
        }
    }
    // FIXME: throw otherwise
}

void
EffectParser::parseStates(const Json::Value& node, const Scope& scope, StateBlock& states)
{
    auto statesNode = node.get("states", 0);

    if (statesNode.isObject())
    {
        auto defaultValuesProvider = data::Provider::create();

        states.bindings.defaultValues.addProvider(defaultValuesProvider);

        for (auto stateName : statesNode.getMemberNames())
        {
            if (std::find(_stateNames.begin(), _stateNames.end(), stateName) == _stateNames.end())
            {
                // FIXME: log warning because the state name does not match any known state
                throw;
            }
            else
            {
                parseBinding(statesNode[stateName], scope, states.bindings.bindings[stateName]);
                parseDefaultValue(statesNode[stateName], scope, stateName, defaultValuesProvider);
            }
        }
    }
    // FIXME: throw otherwise
}

void
EffectParser::parseBlendMode(const Json::Value&				node,
                             const Scope&                   scope,
                             render::Blending::Source&		srcFactor,
                             render::Blending::Destination&	dstFactor)
{
    auto blendModeNode = node.get("blendMode", 0);

    if (blendModeNode.isArray())
    {
        auto blendSrcFactorString = "src_" + blendModeNode[0].asString();
        if (_blendFactorMap.count(blendSrcFactorString))
            srcFactor = static_cast<render::Blending::Source>(_blendFactorMap[blendSrcFactorString]);

        auto blendDstFactorString = "dst_" + blendModeNode[1].asString();
        if (_blendFactorMap.count(blendDstFactorString))
            dstFactor = static_cast<render::Blending::Destination>(_blendFactorMap[blendDstFactorString]);
    }
    else if (blendModeNode.isString())
    {
        auto blendModeString = blendModeNode.asString();

        if (_blendFactorMap.count(blendModeString))
        {
            auto blendMode = _blendFactorMap[blendModeString];

            srcFactor = static_cast<render::Blending::Source>(blendMode & 0x00ff);
            dstFactor = static_cast<render::Blending::Destination>(blendMode & 0xff00);
        }
    }
}

void
EffectParser::parseZSort(const Json::Value&	node,
                         const Scope&       scope,
                         bool&              zSorted) const
{
    auto zsortedValue = node.get("zSort", 0);

    if (zsortedValue.isBool())
        zSorted = zsortedValue.asBool();
}

void
EffectParser::parseColorMask(const Json::Value&	node,
                             const Scope&       scope,
                             bool&              colorMask) const
{
    auto colorMaskValue = node.get("colorMask", 0);

    if (colorMaskValue.isBool())
        colorMask = colorMaskValue.asBool();
}

void
EffectParser::parseDepthTest(const Json::Value&	    node,
                             const Scope&           scope,
                             bool&                  depthMask,
                             render::CompareMode&   depthFunc)
{
    auto depthTest = node.get("depthTest", 0);

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
        auto depthMaskValue = node.get("depthMask", 0);
        auto depthFuncValue = node.get("depthFunc", 0);

        if (depthMaskValue.isBool())
            depthMask = depthMaskValue.asBool();
        if (depthFuncValue.isString())
            depthFunc = _compareFuncMap[depthFuncValue.asString()];
    }
}

void
EffectParser::parseTriangleCulling(const Json::Value&   node,
                                   const Scope&         scope,
                                   TriangleCulling&     triangleCulling)
{
    auto triangleCullingValue = node.get("triangleCulling", 0);

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
EffectParser::parsePriority(const Json::Value&	node,
                            const Scope&        scope,
                            float               defaultPriority)
{
    auto	priorityNode = node.get("priority", defaultPriority);
    float	ret = defaultPriority;

    if (!priorityNode.isNull())
    {
        if (priorityNode.isInt())
            ret = (float)priorityNode.asInt();
        else if (priorityNode.isDouble())
            ret = (float)priorityNode.asDouble();
        else if (priorityNode.isString())
            ret = getPriorityValue(priorityNode.asString());
        else if (priorityNode.isArray())
        {
            if (priorityNode[0].isString() && priorityNode[1].isDouble())
                ret = getPriorityValue(priorityNode[0].asString()) + (float)priorityNode[1].asDouble();
        }
    }

    return ret;
}

void
EffectParser::parseStencilState(const Json::Value&  node,
                                const Scope&        scope,
								CompareMode&        stencilFunc,
								int&                stencilRef,
								uint&               stencilMask,
								StencilOperation&   stencilFailOp,
								StencilOperation&   stencilZFailOp,
								StencilOperation&   stencilZPassOp)
{
	auto stencilTest	= node.get("stencilTest", 0);

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
		parseStencilOperations(stencilOpsValue, scope, stencilFailOp, stencilZFailOp, stencilZPassOp);
	}
    else if (stencilTest.isArray())
    {
		stencilFunc = _compareFuncMap[stencilTest[0].asString()];
		stencilRef	= stencilTest[1].asInt();
		stencilMask	= stencilTest[2].asUInt();
		parseStencilOperations(stencilTest[3], scope, stencilFailOp, stencilZFailOp, stencilZPassOp);
    }
}

void
EffectParser::parseScissorTest(const Json::Value& node,
                               const Scope&       scope,
							   bool&			  scissorTest,
							   math::ivec4&	      scissorBox)
{
	auto scissorTestNode = node.get("scissorTest", 0);

	if (!scissorTestNode.isNull() && scissorTestNode.isBool())
		scissorTest = scissorTestNode.asBool();

	auto scissorBoxNode	= node.get("scissorBox", 0);

	if (!scissorBoxNode.isNull() && scissorBoxNode.isArray())
	{
		if (scissorBoxNode[0].isInt())
			scissorBox.x = scissorBoxNode[0].asInt();
		if (scissorBoxNode[1].isInt())
			scissorBox.y = scissorBoxNode[1].asInt();
		if (scissorBoxNode[2].isInt())
			scissorBox.z = scissorBoxNode[2].asInt();
		if (scissorBoxNode[3].isInt())
			scissorBox.w = scissorBoxNode[3].asInt();
	}
}

void
EffectParser::parseStencilOperations(const Json::Value& node,
                                     const Scope&       scope,
									 StencilOperation& 	stencilFailOp,
									 StencilOperation& 	stencilZFailOp,
									 StencilOperation& 	stencilZPassOp)
{
	if (node.isArray())
	{
		if (node[0].isString())
			stencilFailOp = _stencilOpMap[node[0].asString()];
		if (node[1].isString())
			stencilZFailOp = _stencilOpMap[node[1].asString()];
		if (node[2].isString())
			stencilZPassOp = _stencilOpMap[node[2].asString()];
	}
	else
	{
		auto failValue	= node.get("fail", 0);
		auto zfailValue	= node.get("zfail", 0);
		auto zpassValue	= node.get("zpass", 0);

		if (failValue.isString())
			stencilFailOp = _stencilOpMap[failValue.asString()];
		if (zfailValue.isString())
			stencilZFailOp = _stencilOpMap[zfailValue.asString()];
		if (zpassValue.isString())
			stencilZPassOp = _stencilOpMap[zpassValue.asString()];
	}
}

void
EffectParser::parseBinding(const Json::Value& node, const Scope& scope, Binding& binding)
{
    binding.source = Binding::Source::TARGET;

    if (node.isString())
    {
        binding.propertyName = node.asString();
    }
    else
    {
        auto bindingNode = node.get("binding", 0);

        if (bindingNode.isString())
        {
            binding.propertyName = bindingNode.asString();
        }
        else if (bindingNode.isObject())
        {
            auto propertyNode = bindingNode.get("property", 0);
            auto sourceNode = bindingNode.get("source", 0);

            if (propertyNode.isString())
                binding.propertyName = propertyNode.asString();
            // FIXME: throw otherwise

            if (sourceNode.isString())
            {
                auto sourceStr = sourceNode.asString();

                if (sourceStr == "target")
                    binding.source = Binding::Source::TARGET;
                else if (sourceStr == "renderer")
                    binding.source = Binding::Source::RENDERER;
                else if (sourceStr == "root")
                    binding.source = Binding::Source::ROOT;
            }
            // FIXME: throw otherwise
        }
    }
}

void
EffectParser::parseMacroBinding(const Json::Value& node, const Scope& scope, MacroBinding& binding)
{
    if (!node.isObject())
        return;

    auto bindingNode = node.get("binding", 0);

    if (!bindingNode.isObject())
        return;

    auto typeNode = bindingNode.get("type", 0);
    if (typeNode.isString())
    {
        auto typeStr = typeNode.asString();

        if (typeStr == "bool")
            binding.type = data::MacroBinding::Type::BOOL;
        else if (typeStr == "int")
            binding.type = data::MacroBinding::Type::INT;
        else if (typeStr == "float")
            binding.type = data::MacroBinding::Type::FLOAT;

        // FIXME: handle other Binding::Type values
    }
    // FIXME: throw otherwise

    auto minNode = bindingNode.get("min", "");
    if (minNode.isInt())
        binding.minValue = minNode.asInt();
    // FIXME: throw otherwise

    auto maxNode = bindingNode.get("max", "");
    if (maxNode.isInt())
        binding.maxValue = maxNode.asInt();
    // FIXME: throw otherwise
}

render::Shader::Ptr
EffectParser::parseShader(const Json::Value& node, const Scope& scope, render::Shader::Type type)
{
    if (!node.isString())
        throw;

    std::string glsl = node.asString();

    auto shader = Shader::create(_options->context(), type, glsl);
    auto blocks = std::shared_ptr<GLSLBlockList>(new GLSLBlockList());

    blocks->push_front(GLSLBlock(GLSLBlockType::TEXT, ""));
    _shaderToGLSL[shader] = blocks;
    parseGLSL(glsl, _options, blocks, blocks->begin());

    return shader;
}

void
EffectParser::parseGLSL(const std::string&      glsl,
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
        if (firstSharpPos != std::string::npos && line.substr(firstSharpPos, 16) == "#pragma include "
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
EffectParser::loadGLSLDependencies(GLSLBlockListPtr blocks, file::Options::Ptr options)
{
    auto loader = Loader::create(options);

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
EffectParser::dependencyErrorHandler(std::shared_ptr<Loader> loader, const ParserError& error, const std::string& filename)
{
    /*LOG_ERROR("unable to load dependency '" << filename << "', included paths are:");
    for (auto& path : loader->options()->includePaths())
        LOG_ERROR("\t" << path);*/

    throw file::ParserError("Unable to load dependencies.");
}

void
EffectParser::glslIncludeCompleteHandler(LoaderPtr 			        loader,
                                         GLSLBlockListPtr 			blocks,
                                         GLSLBlockList::iterator 	blockIt,
                                         const std::string&         filename)
{
    auto& block = *blockIt;

    block.first = GLSLBlockType::TEXT;

    // FIXME: use the GLSL "line" directive instead
#ifdef DEBUG
    block.second = "//#pragma include(\"" + filename + "\")\n";
#else
    block.second = "\n";
#endif

    ++_numLoadedDependencies;

    auto file = loader->files().at(filename);
    auto resolvedFilename = file->resolvedFilename();
    auto options = _options;
    auto pos = resolvedFilename.find_last_of("/\\");

    if (pos != std::string::npos)
    {
        options = file::Options::create(options);
        options->includePaths().clear();
        options->includePaths().push_back(resolvedFilename.substr(0, pos));
    }

    parseGLSL(std::string((const char*)&file->data()[0], file->data().size()), options, blocks, blockIt);

    if (_numDependencies == _numLoadedDependencies && _effect)
        finalize();
}

void
EffectParser::loadTexture(const std::string&	textureFilename,
                          const std::string&    uniformName,
						  data::Provider::Ptr   defaultValues)
{
    auto loader = Loader::create(_options);

	_numDependencies++;

	_loaderCompleteSlots[loader] = loader->complete()->connect([&](file::Loader::Ptr loader)
	{
        auto texture = _assetLibrary->texture(textureFilename);

        //value.textureValues.push_back(texture);
        defaultValues->set(uniformName, texture->sampler());
		texture->upload();

        ++_numLoadedDependencies;

        if (_numDependencies == _numLoadedDependencies && _effect)
            finalize();
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

std::shared_ptr<render::States>
EffectParser::createStates(const StateBlock& block)
{
    return nullptr;
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
    for (auto& technique : _globalScope.techniques)
    {
        _effect->addTechnique(technique.first, technique.second);

        for (auto pass : technique.second)
        {
            auto vs = pass->program()->vertexShader();
            auto fs = pass->program()->fragmentShader();

            vs->source("#define VERTEX_SHADER\n" + concatenateGLSLBlocks(_shaderToGLSL[vs]));
            fs->source("#define FRAGMENT_SHADER\n" + concatenateGLSLBlocks(_shaderToGLSL[fs]));
        }
    }

    _options->assetLibrary()->effect(_filename, _effect);

    _complete->execute(shared_from_this());
}
