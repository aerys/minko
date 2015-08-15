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

#pragma once

#include "minko/Common.hpp"

#include "minko/Signal.hpp"
#include "minko/file/AbstractParser.hpp"
#include "minko/file/FileProtocol.hpp"
#include "minko/render/Blending.hpp"
#include "minko/render/Shader.hpp"
#include "minko/scene/Layout.hpp"
#include "minko/data/MacroBinding.hpp"
#include "minko/data/BindingMap.hpp"
#include "minko/data/Store.hpp"
#include "minko/render/States.hpp"
#include "minko/render/SamplerStates.hpp"

namespace Json {
    class Value;
}

namespace minko
{
    namespace file
    {
        class EffectParser :
            public AbstractParser
        {
        public:
            typedef std::shared_ptr<EffectParser>	Ptr;

            static const float UNSET_PRIORITY_VALUE;

        private:
            typedef std::shared_ptr<render::AbstractTexture>	            AbstractTexturePtr;
            typedef std::shared_ptr<Loader>								    LoaderPtr;
            typedef std::shared_ptr<render::Effect>						    EffectPtr;
            typedef std::shared_ptr<render::Pass>						    PassPtr;
            typedef std::shared_ptr<render::Shader>						    ShaderPtr;
            typedef std::shared_ptr<file::Options>						    OptionsPtr;
            typedef std::unordered_map<std::string, AbstractTexturePtr>     TexturePtrMap;
            typedef Signal<LoaderPtr, const Error&>::Slot                   LoaderErrorSlot;
            typedef std::vector<PassPtr>                                    Technique;
            typedef std::unordered_map<std::string, Technique>              Techniques;
            typedef std::vector<PassPtr>                                    Passes;
            typedef std::unordered_map<LoaderPtr, Signal<LoaderPtr>::Slot>  LoaderCompleteSlotMap;
            typedef std::unordered_map<LoaderPtr, LoaderErrorSlot>          LoaderErrorSlotMap;

            enum class GLSLBlockType
            {
                TEXT,
                FILE
            };

            typedef std::pair<GLSLBlockType, std::string>           GLSLBlock;
            typedef std::forward_list<GLSLBlock> 			        GLSLBlockList;
            typedef std::shared_ptr<GLSLBlockList>			        GLSLBlockListPtr;
            typedef std::unordered_map<ShaderPtr, GLSLBlockListPtr> ShaderToGLSLBlocks;

            template <typename T>
            struct Block
            {
                T bindingMap;

                virtual
                ~Block()
                {}
            };

            struct StateBlock : public Block<data::BindingMap>
            {
                render::States states;

                StateBlock()
                {
                    // we set the priority to a special value in order to know
                    // wether it was actually read from the file or not
                    states.priority(EffectParser::UNSET_PRIORITY_VALUE);
                    bindingMap.defaultValues.addProvider(states.data());
                }

                StateBlock(const StateBlock& s) :
                    Block(s),
                    states(s.states)
                {
                    // data::Store copy constructor makes a shallow copy, to avoid ending up with
                    // data::Provider shared by multiple blocks/scopes, we have to simulate a deep copy
                    // by emptying the data::Store and then add the actual data::Provider of the new
                    // render::States object
                    bindingMap.defaultValues.removeProvider(bindingMap.defaultValues.providers().front());
                    bindingMap.defaultValues.addProvider(states.data());
                }
            };

            typedef Block<data::BindingMap> AttributeBlock;
            typedef Block<data::MacroBindingMap> MacroBlock;
            typedef Block<data::BindingMap> UniformBlock;

            struct Scope
            {
                const Scope* parent;
                std::vector<const Scope*> children;
                AttributeBlock attributeBlock;
                UniformBlock uniformBlock;
                StateBlock stateBlock;
                MacroBlock macroBlock;
                std::string defaultTechnique;
                Passes passes;
                Techniques techniques;

                Scope() :
                    parent(nullptr)
                {}

                Scope(const Scope& scope) :
                    parent(scope.parent),
                    children(),
                    attributeBlock(scope.attributeBlock),
                    uniformBlock(scope.uniformBlock),
                    stateBlock(scope.stateBlock),
                    macroBlock(scope.macroBlock),
                    defaultTechnique(scope.defaultTechnique),
                    passes(scope.passes),
                    techniques(scope.techniques)
                {}

                Scope(const Scope& scope, Scope& parent) :
                    parent(&parent),
                    children(),
                    attributeBlock(scope.attributeBlock),
                    uniformBlock(scope.uniformBlock),
                    stateBlock(scope.stateBlock),
                    macroBlock(scope.macroBlock),
                    defaultTechnique(scope.defaultTechnique),
                    // passes(scope.passes),
                    techniques(scope.techniques)
                {
                    parent.children.push_back(this);
                }
            };

            static const std::string    EXTRA_PROPERTY_BLENDING_MODE;
            static const std::string    EXTRA_PROPERTY_STENCIL_TEST;
            static const std::string    EXTRA_PROPERTY_STENCIL_OPS;
            static const std::string    EXTRA_PROPERTY_STENCIL_FAIL_OP;
            static const std::string    EXTRA_PROPERTY_STENCIL_Z_FAIL_OP;
            static const std::string    EXTRA_PROPERTY_STENCIL_Z_PASS_OP;

            static const std::unordered_map<std::string, unsigned int>				_blendingSourceMap;
            static const std::unordered_map<std::string, unsigned int>				_blendingDestinationMap;
            static const std::unordered_map<std::string, unsigned int>				_blendingModeMap;
            static const std::unordered_map<std::string, render::CompareMode>		_compareFuncMap;
            static const std::unordered_map<std::string, render::TriangleCulling>	_triangleCullingMap;
            static const std::unordered_map<std::string, render::StencilOperation>	_stencilOpMap;
            static const std::unordered_map<std::string, float>					    _priorityMap;
            static const std::array<std::string, 2>                                 _extraStateNames;

            std::string						_filename;
            std::string						_resolvedFilename;
            std::shared_ptr<file::Options>	_options;
            std::shared_ptr<render::Effect> _effect;
            std::string						_effectName;
            std::shared_ptr<AssetLibrary>	_assetLibrary;

            Scope                           _globalScope;
            ShaderToGLSLBlocks              _shaderToGLSL;
            unsigned int					_numDependencies;
            unsigned int					_numLoadedDependencies;
            std::shared_ptr<data::Provider> _effectData;

            LoaderCompleteSlotMap           _loaderCompleteSlots;
            LoaderErrorSlotMap              _loaderErrorSlots;

        public:
            inline static
            Ptr
            create()
            {
                return std::shared_ptr<EffectParser>(new EffectParser());
            }

            inline
            std::shared_ptr<render::Effect>
            effect()
            {
                return _effect;
            }

            inline
            const std::string&
            effectName()
            {
                return _effectName;
            }

            void
            parse(const std::string&				filename,
                  const std::string&                resolvedFilename,
                  std::shared_ptr<Options>          options,
                  const std::vector<unsigned char>&	data,
                  std::shared_ptr<AssetLibrary>		assetLibrary);

        private:
            EffectParser();

            float
            getPriorityValue(const std::string& name);

            void
            parseGlobalScope(const Json::Value& node, Scope& scope);

            bool
            parseConfiguration(const Json::Value& node);

            void
            fixMissingPassPriorities(std::vector<PassPtr>& passes);

            void
            parseTechniques(const Json::Value& node, Scope& scope, Techniques& techniques);

            std::shared_ptr<render::Pass>
            getPassToExtend(const Json::Value& extendNode);

            std::shared_ptr<render::Pass>
            findPassFromEffectFilename(const std::string& effectFilename,
                                       const std::string& techniqueName,
                                       const std::string& passName);

            void
            parsePass(const Json::Value& node, Scope& scope, std::vector<PassPtr>& passes);

            void
            checkDeferredPassBindings(const Scope& passScope);

            void
            parsePasses(const Json::Value& node, Scope& scope, std::vector<PassPtr>& passes);

            void
            parseDefaultValue(const Json::Value& node, const Scope& scope);

            void
            parseDefaultValue(const Json::Value&    node,
                              const Scope&          scope,
                              const std::string&    valueName,
                              data::Provider::Ptr   defaultValues);

            template<typename T>
            void
            parseDefaultValueSamplerStates(const Json::Value&    node,
                                    const Scope&          scope,
                                    const std::string&    valueName,
                                    data::Provider::Ptr   defaultValues);

            void
            parseDefaultValueStates(const Json::Value&    node,
                                    const Scope&          scope,
                                    const std::string&    stateName,
                                    data::Provider::Ptr   defaultValues);

            void
            parseDefaultValueVectorArray(const Json::Value&    defaultValueNode,
                                         const Scope&          scope,
                                         const std::string&    valueName,
                                         data::Provider::Ptr   defaultValues);

            void
            parseDefaultValueVectorObject(const Json::Value&    node,
                                          const Scope&          scope,
                                          const std::string&    valueName,
                                          data::Provider::Ptr   defaultValues);

            bool
            parseBinding(const Json::Value& node, const Scope& scope, data::Binding& binding);

            void
            parseMacroBinding(const Json::Value& node, const Scope& scope, data::MacroBinding& binding);

            void
            parseMacroBindings(const Json::Value& node, const Scope& scope, data::MacroBindingMap& bindings);

            void
            parseAttributes(const Json::Value&node, const Scope& scope, AttributeBlock& attributes);

            void
            parseUniforms(const Json::Value& node, const Scope& scope, UniformBlock& uniforms);

            void
            parseMacros(const Json::Value& node, const Scope& scope, MacroBlock& macros);

            void
            parseStates(const Json::Value& node, const Scope& scope, StateBlock& states);

            void
            parseState(const Json::Value& node, const Scope& scope, StateBlock& stateBlock, const std::string& stateProperty);

            void
            parsePriority(const Json::Value&    node,
                          const Scope&          scope,
                          StateBlock&           stateBlock);

            void
            parseBlendingMode(const Json::Value&    node,
                              const Scope&          scope,
                              StateBlock&           stateBlock);

            void
            parseBlendingSource(const Json::Value&  node,
                                const Scope&        scope,
                                StateBlock&         stateBlock);

            void
            parseBlendingDestination(const Json::Value&  node,
                                     const Scope&        scope,
                                     StateBlock&         stateBlock);

            void
            parseZSort(const Json::Value&   node,
                       const Scope&         scope,
                       StateBlock&          stateBlock);

            void
            parseColorMask(const Json::Value&   node,
                           const Scope&         scope,
                           StateBlock&          stateBlock) const;

            void
            parseDepthMask(const Json::Value&	node,
                           const Scope&         scope,
                           StateBlock&          stateBlock);

            void
            parseDepthFunction(const Json::Value&	node,
                               const Scope&         scope,
                               StateBlock&          stateBlock);

            void
            parseTriangleCulling(const Json::Value&         node,
                                 const Scope&               scope,
                                 StateBlock&          stateBlock);

            void
            parseTarget(const Json::Value&  node,
                        const Scope&        scope,
                        StateBlock&         stateBlock);

            void
            parseStencilState(const Json::Value&    node,
                              const Scope&          scope,
                              StateBlock&           stateBlock);

            void
            parseStencilFunction(const Json::Value&    node,
                                const Scope&          scope,
                                StateBlock&           stateBlock);

            void
            parseStencilReference(const Json::Value&    node,
                                  const Scope&          scope,
                                  StateBlock&           stateBlock);

            void
            parseStencilMask(const Json::Value&    node,
                             const Scope&          scope,
                             StateBlock&           stateBlock);

            void
            parseStencilOperations(const Json::Value&   node,
                                   const Scope&         scope,
                                   StateBlock&          stateBlock);

            void
            parseStencilFailOperation(const Json::Value&   node,
                                      const Scope&         scope,
                                      StateBlock&          stateBlock);

            void
            parseStencilZFailOperation(const Json::Value&   node,
                                       const Scope&         scope,
                                       StateBlock&          stateBlock);

            void
            parseStencilZPassOperation(const Json::Value&   node,
                                       const Scope&         scope,
                                       StateBlock&          stateBlock);

            void
            parseScissorTest(const Json::Value&     node,
                             const Scope&           scope,
                             StateBlock&            stateBlock);

            void
            parseScissorBox(const Json::Value&     node,
                            const Scope&           scope,
                            StateBlock&            stateBlock);

            void
            parseSamplerStates(const Json::Value& node,
                               const Scope& scope,
                               const std::string uniformName,
                               data::Provider::Ptr defaultValues,
                               data::BindingMap& bindings);

            ShaderPtr
            parseShader(const Json::Value& node, const Scope& scope, render::Shader::Type type);

            void
            parseGLSL(const std::string&        glsl,
                      OptionsPtr         		options,
                      GLSLBlockListPtr		    blocks,
                      GLSLBlockList::iterator	insertIt);

            void
            loadGLSLDependencies(GLSLBlockListPtr		    blocks,
                                 GLSLBlockList::iterator    begin,
                                 GLSLBlockList::iterator    end,
                                 OptionsPtr                 options);

            void
            dependencyErrorHandler(std::shared_ptr<Loader>  loader,
                                   const Error&       		error,
                                   const std::string&       filename);

            void
            glslIncludeCompleteHandler(LoaderPtr 			    loader,
                                       GLSLBlockListPtr 		blocks,
                                       GLSLBlockList::iterator 	blockIt,
                                       const std::string&       filename);

            std::string
            concatenateGLSLBlocks(GLSLBlockListPtr blocks);

            void
            loadTexture(const std::string&  textureFilename,
                        const std::string&  uniformName,
                        data::Provider::Ptr     defaultValues);

            std::shared_ptr<render::States>
            createStates(const StateBlock& block);

            void
            finalize();
        };
    }
}
