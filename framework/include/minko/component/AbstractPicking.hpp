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
#include "minko/data/Provider.hpp"

namespace minko
{
	namespace component
	{
		class AbstractPicking :
			public AbstractComponent
		{
		public:
#ifdef MINKO_USE_SPARSE_HASH_MAP
            template <typename... H>
            using map = google::sparse_hash_map<H...>;
#else
            template <class K, typename... V>
            using map = std::unordered_map<K, V...>;
#endif

			typedef std::shared_ptr<AbstractPicking>			Ptr;
			typedef std::shared_ptr<Renderer>					RendererPtr;
			typedef std::shared_ptr<AbstractComponent>			AbsCtrlPtr;
			typedef std::shared_ptr<scene::Node>				NodePtr;
            typedef std::shared_ptr<render::Effect>			    EffectPtr;
            typedef std::shared_ptr<render::Texture>			TexturePtr;
			typedef std::shared_ptr<render::AbstractContext>	ContextPtr;
			typedef std::shared_ptr<SceneManager>				SceneManagerPtr;
			typedef std::shared_ptr<input::Mouse>				MousePtr;
            typedef std::shared_ptr<input::Touch>               TouchPtr;
			typedef std::shared_ptr<Surface>					SurfacePtr;
			typedef std::shared_ptr<data::Provider>	            ProviderPtr;
			typedef std::shared_ptr<AbstractCanvas>				AbstractCanvasPtr;

		protected:
            Signal<Ptr, SurfacePtr>::Ptr _pickingComplete;

            int           _priority;
			SurfacePtr    _lastPickedSurface;
            float         _lastDepthValue;
            unsigned char _lastMergingMask;
            uint          _lastPickedSurfaceId;

		public:
            inline
			SurfacePtr
			pickedSurface()
			{
				return _lastPickedSurface;
			}

            inline
            uint
            pickedSurfaceId()
            {
                return _lastPickedSurfaceId;
            }

            inline
            float
            pickedDepth() const
            {
                return _lastDepthValue;
            }

            inline
            unsigned char
            pickedMergingMask() const
			{
			    return _lastMergingMask;
			}

            inline
            Signal<Ptr, SurfacePtr>::Ptr
            pickingComplete()
            {
                return _pickingComplete;
            }

            virtual
            int
            priority() const
            {
                return _priority;
            }

            virtual
            map<NodePtr, std::set<unsigned char>>
            pickArea(const minko::math::vec2& bottomLeft, const minko::math::vec2& topRight, bool fullyInside = true) = 0;

            virtual
            map<SurfacePtr, std::map<unsigned char, std::vector<minko::math::vec2>>>
            pickSurfacesInArea(const minko::math::vec2& bottomLeft, const minko::math::vec2& topRight, bool fullyInside = true) = 0;

            virtual
            void
            pick(const minko::math::vec2& point, const minko::math::vec2& normalizedPoint) = 0;

            virtual
            bool
            enabled() const = 0;

            virtual
            void
            enabled(bool enabled) = 0;

            AbstractPicking(int priority)
                : _lastPickedSurface(nullptr),
                  _lastDepthValue(0.0),
                  _lastMergingMask(0),
                  _lastPickedSurfaceId(0),
                  _priority(priority),
                  _pickingComplete(Signal<Ptr, SurfacePtr>::create())
            { }
		};
	}
}
