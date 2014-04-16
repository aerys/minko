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

#include "minko/Signal.hpp"
#include "minko/render/Blending.hpp"
#include "minko/data/Container.hpp"
#include "minko/render/Pass.hpp"
#include "minko/render/ProgramInputs.hpp"
#include "minko/render/States.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Priority.hpp"
#include "minko/scene/Layout.hpp"

namespace minko
{
	namespace render
	{
		class DrawCallZSorter;

		class DrawCall :
            public std::enable_shared_from_this<DrawCall>
		{
		public:
			typedef std::shared_ptr<DrawCall>								Ptr;

		private:
			enum class ContainerId{ FULL = 0, LOCAL };

			typedef std::shared_ptr<scene::Node>							NodePtr;
            typedef std::shared_ptr<AbstractContext>						AbsCtxPtr;
			typedef std::shared_ptr<AbstractTexture>						AbsTexturePtr;
			typedef std::shared_ptr<data::Provider>							ProviderPtr;
            typedef std::shared_ptr<data::Container>						ContainerPtr;
			typedef std::shared_ptr<Program>								ProgramPtr;

			typedef data::Container::PropertyChangedSignal::Slot			PropertyChangedSlot;

			typedef std::tuple<int, int>									Int2;
			typedef std::tuple<int, int, int>								Int3;
			typedef std::tuple<int, int, int, int>							Int4;					

		private:
			static const unsigned int										MAX_NUM_TEXTURES;
			static const unsigned int										MAX_NUM_VERTEXBUFFERS;

            static SamplerState												_defaultSamplerState;

			std::shared_ptr<render::Pass>									_pass;

			std::shared_ptr<Program>										_program;
			std::shared_ptr<data::Container>								_targetData;
			std::shared_ptr<data::Container>								_rendererData;
            std::shared_ptr<data::Container>								_rootData;

			std::list<Signal<ContainerPtr, ProviderPtr>::Slot>				_distantProviderRemovedSlots;

			FormatNameFunction												_formatFunction;

            std::vector<int>												_vertexBufferIds;
            std::vector<int>												_vertexBufferLocations;
            std::vector<int>												_vertexSizes;
            std::vector<int>												_vertexAttributeSizes;
            std::vector<int>												_vertexAttributeOffsets;
            std::vector<int>												_textureIds;
            std::vector<int>												_textureLocations;
            std::vector<WrapMode>											_textureWrapMode;
            std::vector<TextureFilter>										_textureFilters;
            std::vector<MipFilter>											_textureMipFilters;
			std::vector<TextureType>										_textureTypes;
            uint															_numIndices;
            uint															_indexBuffer;
            AbsTexturePtr													_target;
            render::Blending::Mode											_blendMode;
			bool															_colorMask;
            bool															_depthMask;
            render::CompareMode												_depthFunc;
            render::TriangleCulling											_triangleCulling;
			render::CompareMode												_stencilFunc;
			int																_stencilRef;
			uint															_stencilMask;
			render::StencilOperation										_stencilFailOp;
			render::StencilOperation										_stencilZFailOp;
			render::StencilOperation										_stencilZPassOp;
			bool															_scissorTest;
			render::ScissorBox												_scissorBox;
			Layouts															_layouts;
			float															_priority;
			bool															_zsorted;
            std::unordered_map<uint, float>									_uniformFloat;
            std::unordered_map<uint, std::shared_ptr<math::Vector2>>		_uniformFloat2;
            std::unordered_map<uint, std::shared_ptr<math::Vector3>>		_uniformFloat3;
            std::unordered_map<uint, std::shared_ptr<math::Vector4>>		_uniformFloat4;
            std::unordered_map<uint, const float*>							_uniformFloat16;
			std::unordered_map<uint, int>									_uniformInt;
			std::unordered_map<uint, Int2>									_uniformInt2;
			std::unordered_map<uint, Int3>									_uniformInt3;
			std::unordered_map<uint, Int4>									_uniformInt4;
			std::unordered_map<uint, data::UniformArrayPtr<float>>			_uniformFloats;
			std::unordered_map<uint, data::UniformArrayPtr<float>>			_uniformFloats2;
			std::unordered_map<uint, data::UniformArrayPtr<float>>			_uniformFloats3;
			std::unordered_map<uint, data::UniformArrayPtr<float>>			_uniformFloats4;
			std::unordered_map<uint, data::UniformArrayPtr<float>>			_uniformFloats16;
			std::unordered_map<uint, data::UniformArrayPtr<int>>			_uniformInts;
			std::unordered_map<uint, data::UniformArrayPtr<int>>			_uniformInts2;
			std::unordered_map<uint, data::UniformArrayPtr<int>>			_uniformInts3;
			std::unordered_map<uint, data::UniformArrayPtr<int>>			_uniformInts4;

			std::unordered_map<std::string, std::list<Any>>					_referenceChangedSlots;		// Any = PropertyChangedSlot
			std::list<PropertyChangedSlot>									_macroAddedOrRemovedSlots;
			std::unordered_map<data::ContainerAndName, PropertyChangedSlot>	_macroChangedSlots;			// Any = PropertyChangedSlot
			Signal<std::shared_ptr<IndexBuffer>>::Slot						_indicesChangedSlot;
			Signal<ContainerPtr, const std::string&>::Slot					_layoutsPropertyChangedSlot;

			Signal<Ptr>::Ptr												_zsortNeeded;
			Signal<Ptr, ContainerPtr, const std::string&>::Ptr				_macroChanged;	

			std::unordered_map<uint, std::set<std::string>>					_containerMacroPNames;
			std::unordered_map<uint, std::list<std::regex>>					_containerMacroRegex;

			std::shared_ptr<DrawCallZSorter>								_zSorter;

		public:
			static inline
			Ptr
			create(std::shared_ptr<Pass> pass)
			{
				Ptr ptr = std::shared_ptr<DrawCall>(new DrawCall(pass));

				ptr->initialize();

				return ptr;
			}

			inline
			std::shared_ptr<Pass>
			pass() const
			{
				return _pass;
			}

			inline
			ContainerPtr
			targetData() const
			{
				return _targetData;
			}

			inline
			ContainerPtr
			rendererData() const
			{
				return _rendererData;
			}

			inline
			ContainerPtr
			rootData() const
			{
				return _rootData;
			}

			inline
			Ptr
			formatNameFunction(FormatNameFunction func)
			{
				_formatFunction = func;

				return shared_from_this();
			}

			inline
			std::string
			formatPropertyName(const std::string& rawPropertyName)
			{
				return _formatFunction ? _formatFunction(rawPropertyName) : rawPropertyName;
			}

            inline
            AbsTexturePtr
            target() const
            {
                return _target;
            }

			inline
			float
			priority() const
			{
				return _priority;
			}

			inline
			Layouts
			layouts() const
			{
				return _layouts;
			}

			inline
			bool
			zSorted() const
			{
				return _zsorted && Priority::LAST < _priority && !( _priority > Priority::TRANSPARENT);
			}
			

            void
            configure(ProgramPtr,
					  FormatNameFunction,
					  ContainerPtr	fullTargetData, // original containers
					  ContainerPtr	fullRendererData,
                      ContainerPtr	fullRootData,
                      ContainerPtr	targetData, // filtered containers
					  ContainerPtr	rendererData,
                      ContainerPtr	rootData);

			void
			unbind();

			void
			render(const std::shared_ptr<AbstractContext>& context, AbsTexturePtr renderTarget);

			void
			initialize(ContainerPtr				                    data,
					   const std::map<std::string, std::string>&	inputNameToBindingName);

			std::shared_ptr<math::Vector3>
			getEyeSpacePosition(std::shared_ptr<math::Vector3> output = nullptr);

			inline
			Signal<Ptr>::Ptr
			zsortNeeded() const
			{
				return _zsortNeeded;
			}

			inline
			Signal<Ptr, ContainerPtr, const std::string&>::Ptr
			macroChanged() const
			{
				return _macroChanged;
			}

		private:
			explicit
			DrawCall(std::shared_ptr<Pass>);

			void
			initialize();

			void
			initializePassMacroRegex();

			void
			reset();

			void
            bind(ContainerPtr targetData, // filtered containers
				 ContainerPtr rendererData, 
				 ContainerPtr rootData);

			void
			trackMacros(ContainerPtr fullTargetData, // complete containers
						ContainerPtr fullRendererData, 
						ContainerPtr fullRootData);

			void
			bindProgramDefaultUniforms();

			void
			bindProgramInputs();

			void
			bindIndexBuffer();

			void
			bindTargetLayouts();

			void
			bindStates();
			
			uint
			bindVertexAttribute(const std::string& propertyName, int location, uint vertexBufferIndex, bool = true);
			
			uint
			bindTextureSampler(const std::string& propertyName, int location, uint textureIndex, const SamplerState&, bool = true);

			void
			bindUniform(const std::string& propertyName, ProgramInputs::Type, int location);

			void
			bindUniformArray(const std::string&	propertyName, ContainerPtr, ProgramInputs::Type, int location);

			void
			bindFloatUniformArray(const std::string& propertyName, ContainerPtr, ProgramInputs::Type, int location);

			void
			bindIntegerUniformArray(const std::string& propertyName, ContainerPtr, ProgramInputs::Type, int location);

			void
			watchUniformRefChange(ContainerPtr, const std::string& propertyName, ProgramInputs::Type, int location);

			void
			targetLayoutsChangedHandler(NodePtr, NodePtr);

			ContainerPtr
			getDataContainer(const data::BindingSource& source) const;

			template <typename T>
			void
			bindState(const std::string& stateName, T defaultValue, T& stateValue)
			{
				const auto&				stateBindings	= _pass->stateBindings();
				data::Container::Ptr	container		= nullptr;
				std::string				propertyName	= "";
					
				if (stateBindings.count(stateName) > 0)
				{
					const auto&	binding	= stateBindings.at(stateName);
					
					propertyName		= formatPropertyName(std::get<0>(binding));
					container			= getDataContainer(std::get<1>(binding));
				}


				if (container)
				{
					stateValue = container->hasProperty(propertyName)
						? container->get<T>(propertyName)
						: defaultValue;
				
					if (_referenceChangedSlots.count(propertyName) == 0)
					{
#if defined(EMSCRIPTEN)
			// See issue #1848 in Emscripten: https://github.com/kripken/emscripten/issues/1848
						auto that = shared_from_this();

						_referenceChangedSlots[propertyName].push_back(container->propertyReferenceChanged(propertyName)->connect([&, that, defaultValue](data::Container::Ptr, const std::string&) {
							that->bindState<T>(stateName, defaultValue, stateValue);
						}));
#else
						_referenceChangedSlots[propertyName].push_back(container->propertyReferenceChanged(propertyName)->connect(std::bind(
							&DrawCall::bindState<T>,
							shared_from_this(),
							stateName, 
							defaultValue, 
							stateValue
						)));
#endif
					}
				}
				else
					stateValue = defaultValue;

				uploadIfTexture<T>(stateValue);
			}

			
			template <typename T>
			typename std::enable_if<std::is_convertible< T, std::shared_ptr<render::AbstractTexture> >::value, T>::type
			uploadIfTexture(T value)
			{
				render::AbstractTexture::Ptr texture = value;

				if (texture && !texture->isReady())
					texture->upload();

				return value;
			}
			template <typename T>
			typename std::enable_if<!std::is_convertible< T, std::shared_ptr<render::AbstractTexture> >::value, T>::type
			uploadIfTexture(T value)
			{
				// actually does nothing
				return value;
			}

			bool
			isTrackedMacro(const std::string&, ContainerId) const;

			void
			macroAddedHandler(ContainerPtr, ContainerId, const std::string&);

			void
			macroChangedHandler(ContainerPtr, const std::string&);

			void
			macroRemovedHandler(ContainerPtr, ContainerId, const std::string&);

			void
			distantProviderRemovedHandler(data::BindingSource, ProviderPtr);
		};		
	}
}
