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

#include "minko/component/BoundingBox.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/MasterLodScheduler.hpp"
#include "minko/component/Renderer.hpp"
#include "minko/component/TextureLodScheduler.hpp"
#include "minko/data/Provider.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/StreamingOptions.hpp"
#include "minko/material/Material.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/scene/Node.hpp"

#include "sparsehash/sparse_hash_map"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::file;
using namespace minko::material;
using namespace minko::render;
using namespace minko::scene;

TextureLodScheduler::TextureLodScheduler(AssetLibraryPtr assetLibrary) :
    AbstractLodScheduler(),
    _textureResources(),
    _assetLibrary(assetLibrary)
{
}

void
TextureLodScheduler::sceneManagerSet(SceneManager::Ptr sceneManager)
{
    AbstractLodScheduler::sceneManagerSet(sceneManager);

    _sceneManager = sceneManager;
}

void
TextureLodScheduler::rendererSet(Renderer::Ptr renderer)
{
    AbstractLodScheduler::rendererSet(renderer);

    _renderer = renderer;
}

void
TextureLodScheduler::masterLodSchedulerSet(MasterLodScheduler::Ptr masterLodScheduler)
{
    AbstractLodScheduler::masterLodSchedulerSet(masterLodScheduler);

    if (!masterLodScheduler)
    {
        _deferredTextureRegisteredSlot = nullptr;
        _deferredTextureReadySlot = nullptr;

        return;
    }

    for (auto deferredTextureData : masterLodScheduler->deferredTextureDataSet())
        textureRegistered(deferredTextureData);

    _deferredTextureRegisteredSlot = masterLodScheduler->deferredTextureRegistered()->connect(
        [this](MasterLodScheduler::Ptr  masterLodScheduler,
               ProviderPtr              data)
        {
            textureRegistered(data);
        }
    );

    _deferredTextureReadySlot = masterLodScheduler->deferredTextureReady()->connect(
        [this](MasterLodScheduler::Ptr                  masterLodScheduler,
               ProviderPtr                              data,
               const std::unordered_set<ProviderPtr>&   materialDataSet,
               const Flyweight<std::string>&            textureType,
               AbstractTexture::Ptr                     texture)
        {
            textureReady(_textureResources.at(data->uuid()), data, materialDataSet, textureType, texture);
        }
    );
}

void
TextureLodScheduler::surfaceAdded(Surface::Ptr surface)
{
    AbstractLodScheduler::surfaceAdded(surface);

    auto surfaceTarget = surface->target();
    auto material = surface->material();

    // Add surface to cache of material to surfaces.

    auto materialToSurfacesIt = _materialToSurfaces.emplace(material->data(), std::vector<Surface::Ptr>());
    auto& surfaces = materialToSurfacesIt.first->second;
    if (std::find(surfaces.begin(), surfaces.end(), surface) == surfaces.end())
        surfaces.push_back(surface);

    auto textures = std::unordered_map<AbstractTexture::Ptr, Flyweight<std::string>>();

    for (const auto& propertyNameToValuePair : material->data()->values())
    {
        const auto propertyName = propertyNameToValuePair.first;

        if (!material->data()->propertyHasType<TextureSampler>(propertyName))
            continue;

        auto texture = _assetLibrary->getTextureByUuid(
            material->data()->get<TextureSampler>(propertyName).uuid
        );

        textures.insert(std::make_pair(texture, propertyName));
    }

    for (auto textureToTextureTypePair : textures)
    {
        auto texture = textureToTextureTypePair.first;
        const auto textureType = *textureToTextureTypePair.second;

        auto masterLodScheduler = this->masterLodScheduler();

        auto textureData = masterLodScheduler->textureData(texture);

        if (textureData == nullptr)
            continue;

        auto resourceIt = _textureResources.find(textureData->uuid());

        if (resourceIt == _textureResources.end())
        {
            textureRegistered(textureData);
            textureReady(resourceIt->second, textureData, { material->data() }, textureType, texture);
        }
    }
}

void
TextureLodScheduler::textureRegistered(ProviderPtr data)
{
    auto resourceIt = _textureResources.find(data->uuid());

    if (resourceIt == _textureResources.end())
    {
        auto& resourceBase = registerResource(data);

        auto newResourceIt = _textureResources.insert(std::make_pair(
            resourceBase.uuid(),
            TextureResourceInfo()
        ));

        auto& newResource = newResourceIt.first->second;

        newResource.base = &resourceBase;
    }
}

