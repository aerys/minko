/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 	IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "minko/Common.hpp"

#include "minko/component/AbstractComponent.hpp"
#include "minko/component/Surface.hpp"
#include "minko/Signal.hpp"

namespace minko
{
    namespace component
    {
        class FrustumDisplay : public component::AbstractComponent
        {
        public:
            typedef std::shared_ptr<FrustumDisplay> Ptr;

        private:
            typedef std::shared_ptr<scene::Node>            NodePtr;
            typedef Signal<NodePtr, NodePtr, NodePtr>::Slot NodeSignalSlot;

        private:
            math::mat4                                  _projection;
            std::shared_ptr<Surface>                    _surface;
            std::shared_ptr<Surface>                    _lines;
            std::shared_ptr<material::BasicMaterial>    _material;
            NodeSignalSlot                              _addedSlot;

        public:
            inline static
            Ptr
            create(const math::mat4& projection)
            {
                return std::shared_ptr<FrustumDisplay>(new FrustumDisplay(projection));
            }

            inline
            std::shared_ptr<material::BasicMaterial>
            material()
            {
                return _material;
            }

        protected:
            void
            targetAdded(std::shared_ptr<scene::Node> target);

            void
            targetRemoved(std::shared_ptr<scene::Node> target);

        private:
            FrustumDisplay(const math::mat4& projection);

            void
            initialize();

            void
            initializeLines(const std::vector<math::vec3>& vertices);

            std::shared_ptr<geometry::Geometry>
            initializeFrustumGeometry(const std::vector<math::vec3>&            vertices,
                                      std::shared_ptr<render::AbstractContext>  context);

            std::vector<math::vec3>
            getVertices();

            void
            addedHandler(std::shared_ptr<scene::Node> node,
                         std::shared_ptr<scene::Node> target,
                         std::shared_ptr<scene::Node> added);
        };
    }
}
