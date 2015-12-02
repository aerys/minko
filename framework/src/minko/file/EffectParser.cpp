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

const std::string EffectParser::EXTRA_PROPERTY_BLENDING_MODE = "blendingMode";
const std::string EffectParser::EXTRA_PROPERTY_STENCIL_TEST = "stencilTest";

const std::array<std::string, 2> EffectParser::_extraStateNames = {
    EffectParser::EXTRA_PROPERTY_BLENDING_MODE,
    EffectParser::EXTRA_PROPERTY_STENCIL_TEST
};

// These following extra properties are only parsed as a subnode of a state
// we don't need them into the extraStateNames array
const std::string EffectParser::EXTRA_PROPERTY_STENCIL_OPS = "stencilOps";
const std::string EffectParser::EXTRA_PROPERTY_STENCIL_FAIL_OP = "fail";
const std::string EffectParser::EXTRA_PROPERTY_STENCIL_Z_FAIL_OP = "zfail";
const std::string EffectParser::EXTRA_PROPERTY_STENCIL_Z_PASS_OP = "zpass";

const float EffectParser::UNSET_PRIORITY_VALUE = -std::numeric_limits<float>::max();

const std::unordered_map<std::string, unsigned int> EffectParser::_blendingSourceMap = {
    { "zero", static_cast<uint>(render::Blending::Source::ZERO) },
    { "one", static_cast<uint>(render::Blending::Source::ONE) },
    { "color", static_cast<uint>(render::Blending::Source::SRC_COLOR) },
    { "one_minus_src_color", static_cast<uint>(render::Blending::Source::ONE_MINUS_SRC_COLOR) },
    { "src_alpha", static_cast<uint>(render::Blending::Source::SRC_ALPHA) },
    { "one_minus_src_alpha", static_cast<uint>(render::Blending::Source::ONE_MINUS_SRC_ALPHA) },
    { "dst_alpha", static_cast<uint>(render::Blending::Source::DST_ALPHA) },
    { "one_minus_dst_alpha", static_cast<uint>(render::Blending::Source::ONE_MINUS_DST_ALPHA) },
};

const std::unordered_map<std::string, unsigned int> EffectParser::_blendingDestinationMap = {
    { "zero", static_cast<uint>(render::Blending::Destination::ZERO) },
    { "one", static_cast<uint>(render::Blending::Destination::ONE) },
    { "dst_color", static_cast<uint>(render::Blending::Destination::DST_COLOR) },
    { "one_minus_dst_color", static_cast<uint>(render::Blending::Destination::ONE_MINUS_DST_COLOR) },
    { "src_alpha_saturate", static_cast<uint>(render::Blending::Destination::SRC_ALPHA_SATURATE) },
    { "one_minus_src_alpha", static_cast<uint>(render::Blending::Destination::ONE_MINUS_SRC_ALPHA) },
    { "dst_alpha", static_cast<uint>(render::Blending::Destination::DST_ALPHA) },
    { "one_minus_dst_alpha", static_cast<uint>(render::Blending::Destination::ONE_MINUS_DST_ALPHA) },
};

const std::unordered_map<std::string, unsigned int> EffectParser::_blendingModeMap = {
    { "default", static_cast<uint>(render::Blending::Mode::DEFAULT) },
    { "alpha", static_cast<uint>(render::Blending::Mode::ALPHA) },
    { "additive", static_cast<uint>(render::Blending::Mode::ADDITIVE) }
};

const std::unordered_map<std::string, render::CompareMode> EffectParser::_compareFuncMap = {
	{ "always", render::CompareMode::ALWAYS },
	{ "equal", render::CompareMode::EQUAL },
	{ "greater", render::CompareMode::GREATER },
	{ "greater_equal", render::CompareMode::GREATER_EQUAL },
	{ "less", render::CompareMode::LESS },
	{ "less_equal", render::CompareMode::LESS_EQUAL },
	{ "never", render::CompareMode::NEVER },
	{ "not_equal", render::CompareMode::NOT_EQUAL }
};

const std::unordered_map<std::string, render::TriangleCulling> EffectParser::_triangleCullingMap = {
    { "none", render::TriangleCulling::NONE },
    { "front", render::TriangleCulling::FRONT },
    { "back", render::TriangleCulling::BACK },
    { "both", render::TriangleCulling::BOTH }
};

