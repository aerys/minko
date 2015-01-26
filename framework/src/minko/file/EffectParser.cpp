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
#include "minko/log/Logger.hpp"

#include "json/json.h"

using namespace minko;
using namespace minko::data;
using namespace minko::file;
using namespace minko::render;

std::unordered_map<std::string, unsigned int> EffectParser::_blendFactorMap = {
	{ "src_zero", static_cast<uint>(render::Blending::Source::ZERO) },
	{ "src_one", static_cast<uint>(render::Blending::Source::ONE) },
	{ "src_color", static_cast<uint>(render::Blending::Source::SRC_COLOR) },
	{ "src_one_minus_src_color", static_cast<uint>(render::Blending::Source::ONE_MINUS_SRC_COLOR) },
	{ "src_src_alpha", static_cast<uint>(render::Blending::Source::SRC_ALPHA) },
	{ "src_one_minus_src_alpha", static_cast<uint>(render::Blending::Source::ONE_MINUS_SRC_ALPHA) },
	{ "src_dst_alpha", static_cast<uint>(render::Blending::Source::DST_ALPHA) },
	{ "src_one_minus_dst_alpha", static_cast<uint>(render::Blending::Source::ONE_MINUS_DST_ALPHA) },

	{ "dst_zero", static_cast<uint>(render::Blending::Destination::ZERO) },
	{ "dst_one", static_cast<uint>(render::Blending::Destination::ONE) },
	{ "dst_dst_color", static_cast<uint>(render::Blending::Destination::DST_COLOR) },
	{ "dst_one_minus_dst_color", static_cast<uint>(render::Blending::Destination::ONE_MINUS_DST_COLOR) },
	{ "dst_src_alpha_saturate", static_cast<uint>(render::Blending::Destination::SRC_ALPHA_SATURATE) },
	{ "dst_one_minus_src_alpha", static_cast<uint>(render::Blending::Destination::ONE_MINUS_SRC_ALPHA) },
	{ "dst_dst_alpha", static_cast<uint>(render::Blending::Destination::DST_ALPHA) },
	{ "dst_one_minus_dst_alpha", static_cast<uint>(render::Blending::Destination::ONE_MINUS_DST_ALPHA) },

	{ "default", static_cast<uint>(render::Blending::Mode::DEFAULT) },
	{ "alpha", static_cast<uint>(render::Blending::Mode::ALPHA) },
	{ "additive", static_cast<uint>(render::Blending::Mode::ADDITIVE) }
};

std::unordered_map<std::string, render::CompareMode> EffectParser::_compareFuncMap = {
	{ "always", render::CompareMode::ALWAYS },
	{ "equal", render::CompareMode::EQUAL },
	{ "greater", render::CompareMode::GREATER },
	{ "greater_equal", render::CompareMode::GREATER_EQUAL },
	{ "less", render::CompareMode::LESS },
	{ "less_equal", render::CompareMode::LESS_EQUAL },
	{ "never", render::CompareMode::NEVER },
	{ "not_equal", render::CompareMode::NOT_EQUAL },
};

std::unordered_map<std::string, render::StencilOperation> EffectParser::_stencilOpMap = {
	{ "keep", render::StencilOperation::KEEP },
	{ "zero", render::StencilOperation::ZERO },
	{ "replace", render::StencilOperation::REPLACE },
	{ "incr", render::StencilOperation::INCR },
	{ "incr_wrap", render::StencilOperation::INCR_WRAP },
	{ "decr", render::StencilOperation::DECR },
	{ "decr_wrap", render::StencilOperation::DECR_WRAP },
	{ "invert", render::StencilOperation::INVERT }
};

std::unordered_map<std::string, float> EffectParser::_priorityMap = {
	{ "first", Priority::FIRST },
	{ "background", Priority::BACKGROUND },
	{ "opaque", Priority::OPAQUE },
	{ "transparent", Priority::TRANSPARENT },
	{ "last", Priority::LAST }
};

