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
#include "minko/render/Blending.hpp"
#include "minko/data/Container.hpp"
#include "minko/render/ProgramInputs.hpp"
#include "minko/render/States.hpp"

namespace minko
{
	namespace render
	{
		class DrawCall :
            public std::enable_shared_from_this<DrawCall>
		{
		public:
			typedef std::shared_ptr<DrawCall> Ptr;

		private:
            typedef std::shared_ptr<AbstractContext>				AbsCtxPtr;
			typedef std::shared_ptr<data::Provider>					ProviderPtr;
            typedef std::shared_ptr<data::Container>				ContainerPtr;
			typedef data::Container::PropertyChangedSignal::Slot	ContainerPropertyChangedSlot;

		private:
			static const unsigned int									MAX_NUM_TEXTURES;
			static const unsigned int									MAX_NUM_VERTEXBUFFERS;

            static SamplerState                                         _defaultSamplerState;

			std::shared_ptr<Program>									_program;
			std::shared_ptr<data::Container>					        _data;
			std::shared_ptr<data::Container>					        _rendererData;
            std::shared_ptr<data::Container>					        _rootData;
            const data::BindingMap&	                                    _attributeBindings;
			const data::BindingMap&	                                    _uniformBindings;
			const data::BindingMap&	                                    _stateBindings;

            std::shared_ptr<States>                                     _states;
            std::vector<int>                                            _vertexBuffers;
            std::vector<int>                                            _vertexBufferLocations;
            std::vector<int>                                            _vertexSizes;
            std::vector<int>                                            _vertexAttributeSizes;
            std::vector<int>                                            _vertexAttributeOffsets;
            std::vector<int>                                            _textures;
            std::vector<int>                                            _textureLocations;
            std::vector<WrapMode>                                       _textureWrapMode;
            std::vector<TextureFilter>                                  _textureFilters;
            std::vector<MipFilter>                                      _textureMipFilters;
            uint                                                        _numIndices;
            uint                                                        _indexBuffer;
            std::shared_ptr<render::Texture>                            _target;
            render::Blending::Mode                                      _blendMode;
            bool                                                        _depthMask;
            render::CompareMode                                         _depthFunc;
            render::TriangleCulling                                     _triangleCulling;
			render::CompareMode											_stencilFunc;
			int															_stencilRef;
			uint														_stencilMask;
			render::StencilOperations									_stencilOps;
            std::unordered_map<uint, float>                             _uniformFloat;
            std::unordered_map<uint, std::shared_ptr<math::Vector2>>    _uniformFloat2;
            std::unordered_map<uint, std::shared_ptr<math::Vector3>>    _uniformFloat3;
            std::unordered_map<uint, std::shared_ptr<math::Vector4>>    _uniformFloat4;
            std::unordered_map<uint, const float*>                      _uniformFloat16;

            std::list<ContainerPropertyChangedSlot>							_propertyChangedSlots;
			std::unordered_map<std::string, ContainerPropertyChangedSlot>	_referenceChangedSlots;

		public:
			static inline
			Ptr
			create(const data::BindingMap&	attributeBindings,
				   const data::BindingMap&	uniformBindings,
				   const data::BindingMap&	stateBindings,
                   std::shared_ptr<States>  states)
			{
				return std::shared_ptr<DrawCall>(new DrawCall(
                    attributeBindings, uniformBindings, stateBindings, states
                ));;
			}

            inline
            std::shared_ptr<Texture>
            target() const
            {
                return _target;
            }

			inline
			float
			priority() const
			{
				return _states->priority();
			}

            void
            configure(std::shared_ptr<Program>  program,
                      ContainerPtr              data,
					  ContainerPtr              rendererData,
                      ContainerPtr              rootData);

			void
			render(const std::shared_ptr<AbstractContext>& context, std::shared_ptr<render::Texture> renderTarget);

			void
			initialize(ContainerPtr				                    data,
					   const std::map<std::string, std::string>&	inputNameToBindingName);

		private:
			DrawCall(const data::BindingMap&	attributeBindings,
				     const data::BindingMap&	uniformBindings,
					 const data::BindingMap&	stateBindings,
                     std::shared_ptr<States>    states);

			void
			reset();

			void
            bind(ContainerPtr data, ContainerPtr rendererData, ContainerPtr rootData);

			void
			bindProgramInputs();

			void
			bindStates();

			void
			bindVertexAttribute(ContainerPtr, const std::string& propertyName, int location, int vertexBufferId);

			void
			bindTextureSampler2D(ContainerPtr, const std::string& inputName, const std::string& propertyName, int location, int textureId);

			void
			bindUniform(ContainerPtr, const std::string& propertyName, ProgramInputs::Type, int location);

            template <typename T>
            T
            getDataProperty(const std::string& propertyName)
            {
                //watchProperty(propertyName);

                if (_data->hasProperty(propertyName))
                    return _data->get<T>(propertyName);

                if (_rootData->hasProperty(propertyName))
                    return _rootData->get<T>(propertyName);

				std::stringstream stream;
				stream << "failed to find property \'" << propertyName << "' in drawcall's providers." << std::endl;
				throw std::logic_error(stream.str());
            }

			template <typename T>
            T
            getDataProperty(const std::string& propertyName, T defaultValue)
            {
				if (dataHasProperty(propertyName))
					return _data->get<T>(propertyName);

				return defaultValue;
            }

			ContainerPtr
			getDataContainer(const std::string& propertyName) const;

            bool
            dataHasProperty(const std::string& propertyName);

            void
            watchProperty(const std::string& propertyName);

            void
            propertyChangedHandler(ContainerPtr        data,
                                   const std::string&  propertyName);
		};		
	}
}
