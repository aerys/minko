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
#include "minko/Minko.hpp"
#ifdef MINKO_PLUGIN_HTML_OVERLAY
#include "minko/MinkoHtmlOverlay.hpp"
#endif
/*
#include "player/Config.hpp"
*/
#include "PlayerOptions.hpp"

namespace player
{
    namespace component
    {
        class ClippingPlaneScript :
            public minko::component::AbstractScript
        {
        public:
            typedef std::shared_ptr<ClippingPlaneScript>                                  Ptr;

        private:

            typedef minko::component::SceneManager::Ptr                                 SceneManagerPtr;
            typedef minko::component::Surface::Ptr                                      SurfacePtr;
            typedef minko::AbstractCanvas::Ptr                                          AbstractCanvasPtr;

            typedef minko::scene::Node::Ptr                                             NodePtr;
            typedef minko::math::vec3                                                   Vector3;

            typedef minko::input::Keyboard::Ptr                                         KeyboardPtr;
            typedef minko::input::Mouse::Ptr                                            MousePtr;
            typedef minko::input::Touch::Ptr                                            TouchPtr;

            typedef std::shared_ptr<minko::data::Provider>                              ProviderPtr;
            
            typedef minko::Signal<
                minko::data::Store&,
                ProviderPtr,
                const minko::data::Provider::PropertyName&
            >::Slot                                                                     PropertyChangedSlot;
 
            typedef minko::Signal<AbstractCanvasPtr, minko::uint, minko::uint>::Slot    ResizedSlot;

            typedef minko::component::Picking::Ptr                                      PickingPtr;

            typedef minko::Signal<NodePtr>::Slot                                        PickingClickSignalSlot;

            typedef player::PlayerOptions::Ptr                                          PlayerOptionsPtr;

            struct SurfaceEntry
            {
                std::string                 uuid;
                std::list<unsigned char>    subsurfaces;
                SurfacePtr                  surface;
            };

#ifdef MINKO_PLUGIN_HTML_OVERLAY
            typedef minko::component::HtmlOverlay::Ptr                                  HtmlOverlayPtr;
            typedef minko::dom::AbstractDOM::Ptr                                        AbstractDOMPtr;
            typedef minko::Signal<AbstractDOMPtr, std::string>::Slot                    HtmlOverlayMessageSlot;

            HtmlOverlayPtr                                                              _overlay;
            HtmlOverlayMessageSlot                                                      _overlayMessageSlot;
#endif

            NodePtr                                                                     _root;
            SceneManagerPtr                                                             _sceneManager;
            AbstractCanvasPtr                                                           _canvas;


            minko::Signal<minko::input::Mouse::Ptr>::Slot                               _leftButtonClickSlot;

            PlayerOptionsPtr                                                            _playerOptions;

            PropertyChangedSlot                                                          _cameraModelToWorldChangedSlot;
            NodePtr _cameraNode;

            NodePtr _activeClippingPlaneNode;

            int _visibleClippingPlane;
            bool _editionIsActive;

            minko::math::mat4 _editionCameraBaseTransform;

            int _activePlane;
            std::vector<minko::math::mat4> _planeTransforms;

        public:
            ~ClippingPlaneScript() = default;

            inline
            static
            Ptr
            create(PlayerOptionsPtr playerOptions)
            {
                auto script = Ptr(new ClippingPlaneScript());

                script->initialize(playerOptions);

                return script;
            }

            NodePtr
            activeClippingPlaneNode()
            {
                return _activeClippingPlaneNode;
            }

        protected:
            inline
            void
            initialize(PlayerOptionsPtr playerOptions)
            {
                _playerOptions = playerOptions;
            }

            inline
            float
            priority() override
            {
                return -9999.0f;
            }

        public:
            void
            start(NodePtr target) override;

            void
            update(NodePtr target) override;

            void
            stop(NodePtr target) override;

            void
            showClippingPlane(int id);

            void
            hideClippingPlane();

        private:
#ifdef MINKO_PLUGIN_HTML_OVERLAY
            void
            htmlOverlayMessageHandler(AbstractDOMPtr dom, std::string message);
#endif
            ClippingPlaneScript();

            void
            leftButtonClickHandler();

            void
            setClippingPlaneEffect();

            void
            unsetClippingPlaneEffect();

            void
            cameraModelToWorldChanged(const minko::math::mat4& matrix);

            void
            beginEdition();

            void
            endEdition();
        };
    }
}
