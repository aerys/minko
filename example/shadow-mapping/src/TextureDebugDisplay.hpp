#pragma once

#include "minko/Minko.hpp"

class TextureDebugDisplay : public minko::component::AbstractComponent
{
public:
    typedef std::shared_ptr<TextureDebugDisplay>    Ptr;

    static
    Ptr
    create()
    {
        return std::make_shared<TextureDebugDisplay>();
    }

    void
    initialize(minko::file::AssetLibrary::Ptr   assets,
               minko::render::Texture::Ptr      texture)
    {
        if (!texture)
            throw std::invalid_argument("texture");

        auto geom = assets->geometry("debug-quad");

        if (!geom)
        {
            geom = minko::geometry::QuadGeometry::create(assets->context());
            assets->geometry("debug-quad", geom);
        }

        auto fx = assets->effect("effect/debug/TextureDebugDisplay.effect");

        if (!fx)
        {
            auto loader = minko::file::Loader::create(assets->loader());

            loader->options()->loadAsynchronously(false);
            loader->queue("effect/debug/TextureDebugDisplay.effect");
            auto _ = loader->complete()->connect([&](minko::file::Loader::Ptr loader)
            {
                fx = assets->effect("effect/debug/TextureDebugDisplay.effect");
            });
            loader->load();
        }

        _material = minko::material::Material::create();
        _material->data()->set("texture", texture->sampler());

        _surface = minko::component::Surface::create(geom, _material, fx);
        _surface->layoutMask(minko::scene::BuiltinLayout::DEBUG_ONLY);
    }

protected:
    void
    targetAdded(minko::scene::Node::Ptr target)
    {
        target->addComponent(_surface);
        target->layout(target->layout() | minko::scene::BuiltinLayout::DEBUG_ONLY);
    }

    void
    targetRemoved(minko::scene::Node::Ptr target)
    {
        target->removeComponent(_surface);
    }

private:
    minko::component::Surface::Ptr  _surface;
    minko::material::Material::Ptr  _material;

};
