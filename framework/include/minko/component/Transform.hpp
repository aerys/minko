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

#include "minko/scene/Node.hpp"
#include "minko/component/AbstractComponent.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/Any.hpp"
#include "minko/data/Provider.hpp"

namespace minko
{
	namespace component
	{
		class Transform :
			public AbstractComponent
		{

		public:
			typedef std::shared_ptr<Transform>			Ptr;

		private:
			typedef std::shared_ptr<scene::Node>		NodePtr;
			typedef std::shared_ptr<AbstractComponent>	AbsCtrlPtr;

		private:
			math::mat4*										_matrix;
			math::mat4*										_modelToWorld;
			std::shared_ptr<data::Provider>		            _data;

			Signal<NodePtr, NodePtr, NodePtr>::Slot 		_addedSlot;
			Signal<NodePtr, NodePtr, NodePtr>::Slot 		_removedSlot;

            bool                                            _dirty;

		public:
			inline static
			Ptr
			create()
			{
                return std::shared_ptr<Transform>(new Transform());
			}

			inline static
			Ptr
			create(const math::mat4& transform)
			{
				auto ctrl = create();

				*ctrl->_matrix = transform;

				return ctrl;
			}

            AbstractComponent::Ptr
            clone(const CloneOption& option);

            ~Transform() = default;

			inline
			const math::mat4&
			matrix()
			{
				return *_matrix;
			}

			inline
			void
			matrix(const math::mat4& matrix)
			{
                if (matrix == *_matrix)
                    return;

				*_matrix = matrix;

                if (target() != nullptr)
                {
                    auto rootTransform = target()->root()->component<RootTransform>();

                    if (rootTransform && !rootTransform->_invalidLists)
                        rootTransform->_nodeTransformCache.at(rootTransform->_nodeToId[target()])._dirty = true;
                }
			}

			inline
			const math::mat4&
			modelToWorldMatrix()
			{
				return modelToWorldMatrix(false);
			}

			inline
			const math::mat4&
			modelToWorldMatrix(bool forceUpdate)
			{
				if (forceUpdate)
                    updateModelToWorldMatrix();

				return *_modelToWorld;
			}

            inline
            void
            updateModelToWorldMatrix()
            {
                target()->root()->component<RootTransform>()->forceUpdate(target(), true);
            }

        protected:
			void
			targetAdded(NodePtr target);

			void
			targetRemoved(NodePtr target);

		private:
			Transform();

			void
			addedOrRemovedHandler(NodePtr node, NodePtr target, NodePtr ancestor);

#ifdef MINKO_TEST
		public:
#else
		private:
#endif // MINKO_TEST
			class RootTransform :
				public AbstractComponent
			{
				friend class Transform;

			public:
				typedef std::shared_ptr<RootTransform> Ptr;

			private:
				typedef std::shared_ptr<SceneManager>				SceneMgrPtr;
				typedef std::shared_ptr<Renderer>					RendererCtrlPtr;
                typedef std::shared_ptr<render::AbstractTexture>    AbsTexPtr;
				typedef Signal<RendererCtrlPtr>::Slot 				EnterFrameCallback;
				typedef std::shared_ptr<render::AbstractTexture> 	AbsTexturePtr;
				typedef Signal<SceneMgrPtr, uint, AbsTexturePtr> 	RenderingBeginSignal;
				typedef RenderingBeginSignal::Slot 					RenderingBeginSlot;
                typedef std::shared_ptr<data::Provider>             ProviderPtr;

                struct NodeTransformCacheEntry
                {
                    NodePtr             _node;
				    const math::mat4*   _matrix;
				    math::mat4*		    _modelToWorldMatrix;

                    int                 _parentId;
                    int                 _firstChildId;
                    int                 _numChildren;

                    bool                _dirty;

                    ProviderPtr         _provider;

                    NodeTransformCacheEntry();
                };

			public:
				inline static
				Ptr
				create()
				{
                    return std::shared_ptr<RootTransform>(new RootTransform());
				}

                AbstractComponent::Ptr
                clone(const CloneOption& option);

				void
				forceUpdate(NodePtr node, bool updateTransformLists = false);

                ~RootTransform()
                {
                    _nodeTransformCache.clear();
                    _nodeToId.clear();
                    _targetSlots.clear();
                    _renderingBeginSlot = nullptr;
                }

			private:
                std::vector<NodeTransformCacheEntry>            _nodeTransformCache;

                std::unordered_map<NodePtr, unsigned int>	    _nodeToId;
                std::list<NodePtr>                              _nodes;
				bool							                _invalidLists;

				std::list<Any>					                _targetSlots;
                Signal<SceneMgrPtr, uint, AbsTexPtr>::Slot      _renderingBeginSlot;

                std::list<NodePtr>                              _toAdd;
                std::list<NodePtr>                              _toRemove;

                std::unordered_map<
                    NodePtr,
                    Signal<data::Store&, ProviderPtr, const data::Provider::PropertyName&>::Slot
                >                                               _nodeToPropertyChangedSlot;

            protected:
            	void
				targetAdded(NodePtr target);

				void
				targetRemoved(NodePtr target);

			private:
				void
				componentRemovedHandler(NodePtr node, NodePtr target, AbsCtrlPtr ctrl);

				void
				componentAddedHandler(NodePtr node, NodePtr target, AbsCtrlPtr	ctrl);

				void
				removedHandler(NodePtr node, NodePtr target, NodePtr parent);

				void
				addedHandler(NodePtr node, NodePtr target, NodePtr parent);

				void
				updateTransformsList();

				void
				updateTransforms();

				void
				updateTransformPath(const std::vector<unsigned int>& path);

				void
                renderingBeginHandler(std::shared_ptr<SceneManager>             sceneManager,
                                      uint                                      frameId,
                                      std::shared_ptr<render::AbstractTexture>  abstractTexture);


				void
				sortNodes();
			};
		};
	}
}