void
TextureLodScheduler::textureReady(TextureResourceInfo&                      resource,
                                  ProviderPtr                               data,
                                  const std::unordered_set<ProviderPtr>&    materialDataSet,
                                  const Flyweight<std::string>&             textureType,
                                  AbstractTexturePtr                        texture)
{
    resource.texture = texture;
    resource.textureType = *textureType;

    resource.maxLod = data->get<int>("maxLod");
    resource.activeLod = std::max(resource.activeLod, DEFAULT_LOD);

    for (auto materialData : materialDataSet)
    {
        resource.materialDataSet.insert(materialData);

        materialData->set(
            *textureType + std::string("MaxAvailableLod"),
            static_cast<float>(lodToMipLevel(
                DEFAULT_LOD,
                resource.texture->width(),
                resource.texture->height())
            )
        );

        materialData->set(
            *textureType + std::string("Size"),
            math::vec2(texture->width(), texture->height())
        );

        materialData->set(
            *textureType + std::string("LodEnabled"),
            true
        );

        if (masterLodScheduler()->streamingOptions()->streamedTextureLodBlendingEnabled())
        {
            materialData->set(*textureType + "LodBlendingEnabled", true);
            materialData->set(*textureType + "LodBlendingStartLod", 0.f);
            materialData->set(*textureType + "LodBlendingStartTime", 0.f);
        }
    }

    // Retrieve surfaces related to the texture.

    for (auto materialData : materialDataSet)
    {
        const auto materialToSurfacesIt = _materialToSurfaces.find(materialData);

        if (materialToSurfacesIt == _materialToSurfaces.end())
            continue;

        for (auto surface : materialToSurfacesIt->second)
            if (std::find(resource.surfaces.begin(), resource.surfaces.end(), surface) == resource.surfaces.end())
                resource.surfaces.push_back(surface);
    }

    const auto& lodDependencyProperties =
        this->masterLodScheduler()->streamingOptions()->streamedTextureLodDependencyProperties();

    for (auto surface : resource.surfaces)
    {
        auto surfaceTarget = surface->target();

        for (const auto& propertyName : lodDependencyProperties)
        {
            resource.propertyChangedSlots.insert(std::make_pair(
                surfaceTarget,
                surfaceTarget->data().propertyChanged(propertyName).connect(
                    [=](Store&          	store,
                        Provider::Ptr       provider,
                        const data::Provider::PropertyName&)
                    {
                        invalidateLodRequirement(*resource.base);
                    }
                )
            ));
        }

        auto resourcePtr = &resource;

        resource.layoutChangedSlots.emplace(surfaceTarget, surfaceTarget->layoutChanged().connect(
            [this, resourcePtr](Node::Ptr node, Node::Ptr target)
            {
                if (node != target)
                    return;

                layoutChanged(*resourcePtr);
            }
        ));

        resource.layoutMaskChangedSlots.emplace(surface, surface->layoutMaskChanged().connect(
            [this, resourcePtr](AbstractComponent::Ptr component)
            {
                layoutChanged(*resourcePtr);
            }
        ));
    }
}

void
TextureLodScheduler::layoutChanged(TextureResourceInfo& resource)
{
    invalidateLodRequirement(*resource.base);
}

void
TextureLodScheduler::viewPropertyChanged(const math::mat4&   worldToScreenMatrix,
                                         const math::mat4&   viewMatrix,
                                         const math::vec3&   eyePosition,
                                         float               fov,
                                         float               aspectRatio,
                                         float               zNear,
                                         float               zFar)
{
    AbstractLodScheduler::viewPropertyChanged(
        worldToScreenMatrix,
        viewMatrix,
        eyePosition,
        fov,
        aspectRatio,
        zNear,
        zFar
    );

    _eyePosition = eyePosition;
    _fov = fov;
    _aspectRatio = aspectRatio;

    invalidateLodRequirement();
}

void
TextureLodScheduler::viewportChanged(const math::vec4& viewport)
{
    _viewport = viewport;

    invalidateLodRequirement();
}

void
TextureLodScheduler::maxAvailableLodChanged(ResourceInfo&    resource,
                                            int              maxAvailableLod)
{
    AbstractLodScheduler::maxAvailableLodChanged(resource, maxAvailableLod);

    invalidateLodRequirement(resource);

    auto& textureResource = _textureResources.at(resource.uuid());

    textureResource.maxAvailableLod = maxAvailableLod;
}

TextureLodScheduler::LodInfo
TextureLodScheduler::lodInfo(ResourceInfo&  resource,
                             float          time)
{
    auto lodInfo = LodInfo();

    auto textureData = resource.data;
    auto& textureResource = _textureResources.at(resource.uuid());

    const auto previousActiveLod = textureResource.activeLod;

    auto maxRequiredLod = 0;
    auto maxPriority = 0.f;
    auto maxRequiredLodSurface = Surface::Ptr();

    static const auto emptySurfaces = std::vector<Surface::Ptr>{ nullptr };
    const auto& surfaces = textureResource.surfaces.empty()
        ? emptySurfaces
        : textureResource.surfaces;

    for (auto surface : surfaces)
    {
        const auto requiredLod = computeRequiredLod(textureResource, surface);
        const auto priority = computeLodPriority(textureResource, surface, requiredLod, textureResource.activeLod, time);

        if (requiredLod > maxRequiredLod)
            maxRequiredLodSurface = surface;

        maxRequiredLod = std::max(requiredLod, maxRequiredLod);
        maxPriority = std::max(priority, maxPriority);
    }

    lodInfo.requiredLod = maxRequiredLod;
    lodInfo.priority = maxPriority;

    const auto activeLod = std::min(maxRequiredLod, textureResource.maxAvailableLod);

    if (previousActiveLod != activeLod)
    {
        textureResource.activeLod = activeLod;

        activeLodChanged(textureResource, maxRequiredLodSurface, previousActiveLod, activeLod, time);
    }

    return lodInfo;
}

