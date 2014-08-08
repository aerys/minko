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
#include "minko/Signal.hpp"
#include "minko/data/Container.hpp"
#include "minko/data/Collection.hpp"
#include "minko/data/Provider.hpp"
#include "minko/scene/Node.hpp"

namespace minko
{
    namespace component
    {
	    class AbstractRootDataComponent :
            public AbstractComponent
	    {
        private:
            typedef std::shared_ptr<scene::Node>            NodePtr;

        private:
            std::shared_ptr<data::Provider>                 _provider;
            std::shared_ptr<data::Collection>               _collection;
            bool                                            _enabled;
            NodePtr                                         _root;

            Signal<AbstractComponent::Ptr, NodePtr>::Slot   _targetAddedSlot;
            Signal<AbstractComponent::Ptr, NodePtr>::Slot   _targetRemovedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot         _addedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot         _removedSlot;

	    protected:
            inline
            std::shared_ptr<data::Provider>
            provider() const
            {
                return _provider;
            }

            inline
            std::shared_ptr<data::Collection>
            collection() const
            {
                return _collection;
            }

            inline
            NodePtr
            root()
            {
                return _root;
            }

            AbstractRootDataComponent(std::shared_ptr<data::Provider>   provider,
                                      std::shared_ptr<data::Collection> collection = nullptr) :
                _provider(provider),
                _collection(collection),
                _enabled(true)
            {
                assert(provider);
            }

            virtual
            void
            initialize()
            {
                _targetAddedSlot = targetAdded()->connect(std::bind(
                    &AbstractRootDataComponent::targetAddedHandler,
                    std::static_pointer_cast<AbstractRootDataComponent>(shared_from_this()),
                    std::placeholders::_1,
                    std::placeholders::_2
                ));

                _targetRemovedSlot = targetRemoved()->connect(std::bind(
                    &AbstractRootDataComponent::targetRemovedHandler,
                    std::static_pointer_cast<AbstractRootDataComponent>(shared_from_this()),
                    std::placeholders::_1,
                    std::placeholders::_2
                ));
            }

            virtual
            void
            targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
            {
                if (targets().size() > 1)
                    throw std::logic_error("This component cannot have more than 1 target.");

                auto cb = std::bind(
                    &AbstractRootDataComponent::addedOrRemovedHandler,
                    std::static_pointer_cast<AbstractRootDataComponent>(shared_from_this()),
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3
                );

                _addedSlot = target->added()->connect(cb);
                _removedSlot = target->removed()->connect(cb);

                updateRoot(target->root());
            }

            virtual
            void
            targetRemovedHandler(AbstractComponent::Ptr ctrl, NodePtr target)
            {
                updateRoot(nullptr);
            }

            virtual
            void
            addedOrRemovedHandler(NodePtr node, NodePtr target, NodePtr ancestor)
            {
                updateRoot(node->root());
            }

            virtual
            void
            updateRoot(NodePtr root)
            {
                if (root == _root)
                    return;

                if (_root)
                {
                    if (_collection)
                    {
                        const auto& collections = _root->data()->collections();
                        auto collectionIt = std::find_if(collections.begin(), collections.end(), [&](data::Collection::Ptr c)
                        {
                            return c->name() == _collection->name();
                        });

                        if (*collectionIt == _collection)
                            _root->data()->removeCollection(_collection);
                        else
                            (*collectionIt)->remove(_provider);
                    }
                    else if (_provider)
                        _root->data()->removeProvider(_provider);
                }
                
                _root = root;

                if (_root)
                {
                    if (_collection)
                    {
                        const auto& collections = _root->data()->collections();
                        auto collectionIt = std::find_if(collections.begin(), collections.end(), [&](data::Collection::Ptr c)
                        {
                            return c->name() == _collection->name();
                        });

                        if (collectionIt == collections.end())
                            _root->data()->addCollection(_collection);
                        else
                            (*collectionIt)->pushBack(_provider);
                    }
                    else if (_provider)
                        _root->data()->addProvider(_provider);
                }
            }
	    };
    }
}
