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
	    class SceneManager :
            public AbstractComponent,
            public std::enable_shared_from_this<SceneManager>
	    {
	    public:
		    typedef std::shared_ptr<SceneManager> Ptr;

        private:
            typedef std::shared_ptr<scene::Node>				NodePtr;
			typedef std::shared_ptr<render::AbstractTexture>	AbsTexturePtr;

        private:
            uint                                            _frameId;
            float                                           _time;
            std::shared_ptr<file::AssetLibrary>             _assets;

            Signal<Ptr, float, float>::Ptr                  _frameBegin;
            Signal<Ptr, float, float>::Ptr                  _frameEnd;
			Signal<Ptr>::Ptr                                _cullBegin;
			Signal<Ptr>::Ptr                                _cullEnd;
			Signal<Ptr, uint, AbsTexturePtr>::Ptr           _renderBegin;
			Signal<Ptr, uint, AbsTexturePtr>::Ptr           _renderEnd;

            Signal<AbstractComponent::Ptr, NodePtr>::Slot   _targetAddedSlot;
            Signal<AbstractComponent::Ptr, NodePtr>::Slot   _targetRemovedSlot;
            Signal<NodePtr, NodePtr, NodePtr>::Slot         _addedSlot;

	    public:
		    inline static
		    Ptr
		    create(const std::shared_ptr<render::AbstractContext>& context)
		    {
                auto sm = std::shared_ptr<SceneManager>(new SceneManager(context));

                sm->initialize();

			    return sm;
		    }

            ~SceneManager()
            {
            }

            inline
            uint
            frameId()
            {
                return _frameId;
            }

            inline
            std::shared_ptr<file::AssetLibrary>
            assets()
            {
                return _assets;
            }

            inline
            Signal<Ptr, float, float>::Ptr
            frameBegin() const
            {
                return _frameBegin;
            }

            inline
            Signal<Ptr, float, float>::Ptr
            frameEnd() const
            {
                return _frameEnd;
            }

			inline
			Signal<Ptr>::Ptr
			cullingBegin()
			{
				return _cullBegin;
			}

			inline
			Signal<Ptr>::Ptr
			cullingEnd()
			{
				return _cullEnd;
			}

			inline
			Signal<Ptr, uint, AbsTexturePtr>::Ptr
			renderingBegin()
			{
				return _renderBegin;
			}

			inline
			Signal<Ptr, uint, AbsTexturePtr>::Ptr
			renderingEnd()
			{
				return _renderEnd;
			}

            inline
            float
            time() const
            {
                return _time; // in milliseconds
            }

            void
            initialize();

            void
            nextFrame(float time, float deltaTime, AbsTexturePtr target = nullptr);

	    private:
		    SceneManager(const std::shared_ptr<render::AbstractContext>& context);

            void
            targetAddedHandler(AbstractComponent::Ptr ctrl, NodePtr target);

            void
            targetRemovedHandler(AbstractComponent::Ptr ctrl, NodePtr target);

            void
            addedHandler(NodePtr node, NodePtr target, NodePtr ancestor);
	    };
    }
}
