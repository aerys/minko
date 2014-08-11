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

#pragma once

#include "minko/Common.hpp"

#include "minko/Signal.hpp"
#include "minko/data/Container.hpp"
#include "minko/render/Pass.hpp"
#include "minko/render/States.hpp"

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

            typedef std::shared_ptr<AbstractContext>	AbsCtxPtr;
			typedef std::shared_ptr<AbstractTexture>	AbsTexturePtr;
            typedef std::shared_ptr<data::Container>	ContainerPtr;
			typedef std::shared_ptr<Program>			ProgramPtr;

		private:
            const data::MacroBindingMap&        _macroBindings;

			std::shared_ptr<render::Pass>		_pass;
			std::shared_ptr<Program>			_program;
            uint*								_numIndices;
            uint*                               _firstIndex;
            uint*								_indexBuffer;
            std::vector<UniformValue<int>>      _uniformInt;
            std::vector<UniformValue<float>>    _uniformFloat;
            std::vector<UniformValue<bool>>     _uniformBool;
            std::vector<SamplerValue>           _samplers;
            std::vector<AttributeValue>         _attributes;
            std::shared_ptr<States>             _states;

		public:
            DrawCall(const data::MacroBindingMap& macroBindings) :
                _macroBindings(macroBindings)
            {

            }

            const data::MacroBindingMap&
            macroBindings()
            {
                return _macroBindings;
            }
            
			void
			render(std::shared_ptr<AbstractContext> context, AbsTexturePtr renderTarget) const;

            void
            bind(std::shared_ptr<Program>   program,
                 ContainerPtr               rootData,
                 ContainerPtr               rendererData,
                 ContainerPtr               targetData,
                 const data::BindingMap&    attributeBindings,
                 const data::BindingMap&    uniformBindings,
                 const data::BindingMap&    stateBindings);

		private:
            void
            bindUniform(std::shared_ptr<Program>    program,
                        ConstUniformInputRef        input,
                        ContainerPtr                container,
                        const std::string&          propertyName);

            void
            bindAttribute(std::shared_ptr<Program>    program,
                          ConstAttrInputRef           input,
						  ContainerPtr                container,
						  const std::string&          propertyName);

			void
			bindIndexBuffer();

			void
			bindStates();
			
			void
			bindState(const std::string& stateName);

            ContainerPtr
            getContainer(ContainerPtr           rootData,
                         ContainerPtr           rendererData,
                         ContainerPtr           targetData,
                         data::BindingSource    source);
		};		
	}
}
