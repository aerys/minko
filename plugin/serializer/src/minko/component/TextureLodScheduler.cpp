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

#include "minko/StreamingOptions.hpp"
#include "minko/component/BoundingBox.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/MasterLodScheduler.hpp"
#include "minko/component/TextureLodScheduler.hpp"
#include "minko/data/Provider.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/log/Logger.hpp"
#include "minko/material/Material.hpp"
#include "minko/math/Ray.hpp"
#include "minko/render/AbstractContext.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/OpenGLES2Context.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

#include "sparsehash/sparse_hash_map"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::material;
using namespace minko::render;
using namespace minko::scene;

TextureLodScheduler::TextureLodScheduler() :
    AbstractLodScheduler(),
    _textureResources()
{
}

void
TextureLodScheduler::sceneManagerSet(SceneManager::Ptr sceneManager)
{
    AbstractLodScheduler::sceneManagerSet(sceneManager);

    _sceneManager = sceneManager;
}

void
TextureLodScheduler::surfaceAdded(Surface::Ptr surface)
{
    AbstractLodScheduler::surfaceAdded(surface);

    auto surfaceTarget = surface->target();
    auto material = surface->material();

    for (const auto& propertyNameToValuePair : material->data()->values())
    {
        const auto& propertyName = propertyNameToValuePair.first;

        if (!material->data()->propertyHasType<TextureSampler>(propertyName))
            continue;

        auto texture = _sceneManager->assets()->getTextureByUuid(
            material->data()->get<TextureSampler>(propertyName).uuid
        );

        auto masterLodScheduler = this->masterLodScheduler();

        auto textureData = masterLodScheduler->textureData(texture);

        if (textureData == nullptr)
            continue;

        auto resourceIt = _textureResources.find(textureData->uuid());

        TextureResourceInfo* resource = nullptr;

        if (resourceIt == _textureResources.end())
        {
            auto& resourceBase = registerResource(textureData);

            auto newResourceIt = _textureResources.insert(std::make_pair(
                resourceBase.uuid(),
                TextureResourceInfo()
            ));
    
            auto& newResource = newResourceIt.first->second;

            newResource.base = &resourceBase;

            resource = &newResource;
        }
        else
        {
            resource = &resourceIt->second;
        }

        resource->texture = texture;
        resource->surfaceToActiveLodMap.insert(std::make_pair(surface, DEFAULT_LOD));

        resource->modelToWorldMatrixChangedSlots.insert(std::make_pair(
            surfaceTarget,
            surfaceTarget->data().propertyChanged("modelToWorldMatrix").connect(
                [=](Store&								 store,
                    Provider::Ptr						 provider,
					const data::Provider::PropertyName&  propertyName)
                {
                    invalidateLodRequirement(*resource->base);
                }
            )
        ));

        material->data()->set(
			*propertyName + std::string(".") + std::string("maxAvailableLod"),
            static_cast<float>(lodToMipLevel(
                DEFAULT_LOD,
                resource->texture->width(),
                resource->texture->height())
            )
        );

        material->data()->set(
			*propertyName + std::string(".") + std::string("size"),
            math::vec2(texture->width(), texture->height())
        );

        material->data()->set(
			*propertyName + std::string("LodEnabled"),
            true
        );

        material->data()->set("diffuseColor", math::vec4(1.f, 1.f, 1.f, 1.f));

        resource->textureName = *propertyName;

        resource->materials.insert(material);
        resource->surfaceToActiveLodMap.insert(std::make_pair(surface, -1));
    }
}

