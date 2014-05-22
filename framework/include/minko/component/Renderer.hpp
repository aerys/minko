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
#include "minko/scene/Layout.hpp"

namespace minko
{
	namespace data
	{
		class LightMaskFilter;
	}

	namespace component
	{
		class Renderer :
			public AbstractComponent
		{
		public:
			typedef std::shared_ptr<Renderer>							Ptr;

		private:
			typedef std::shared_ptr<scene::Node>						NodePtr;
			typedef std::shared_ptr<AbstractComponent>					AbsCmpPtr;
			typedef std::shared_ptr<render::AbstractContext>			AbsContextPtr;
			typedef std::shared_ptr<Surface>							SurfacePtr;
			typedef std::shared_ptr<render::DrawCall>					DrawCallPtr;
			typedef std::list<DrawCallPtr>								DrawCallList;
			typedef std::shared_ptr<SceneManager>						SceneManagerPtr;
			typedef std::shared_ptr<render::AbstractTexture>			AbsTexturePtr;
			typedef std::shared_ptr<render::Effect>						EffectPtr;
			typedef std::shared_ptr<render::DrawCallPool>				DrawCallFactoryPtr;
			typedef std::shared_ptr<data::AbstractFilter>				AbsFilterPtr;
			typedef Signal<SurfacePtr, const std::string&, bool>::Slot	SurfaceTechniqueChangedSlot;
			typedef Signal<AbsFilterPtr, SurfacePtr>::Slot				FilterChangedSlot;
			typedef Signal<Ptr, AbsFilterPtr, data::BindingSource, SurfacePtr>		RendererFilterChangedSignal;

		private:
			DrawCallList												_drawCalls;
			std::unordered_map<SurfacePtr, DrawCallList>				_surfaceDrawCalls; 

			unsigned int												_backgroundColor;
            render::ScissorBox											_viewportBox;
			render::ScissorBox											_scissorBox;
			std::shared_ptr<SceneManager>								_sceneManager;
			Signal<Ptr>::Ptr											_renderingBegin;
			Signal<Ptr>::Ptr											_renderingEnd;
			Signal<Ptr>::Ptr											_beforePresent;
			AbsTexturePtr												_renderTarget;

			std::set<std::shared_ptr<Surface>>							_toCollect;
			EffectPtr													_effect;
			float														_priority;

			Signal<AbsCmpPtr, NodePtr>::Slot							_targetAddedSlot;
			Signal<AbsCmpPtr, NodePtr>::Slot							_targetRemovedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot						_addedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot						_removedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot						_rootDescendantAddedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot						_rootDescendantRemovedSlot;
			Signal<NodePtr, NodePtr, AbsCmpPtr>::Slot					_componentAddedSlot;
			Signal<NodePtr, NodePtr, AbsCmpPtr>::Slot					_componentRemovedSlot;
			Signal<SceneManagerPtr, uint, AbsTexturePtr>::Slot			_renderingBeginSlot;
			std::unordered_map<SurfacePtr, SurfaceTechniqueChangedSlot>	_surfaceTechniqueChangedSlot;

			DrawCallFactoryPtr											_drawCallPool;

			std::set<AbsFilterPtr>										_targetDataFilters;
			std::set<AbsFilterPtr>										_rendererDataFilters;
			std::set<AbsFilterPtr>										_rootDataFilters;
			std::shared_ptr<data::LightMaskFilter>						_lightMaskFilter;

			std::unordered_map<AbsFilterPtr, FilterChangedSlot>			_targetDataFilterChangedSlots;
			std::unordered_map<AbsFilterPtr, FilterChangedSlot>			_rendererDataFilterChangedSlots;
			std::unordered_map<AbsFilterPtr, FilterChangedSlot>			_rootDataFilterChangedSlots;

			std::shared_ptr<RendererFilterChangedSignal>				_filterChanged;
			bool														_enable;

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
			create(uint					backgroundColor, 
				   AbsTexturePtr		renderTarget = nullptr,
				   EffectPtr			effect			= nullptr,
				   float				priority		= 0.f)
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
			EffectPtr
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
			void viewport(const int x, const int y, const int w, const int h)
            {
				_viewportBox.x			= x;
				_viewportBox.y			= y;
				_viewportBox.width		= w;
				_viewportBox.height		= h;
            }

			inline
			void scissor(const int x, const int y, const int w, const int h)
			{
				_scissorBox.x		= x;
				_scissorBox.y		= y;
				_scissorBox.width	= w;
				_scissorBox.height	= h;
			}

			inline
			AbsTexturePtr
			target()
			{
				return _renderTarget;
			}

			inline
			void
			target(AbsTexturePtr target)
			{
				_renderTarget = target;
			}

			inline
			bool
			enable()
			{
				return _enable;
			}

			inline
			void
			enable(bool value)
			{
				_enable = value;
			}

			void
			render(std::shared_ptr<render::AbstractContext> context,
				   AbsTexturePtr 		renderTarget = nullptr);

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

			Ptr
			addFilter(AbsFilterPtr, data::BindingSource);

			Ptr
			removeFilter(AbsFilterPtr, data::BindingSource);

			const std::set<AbsFilterPtr>&
			filters(data::BindingSource source) const
			{
				return 
				source == data::BindingSource::TARGET
				? _targetDataFilters
				: source == data::BindingSource::RENDERER
					? _rendererDataFilters
					: _rootDataFilters;
			}

			Ptr
			setFilterSurface(SurfacePtr);

			inline
			std::shared_ptr<RendererFilterChangedSignal>
			filterChanged() const
			{
				return _filterChanged;
			}

		private:
			Renderer(AbsTexturePtr		renderTarget = nullptr,
					 EffectPtr			effect			= nullptr,
					 float				priority		= 0.f);

			void
			initialize();

			void
			targetAddedHandler(AbsCmpPtr ctrl, NodePtr target);

			void
			targetRemovedHandler(AbsCmpPtr ctrl, NodePtr target);

			void
			addedHandler(NodePtr node, NodePtr target, NodePtr parent);

			void
			removedHandler(NodePtr node, NodePtr target, NodePtr parent);

			void
			rootDescendantAddedHandler(NodePtr node, NodePtr target, NodePtr parent);

			void
			rootDescendantRemovedHandler(NodePtr node, NodePtr target, NodePtr parent);

			void
			componentAddedHandler(NodePtr node, NodePtr target, AbsCmpPtr	ctrl);

			void
			componentRemovedHandler(NodePtr node, NodePtr target, AbsCmpPtr ctrl);

			void
			addSurface(SurfacePtr);

			void
			removeSurface(SurfacePtr);

			void
			geometryChanged(SurfacePtr ctrl);

			void
			materialChanged(SurfacePtr ctrl);

			void
			sceneManagerRenderingBeginHandler(std::shared_ptr<SceneManager>	sceneManager,
											  uint							frameId,
											  AbsTexturePtr					renderTarget);

			void
			findSceneManager();

			void
			setSceneManager(std::shared_ptr<SceneManager> sceneManager);

			inline
			std::set<AbsFilterPtr>&
			filtersRef(data::BindingSource source)
			{
				return source == data::BindingSource::TARGET
				? _targetDataFilters
				: source == data::BindingSource::RENDERER
					? _rendererDataFilters
					: _rootDataFilters;
			}

			inline
			std::unordered_map<AbsFilterPtr, FilterChangedSlot>&
			filterChangedSlotsRef(data::BindingSource source)
			{
				return source == data::BindingSource::TARGET
				? _targetDataFilterChangedSlots
				: source == data::BindingSource::RENDERER
					? _rendererDataFilterChangedSlots
					: _rootDataFilterChangedSlots;
			}

			void
			filterChangedHandler(AbsFilterPtr, data::BindingSource, SurfacePtr);
		};
	}
}
