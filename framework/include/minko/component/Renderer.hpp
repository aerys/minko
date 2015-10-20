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
#include "minko/component/AbstractComponent.hpp"
#include "minko/scene/Layout.hpp"
#include "minko/render/DrawCallPool.hpp"
#include "minko/Flyweight.hpp"

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
			typedef std::shared_ptr<Renderer>   Ptr;

		private:
			typedef std::shared_ptr<scene::Node>						                    NodePtr;
			typedef std::shared_ptr<AbstractComponent>					                    AbsCmpPtr;
			typedef std::shared_ptr<render::AbstractContext>			                    AbsContext;
			typedef std::shared_ptr<Surface>							                    SurfacePtr;
			typedef std::shared_ptr<render::DrawCall>					                    DrawCallPtr;
			typedef std::shared_ptr<SceneManager>						                    SceneManagerPtr;
			typedef std::shared_ptr<render::AbstractTexture>			                    AbsTexturePtr;
			typedef std::shared_ptr<render::Effect>						                    EffectPtr;
			typedef std::shared_ptr<data::AbstractFilter>				                    AbsFilterPtr;
			typedef Signal<SurfacePtr, const std::string&, bool>::Slot	                    SurfaceTechniqueChangedSlot;
			typedef Signal<AbsFilterPtr, SurfacePtr>::Slot				                    FilterChangedSlot;
            typedef data::Store                                                 	        Store;
            typedef std::shared_ptr<data::Collection>                                       CollectionPtr;
            typedef std::shared_ptr<data::Provider>                                         ProviderPtr;
			typedef Signal<Ptr, AbsFilterPtr, data::Binding::Source, SurfacePtr>	        RendererFilterChangedSignal;
            typedef Signal<SurfacePtr>                                                      SurfaceChangedSignal;
            typedef render::DrawCallPool::DrawCallIteratorPair                              DrawCallIteratorPair;
            typedef std::unordered_map<SurfacePtr, std::list<SurfaceChangedSignal::Slot>>   SurfaceSlotMap;
            typedef Signal<render::DrawCall*>                                               ZSortNeeded;
		private:
			std::string											                    _name;

			unsigned int										                    _backgroundColor;
            math::ivec4      									                    _viewportBox;
			math::ivec4      									                    _scissorBox;
			std::shared_ptr<SceneManager>						                    _sceneManager;
			Signal<Ptr>::Ptr									                    _renderingBegin;
			Signal<Ptr>::Ptr									                    _renderingEnd;
			Signal<Ptr>::Ptr									                    _beforePresent;
			AbsTexturePtr										                    _renderTarget;
            bool                                                                    _clearBeforeRender;
            render::EffectVariables		        				                    _variables;

			std::set<std::shared_ptr<Surface>>					                    _toCollect;
			EffectPtr											                    _effect;
			std::string											                    _effectTechnique;
			float												                    _priority;
			bool												                    _enabled;
			std::shared_ptr<geometry::Geometry>					                    _postProcessingGeom;
			bool                                                                    _mustZSort;

			std::unordered_map<const render::DrawCall*, ZSortNeeded::Slot>          _drawCallToZSortNeededSlot;

			Signal<AbsCmpPtr, NodePtr>::Slot					                    _targetAddedSlot;
			Signal<AbsCmpPtr, NodePtr>::Slot					                    _targetRemovedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot				                    _addedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot				                    _removedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot				                    _rootDescendantAddedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot				                    _rootDescendantRemovedSlot;
			Signal<NodePtr, NodePtr, AbsCmpPtr>::Slot			                    _componentAddedSlot;
			Signal<NodePtr, NodePtr, AbsCmpPtr>::Slot			                    _componentRemovedSlot;
			Signal<SceneManagerPtr, uint, AbsTexturePtr>::Slot	                    _renderingBeginSlot;
            SurfaceSlotMap                                                          _surfaceChangedSlots;
            Signal<Store&, ProviderPtr, const data::Provider::PropertyName&>::Slot  _worldToScreenMatrixPropertyChangedSlot;

			render::DrawCallPool								                    _drawCallPool;
            std::unordered_map<SurfacePtr, uint>                                    _surfaceToDrawCallIterator;

			/*std::set<AbsFilterPtr>										        _targetDataFilters;
			std::set<AbsFilterPtr>										            _rendererDataFilters;
			std::set<AbsFilterPtr>										            _rootDataFilters;
			std::shared_ptr<data::LightMaskFilter>						            _lightMaskFilter;*/

			/*std::unordered_map<AbsFilterPtr, FilterChangedSlot>			        _targetDataFilterChangedSlots;
			std::unordered_map<AbsFilterPtr, FilterChangedSlot>			            _rendererDataFilterChangedSlots;
			std::unordered_map<AbsFilterPtr, FilterChangedSlot>			            _rootDataFilterChangedSlots;*/

			std::shared_ptr<RendererFilterChangedSignal>				            _filterChanged;
			std::unordered_map<NodePtr, Signal<NodePtr, NodePtr>::Slot>		        _nodeLayoutChangedSlot;
            std::unordered_map<SurfacePtr, Signal<AbsCmpPtr>::Slot>			        _surfaceLayoutMaskChangedSlot;

			uint																	_numDrawCalls;
			uint																	_numTriangles;

		public:
			inline static
			Ptr
			create()
			{
                return std::shared_ptr<Renderer>(new Renderer());
			}

			inline static
			Ptr
			create(uint					backgroundColor,
				   AbsTexturePtr		renderTarget 	= nullptr,
				   EffectPtr			effect			= nullptr,
				   const std::string&	effectTechnique	= "default",
				   float				priority		= 0.f,
				   std::string			name			= "")
			{
				auto ctrl = std::shared_ptr<Renderer>(new Renderer(renderTarget, effect, effectTechnique, priority));

				ctrl->backgroundColor(backgroundColor);
				ctrl->name(name);

				return ctrl;
			}

            // TODO #Clone
            /*
			AbstractComponent::Ptr
			clone(const CloneOption& option);
			*/

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
				return _numDrawCalls;
			}

			inline
			unsigned int
			numTriangles()
			{
				return _numTriangles;
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
			void
			name(const std::string name)
			{
				_name = name;
			}

			inline
			std::string
			name()
			{
				return _name;
			}

			float
			priority()
			{
				return _priority;
			}

			inline
			void
			priority(const float priority)
			{
				_priority = priority;
			}

            inline
			void viewport(const math::ivec4& viewport)
            {
				_viewportBox = viewport;
            }

			inline
			void scissorBox(const int x, const int y, const int w, const int h)
			{
				_scissorBox.x = x;
				_scissorBox.y = y;
				_scissorBox.z = w;
				_scissorBox.w = h;
			}

			inline
			AbsTexturePtr
			renderTarget()
			{
				return _renderTarget;
			}

			inline
			void
            renderTarget(AbsTexturePtr target)
			{
				_renderTarget = target;
			}

			inline
			bool
            clearBeforeRender()
            {
                return _clearBeforeRender;
            }

			inline
			render::EffectVariables&
			effectVariables()
			{
				return _variables;
			}

			inline
			const std::string&
			effectTechnique() const
			{
				return _effectTechnique;
			}

			inline
			void
			effectTechnique(std::string value)
			{
				_effectTechnique = value;
			}

            inline
            void
            clearBeforeRender(bool value)
            {
                _clearBeforeRender = value;
            }

            inline
            bool
			enabled()
			{
				return _enabled;
			}

			inline
			void
			enabled(bool value)
			{
				_enabled = value;
			}

			inline
			const render::DrawCallPool&
			drawCallPool()
			{
				return _drawCallPool;
			}

			void
			render(std::shared_ptr<render::AbstractContext> context,
				   AbsTexturePtr 							renderTarget = nullptr);

			void
			clear(std::shared_ptr<AbstractCanvas> canvas);

			void
			clear(std::shared_ptr<AbstractCanvas> canvas, const math::vec4& clearColor);

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
			addFilter(AbsFilterPtr, data::Binding::Source);

			Ptr
			removeFilter(AbsFilterPtr, data::Binding::Source);

			inline
			const scene::Layout&
			layoutMask() const
			{
				return AbstractComponent::layoutMask();
			}

			void
            layoutMask(scene::Layout value);

			/*const std::set<AbsFilterPtr>&
			filters(data::BindingSource source) const
			{
				return
				source == data::BindingSource::TARGET
				? _targetDataFilters
				: source == data::BindingSource::RENDERER
					? _rendererDataFilters
					: _rootDataFilters;
			}*/

			/*Ptr
			setFilterSurface(SurfacePtr);*/

			/*inline
			std::shared_ptr<RendererFilterChangedSignal>
			filterChanged() const
			{
				return _filterChanged;
			}*/

        protected:
			Renderer(const Renderer& renderer, const CloneOption& option);

			void
			targetAdded(NodePtr target);

			void
			targetRemoved(NodePtr target);

		private:
			Renderer(AbsTexturePtr		renderTarget 	= nullptr,
					 EffectPtr			effect			= nullptr,
					 const std::string& effectTechnique	= "default",
					 float				priority		= 0.f);

            void
            reset();

			void
			initializePostProcessingGeometry();

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
            surfaceGeometryOrMaterialChangedHandler(SurfacePtr surface);

            void
            surfaceEffectChangedHandler(SurfacePtr surface);

			/*void
			geometryChanged(SurfacePtr ctrl);

			void
			materialChanged(SurfacePtr ctrl);*/

			void
			sceneManagerRenderingBeginHandler(std::shared_ptr<SceneManager>	sceneManager,
											  uint							frameId,
											  AbsTexturePtr					renderTarget);

			void
			findSceneManager();

			void
			setSceneManager(std::shared_ptr<SceneManager> sceneManager);

			/*inline
			std::set<AbsFilterPtr>&
			filtersRef(data::BindingSource source)
			{
				return source == data::BindingSource::TARGET
				    ? _targetDataFilters
				    : source == data::BindingSource::RENDERER
					    ? _rendererDataFilters
					    : _rootDataFilters;
			}*/

			/*inline
			std::unordered_map<AbsFilterPtr, FilterChangedSlot>&
			filterChangedSlotsRef(data::BindingSource source)
			{
				return source == data::BindingSource::TARGET
				    ? _targetDataFilterChangedSlots
				    : source == data::BindingSource::RENDERER
					    ? _rendererDataFilterChangedSlots
					    : _rootDataFilterChangedSlots;
			}*/

			void
			filterChangedHandler(AbsFilterPtr, data::Binding::Source, SurfacePtr);

			void
			surfaceLayoutMaskChangedHandler(SurfacePtr surface);

			void
			watchSurface(SurfacePtr surface);

			void
			unwatchSurface(SurfacePtr surface, NodePtr node);

			bool
			checkSurfaceLayout(SurfacePtr surface);

            void
            enableDrawCalls(SurfacePtr surface, bool enabled);
		};
	}
}