std::array<std::string, 1> EffectParser::_extraStateNames = {
    "blendingMode"
};

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
	_numLoadedDependencies(0),
	_effectData(data::Provider::create())
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
		_error->execute(shared_from_this(), file::Error(resolvedFilename + ": " + reader.getFormattedErrorMessages()));

	_options = options->clone();

    int pos	= resolvedFilename.find_last_of("/\\");
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
    parseAttributes(node, scope, scope.attributeBlock);
    parseUniforms(node, scope, scope.uniformBlock);
    parseMacros(node, scope, scope.macroBlock);
    parseStates(node, scope, scope.stateBlock);
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

            parseAttributes(techniqueNode, techniqueScope, techniqueScope.attributeBlock);
            parseUniforms(techniqueNode, techniqueScope, techniqueScope.uniformBlock);
            parseMacros(techniqueNode, techniqueScope, techniqueScope.macroBlock);
            parseStates(techniqueNode, techniqueScope, techniqueScope.stateBlock);
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
            if (passNode.isObject() && !parseConfiguration(passNode))
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
		auto pass = findPassByName(passName, scope);

		if (pass == nullptr)
			throw std::runtime_error("Undefined pass with name '" + passName + "'.");

        passes.push_back(pass);
    }
    else
    {
        // If the pass is an actual pass object, we parse all its data, create the corresponding
        // Pass object and add it to the vector.

        Scope passScope(scope, scope);

		render::Shader::Ptr vertexShader;
		render::Shader::Ptr fragmentShader;
        auto passName = _effectName + "-pass" + std::to_string(scope.passes.size());
        auto nameNode = node.get("name", 0);
		auto isPostProcessing = false;
        if (nameNode.isString())
            passName = nameNode.asString();
        // FIXME: throw otherwise

		if (node.isMember("extends"))
		{
			auto extendNode = node.get("extends", 0);

			// if a pass "extends" another pass, then we have to init. its properties from that previously defined pass
			if (extendNode.isString())
			{
				auto passName = extendNode.asString();
				auto pass = findPassByName(passName, scope);

				if (pass == nullptr)
					throw std::runtime_error("Undefined pass with name '" + passName + "'.");

				passScope.attributeBlock.bindingMap = {
					pass->attributeBindings().bindings,
					data::Store(pass->attributeBindings().defaultValues, true)
				};

				passScope.uniformBlock.bindingMap = {
					pass->uniformBindings().bindings,
					data::Store(pass->uniformBindings().defaultValues, true)
				};

				passScope.stateBlock.bindingMap.bindings = pass->stateBindings().bindings;
				passScope.stateBlock.states = render::States(pass->states());
				passScope.stateBlock.bindingMap.defaultValues.removeProvider(
					passScope.stateBlock.bindingMap.defaultValues.providers().front()
				);
				passScope.stateBlock.bindingMap.defaultValues.addProvider(passScope.stateBlock.states.data());

				passScope.macroBlock.bindingMap = MacroBindingMap(
					pass->macroBindings().bindings,
					data::Store(pass->macroBindings().defaultValues, true),
					pass->macroBindings().types
				);

				vertexShader = pass->program()->vertexShader();
				fragmentShader = pass->program()->fragmentShader();
				isPostProcessing = pass->isPostProcessing();
			}
			// FIXME: throw otherwise
		}

        parseAttributes(node, passScope, passScope.attributeBlock);
        parseUniforms(node, passScope, passScope.uniformBlock);
        parseMacros(node, passScope, passScope.macroBlock);
        parseStates(node, passScope, passScope.stateBlock);

		if (node.isMember("vertexShader"))
        	vertexShader = parseShader(node.get("vertexShader", 0), passScope, Shader::Type::VERTEX_SHADER);
		else if (!vertexShader)
			throw std::runtime_error("Missing vertex shader for pass \"" + passName + "\"");

		if (node.isMember("fragmentShader"))
        	fragmentShader = parseShader(node.get("fragmentShader", 0), passScope, Shader::Type::FRAGMENT_SHADER);
		else if (!fragmentShader)
			throw std::runtime_error("Missing fragment shader for pass \"" + passName + "\"");

		if (node.isMember("isPostProcessing"))
			isPostProcessing = node.get("isPostProcessing", false).asBool();

        passes.push_back(Pass::create(
            passName,
			isPostProcessing,
            Program::create(passName, _options->context(), vertexShader, fragmentShader),
            passScope.attributeBlock.bindingMap,
            passScope.uniformBlock.bindingMap,
            passScope.stateBlock.bindingMap,
            passScope.macroBlock.bindingMap,
            passScope.stateBlock.states
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
        parseDefaultValueVectorObject(defaultValueNode, scope, valueName, defaultValues);
    else if (defaultValueNode.isArray())
    {
        if (defaultValueNode.size() == 1 && defaultValueNode[0].isArray())
            parseDefaultValueVectorArray(defaultValueNode[0], scope, valueName, defaultValues);
        else
            throw; // FIXME: support array default values
    }
    else if (defaultValueNode.isBool())
        defaultValues->set(valueName, defaultValueNode.asBool() ? 1 : 0);
    else if (defaultValueNode.isInt())
        defaultValues->set(valueName, defaultValueNode.asInt());
    else if (defaultValueNode.isDouble())
        defaultValues->set(valueName, defaultValueNode.asFloat());
    else if (defaultValueNode.isString())
        loadTexture(defaultValueNode.asString(), valueName, defaultValues);
}

template<typename T>
void
EffectParser::parseDefaultValueSamplerStates(const Json::Value&    node,
                                             const Scope&          scope,
                                             const std::string&    valueName,
                                             data::Provider::Ptr   defaultValues)
{
    if (!node.isObject())
        return;

    auto memberNames = node.getMemberNames();
    if (std::find(memberNames.begin(), memberNames.end(), "default") == memberNames.end())
        return;

    auto defaultValueNode = node.get("default", 0);

    if (defaultValueNode.isString())
    {
        if (typeid(T) == typeid(WrapMode))
            defaultValues->set(valueName, SamplerStates::stringToWrapMode(defaultValueNode.asString()));
        else if (typeid(T) == typeid(TextureFilter))
            defaultValues->set(valueName, SamplerStates::stringToTextureFilter(defaultValueNode.asString()));
        else if (typeid(T) == typeid(MipFilter))
            defaultValues->set(valueName, SamplerStates::stringToMipFilter(defaultValueNode.asString()));
    }
}

void
EffectParser::parseDefaultValueVectorArray(const Json::Value&    defaultValueNode,
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
        // GLSL bool uniforms are set using integers, thus even if the default value is written
        // using boolean values, we store it as integers
        // https://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
        std::vector<int> value(size);
        for (auto i = 0u; i < size; ++i)
            value[i] = defaultValueNode[i].asBool() ? 1 : 0;
        if (size == 2)
            defaultValues->set(valueName, math::make_vec2<int>(&value[0]));
        else if (size == 3)
            defaultValues->set(valueName, math::make_vec3<int>(&value[0]));
        else if (size == 4)
            defaultValues->set(valueName, math::make_vec4<int>(&value[0]));
    }
}

void
EffectParser::parseDefaultValueVectorObject(const Json::Value&    defaultValueNode,
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
        // GLSL bool uniforms are set using integers, thus even if the default value is written
        // using boolean values, we store it as integers
        // https://www.opengl.org/sdk/docs/man/html/glUniform.xhtml
        std::vector<int> value(size);
        for (auto i = 0u; i < size; ++i)
            value[i] = defaultValueNode[offsets[i]].asBool() ? 1 : 0;
        if (size == 2)
            defaultValues->set(valueName, math::make_vec2<int>(&value[0]));
        else if (size == 3)
            defaultValues->set(valueName, math::make_vec3<int>(&value[0]));
        else if (size == 4)
            defaultValues->set(valueName, math::make_vec4<int>(&value[0]));
    }
}