void
TextureLodScheduler::viewPropertyChanged(const math::mat4&   worldToScreenMatrix,
                                         const math::vec3&   eyePosition,
                                         float               fov,
                                         float               aspectRatio,
                                         float               zNear,
                                         float               zFar)
{
    AbstractLodScheduler::viewPropertyChanged(
        worldToScreenMatrix,
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
}

TextureLodScheduler::LodInfo
TextureLodScheduler::lodInfo(ResourceInfo& resource)
{
    auto lodInfo = LodInfo();

    auto textureData = resource.data;
    auto& textureResource = _textureResources.at(resource.uuid());

    auto maxRequiredLod = 0;
    auto maxPriority = 0.f;

    for (auto& surfaceToActiveLodPair : textureResource.surfaceToActiveLodMap)
    {
        auto surface = surfaceToActiveLodPair.first;
        const auto previousActiveLod = surfaceToActiveLodPair.second;

        const auto requiredLod = computeRequiredLod(textureResource, surface);

        const auto activeLod = std::min(requiredLod, textureData->get<int>("maxAvailableLod"));

        if (previousActiveLod != activeLod)
        {
            surfaceToActiveLodPair.second = activeLod;

            activeLodChanged(textureResource, surface, previousActiveLod, activeLod);
        }

        maxRequiredLod = std::max(requiredLod, maxRequiredLod);

        const auto priority = computeLodPriority(textureResource, surface, requiredLod, activeLod);

        maxPriority = std::max(priority, maxPriority);
    }

    lodInfo.requiredLod = maxRequiredLod;
    lodInfo.priority = maxPriority;

    return lodInfo;
}

void
TextureLodScheduler::activeLodChanged(TextureResourceInfo&   resource,
                                      Surface::Ptr           surface,
                                      int                    previousLod,
                                      int                    lod)
{
    auto textureData = resource.base->data;

    const auto maxAvailableLod = textureData->get<int>("maxAvailableLod");

    const auto& textureName = resource.textureName;

    const auto maxAvailableLodPropertyName = textureName + std::string(".") + "maxAvailableLod";
    const auto lodEnabledPropertyName = textureName + "LodEnabled";

    for (auto material : resource.materials)
    {
        if (material->data()->hasProperty(maxAvailableLodPropertyName) &&
            material->data()->get<float>(maxAvailableLodPropertyName) ==
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

        if (maxAvailableLod == maxLod)
        {
            material->data()->unset(lodEnabledPropertyName);
        }

        material->data()->set(maxAvailableLodPropertyName, mipLevel);
    }
}

int
TextureLodScheduler::computeRequiredLod(const TextureResourceInfo&  resource,
										Surface::Ptr 				surface)
{
    if (masterLodScheduler()->streamingOptions()->streamedTextureLodFunction())
        return masterLodScheduler()->streamingOptions()->streamedTextureLodFunction()(surface);

    auto target = surface->target();
    const auto targetDistance = distanceFromEye(resource, surface, _eyePosition);

    if (targetDistance <= 0)
        return std::numeric_limits<int>::max();

    const auto fov = _fov;
    const auto viewportHeight = _viewport.w > 0.f ? _viewport.w : 600.f;

    const auto unitSize = std::abs(2.0f * std::tan(0.5f * fov) *
                          targetDistance / (viewportHeight));

    auto box = target->component<BoundingBox>()->box();
    const auto boxWidth = box->width();
    const auto boxHeight = box->height();

    const auto screenSpaceBoxWidth = boxWidth * unitSize;
    const auto screenSpaceBoxHeight = boxHeight * unitSize;
    const auto numPixels = screenSpaceBoxWidth * screenSpaceBoxHeight;

    const auto textureWidth = resource.texture->width();
    const auto textureHeight = resource.texture->height();
    const auto maxMipLevel = math::getp2(textureWidth);

    auto requiredMipLevel = maxMipLevel;

    for (auto i = 0u; i < maxMipLevel; ++i)
    {
        const auto mipWidth = textureWidth >> i;

        const auto numTexels = mipWidth * mipWidth;

        if (numTexels <= numPixels)
        {
            requiredMipLevel = i;

            break;
        }
    }

    const auto requiredLod = mipLevelToLod(requiredMipLevel, textureWidth, textureHeight);

    return requiredLod;
}

float
TextureLodScheduler::computeLodPriority(const TextureResourceInfo& 	resource,
                                        Surface::Ptr                surface,
										int 						requiredLod,
										int 						activeLod)
{
    if (activeLod >= requiredLod)
        return 0.f;

    auto priority = 0.f;

    const auto distanceFromEye = this->distanceFromEye(resource, surface, _eyePosition);
    const auto distanceFactor = (1000.f - distanceFromEye) * 0.001f;

    if (activeLod < 3)
    {
        priority += 10.f + distanceFactor;
    }
    else if (activeLod < 10)
    {
        priority += 2.f + distanceFactor;
    }
    else
    {
        priority += 1.f + distanceFactor;
    }

    return priority;
}

float
TextureLodScheduler::distanceFromEye(const TextureResourceInfo&  resource,
                                     Surface::Ptr                surface,
                                     const math::vec3&           eyePosition)
{
    auto target = surface->target();
    auto box = target->component<BoundingBox>()->box();

    const auto boxCenter = (box->bottomLeft() + box->topRight()) / 2.f;

    if (boxCenter == eyePosition)
        return 0.f;

    auto ray = math::Ray::create(eyePosition, math::normalize(boxCenter - eyePosition));
    auto targetDistance = 0.f;

    if (!box->cast(ray, targetDistance))
        return math::distance(eyePosition, boxCenter);

    return math::max(0.f, targetDistance);
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
