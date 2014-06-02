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
#include "minko/component/AbstractComponent.hpp"
#include "minko/FXCommon.hpp"

namespace minko
{
	namespace component
	{
		class Hologram :
			public AbstractComponent
		{
		public:
			typedef std::shared_ptr<Hologram>	Ptr;

		private:
			typedef std::shared_ptr<scene::Node>		        NodePtr;
			typedef std::shared_ptr<AbstractComponent>	        AbsCmpPtr;
			typedef std::shared_ptr<data::StructureProvider>	ContainerPtr;
			typedef std::shared_ptr<Renderer>					RendererPtr;

		private:
			Signal<AbsCmpPtr, NodePtr>::Ptr											_rootAdded;

			Signal<AbsCmpPtr, NodePtr>::Slot										_targetAddedSlot;
			Signal<AbsCmpPtr, NodePtr>::Slot										_targetRemovedSlot;
			Signal<AbsCmpPtr, NodePtr>::Slot										_rootAddedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot									_addedToSceneSlot;
			Signal<std::shared_ptr<component::SceneManager>, float, float>::Slot    _frameBeginSlot;
			
			std::shared_ptr<render::Effect>											_frontEffect;
			std::shared_ptr<render::Effect>											_backEffect;
			std::shared_ptr<render::Effect>											_depthEffect;
			std::shared_ptr<render::AbstractContext>								_context;

			static RendererPtr _frontFaceNormalRenderer;
			static RendererPtr _backFaceNormalRenderer;
			static RendererPtr _depthMapRenderer;

			static std::shared_ptr<render::Texture> _frontFaceNormalRenderTarget;
			static std::shared_ptr<render::Texture> _backFaceNormalRenderTarget;
			static std::shared_ptr<render::Texture>	_depthMapRenderTarget;

			static const uint MAP_RESOLUTION;

		public:
			
			inline static
			Ptr
			create(std::shared_ptr<render::Effect>			frontEffect, 
				   std::shared_ptr<render::Effect>			backEffect, 
				   std::shared_ptr<render::Effect>			depthEffect,
				   std::shared_ptr<render::AbstractContext> context)
			{
				Ptr hologram = std::shared_ptr<Hologram>(new Hologram(frontEffect, backEffect, depthEffect, context));

				hologram->initialize();

				return hologram;
			}

			inline
			Signal<AbsCmpPtr, std::shared_ptr<scene::Node>>::Ptr
			rootAdded() const
			{
				return _rootAdded;
			}

		private:

			Hologram(std::shared_ptr<render::Effect>			frontEffect, 
					 std::shared_ptr<render::Effect>			backEffect,
					 std::shared_ptr<render::Effect>			depthEffect, 
					 std::shared_ptr<render::AbstractContext>	context);

			void
			initialize();

			void
			targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target);

			void
			targetAddedToScene(NodePtr node, NodePtr target, NodePtr ancestor);

			void
			initTarget(AbstractComponent::Ptr cmp, NodePtr target, NodePtr ancestor);
		};
	}
}
