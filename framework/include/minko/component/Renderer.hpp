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
#include "minko/component/AbstractComponent.hpp"

namespace minko
{
	namespace component
	{
		class Renderer :
			public AbstractComponent,
			public std::enable_shared_from_this<Renderer>
		{
		public:
			typedef std::shared_ptr<Renderer>							Ptr;

		private:
			typedef std::shared_ptr<scene::Node>						NodePtr;
			typedef std::shared_ptr<AbstractComponent>					AbsCtrlPtr;
			typedef std::shared_ptr<Surface>							SurfacePtr;
			typedef std::shared_ptr<render::DrawCall>					DrawCallPtr;
			typedef std::list<DrawCallPtr>								DrawCallList;
			typedef std::shared_ptr<SceneManager>						SceneManagerPtr;
			typedef std::shared_ptr<render::Texture>					TexturePtr;
			typedef Signal<SurfacePtr, const std::string&, bool>::Slot	SurfaceTechniqueChangedSlot;
			typedef std::shared_ptr<render::DrawCallPool>			DrawCallFactoryPtr;

		private:
			DrawCallList												_drawCalls;
			std::unordered_map<SurfacePtr, DrawCallList>				_surfaceDrawCalls; 

			unsigned int												_backgroundColor;
			std::shared_ptr<SceneManager>								_sceneManager;
			Signal<Ptr>::Ptr											_renderingBegin;
			Signal<Ptr>::Ptr											_renderingEnd;
			Signal<Ptr>::Ptr											_beforePresent;
			std::shared_ptr<render::Texture>							_renderTarget;

			std::set<std::shared_ptr<Surface>>							_toCollect;
			std::shared_ptr<render::Effect>								_effect;
			float														_priority;

			Signal<AbsCtrlPtr, NodePtr>::Slot							_targetAddedSlot;
			Signal<AbsCtrlPtr, NodePtr>::Slot							_targetRemovedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot						_addedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot						_removedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot						_rootDescendantAddedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot						_rootDescendantRemovedSlot;
			Signal<NodePtr, NodePtr, AbsCtrlPtr>::Slot					_componentAddedSlot;
			Signal<NodePtr, NodePtr, AbsCtrlPtr>::Slot					_componentRemovedSlot;
			Signal<SceneManagerPtr, uint, TexturePtr>::Slot				_renderingBeginSlot;
			std::unordered_map<SurfacePtr, SurfaceTechniqueChangedSlot>	_surfaceTechniqueChangedSlot;

			DrawCallFactoryPtr											_drawCallPool;

			static const unsigned int									NUM_FALLBACK_ATTEMPTS;

		public:
			inline static
			Ptr
			create()
			{
				auto ctrl = std::shared_ptr<Renderer>(new Renderer());

				ctrl->initialize();
				return ctrl;
			}

			inline static
			Ptr
			create(uint									backgroundColor, 
				   std::shared_ptr<render::Texture>		renderTarget	= nullptr, 
				   std::shared_ptr<render::Effect>		effect			= nullptr,
				   float								priority		= 0.f)
			{
				auto ctrl = std::shared_ptr<Renderer>(new Renderer(renderTarget, effect, priority));

				ctrl->initialize();
				ctrl->backgroundColor(backgroundColor);

				return ctrl;
			}

			~Renderer()
			{
			}

			inline
			std::shared_ptr<render::Effect>
			effect()
			{
				return _effect;
			}

			inline
			unsigned int
			numDrawCalls()
			{
				return _drawCalls.size();
			}

			inline
			unsigned int
			backgroundColor()
			{
				return _backgroundColor;
			}

			inline
			void
			backgroundColor(const unsigned int backgroundColor)
			{
				_backgroundColor = backgroundColor;
			}

			inline
			std::shared_ptr<render::Texture>
			target()
			{
				return _renderTarget;
			}

			inline
			void
			target(std::shared_ptr<render::Texture> target)
			{
				_renderTarget = target;
			}

			void
			render(std::shared_ptr<render::AbstractContext> context,
				   std::shared_ptr<render::Texture> 		renderTarget = nullptr);

			inline
			Signal<Ptr>::Ptr
			renderingBegin()
			{
				return _renderingBegin;
			}

			inline
			Signal<Ptr>::Ptr
			beforePresent()
			{
				return _beforePresent;
			}

			inline
			Signal<Ptr>::Ptr
			renderingEnd()
			{
				return _renderingEnd;
			}

		private:
			Renderer(std::shared_ptr<render::Texture>	renderTarget	= nullptr, 
					 std::shared_ptr<render::Effect>	effect			= nullptr,
					 float								priority		= 0.f);

			void
			initialize();

			void
			targetAddedHandler(AbsCtrlPtr ctrl, NodePtr target);

			void
			targetRemovedHandler(AbsCtrlPtr ctrl, NodePtr target);

			void
			addedHandler(NodePtr node, NodePtr target, NodePtr parent);

			void
			removedHandler(NodePtr node, NodePtr target, NodePtr parent);

			void
			rootDescendantAddedHandler(NodePtr node, NodePtr target, NodePtr parent);

			void
			rootDescendantRemovedHandler(NodePtr node, NodePtr target, NodePtr parent);

			void
			componentAddedHandler(NodePtr node, NodePtr target, AbsCtrlPtr	ctrl);

			void
			componentRemovedHandler(NodePtr node, NodePtr target, AbsCtrlPtr ctrl);

			void
			addSurface(SurfacePtr);

			void
			removeSurface(SurfacePtr);

			void
			geometryChanged(SurfacePtr ctrl);

			void
			materialChanged(SurfacePtr ctrl);

			void
			sceneManagerRenderingBeginHandler(std::shared_ptr<SceneManager>		sceneManager,
											  uint								frameId,
											  std::shared_ptr<render::Texture>	renderTarget);

			void
			findSceneManager();

			void
			setSceneManager(std::shared_ptr<SceneManager> sceneManager);
		};
	}
}
