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
#include "minko/data/Provider.hpp"
#include "minko/scene/Node.hpp"

namespace minko
{
    namespace component
    {
        template <class ProviderClass>
	    class AbstractRootDataComponent<ProviderClass, typename std::enable_if<std::is_base_of<data::Provider, ProviderClass>::value>::type> :
            public AbstractComponent
	    {
        private:
            typedef std::shared_ptr<scene::Node>            NodePtr;

        private:
            std::shared_ptr<ProviderClass>                  _data;
            bool                                            _enabled;
            NodePtr                                         _root;

            Signal<AbstractComponent::Ptr, NodePtr>::Slot   _targetAddedSlot;
            Signal<AbstractComponent::Ptr, NodePtr>::Slot   _targetRemovedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot         _addedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot         _removedSlot;

	    protected:
            inline
            std::shared_ptr<ProviderClass>
            data() const
            {
                return _data;
            }

            inline
            NodePtr
            root()
            {
                return _root;
            }

            AbstractRootDataComponent(std::shared_ptr<ProviderClass> provider) :
                _data(provider),
                _enabled(true)
            {
            }

            virtual
            void
            initialize()
            {
                _targetAddedSlot = targetAdded()->connect(std::bind(
                    &AbstractRootDataComponent<ProviderClass>::targetAddedHandler,
                    std::static_pointer_cast<AbstractRootDataComponent<ProviderClass>>(shared_from_this()),
                    std::placeholders::_1,
                    std::placeholders::_2
                ));

                _targetRemovedSlot = targetRemoved()->connect(std::bind(
                    &AbstractRootDataComponent<ProviderClass>::targetRemovedHandler,
                    std::static_pointer_cast<AbstractRootDataComponent<ProviderClass>>(shared_from_this()),
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
                    std::static_pointer_cast<AbstractRootDataComponent<ProviderClass>>(shared_from_this()),
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
                    _root->data()->removeProvider(_data);
                
                _root = root;

                if (_root)
                    _root->data()->addProvider(_data);
            }
	    };
    }
}