void
TextureLodScheduler::activeLodChanged(TextureResourceInfo&   resource,
                                      Surface::Ptr           surface,
                                      int                    previousLod,
                                      int                    lod,
                                      float                  time)
{
    auto textureData = resource.base->data;

    const auto maxAvailableLod = textureData->get<int>("maxAvailableLod");

    const auto& textureType = resource.textureType;

    const auto maxAvailableLodPropertyName = textureType + "MaxAvailableLod";
    const auto lodEnabledPropertyName = textureType + "LodEnabled";

    for (auto materialData : resource.materialDataSet)
    {
        if (materialData->hasProperty(maxAvailableLodPropertyName) &&
            materialData->get<float>(maxAvailableLodPropertyName) ==
            float(lodToMipLevel(maxAvailableLod, resource.texture->width(), resource.texture->height())))
        {
            continue;
        }

        const auto maxLod = mipLevelToLod(0, resource.texture->width(), resource.texture->height());

        const auto mipLevel = float(lodToMipLevel(
            maxAvailableLod,
            resource.texture->width(),
            resource.texture->height()
        ));

        // fixme find proper alternative to unsetting *LodEnabled
        // property, causing performance drop
/*
        if (maxAvailableLod == maxLod)
        {
            material->data()->unset(lodEnabledPropertyName);
        }
*/

        materialData->set(maxAvailableLodPropertyName, mipLevel);

        if (masterLodScheduler()->streamingOptions()->streamedTextureLodBlendingEnabled())
        {
            auto lodBlendingActive = false;

            if (materialData->hasProperty(textureType + "LodBlendingStartLod"))
            {
                const auto oldLodBlendingStartLod = materialData->get<float>(textureType + "LodBlendingStartLod");
                const auto oldLodBlendingStartTime = materialData->get<float>(textureType + "LodBlendingStartTime");

                const auto lodBlendingDuration = masterLodScheduler()->streamingOptions()->streamedTextureLodBlendingPeriod() *
                    (lod - previousLod);

                lodBlendingActive = (time - oldLodBlendingStartTime) < lodBlendingDuration;
            }

            if (!lodBlendingActive)
            {
                materialData->set(
                    textureType + "LodBlendingStartLod",
                    float(lodToMipLevel(previousLod, resource.texture->width(), resource.texture->height()))
                );
                materialData->set(textureType + "LodBlendingStartTime", time);
            }
        }
    }
}

int
TextureLodScheduler::computeRequiredLod(const TextureResourceInfo&  resource,
										Surface::Ptr 				surface)
{
    return masterLodScheduler()->streamingOptions()->streamedTextureLodFunction()
        ? masterLodScheduler()->streamingOptions()->streamedTextureLodFunction()(
            std::numeric_limits<int>::max(),
            resource.maxLod,
            resource.maxLod,
            0.f,
            surface
        )
        : std::numeric_limits<int>::max();
}

float
TextureLodScheduler::computeLodPriority(const TextureResourceInfo& 	resource,
                                        Surface::Ptr                surface,
										int 						requiredLod,
										int 						activeLod,
                                        float                       time)
{
    if (activeLod >= requiredLod)
        return 0.f;

    const auto& lodPriorityFunction = masterLodScheduler()->streamingOptions()->streamedTextureLodPriorityFunction();

    if (lodPriorityFunction)
    {
        return lodPriorityFunction(
            activeLod,
            requiredLod,
            surface,
            surface ? surface->target()->data() : target()->data(),
            _sceneManager->target()->data(),
            _renderer->target()->data()
        );
    }

    return requiredLod - activeLod;
}

float
TextureLodScheduler::distanceFromEye(const TextureResourceInfo&  resource,
                                     Surface::Ptr                surface,
                                     const math::vec3&           eyePosition)
{
    auto box = surface->target()->component<BoundingBox>()->box();

    const auto distance = box->distance(eyePosition);

    return math::max(0.f, distance);
}

int
TextureLodScheduler::lodToMipLevel(int lod, int textureWidth, int textureHeight)
{
    return math::getp2(textureWidth) - lod;
}

int
TextureLodScheduler::mipLevelToLod(int mipLevel, int textureWidth, int textureHeight)
{
    return math::getp2(textureWidth) - mipLevel;
}
