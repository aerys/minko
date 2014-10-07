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
#include "minko/data/Container.hpp"
#include "minko/render/Pass.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/States.hpp"
#include "minko/data/Binding.hpp"

namespace minko
{
	namespace render
	{
		class DrawCall :
            public std::enable_shared_from_this<DrawCall>
		{
        public:
            static const unsigned int	MAX_NUM_TEXTURES;
            static const unsigned int   MAX_NUM_VERTEXBUFFERS;

		private:
            typedef const ProgramInputs::UniformInput&      ConstUniformInputRef;
            typedef const ProgramInputs::AttributeInput&    ConstAttrInputRef;

            template <typename T>
            struct UniformValue
            {
                const int location;
                const uint size;
                const T* data;
            };

            struct SamplerValue
            {
                const uint position;
                const int* resourceId;
                const int location;
                /*
                WrapMode* wrapMode;
                TextureFilter* textureFilter;
                MipFilter* mipFilter;
                TextureType* type;
                */
            };

            struct AttributeValue
            {
                const uint position;
                const int location;
                const int* resourceId;
                const uint size;
                const uint* stride;
                const uint offset;
            };

            typedef std::shared_ptr<AbstractContext>	            AbsCtxPtr;
			typedef std::shared_ptr<AbstractTexture>	            AbsTexturePtr;
			typedef std::shared_ptr<Program>			            ProgramPtr;
            typedef std::unordered_map<std::string, std::string>    StringMap;

		private:
            const data::Container&              _rootData;
            const data::Container&              _rendererData;
            const data::Container&              _targetData;
            StringMap                           _variables;

			std::shared_ptr<render::Pass>		_pass;
			std::shared_ptr<Program>			_program;
            int*								_indexBuffer;
            uint*                               _firstIndex;
            uint*								_numIndices;
            std::vector<UniformValue<int>>      _uniformInt;
            std::vector<UniformValue<float>>    _uniformFloat;
            std::vector<UniformValue<bool>>     _uniformBool;
            std::vector<SamplerValue>           _samplers;
            std::vector<AttributeValue>         _attributes;

            float*				                _priority;
            bool*						        _zsorted;
            Blending::Source*		            _blendingSourceFactor;
            Blending::Destination*	            _blendingDestinationFactor;
            bool*						        _colorMask;
            bool*					            _depthMask;
            CompareMode*		    		    _depthFunc;
            TriangleCulling*                    _triangleCulling;
            CompareMode*					    _stencilFunction;
            int*							    _stencilReference;
            uint*						        _stencilMask;
            StencilOperation*			        _stencilFailOp;
            StencilOperation*			        _stencilZFailOp;
            StencilOperation*			        _stencilZPassOp;
            bool*						        _scissorTest;
            ScissorBox*					        _scissorBox;
            /*SamplerStates               _samplerStates;
            AbstractTexturePtr		    _target;*/

		public:
            DrawCall(std::shared_ptr<render::Pass>  pass,
                     const StringMap&               variables,
                     const data::Container&         rootData,
                     const data::Container&         rendererData,
                     const data::Container&         targetData) :
                _pass(pass),
                _variables(variables),
                _rootData(rootData),
                _rendererData(rendererData),
                _targetData(targetData)
            {

            }

            DrawCall(const DrawCall& drawCall) :
                _pass(drawCall._pass),
                _variables(drawCall._variables),
                _rootData(drawCall._rootData),
                _rendererData(drawCall._rendererData),
                _targetData(drawCall._targetData)
            {
            }
            
            inline
            std::shared_ptr<Pass>
            pass() const
            {
                return _pass;
            }

            inline
            std::shared_ptr<Program>
            program() const
            {
                return _program;
            }

            inline
            StringMap&
            variables()
            {
                return _variables;
            }

            inline
            const data::Container&
            rootData() const
            {
                return _rootData;
            }

            inline
            const data::Container&
            rendererData() const
            {
                return _rendererData;
            }

            inline
            const data::Container&
            targetData() const
            {
                return _targetData;
            }

			void
			render(std::shared_ptr<AbstractContext> context, AbsTexturePtr renderTarget) const;

            void
            bind(std::shared_ptr<Program>   program,
                 const data::BindingMap&    attributeBindings,
                 const data::BindingMap&    uniformBindings,
                 const data::BindingMap&    stateBindings);

		private:
            void
            reset();

            void
            bindUniform(std::shared_ptr<Program>    program,
                        ConstUniformInputRef        input,
                        const data::Container&      container,
                        const std::string&          propertyName);

            void
            bindAttribute(std::shared_ptr<Program>    program,
                          ConstAttrInputRef           input,
						  const data::Container&      container,
						  const std::string&          propertyName);

			void
			bindIndexBuffer(const std::unordered_map<std::string, std::string>& variables,
                            const data::Container&                              targetData);

			void
            bindStates(const data::BindingMap& stateBindings);
			
            const data::Container&
            getContainer(data::Binding::Source source);

            template <typename T>
            T*
            bindState(const std::string         stateName,
                      const data::BindingMap&   stateBindings,
                      T*                        defaultValue)
            {
                if (stateBindings.count(stateName) == 0)
                    return defaultValue;

                const auto& binding = stateBindings.at(stateName);
                auto& container = getContainer(binding.source());

                return container.getUnsafePointer<T>(
                    data::Container::getActualPropertyName(_variables, binding.propertyName())
                );
            }
		};
	}
}
