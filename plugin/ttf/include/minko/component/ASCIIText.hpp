#pragma once

#include "minko/Common.hpp"
#include "minko/component/AbstractComponent.hpp"

namespace minko
{
    namespace component
    {
        class ASCIIText : public AbstractComponent
        {
        public:
            using Ptr = std::shared_ptr<ASCIIText>;

        private:
            std::shared_ptr<render::AbstractTexture>    _atlas;
            std::string                                 _text;

            std::shared_ptr<Surface>                    _textSurface;
            std::shared_ptr<material::Material>         _material;
            std::shared_ptr<render::Effect>             _effect;

        public:
            static
            Ptr
            create(std::shared_ptr<render::AbstractTexture>     atlas,
                   const std::string&                           text,
                   std::shared_ptr<material::Material>          material,
                   std::shared_ptr<render::Effect>              effect)
            {
                auto instance = Ptr(new ASCIIText());

                instance->_atlas = atlas;
                instance->_material = material;
                instance->_effect = effect;
                instance->text(text);

                return instance;
            }

            const std::string&
            text() const
            {
                return _text;
            }

            Ptr
            text(const std::string& text);

        private:
            ASCIIText() = default;

            void
            targetAdded(std::shared_ptr<scene::Node> target) override;

            void
            targetRemoved(std::shared_ptr<scene::Node> target) override;

            void
            rebuildGeometry();
        };
    }
}