void
EffectParser::parseAttributes(const Json::Value& node, const Scope& scope, AttributeBlock& attributes)
{
    auto attributesNode = node.get("attributes", 0);

    if (attributesNode.isObject())
    {
        auto defaultValuesProvider = data::Provider::create();

        attributes.bindingMap.defaultValues.addProvider(defaultValuesProvider);

        for (auto attributeName : attributesNode.getMemberNames())
        {
            auto attributeNode = attributesNode[attributeName];

			data::Binding binding;
            if (parseBinding(attributeNode, scope, binding))
				attributes.bindingMap.bindings[attributeName] = binding;

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
        data::Provider::Ptr defaultValuesProvider;

		if (uniforms.bindingMap.defaultValues.providers().size() != 0)
			defaultValuesProvider = uniforms.bindingMap.defaultValues.providers().front();
		else
		{
			defaultValuesProvider = data::Provider::create();
        	uniforms.bindingMap.defaultValues.addProvider(defaultValuesProvider);
		}

        for (auto uniformName : uniformsNode.getMemberNames())
        {
            auto uniformNode = uniformsNode[uniformName];

			data::Binding binding;
            if (parseBinding(uniformNode, scope, binding))
				uniforms.bindingMap.bindings[uniformName] = binding;

            parseSamplerStates(uniformNode, scope, uniformName, defaultValuesProvider, uniforms.bindingMap);

            parseDefaultValue(uniformNode, scope, uniformName, defaultValuesProvider);
        }
    }
    // FIXME: throw otherwise
}

void
EffectParser::parseSamplerStates(const Json::Value& node, const Scope& scope, const std::string uniformName, data::Provider::Ptr defaultValues, data::BindingMap& bindingMap)
{
    if (node.isObject())
    {
        auto wrapModeNode = node.get(SamplerStates::PROPERTY_WRAP_MODE, 0);

        if (wrapModeNode.isString())
        {
            auto wrapModeStr = wrapModeNode.asString();

            auto wrapMode = SamplerStates::stringToWrapMode(wrapModeStr);

            defaultValues->set(
                SamplerStates::uniformNameToSamplerStateName(
                    uniformName,
                    SamplerStates::PROPERTY_WRAP_MODE
                ),
                wrapMode
            );
        }
        else if (wrapModeNode.isObject())
        {
            auto uniformWrapModeBindingName = SamplerStates::uniformNameToSamplerStateName(
                uniformName,
                SamplerStates::PROPERTY_WRAP_MODE
            );

            parseBinding(
                wrapModeNode,
                scope,
                bindingMap.bindings[uniformWrapModeBindingName]
            );

            parseDefaultValueSamplerStates<WrapMode>(wrapModeNode, scope, uniformWrapModeBindingName, defaultValues);
        }

        auto textureFilterNode = node.get(SamplerStates::PROPERTY_TEXTURE_FILTER, 0);

        if (textureFilterNode.isString())
        {
            auto textureFilterStr = textureFilterNode.asString();

            auto textureFilter = SamplerStates::stringToTextureFilter(textureFilterStr);

            defaultValues->set(
                SamplerStates::uniformNameToSamplerStateName(
                    uniformName,
                    SamplerStates::PROPERTY_TEXTURE_FILTER
                ),
                textureFilter
            );
        }
        else if (textureFilterNode.isObject())
        {
            auto uniformTextureFilterBindingName = SamplerStates::uniformNameToSamplerStateName(
                uniformName,
                SamplerStates::PROPERTY_TEXTURE_FILTER
            );

            parseBinding(
                textureFilterNode,
                scope,
                bindingMap.bindings[uniformTextureFilterBindingName]
            );

            parseDefaultValueSamplerStates<TextureFilter>(textureFilterNode, scope, uniformTextureFilterBindingName, defaultValues);
        }

        auto mipFilterNode = node.get(SamplerStates::PROPERTY_MIP_FILTER, 0);

        if (mipFilterNode.isString())
        {
            auto mipFilterStr = mipFilterNode.asString();

            auto mipFilter = SamplerStates::stringToMipFilter(mipFilterStr);

            defaultValues->set(
                SamplerStates::uniformNameToSamplerStateName(
                    uniformName,
                    SamplerStates::PROPERTY_MIP_FILTER
                ),
                mipFilter
            );
        }
        else if (mipFilterNode.isObject())
        {
            auto uniformMipFilterBindingName = SamplerStates::uniformNameToSamplerStateName(
                uniformName,
                SamplerStates::PROPERTY_MIP_FILTER
            );

            parseBinding(
                mipFilterNode,
                scope,
                bindingMap.bindings[uniformMipFilterBindingName]
            );

            parseDefaultValueSamplerStates<MipFilter>(mipFilterNode, scope, uniformMipFilterBindingName, defaultValues);
        }
    }
}

void
EffectParser::parseMacros(const Json::Value& node, const Scope& scope, MacroBlock& macros)
{
    auto macrosNode = node.get("macros", 0);

    if (macrosNode.isObject())
    {
        data::Provider::Ptr defaultValuesProvider;

		if (macros.bindingMap.defaultValues.providers().size() != 0)
			defaultValuesProvider = macros.bindingMap.defaultValues.providers().front();
		else
		{
			defaultValuesProvider = data::Provider::create();
			macros.bindingMap.defaultValues.addProvider(defaultValuesProvider);
		}

        for (auto macroName : macrosNode.getMemberNames())
        {
            auto macroNode = macrosNode[macroName];

			data::MacroBinding binding;
			if (parseBinding(macroNode, scope, binding))
			{
            	parseMacroBinding(macroNode, scope, binding);
				macros.bindingMap.bindings[macroName] = binding;
			}

            parseDefaultValue(macroNode, scope, macroName, defaultValuesProvider);

			macros.bindingMap.types[macroName] = MacroBindingMap::MacroType::UNSET;
			if (macroNode.isObject())
			{
			    auto typeNode = macroNode.get("type", 0);
			    if (typeNode.isString())
					macros.bindingMap.types[macroName] = MacroBindingMap::stringToMacroType(typeNode.asString());
			}
        }
    }
    // FIXME: throw otherwise
}

void
EffectParser::parseStates(const Json::Value& node, const Scope& scope, StateBlock& stateBlock)
{
    auto statesNode = node.get("states", 0);

    if (statesNode.isObject())
    {
        for (auto stateName : statesNode.getMemberNames())
        {
            if (std::find(States::PROPERTY_NAMES.begin(), States::PROPERTY_NAMES.end(), stateName) == States::PROPERTY_NAMES.end() &&
                std::find(_extraStateNames.begin(), _extraStateNames.end(), stateName) == _extraStateNames.end())
                throw; // FIXME: log warning because the state name does not match any known state

            if (statesNode[stateName].isObject())
            {
				data::Binding binding;
                if (parseBinding(statesNode[stateName], scope, binding))
					stateBlock.bindingMap.bindings[stateName] = binding;
            }
        }

        // parse & set priority default value
        float priority = parsePriority(statesNode, scope, render::States::DEFAULT_PRIORITY);
        stateBlock.states.priority(priority);

        // parse & set z-sorted default value
        bool zSorted = render::States::DEFAULT_ZSORTED;
        parseZSort(statesNode, scope, zSorted);
        stateBlock.states.zSorted(zSorted);

        // parse & set blending factors default values
        render::Blending::Source blendSrcFactor = render::States::DEFAULT_BLENDING_SOURCE;
        render::Blending::Destination blendDstFactor = render::States::DEFAULT_BLENDING_DESTINATION;
        parseBlendingMode(statesNode, scope, blendSrcFactor, blendDstFactor);
        stateBlock.states.blendingSourceFactor(blendSrcFactor);
        stateBlock.states.blendingDestinationFactor(blendDstFactor);

        // parse & set color mask default value
        bool colorMask = render::States::DEFAULT_COLOR_MASK;
        parseColorMask(statesNode, scope, colorMask);
        stateBlock.states.colorMask(colorMask);

        // parse & set depth mask/func default values
        bool depthMask = render::States::DEFAULT_DEPTH_MASK;
        parseDepthMask(statesNode, scope, depthMask);
        CompareMode depthFunction = render::States::DEFAULT_DEPTH_FUNCTION;
        parseDepthFunction(statesNode, scope, depthFunction);
        stateBlock.states.depthMask(depthMask);
        stateBlock.states.depthFunction(depthFunction);

        // parse & set triangle culling default value
        TriangleCulling triangleCulling = render::States::DEFAULT_TRIANGLE_CULLING;
        parseTriangleCulling(statesNode, scope, triangleCulling);
        stateBlock.states.triangleCulling(triangleCulling);

        // parse & set stencil default values
        CompareMode stencilFunc = render::States::DEFAULT_STENCIL_FUNCTION;
        int stencilRef = render::States::DEFAULT_STENCIL_REFERENCE;
        uint stencilMask = render::States::DEFAULT_STENCIL_MASK;
        StencilOperation stencilFailOp = render::States::DEFAULT_STENCIL_FAIL_OP;
        StencilOperation stencilZFailOp = render::States::DEFAULT_STENCIL_ZFAIL_OP;
        StencilOperation stencilZPassOp = render::States::DEFAULT_STENCIL_ZPASS_OP;
        parseStencilState(statesNode, scope, stencilFunc, stencilRef, stencilMask, stencilFailOp, stencilZFailOp, stencilZPassOp);
        stateBlock.states.stencilFunction(stencilFunc);
        stateBlock.states.stencilReference(stencilRef);
        stateBlock.states.stencilMask(stencilMask);
        stateBlock.states.stencilFailOperation(stencilFailOp);
        stateBlock.states.stencilZFailOperation(stencilZFailOp);
        stateBlock.states.stencilZPassOperation(stencilZPassOp);

        // parse & set scissor test/box default values
        bool scissorTest = render::States::DEFAULT_SCISSOR_TEST;
        math::ivec4 scissorBox = render::States::DEFAULT_SCISSOR_BOX;
        parseScissorTest(statesNode, scope, scissorTest, scissorBox);
        stateBlock.states.scissorTest(scissorTest);
        stateBlock.states.scissorBox(scissorBox);

		auto target = parseTarget(statesNode, scope);
		if (target)
			stateBlock.states.target(target->sampler());
    }
    // FIXME: throw otherwise
}

void
EffectParser::parseBlendingMode(const Json::Value&				node,
                                const Scope&                    scope,
                                render::Blending::Source&		srcFactor,
                                render::Blending::Destination&	dstFactor)
{
    auto blendingModeNode = node.get(_extraStateNames[0], 0);

    if (blendingModeNode.isArray())
    {
        auto blendSrcFactorString = "src_" + blendingModeNode[0].asString();
        if (_blendFactorMap.count(blendSrcFactorString))
            srcFactor = static_cast<render::Blending::Source>(_blendFactorMap[blendSrcFactorString]);

        auto blendDstFactorString = "dst_" + blendingModeNode[1].asString();
        if (_blendFactorMap.count(blendDstFactorString))
            dstFactor = static_cast<render::Blending::Destination>(_blendFactorMap[blendDstFactorString]);
    }
    else if (blendingModeNode.isString())
    {
        auto blendingModeString = blendingModeNode.asString();

        if (_blendFactorMap.count(blendingModeString))
        {
            auto blendingMode = _blendFactorMap[blendingModeString];

            srcFactor = static_cast<render::Blending::Source>(blendingMode & 0x00ff);
            dstFactor = static_cast<render::Blending::Destination>(blendingMode & 0xff00);
        }
    }
}

void
EffectParser::parseBlendingSource(const Json::Value&        node,
                                  const Scope&              scope,
                                  render::Blending::Source&	srcFactor)
{
    auto blendingSourceNode = node.get(States::PROPERTY_BLENDING_SOURCE, 0);

    if (blendingSourceNode.isString())
    {
        auto blendingSourceString = _blendFactorMap[blendingSourceNode.asString()];

        srcFactor = static_cast<render::Blending::Source>(blendingSourceString);
    }
}

void
EffectParser::parseBlendingSource(const Json::Value&             node,
                                  const Scope&                   scope,
                                  render::Blending::Destination& destFactor)
{
    auto blendingDestinationNode = node.get(States::PROPERTY_BLENDING_DESTINATION, 0);

    if (blendingDestinationNode.isString())
    {
        auto blendingDestination = _blendFactorMap[blendingDestinationNode.asString()];

        destFactor = static_cast<render::Blending::Destination>(blendingDestination);
    }
}

void
EffectParser::parseZSort(const Json::Value&	node,
                         const Scope&       scope,
                         bool&              zSorted)
{
    auto zSortedValue = node.get(States::PROPERTY_ZSORTED, 0);

    if (zSortedValue.isBool())
        zSorted = zSortedValue.asBool();
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
EffectParser::parseDepthMask(const Json::Value&	    node,
                             const Scope&           scope,
                             bool&                  depthMask)
{
    auto depthMaskValue = node.get(States::PROPERTY_DEPTH_MASK, 0);

    if (depthMaskValue.isBool())
        depthMask = depthMaskValue.asBool();
}

void
EffectParser::parseDepthFunction(const Json::Value&		node,
			                     const Scope&         	scope,
			                     render::CompareMode& 	depthFunction)
{
    auto depthFunctionValue = node.get(States::PROPERTY_DEPTH_FUNCTION, 0);

    if (depthFunctionValue.isString())
        depthFunction = _compareFuncMap[depthFunctionValue.asString()];
}

void
EffectParser::parseTriangleCulling(const Json::Value&   node,
                                   const Scope&         scope,
                                   TriangleCulling&     triangleCulling)
{
    auto triangleCullingValue = node.get(States::PROPERTY_TRIANGLE_CULLING, 0);

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
    auto	priorityNode = node.get(States::PROPERTY_PRIORITY, defaultPriority);
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

AbstractTexture::Ptr
EffectParser::parseTarget(const Json::Value& node, const Scope& scope)
{
	auto targetNode = node.get(States::PROPERTY_TARGET, 0);
	AbstractTexture::Ptr target = nullptr;
	std::string	targetName;

	if (targetNode.isObject())
	{
		auto nameValue = targetNode.get("name", 0);

		if (nameValue.isString())
			targetName = nameValue.asString();

		if (!targetNode.isMember("size") && !(targetNode.isMember("width") && targetNode.isMember("height")))
			return nullptr;

		auto width = 0;
		auto height = 0;

		if (targetNode.isMember("size"))
			width = height = targetNode.get("size", 0).asUInt();
		else
		{
			if (!targetNode.isMember("width") || !targetNode.isMember("height"))
			{
				_error->execute(
					shared_from_this(),
					file::Error(
						_resolvedFilename
						+ ": render target definition requires both \"width\" and \"height\" properties."
					)
				);
			}

			width = targetNode.get("width", 0).asUInt();
			height = targetNode.get("height", 0).asUInt();
		}

		const bool isCubeTexture = targetNode.get("isCube", 0).isBool()
			? targetNode.get("isCube", 0).asBool()
			: false;

		if (isCubeTexture)
		{
			target = CubeTexture::create(_options->context(), width, height, false, true);

			if (targetName.length())
				_assetLibrary->cubeTexture(targetName, std::static_pointer_cast<render::CubeTexture>(target));
		}
		else
		{
			target = Texture::create(_options->context(), width, height, false, true);

			if (targetName.length())
				_assetLibrary->texture(targetName, std::static_pointer_cast<render::Texture>(target));
		}

		target->upload();
		_effectData->set(targetName, target->sampler());
	}
	else if (targetNode.isString())
	{
		targetName = targetNode.asString();
		target = _assetLibrary->texture(targetName);
		if (target == nullptr)
			throw;

		_effectData->set(targetName, target->sampler());
	}

	return target;
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
	auto stencilTest = node.get("stencilTest", 0);

	if (stencilTest.isObject())
	{
        auto stencilFuncValue = stencilTest.get(States::PROPERTY_STENCIL_FUNCTION, 0);
        auto stencilRefValue = stencilTest.get(States::PROPERTY_STENCIL_REFERENCE, 0);
        auto stencilMaskValue = stencilTest.get(States::PROPERTY_STENCIL_MASK, 0);
		auto stencilOpsValue = stencilTest.get("stencilOps", 0);

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
	auto scissorTestNode = node.get(States::PROPERTY_SCISSOR_TEST, 0);

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

bool
EffectParser::parseBinding(const Json::Value& node, const Scope& scope, Binding& binding)
{
    binding.source = Binding::Source::TARGET;

    if (node.isString())
    {
        binding.propertyName = node.asString();

		return true;
    }
    else
    {
        auto bindingNode = node.get("binding", 0);

        if (bindingNode.isString())
        {
            binding.propertyName = bindingNode.asString();

			return true;
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

			return true;
        }
    }

	return false;
}

void
EffectParser::parseMacroBinding(const Json::Value& node, const Scope& scope, MacroBinding& binding)
{
    if (!node.isObject())
        return;

    auto bindingNode = node.get("binding", 0);

    if (!bindingNode.isObject())
    	return;

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
        auto pos = line.find("#pragma include ");
        auto commentPos = line.find("//");
        if (pos != std::string::npos && (commentPos == std::string::npos || pos < commentPos) && (line[pos + 16] == '"' || line[pos + 16] == '\''))
        {
            auto filename = line.substr(pos + 17, line.find_last_of(line[pos + 16]) - (pos + 17));

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
EffectParser::dependencyErrorHandler(std::shared_ptr<Loader> loader, const Error& error, const std::string& filename)
{
    /*LOG_ERROR("unable to load dependency '" << filename << "', included paths are:");
    for (auto& path : loader->options()->includePaths())
        LOG_ERROR("\t" << path);*/

    _error->execute(shared_from_this(), file::Error("Unable to load dependencies."));
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
        options = options->clone();
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

	_effect->data()->copyFrom(_effectData);
    _options->assetLibrary()->effect(_filename, _effect);

    _complete->execute(shared_from_this());
}

render::Pass::Ptr
EffectParser::findPassByName(const std::string& passName, const Scope& scope)
{
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

	return pass;
}
