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
#include "minko/component/AbstractPicking.hpp"
#include "minko/data/Provider.hpp"

namespace minko
{
	namespace component
	{
		class Picking :
			public AbstractPicking
		{
		public:
			typedef std::shared_ptr<Picking>					Ptr;
			typedef std::shared_ptr<Renderer>					RendererPtr;
			typedef std::shared_ptr<AbstractComponent>			AbsCtrlPtr;
			typedef std::shared_ptr<scene::Node>				NodePtr;
            typedef std::shared_ptr<render::Effect>			    EffectPtr;
            typedef std::shared_ptr<render::Texture>			TexturePtr;
			typedef std::shared_ptr<render::AbstractContext>	ContextPtr;
			typedef std::shared_ptr<SceneManager>				SceneManagerPtr;
			typedef std::shared_ptr<Surface>					SurfacePtr;
			typedef std::shared_ptr<data::Provider>	            ProviderPtr;
			typedef std::shared_ptr<AbstractCanvas>				AbstractCanvasPtr;

		private:
			TexturePtr							        _renderTarget;
			RendererPtr							        _renderer;
			SceneManagerPtr						        _sceneManager;
			NodePtr								        _camera;
			math::mat4							        _pickingProjection;
			std::map<SurfacePtr, std::vector<uint>>     _surfaceToPickingIds;
			std::map<uint, SurfacePtr>                  _pickingIdToSurface;
			uint							        	_pickingId;
			ContextPtr					        		_context;
            ProviderPtr				        		    _pickingProvider;

            scene::Layout                               _layout;
            scene::Layout                               _depthLayout;

            EffectPtr                                   _pickingEffect;
            EffectPtr                                   _pickingDepthEffect;

            RendererPtr                                 _depthRenderer;

			std::vector<NodePtr>						_descendants;

			Signal<AbsCtrlPtr, NodePtr>::Slot			_targetAddedSlot;
			Signal<AbsCtrlPtr, NodePtr>::Slot			_targetRemovedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot		_addedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot		_removedSlot;
			Signal<RendererPtr>::Slot					_renderingBeginSlot;
			Signal<RendererPtr>::Slot					_renderingEndSlot;
			Signal<RendererPtr>::Slot					_depthRenderingBeginSlot;
			Signal<RendererPtr>::Slot					_depthRenderingEndSlot;
            Signal<SceneManagerPtr, float, float>::Slot _frameBeginSlot;
			Signal<NodePtr, NodePtr, AbsCtrlPtr>::Slot	_componentAddedSlot;
			Signal<NodePtr, NodePtr, AbsCtrlPtr>::Slot	_componentRemovedSlot;

            int                                         _wheelX;
            int                                         _wheelY;

			bool										_addPickingLayout;

            bool                                        _running;
            bool                                        _enabled;
            bool                                        _renderDepth;

            unsigned char								_lastColor[4];
            unsigned char                               _lastDepth[4];

            bool                                        _debug;
            NodePtr                                     _debugQuad;
            RendererPtr                                 _debugRenderer;

            bool                                        _multiselecting;
            minko::math::vec2                           _multiselectionStartPosition;
            minko::math::vec2                           _singleSelectionPosition;
            minko::math::vec2                           _singleSelectionNormalizedPosition;
		public:
			inline static
			Ptr
            create(NodePtr camera, bool addPickingLayoutToNodes = true, EffectPtr pickingEffect = nullptr, EffectPtr pickingDepthEffect = nullptr, int priority = 0)
			{
                Ptr picking = std::shared_ptr<Picking>(new Picking(priority));

                picking->initialize(camera, addPickingLayoutToNodes, pickingEffect, pickingDepthEffect);

				return picking;
			}

            Ptr
            layout(scene::Layout value);

            Ptr
            depthLayout(scene::Layout value);

            AbstractPicking::map<NodePtr, std::set<unsigned char>>
            pickArea(const minko::math::vec2& bottomLeft, const minko::math::vec2& topRight, bool fullyInside = true) override;

            void
            pick(const minko::math::vec2& point, const minko::math::vec2& normalizedPoint) override;

            inline
            void
            debug(bool v)
            {
                _debug = v;
            }


            virtual
            bool
            enabled() const override
            {
                return _enabled;
            }

            virtual
            void
            enabled(bool enabled) override;

        protected:
			void
			targetAdded(NodePtr target) override;

			void
			targetRemoved(NodePtr target) override;

		private:

            void
            initialize(NodePtr      camera,
                       bool         addPickingLayout,
                       EffectPtr    pickingEffect = nullptr,
                       EffectPtr    pickingDepthEffect = nullptr);

			void
            bindSignals();

            void
            unbindSignals();

            void
			addedHandler(NodePtr node, NodePtr target, NodePtr parent);

			void
			removedHandler(NodePtr node, NodePtr target, NodePtr parent);

			void
			componentAddedHandler(NodePtr node, NodePtr target, AbsCtrlPtr	ctrl);

			void
			componentRemovedHandler(NodePtr node, NodePtr target, AbsCtrlPtr ctrl);

			void
			addSurfacesForNode(NodePtr node);

			void
			removeSurfacesForNode(NodePtr node);

			void
			addSurface(SurfacePtr surface);

			void
			removeSurface(SurfacePtr surface, NodePtr node);

			void
			renderingBegin(RendererPtr renderer);

			void
			renderingEnd(RendererPtr renderer);

			void
			depthRenderingBegin(RendererPtr renderer);

			void
			depthRenderingEnd(RendererPtr renderer);

            Picking(int priority);

            void
            dispatchEvents(SurfacePtr pickedSurfaces);

            void
			updateDescendants(NodePtr target);

            void
            running(bool running);

            void
            frameBeginHandler(SceneManagerPtr, float, float);

            void
            renderDepth(RendererPtr renderer, SurfacePtr pickedSurface);

            void
            updatePickingProjection();

            void
            updatePickingOrigin();

            void
            renderPickingFrame();

            void
            pickingLayoutChanged(scene::Layout previousValue, scene::Layout value);

            std::pair<uint, math::vec4>
            createPickingId(SurfacePtr surface);
		};
	}
}
