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
#include "minko/data/Store.hpp"
#include "minko/render/Pass.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/render/States.hpp"
#include "minko/data/Binding.hpp"
#include "minko/data/ResolvedBinding.hpp"
#include "minko/render/Priority.hpp"

namespace minko
{
	namespace render
	{
        class DrawCallZSorter;

        class DrawCall
		{
        public:
            static const unsigned int	MAX_NUM_TEXTURES;
            static const unsigned int   MAX_NUM_VERTEXBUFFERS;

            template <typename T>
            struct UniformValue
            {
                const int location;
                const uint size;
                const T* data;
            };

		private:
            typedef const ProgramInputs::UniformInput&      ConstUniformInputRef;
            typedef const ProgramInputs::AttributeInput&    ConstAttrInputRef;

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
            typedef data::Store::PropertyChangedSignal::Slot        ChangedSlot;

		private:
            std::shared_ptr<Pass>               _pass;
            data::Store&                        _rootData;
            data::Store&                        _rendererData;
            data::Store&                        _targetData;
            StringMap                           _variables;

			std::shared_ptr<Program>			_program;
            int*								_indexBuffer;
            uint*                               _firstIndex;
            uint*								_numIndices;
            std::vector<UniformValue<int>>      _uniformInt;
            std::vector<UniformValue<float>>    _uniformFloat;
            std::vector<UniformValue<int>>      _uniformBool;
            std::vector<SamplerValue>           _samplers;
            std::vector<AttributeValue>         _attributes;

            float*				                _priority;
            bool*						        _zSorted;
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
            math::ivec4*					    _scissorBox;
			TextureSampler*						_target;
            /*SamplerStates               		_samplerStates;*/

            std::map<const data::Binding*, ChangedSlot>    _propAddedOrRemovedSlot;

            std::shared_ptr<DrawCallZSorter>                _zSorter;
            Signal<DrawCall*>::Ptr                          _zSortNeeded;
		public:
            DrawCall(std::shared_ptr<Pass>  pass,
                     const StringMap&       variables,
                     data::Store&           rootData,
                     data::Store&           rendererData,
                     data::Store&           targetData);

            inline
            std::shared_ptr<Pass>
            pass()
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
            const StringMap&
            variables() const
            {
                return _variables;
            }

            inline
            data::Store&
            rootData()
            {
                return _rootData;
            }

            inline
            data::Store&
            rendererData()
            {
                return _rendererData;
            }

            inline
            data::Store&
            targetData()
            {
                return _targetData;
            }

            inline
            const std::vector<UniformValue<int>>&
            boundBoolUniforms() const
            {
                return _uniformBool;
            }

            inline
            const std::vector<UniformValue<int>>&
            boundIntUniforms() const
            {
                return _uniformInt;
            }

            inline
            const std::vector<UniformValue<float>>&
            boundFloatUniforms() const
            {
                return _uniformFloat;
            }

            std::shared_ptr<DrawCallZSorter>
            zSorter() const
            {
                return _zSorter;
            }

            inline
            float
            priority() const
            {
                return *_priority;
            }

            inline
            bool
            zSorted() const
            {
                return *_zSorted;
            }

            inline
            Signal<DrawCall*>::Ptr
            zSortNeeded() const
            {
                return _zSortNeeded;
            }

            void
            bind(std::shared_ptr<Program> program);

			void
			render(std::shared_ptr<AbstractContext>  context,
                   AbsTexturePtr                     renderTarget) const;

            data::ResolvedBinding*
            bindUniform(const ProgramInputs::UniformInput&          input,
                        const std::map<std::string, data::Binding>& uniformBindings,
                        const data::Store&                          defaultValues);

            math::vec3
            getEyeSpacePosition();

		private:

            void
            reset();

            void
            bindAttributes();

            void
            bindAttribute(ConstAttrInputRef     input,
						  const data::Store&    store,
						  const std::string&    propertyName);

			void
			bindIndexBuffer();

			void
            bindStates();

            data::Store&
            getStore(data::Binding::Source source);

            data::ResolvedBinding*
            resolveBinding(const ProgramInputs::AbstractInput&          input,
                           const std::map<std::string, data::Binding>&  bindings);

			void
			setUniformValueFromStore(const ProgramInputs::UniformInput&   input,
									 const std::string&                   propertyName,
									 const data::Store&                   store);

            template <typename T>
            T*
            bindState(const std::string&        stateName,
                      const data::BindingMap&   stateBindings)
            {
                auto& bindings = stateBindings.bindings;

                if (bindings.count(stateName) == 0)
                    return stateBindings.defaultValues.getUnsafePointer<T>(stateName);

                const auto& binding = bindings.at(stateName);
                auto& store = getStore(binding.source);
                auto unsafePointer = store.getUnsafePointer<T>(
                    data::Store::getActualPropertyName(_variables, binding.propertyName)
                );

                if (unsafePointer == nullptr)
                    return stateBindings.defaultValues.getUnsafePointer<T>(stateName);

                return unsafePointer;
            }

            template <typename T>
            void
            setUniformValue(std::vector<UniformValue<T>>& uniforms, int location, uint size, const T* data)
            {
                auto it = std::find_if(uniforms.begin(), uniforms.end(), [&](UniformValue<T>& u)
                {
                    return u.location == location;
                });

                if (it == uniforms.end())
                    uniforms.push_back({ location, size, data });
                else
                    it->data = data;
            }
		};
	}
}
