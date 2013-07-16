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

namespace minko
{
    namespace component
    {
	    class AbstractRootDataComponent :
            public AbstractComponent,
            public std::enable_shared_from_this<AbstractRootDataComponent>
	    {
	    public:
		    typedef std::shared_ptr<AbstractRootDataComponent> Ptr;

        private:
            typedef std::shared_ptr<scene::Node>    NodePtr;

        protected:
            std::shared_ptr<data::Provider>                 _data;

        private:
            bool                                            _enabled;
            NodePtr                                         _root;

            Signal<AbstractComponent::Ptr, NodePtr>::Slot   _targetAddedSlot;
            Signal<AbstractComponent::Ptr, NodePtr>::Slot   _targetRemovedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot         _addedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot         _removedSlot;

	    public:
		    inline static
		    Ptr
		    create()
		    {
                auto comp = std::shared_ptr<AbstractRootDataComponent>(new AbstractRootDataComponent());

                comp->initialize();

			    return comp;
		    }

	    protected:
		    AbstractRootDataComponent();

            void
            initialize();

            virtual
            void
            targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target);

            virtual
            void
            targetRemovedHandler(AbstractComponent::Ptr ctrl, NodePtr target);

        private:
            void
            addedOrRemovedHandler(NodePtr node, NodePtr target, NodePtr ancestor);
	    };
    }
}