const std::unordered_map<std::string, render::StencilOperation> EffectParser::_stencilOpMap = {
	{ "keep", render::StencilOperation::KEEP },
	{ "zero", render::StencilOperation::ZERO },
	{ "replace", render::StencilOperation::REPLACE },
	{ "incr", render::StencilOperation::INCR },
	{ "incr_wrap", render::StencilOperation::INCR_WRAP },
	{ "decr", render::StencilOperation::DECR },
	{ "decr_wrap", render::StencilOperation::DECR_WRAP },
	{ "invert", render::StencilOperation::INVERT }
};

const std::unordered_map<std::string, float> EffectParser::_priorityMap = {
	{ "first", Priority::FIRST },
	{ "background", Priority::BACKGROUND },
	{ "opaque", Priority::OPAQUE },
	{ "transparent", Priority::TRANSPARENT },
	{ "last", Priority::LAST }
};

float
EffectParser::getPriorityValue(const std::string& name)
{
	auto foundPriorityIt = _priorityMap.find(name);

	return foundPriorityIt != _priorityMap.end()
		? foundPriorityIt->second
		: _priorityMap.at("opaque");
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

    // Add a line ending to avoid JSON parsing error
    auto tempData = data;
    tempData.push_back('\n');

    auto parseSuccess = reader.parse(
        reinterpret_cast<const char*>(&tempData[0]),
        reinterpret_cast<const char*>(&tempData[tempData.size() - 1]), root, false
    );

    if (!parseSuccess)
    {
        _error->execute(
            shared_from_this(), 
            file::Error(resolvedFilename + ": " + reader.getFormattedErrorMessages())
        );
    }

	_options = options;

    int pos	= resolvedFilename.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        _options = _options->clone();
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
EffectParser::fixMissingPassPriorities(std::vector<render::Pass::Ptr>& passes)
{
    int numPasses = passes.size();

    if (numPasses == 1 && passes[0]->states().priority() == UNSET_PRIORITY_VALUE)
    {
        passes[0]->states().priority(States::DEFAULT_PRIORITY);
    }
    else
    {
        for (int i = 0; i < numPasses; ++i)
        {
            auto pass = passes[i];

            if (pass->states().priority() == UNSET_PRIORITY_VALUE)
            {
                int nextPassWithPriority = i + 1;
                while (nextPassWithPriority < numPasses
                       && passes[nextPassWithPriority]->states().priority() == UNSET_PRIORITY_VALUE)
                {
                    ++nextPassWithPriority;
                }

                if (nextPassWithPriority >= numPasses)
                    pass->states().priority(States::DEFAULT_PRIORITY + (float)(numPasses - i - 1));
                else
                {
                    pass->states().priority(
                        (float)(nextPassWithPriority - i)
                        + passes[nextPassWithPriority]->states().priority()
                    );
                }
            }
        }
    }
}

void
EffectParser::parseTechniques(const Json::Value& node, Scope& scope, Techniques& techniques)
{
    auto techniquesNode = node.get("techniques", 0);

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
                : techniquesNode.size() == 1
                    ? "default"
                    : _effectName + "-technique-" + std::to_string(techniques.size());

            Scope techniqueScope(scope, scope);

            parseAttributes(techniqueNode, techniqueScope, techniqueScope.attributeBlock);
            parseUniforms(techniqueNode, techniqueScope, techniqueScope.uniformBlock);
            parseMacros(techniqueNode, techniqueScope, techniqueScope.macroBlock);
            parseStates(techniqueNode, techniqueScope, techniqueScope.stateBlock);
            parsePasses(techniqueNode, techniqueScope, techniques[techniqueName]);

            fixMissingPassPriorities(techniques[techniqueName]);
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

render::Pass::Ptr
EffectParser::findPassFromEffectFilename(const std::string& effectFilename,
                                         const std::string& techniqueName,
                                         const std::string& passName)
{
    auto effect = _assetLibrary->effect(effectFilename);

    if (effect == nullptr)
        return nullptr;

    for (auto& techniqueNameAndPasses : effect->techniques())
    {
        if (techniqueNameAndPasses.first == techniqueName)
        {
            for (auto p : techniqueNameAndPasses.second)
            {
                if (p->name() == passName)
                    return p;
            }
        }
    }

    return nullptr;
}

render::Pass::Ptr
EffectParser::getPassToExtend(const Json::Value& extendNode)
{
    render::Pass::Ptr pass;
    std::string passName;

    if (extendNode.isString())
    {
        passName = extendNode.asString();

        // if the "extends" node is just a string, we're extending a "free" pass
        // thus we have to look into the root scope
        auto passIt = std::find_if(_globalScope.passes.begin(), _globalScope.passes.end(), [&](PassPtr p)
        {
            return p->name() == passName;
        });

        if (passIt != _globalScope.passes.end())
            pass = *passIt;
    }
    else if (extendNode.isObject())
    {
        passName = extendNode["pass"].asString();

        auto techniqueName = extendNode["technique"].asString();
        auto effectFilename = extendNode["effect"].asString();

        if (!_assetLibrary->effect(effectFilename))
        {
            auto options = _options->clone();
            auto loader = file::Loader::create(_assetLibrary->loader());

            options->loadAsynchronously(false);
            loader->queue(effectFilename, options);
            // FIXME: handle errors
            auto effectComplete = loader->complete()->connect([&](file::Loader::Ptr l)
            {
                pass = findPassFromEffectFilename(effectFilename, techniqueName, passName);
            });
            loader->load();
        }
        else
        {
            pass = findPassFromEffectFilename(effectFilename, techniqueName, passName);
        }
    }
    else
        throw;

    if (pass == nullptr)
        throw std::runtime_error("Undefined base pass with name '" + passName + "'.");

    return pass;
}

void
EffectParser::parsePass(const Json::Value& node, Scope& scope, std::vector<PassPtr>& passes)
{
    if (node.isString())
    {
        passes.push_back(Pass::create(getPassToExtend(node), true));
    }
    else if (node.isObject())
    {
        // If the pass is an actual pass object, we parse all its data, create the corresponding
        // Pass object and add it to the vector.

        Scope passScope(scope, scope);

		render::Shader::Ptr vertexShader;
		render::Shader::Ptr fragmentShader;
        auto passName = _effectName + "-pass" + std::to_string(scope.passes.size());
        auto nameNode = node.get("name", 0);
		auto isForward = true;

		if (node.isMember("extends"))
		{
			auto extendNode = node.get("extends", 0);
            render::Pass::Ptr pass = getPassToExtend(extendNode);

			// If a pass "extends" another pass, then we have to merge its properties with the already existing ones
            passScope.attributeBlock.bindingMap.bindings.insert(pass->attributeBindings().bindings.begin(), pass->attributeBindings().bindings.end());
            passScope.uniformBlock.bindingMap.bindings.insert(pass->uniformBindings().bindings.begin(), pass->uniformBindings().bindings.end());
            passScope.macroBlock.bindingMap.bindings.insert(pass->macroBindings().bindings.begin(), pass->macroBindings().bindings.end());
            passScope.macroBlock.bindingMap.types.insert(pass->macroBindings().types.begin(), pass->macroBindings().types.end());
            passScope.stateBlock.bindingMap.bindings.insert(pass->stateBindings().bindings.begin(), pass->stateBindings().bindings.end());

            if (pass->attributeBindings().defaultValues.providers().size() > 0)
            {
                if (passScope.attributeBlock.bindingMap.defaultValues.providers().size() == 0)
                    passScope.attributeBlock.bindingMap.defaultValues = data::Store(pass->attributeBindings().defaultValues, true);

                for (auto provider : pass->attributeBindings().defaultValues.providers())
                    passScope.attributeBlock.bindingMap.defaultValues.providers().front()->merge(provider);
            }

            if (pass->uniformBindings().defaultValues.providers().size() > 0)
            {
                if (passScope.uniformBlock.bindingMap.defaultValues.providers().size() == 0)
                    passScope.uniformBlock.bindingMap.defaultValues = data::Store(pass->uniformBindings().defaultValues, true);

                for (auto provider : pass->uniformBindings().defaultValues.providers())
                    passScope.uniformBlock.bindingMap.defaultValues.providers().front()->merge(provider);
            }

            if (pass->macroBindings().defaultValues.providers().size() > 0)
            {
                if (passScope.macroBlock.bindingMap.defaultValues.providers().size() == 0)
                    passScope.macroBlock.bindingMap.defaultValues = data::Store(pass->macroBindings().defaultValues, true);

                for (auto provider : pass->macroBindings().defaultValues.providers())
                    passScope.macroBlock.bindingMap.defaultValues.providers().front()->merge(provider);
            }

            passScope.stateBlock.bindingMap.defaultValues.providers().front()->merge(pass->stateBindings().defaultValues.providers().front());

            vertexShader = pass->program()->vertexShader();
            fragmentShader = pass->program()->fragmentShader();
            isForward = pass->isForward();
            passName = pass->name();
		}

        if (nameNode.isString())
            passName = nameNode.asString();
        // FIXME: throw otherwise

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

		if (node.isMember("forward"))
			isForward = node.get("forward", true).asBool();

        if (!isForward)
            checkDeferredPassBindings(passScope);

        passes.push_back(Pass::create(
            passName,
			isForward,
            Program::create(passName, _options->context(), vertexShader, fragmentShader),
            passScope.attributeBlock.bindingMap,
            passScope.uniformBlock.bindingMap,
            passScope.stateBlock.bindingMap,
            passScope.macroBlock.bindingMap
        ));
    }
}

void
EffectParser::checkDeferredPassBindings(const Scope& passScope)
{
    for (auto& bindingNameAndValue : passScope.attributeBlock.bindingMap.bindings)
        if (bindingNameAndValue.second.source == data::Binding::Source::TARGET)
            throw;

    for (auto& bindingNameAndValue : passScope.uniformBlock.bindingMap.bindings)
        if (bindingNameAndValue.second.source == data::Binding::Source::TARGET)
            throw;

    for (auto& bindingNameAndValue : passScope.stateBlock.bindingMap.bindings)
        if (bindingNameAndValue.second.source == data::Binding::Source::TARGET)
            throw;

    for (auto& bindingNameAndValue : passScope.macroBlock.bindingMap.bindings)
        if (bindingNameAndValue.second.source == data::Binding::Source::TARGET)
            throw;
}

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
EffectParser::parseDefaultValueStates(const Json::Value&    node,
                                      const Scope&          scope,
                                      const std::string&    stateName,
                                      data::Provider::Ptr   defaultValues)
{
    if (!node.isObject())
        return;

    auto memberNames = node.getMemberNames();
    if (std::find(memberNames.begin(), memberNames.end(), "default") == memberNames.end())
        return;

    auto defaultValueNode = node.get("default", 0);

    if (defaultValueNode.isBool())
        defaultValues->set(stateName, defaultValueNode.asBool() ? 1 : 0);
    else if (defaultValueNode.isInt())
        defaultValues->set(stateName, defaultValueNode.asInt());
    else if (defaultValueNode.isDouble())
        defaultValues->set(stateName, defaultValueNode.asFloat());
    else if (defaultValueNode.isString())
        defaultValues->set(stateName, defaultValueNode.asString());
    else if (defaultValueNode.isArray())
    {
        if (stateName == States::PROPERTY_PRIORITY && node[0].isString() && node[1].isDouble())
            defaultValues->set(stateName, getPriorityValue(node[0].asString()) + (float)node[1].asDouble());
        else
            throw; // FIXME: support array default values
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
            if (std::find(States::PROPERTY_NAMES.begin(), States::PROPERTY_NAMES.end(), stateName) != States::PROPERTY_NAMES.end())
            {
                // Parse states
                if (statesNode[stateName].isObject())
                {
                    data::Binding binding;
                    if (parseBinding(statesNode[stateName], scope, binding))
                        stateBlock.bindingMap.bindings[stateName] = binding;
                    else
                        parseState(statesNode[stateName], scope, stateBlock, stateName);

                    // Don't forget to parse default value, even if there is no binding
                    if (statesNode[stateName].isMember("default"))
                    {
                        auto defaultValueNode = statesNode[stateName].get("default", 0);
                        parseState(defaultValueNode, scope, stateBlock, stateName);
                    }
                }
                else
                {
                    parseState(statesNode[stateName], scope, stateBlock, stateName);
                }
            }
            else if (std::find(_extraStateNames.begin(), _extraStateNames.end(), stateName) != _extraStateNames.end())
            {
                // Parse extra states
                if (stateName == EXTRA_PROPERTY_BLENDING_MODE)
                {
                    parseBlendingMode(statesNode[stateName], scope, stateBlock);
                }
                else if (stateName == EXTRA_PROPERTY_STENCIL_TEST)
                {
                    parseStencilState(statesNode[stateName], scope, stateBlock);
                }
            }
            else
            {
                throw; // FIXME: log warning because the state name does not match any known state
            }
        }
    }
}

void
EffectParser::parseState(const Json::Value& node,
                         const Scope&       scope,
                         StateBlock&        stateBlock,
                         const std::string& stateProperty)
{
    if (stateProperty == States::PROPERTY_PRIORITY)
        parsePriority(node, scope, stateBlock);
    else if (stateProperty == _extraStateNames[0])
        parseBlendingMode(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_BLENDING_SOURCE)
        parseBlendingSource(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_BLENDING_DESTINATION)
        parseBlendingDestination(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_ZSORTED)
        parseZSort(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_COLOR_MASK)
        parseColorMask(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_DEPTH_MASK)
        parseDepthMask(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_DEPTH_FUNCTION)
        parseDepthFunction(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_TRIANGLE_CULLING)
        parseTriangleCulling(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_STENCIL_FUNCTION)
        parseStencilFunction(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_STENCIL_REFERENCE)
        parseStencilReference(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_STENCIL_MASK)
        parseStencilMask(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_STENCIL_FAIL_OPERATION)
        parseStencilFailOperation(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_STENCIL_ZFAIL_OPERATION)
        parseStencilZFailOperation(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_STENCIL_ZPASS_OPERATION)
        parseStencilZPassOperation(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_SCISSOR_TEST)
        parseScissorTest(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_SCISSOR_BOX)
        parseScissorBox(node, scope, stateBlock);
    else if (stateProperty == States::PROPERTY_TARGET)
        parseTarget(node, scope, stateBlock);
}

void
EffectParser::parsePriority(const Json::Value&	node,
                            const Scope&        scope,
                            StateBlock&         stateBlock)
{
    if (!node.isNull())
    {
        float priority = 0.f;

        if (node.isInt())
            priority = (float)node.asInt();
        else if (node.isDouble())
            priority = (float)node.asDouble();
        else if (node.isString())
            priority = getPriorityValue(node.asString());
        else if (node.isArray())
        {
            if (node[0].isString() && node[1].isDouble())
                priority = getPriorityValue(node[0].asString()) + (float)node[1].asDouble();
        }

        stateBlock.states.priority(priority);
    }
}

void
EffectParser::parseBlendingMode(const Json::Value&	node,
                                const Scope&        scope,
                                StateBlock&         stateBlock)
{
    if (node.isArray())
    {
        auto blendingSrcString = node[0].asString();
        if (_blendingSourceMap.count(blendingSrcString))
            stateBlock.states.blendingSourceFactor(static_cast<render::Blending::Source>(_blendingSourceMap.at(blendingSrcString)));

        auto blendingDstString = node[1].asString();
        if (_blendingDestinationMap.count(blendingDstString))
            stateBlock.states.blendingDestinationFactor(static_cast<render::Blending::Destination>(_blendingDestinationMap.at(blendingDstString)));
    }
    else if (node.isString())
    {
        auto blendingModeString = node.asString();

        if (_blendingModeMap.count(blendingModeString))
        {
            auto blendingMode = _blendingModeMap.at(blendingModeString);

            stateBlock.states.blendingSourceFactor(static_cast<render::Blending::Source>(blendingMode & 0x00ff));
            stateBlock.states.blendingDestinationFactor(static_cast<render::Blending::Destination>(blendingMode & 0xff00));
        }
    }
}

void
EffectParser::parseBlendingSource(const Json::Value&    node,
                                  const Scope&          scope,
                                  StateBlock&           stateBlock)
{
    if (node.isString())
    {
        auto blendingSourceString = _blendingSourceMap.at(node.asString());

        stateBlock.states.blendingSourceFactor(static_cast<render::Blending::Source>(blendingSourceString));
    }
}

void
EffectParser::parseBlendingDestination(const Json::Value&   node,
                                       const Scope&         scope,
                                       StateBlock&          stateBlock)
{
    if (node.isString())
    {
        auto blendingDestination = _blendingDestinationMap.at(node.asString());

        stateBlock.states.blendingDestinationFactor(static_cast<render::Blending::Destination>(blendingDestination));
    }
}

void
EffectParser::parseZSort(const Json::Value&	node,
                         const Scope&       scope,
                         StateBlock&        stateBlock)
{
    if (node.isBool())
        stateBlock.states.zSorted(node.asBool());
}

void
EffectParser::parseColorMask(const Json::Value&	node,
                             const Scope&       scope,
                             StateBlock&        stateBlock) const
{
    if (node.isBool())
        stateBlock.states.colorMask(node.asBool());
}

void
EffectParser::parseDepthMask(const Json::Value&	    node,
                             const Scope&           scope,
                             StateBlock&            stateBlock)
{
    if (node.isBool())
        stateBlock.states.depthMask(node.asBool());
}

void
EffectParser::parseDepthFunction(const Json::Value&	node,
			                     const Scope&       scope,
                                 StateBlock&        stateBlock)
{
    if (node.isString())
    {
        auto compareModeString = node.asString();
        auto exist = _compareFuncMap.find(compareModeString) != _compareFuncMap.end();

        if (exist)
            stateBlock.states.depthFunction(_compareFuncMap.at(compareModeString));
    }
}

void
EffectParser::parseTriangleCulling(const Json::Value&   node,
                                   const Scope&         scope,
                                   StateBlock&          stateBlock)
{
    if (node.isString())
    {
        auto triangleCullingString = node.asString();
        auto exist = _triangleCullingMap.find(triangleCullingString) != _triangleCullingMap.end();

        if (exist)
            stateBlock.states.triangleCulling(_triangleCullingMap.at(triangleCullingString));
    }
}

void
EffectParser::parseStencilState(const Json::Value&  node,
                                const Scope&        scope,
                                StateBlock&         stateBlock)
{
	if (node.isObject())
	{
        auto stencilFuncValue = node.get(States::PROPERTY_STENCIL_FUNCTION, 0);
        auto stencilRefValue = node.get(States::PROPERTY_STENCIL_REFERENCE, 0);
        auto stencilMaskValue = node.get(States::PROPERTY_STENCIL_MASK, 0);
        auto stencilOpsValue = node.get(EXTRA_PROPERTY_STENCIL_OPS, 0);

        parseStencilFunction(stencilFuncValue, scope, stateBlock);
        parseStencilReference(stencilRefValue, scope, stateBlock);
        parseStencilMask(stencilMaskValue, scope, stateBlock);

        parseStencilOperations(stencilOpsValue, scope, stateBlock);
	}
    else if (node.isArray())
    {
        parseStencilFunction(node[0], scope, stateBlock);
        parseStencilReference(node[1], scope, stateBlock);
        parseStencilMask(node[2], scope, stateBlock);

        parseStencilOperations(node[3], scope, stateBlock);
    }
}

void
EffectParser::parseStencilFunction(const Json::Value&  node,
                                   const Scope&        scope,
                                   StateBlock&         stateBlock)
{
    if (node.isString())
        stateBlock.states.stencilFunction(_compareFuncMap.at(node.asString()));
}

void
EffectParser::parseStencilReference(const Json::Value&  node,
                                    const Scope&        scope,
                                    StateBlock&         stateBlock)
{
    if (node.isInt())
        stateBlock.states.stencilReference(node.asInt());
}

void
EffectParser::parseStencilMask(const Json::Value&  node,
                               const Scope&        scope,
                               StateBlock&         stateBlock)
{
    if (node.isInt())
        stateBlock.states.stencilMask(node.asUInt());
}

void
EffectParser::parseStencilOperations(const Json::Value& node,
                                     const Scope&       scope,
                                     StateBlock&        stateBlock)
{
    if (node.isArray())
    {
        if (node[0].isString())
            stateBlock.states.stencilFailOperation(_stencilOpMap.at(node[0].asString()));
        if (node[1].isString())
            stateBlock.states.stencilZFailOperation(_stencilOpMap.at(node[1].asString()));
        if (node[2].isString())
            stateBlock.states.stencilZPassOperation(_stencilOpMap.at(node[2].asString()));
    }
    else
    {
        parseStencilFailOperation(node.get(EXTRA_PROPERTY_STENCIL_FAIL_OP, 0), scope, stateBlock);
        parseStencilZFailOperation(node.get(EXTRA_PROPERTY_STENCIL_Z_FAIL_OP, 0), scope, stateBlock);
        parseStencilZPassOperation(node.get(EXTRA_PROPERTY_STENCIL_Z_PASS_OP, 0), scope, stateBlock);
    }
}

void
EffectParser::parseStencilFailOperation(const Json::Value& node,
                                        const Scope&       scope,
                                        StateBlock&        stateBlock)
{
    if (node.isString())
        stateBlock.states.stencilFailOperation(_stencilOpMap.at(node.asString()));
}

void
EffectParser::parseStencilZFailOperation(const Json::Value& node,
                                         const Scope&       scope,
                                         StateBlock&        stateBlock)
{
    if (node.isString())
        stateBlock.states.stencilZFailOperation(_stencilOpMap.at(node.asString()));
}

void
EffectParser::parseStencilZPassOperation(const Json::Value& node,
                                         const Scope&       scope,
                                         StateBlock&        stateBlock)
{
    if (node.isString())
        stateBlock.states.stencilZPassOperation(_stencilOpMap.at(node.asString()));
}

void
EffectParser::parseScissorTest(const Json::Value& node,
                               const Scope&       scope,
                               StateBlock&        stateBlock)
{
    if (!node.isNull() && node.isBool())
        stateBlock.states.scissorTest(node.asBool());
}

void
EffectParser::parseScissorBox(const Json::Value& node,
                              const Scope&       scope,
                              StateBlock&        stateBlock)
{
    if (!node.isNull() && node.isArray())
    {
        auto scissorBox = math::ivec4();

        if (node[0].isInt())
            scissorBox.x = node[0].asInt();
        if (node[1].isInt())
            scissorBox.y = node[1].asInt();
        if (node[2].isInt())
            scissorBox.z = node[2].asInt();
        if (node[3].isInt())
            scissorBox.w = node[3].asInt();

        stateBlock.states.scissorBox(scissorBox);
    }
}

void
EffectParser::parseTarget(const Json::Value&    node,
                          const Scope&          scope,
                          StateBlock&           stateBlock)
{
    AbstractTexture::Ptr target = nullptr;
    std::string	targetName;

    if (node.isObject())
    {
        auto nameValue = node.get("name", 0);

        if (nameValue.isString())
            targetName = nameValue.asString();

        if (!node.isMember("size") && !(node.isMember("width") && node.isMember("height")))
            return;

        auto width = 0;
        auto height = 0;

        if (node.isMember("size"))
            width = height = node.get("size", 0).asUInt();
        else
        {
            if (!node.isMember("width") || !node.isMember("height"))
            {
                _error->execute(
                    shared_from_this(),
                    file::Error(
                    _resolvedFilename
                    + ": render target definition requires both \"width\" and \"height\" properties."
                    )
                );
            }

            width = node.get("width", 0).asUInt();
            height = node.get("height", 0).asUInt();
        }

        const bool isCubeTexture = node.get("isCube", 0).isBool()
            ? node.get("isCube", 0).asBool()
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
    else if (node.isString())
    {
        targetName = node.asString();
        target = _assetLibrary->texture(targetName);
        if (target == nullptr)
            throw;

        _effectData->set(targetName, target->sampler());
    }

    if (target)
        stateBlock.states.target(target->sampler());
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
    auto begin = insertIt;

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
        loadGLSLDependencies(blocks, begin, ++insertIt, options);
}

void
EffectParser::loadGLSLDependencies(GLSLBlockListPtr		   blocks,
                                   GLSLBlockList::iterator begin,
                                   GLSLBlockList::iterator end,
                                   file::Options::Ptr      options)
{
    for (auto blockIt = begin; blockIt != end; blockIt++)
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
EffectParser::dependencyErrorHandler(std::shared_ptr<Loader> loader, const Error& error, const std::string& filename)
{
    LOG_DEBUG(
        "Unable to load '" << filename
        << "' required by \"" << _filename
        << "\", included paths are: " << std::to_string(loader->options()->includePaths(), ", ")
    );

    _error->execute(
        shared_from_this(),
        file::Error(
            "Unable to load '"
            + filename
            + "' required by \"" + _filename
            + "', included paths are: "
            + std::to_string(loader->options()->includePaths(), ", ")
        )
    );
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
    auto options = loader->options();
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

            if (_shaderToGLSL.count(vs) != 0)
                vs->source("#define VERTEX_SHADER\n" + concatenateGLSLBlocks(_shaderToGLSL[vs]));
            if (_shaderToGLSL.count(fs) != 0)
                fs->source("#define FRAGMENT_SHADER\n" + concatenateGLSLBlocks(_shaderToGLSL[fs]));
        }
    }

	_effect->data()->copyFrom(_effectData);
    _options->assetLibrary()->effect(_filename, _effect);

    _complete->execute(shared_from_this());

    _loaderCompleteSlots.clear();
    _loaderErrorSlots.clear();
}
