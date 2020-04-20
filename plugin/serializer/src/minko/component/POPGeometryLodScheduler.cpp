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
#include "minko/component/MasterLodScheduler.hpp"
#include "minko/component/Camera.hpp"
#include "minko/component/POPGeometryLodScheduler.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Transform.hpp"
#include "minko/data/Provider.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/StreamingOptions.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/log/Logger.hpp"
#include "minko/math/Box.hpp"
#include "minko/math/Ray.hpp"
#include "minko/scene/Node.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::scene;

const ProgressiveOrderedMeshLodInfo POPGeometryLodScheduler::POPGeometryResourceInfo::defaultLodInfo = ProgressiveOrderedMeshLodInfo();

POPGeometryLodScheduler::POPGeometryLodScheduler() :
    AbstractLodScheduler(),
    _eyePosition(),
    _fov(0.f),
    _aspectRatio(0.f),
    _viewport(),
    _worldToScreenMatrix(),
    _viewMatrix(),
    _blendingRange(0.f)
{
}

void
POPGeometryLodScheduler::sceneManagerSet(SceneManager::Ptr sceneManager)
{
    AbstractLodScheduler::sceneManagerSet(sceneManager);

    _sceneManager = sceneManager;
}

void
POPGeometryLodScheduler::rendererSet(Renderer::Ptr renderer)
{
    AbstractLodScheduler::rendererSet(renderer);

    _renderer = renderer;
}

void
POPGeometryLodScheduler::surfaceAdded(Surface::Ptr surface)
{
	AbstractLodScheduler::surfaceAdded(surface);

	auto surfaceTarget = surface->target();
	auto geometry = surface->geometry();

	auto masterLodScheduler = this->masterLodScheduler();

	auto geometryData = masterLodScheduler->geometryData(geometry);

    if (geometryData == nullptr)
        return;

	auto resourceIt = _popGeometryResources.find(geometryData);

	POPGeometryResourceInfo* resource = nullptr;

	if (resourceIt == _popGeometryResources.end())
	{
        auto& resourceBase = registerResource(geometryData);

        auto newResourceIt = _popGeometryResources.insert(std::make_pair(
            resourceBase.data,
            POPGeometryResourceInfo()
        ));

        auto& newResource = newResourceIt.first->second;

        newResource.base = &resourceBase;

        resource = &newResource;

        resource->geometry = geometry;
        resource->fullPrecisionLod = geometry->data()->get<float>("popFullPrecisionLod");

        const auto* availableLods = resourceBase.data->getPointer<std::map<int, ProgressiveOrderedMeshLodInfo>>("availableLods");

        resource->availableLods = availableLods;

        resource->minLod = availableLods->begin()->second._level;

        auto maxLodIt = availableLods->rbegin();
        if (maxLodIt->second._level == resource->fullPrecisionLod &&
            availableLods->size() > 1)
            ++maxLodIt;

        resource->maxLod = maxLodIt->second._level;

        const auto lodRangeSize = resource->fullPrecisionLod + 1;

        resource->lodToClosestValidLod.resize(lodRangeSize);
        resource->precisionLevelToClosestLod.resize(lodRangeSize);

        updateClosestLods(*resource);
        const auto& lodDependencyProperties =
            this->masterLodScheduler()->streamingOptions()->popGeometryLodDependencyProperties();

        for (const auto& propertyName : lodDependencyProperties)
        {
            resource->propertyChangedSlots.insert(std::make_pair(
                surfaceTarget,
                surfaceTarget->data().propertyChanged(propertyName).connect(
                    [=](Store&              store,
                        Provider::Ptr       provider,
                        const data::Provider::PropertyName&)
                    {
                        invalidateLodRequirement(*resource->base);
                    }
                )
            ));
        }
	}
	else
	{
		resource = &resourceIt->second;
	}

    auto surfaceInfoIt = std::find_if(
        resource->surfaceInfoCollection.begin(),
        resource->surfaceInfoCollection.end(),
        [surface](const SurfaceInfo& surfaceInfo)
        {
            return surfaceInfo.surface == surface;
        }
    );

    if (surfaceInfoIt != resource->surfaceInfoCollection.end())
        return;

    resource->surfaceInfoCollection.emplace_back(surface);
    auto* surfaceInfo = &resource->surfaceInfoCollection.back();

    surfaceInfo->box = surfaceTarget->component<BoundingBox>()->box();

    surfaceInfo->weight = 0.f;

    surfaceInfo->layoutChangedSlot = surfaceTarget->layoutChanged().connect(
        [this, resource, surfaceInfo](Node::Ptr node, Node::Ptr target)
        {
            if (node != target)
                return;

            layoutChanged(*resource, *surfaceInfo);
        }
    );

    surfaceInfo->layoutMaskChangedSlot = surface->layoutMaskChanged().connect(
        [this, resource, surfaceInfo](AbstractComponent::Ptr component)
        {
            layoutChanged(*resource, *surfaceInfo);
        }
    );

    if (!surface->data()->hasProperty("popLodEnabled"))
    {
        surface->numIndices(0u);
        surface->data()->set("popLod", 0.f);
        surface->data()->set("popLodEnabled", true);
    }
    else
    {
        const auto precisionLevel = surface->data()->get<float>("popLod");

        surfaceInfo->requiredPrecisionLevel = precisionLevel;
        surfaceInfo->activeLod = precisionLevelToClosestLod(*resource, precisionLevel)._level;
    }

    if (masterLodScheduler->streamingOptions()->popGeometryLodBlendingEnabled() &&
        !surface->data()->hasProperty("popLodBlendingEnabled"))
    {
        surface->data()->set("popLodBlendingEnabled", true);
        surface->data()->set("popPreviousLod", 0.f);
        surface->data()->set("popLodBlendingTime", 0.f);
    }
}

void
POPGeometryLodScheduler::surfaceRemoved(Surface::Ptr surface)
{
    AbstractLodScheduler::surfaceRemoved(surface);

    auto surfaceTarget = surface->target();
    auto geometry = surface->geometry();

    auto masterLodScheduler = this->masterLodScheduler();

    auto geometryData = masterLodScheduler->geometryData(geometry);

    if (geometryData == nullptr)
        return;

    auto resourceIt = _popGeometryResources.find(geometryData);

    if (resourceIt == _popGeometryResources.end())
        return;

    auto& resource = resourceIt->second;

    resource.surfaceInfoCollection.erase(std::remove_if(
        resource.surfaceInfoCollection.begin(),
        resource.surfaceInfoCollection.end(),
        [&surface](const SurfaceInfo& surfaceInfo)
        {
            return surfaceInfo.surface == surface;
        }), resource.surfaceInfoCollection.end()
    );

    // In theory 1 Surface == 1 Geometry == 1 Resource,
    // so we must remove any resource that doesn't refer to any surface.
    if (resource.surfaceInfoCollection.size() == 0)
    {
        std::cout << "Removing resource that doesn't point to any surface." << std::endl;
        this->unregisterResource(resourceIt->second.base->uuid());
        _popGeometryResources.erase(resourceIt);
    }
}

void
POPGeometryLodScheduler::viewPropertyChanged(const math::mat4&   worldToScreenMatrix,
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
    _worldToScreenMatrix = worldToScreenMatrix;
    _viewMatrix = viewMatrix;

	invalidateLodRequirement();
}

void
POPGeometryLodScheduler::viewportChanged(const math::vec4& viewport)
{
    _viewport = viewport;

    invalidateLodRequirement();
}

void
POPGeometryLodScheduler::maxAvailableLodChanged(ResourceInfo&    resource,
                                                int              maxAvailableLod)
{
    AbstractLodScheduler::maxAvailableLodChanged(resource, maxAvailableLod);

    invalidateLodRequirement(resource);

    auto& popGeometryResource = _popGeometryResources.at(resource.data);

    if (popGeometryResource.minAvailableLod < 0)
        popGeometryResource.minAvailableLod = maxAvailableLod;
    else
        popGeometryResource.minAvailableLod = std::min(maxAvailableLod, popGeometryResource.minAvailableLod);

    popGeometryResource.maxAvailableLod = std::max(maxAvailableLod, popGeometryResource.maxAvailableLod);

    updateClosestLods(popGeometryResource);
}

POPGeometryLodScheduler::LodInfo
POPGeometryLodScheduler::lodInfo(ResourceInfo&  resource,
                                 float          time)
{
	auto lodInfo = LodInfo();

	auto& popGeometryResource = _popGeometryResources.at(resource.data);

    auto maxRequiredLod = 0;
    auto maxPriority = 0.f;

	for (auto& surfaceInfo : popGeometryResource.surfaceInfoCollection)
	{
		auto surface = surfaceInfo.surface;

		const auto previousActiveLod = surfaceInfo.activeLod;

        auto activeLod = previousActiveLod;

        auto requiredPrecisionLevel = 0.f;
		const auto requiredLod = computeRequiredLod(popGeometryResource, surfaceInfo, requiredPrecisionLevel);

        const auto& lod = lodToClosestValidLod(popGeometryResource, requiredLod);

        if (lod.isValid())
            activeLod = lod._level;

        if (previousActiveLod != activeLod)
        {
            surfaceInfo.activeLod = activeLod;

            activeLodChanged(popGeometryResource, surfaceInfo, previousActiveLod, activeLod, requiredPrecisionLevel, time);
        }

        if (surfaceInfo.requiredPrecisionLevel != requiredPrecisionLevel)
        {
            surfaceInfo.requiredPrecisionLevel = requiredPrecisionLevel;

            requiredPrecisionLevelChanged(popGeometryResource, surfaceInfo);
        }

        maxRequiredLod = std::max(requiredLod, maxRequiredLod);

        const auto priority = computeLodPriority(popGeometryResource, surfaceInfo, requiredLod, activeLod, time);

        if (priority > 0.f)
            surfaceInfo.weight = priority;

        maxPriority = std::max(priority, maxPriority);
	}

    lodInfo.requiredLod = maxRequiredLod;
    lodInfo.priority = maxPriority;

	return lodInfo;
}

void
POPGeometryLodScheduler::layoutChanged(POPGeometryResourceInfo&  resource,
                                       SurfaceInfo&              surfaceInfo)
{
    invalidateLodRequirement(*resource.base);
}

void
POPGeometryLodScheduler::activeLodChanged(POPGeometryResourceInfo&   resource,
                             			  SurfaceInfo&               surfaceInfo,
                             			  int                        previousLod,
                             			  int                        lod,
                                          float                      requiredPrecisionLevel,
                                          float                      time)
{
	auto provider = resource.base->data;

	const auto& activeLod = lodToClosestValidLod(resource, lod);

	const auto numIndices = static_cast<unsigned int>(
		(activeLod._indexOffset + activeLod._indexCount)
	);

    const auto previousTargetPrecisionLevel = surfaceInfo.surface->data()->hasProperty("popLod")
        ? surfaceInfo.surface->data()->get<float>("popLod")
        : 0.f;

    const auto targetPrecisionLevel = float(activeLod._precisionLevel);

    surfaceInfo.surface->numIndices(numIndices);
    surfaceInfo.surface->data()->set("popLod", targetPrecisionLevel);

    if (masterLodScheduler()->streamingOptions()->popGeometryLodBlendingEnabled())
    {
        auto popLodBlendingActive = false;

        if (surfaceInfo.surface->data()->hasProperty("popPreviousLod"))
        {
            const auto previousSourcePrecisionLevel = surfaceInfo.surface->data()->get<float>("popPreviousLod");
            const auto oldPopLodBlendingTime = surfaceInfo.surface->data()->get<float>("popLodBlendingTime");

            const auto popLodBlendingDuration = masterLodScheduler()->streamingOptions()->popGeometryLodBlendingPeriod() * (previousTargetPrecisionLevel - previousSourcePrecisionLevel);

            popLodBlendingActive = (time - oldPopLodBlendingTime) < popLodBlendingDuration;
        }

        if (!popLodBlendingActive)
        {
            const auto minPrecisionLevel = masterLodScheduler()->streamingOptions()->popGeometryLodBlendingMinPrecisionLevel();
            const auto minPrecisionLevelLodInfo = precisionLevelToClosestLod(resource, minPrecisionLevel);

            previousLod = math::max(previousLod, minPrecisionLevelLodInfo._level);
            previousLod = math::min(previousLod, resource.maxAvailableLod);

            const auto& previousLodInfo = lodToClosestValidLod(resource, previousLod);

            surfaceInfo.surface->data()->set("popPreviousLod", float(previousLodInfo._precisionLevel));
            surfaceInfo.surface->data()->set("popLodBlendingTime", time);
        }
    }
}

int
POPGeometryLodScheduler::computeRequiredLod(const POPGeometryResourceInfo&  resource,
											SurfaceInfo& 				    surfaceInfo,
                                            float&                          requiredPrecisionLevel)
{
    auto target = surfaceInfo.surface->target();

    auto box = surfaceInfo.box;

    const auto worldMinBound = box->bottomLeft();
    const auto worldMaxBound = box->topRight();

    const auto targetDistance = distanceFromEye(resource, surfaceInfo, _eyePosition);

    surfaceInfo.surface->data()->set("distanceFromEye", targetDistance);

    if (targetDistance <= 0)
    {
        const auto maxPrecisionLevel = std::numeric_limits<int>::max();

        requiredPrecisionLevel = maxPrecisionLevel;

        const auto& requiredLod = precisionLevelToClosestLod(resource, maxPrecisionLevel);

        return masterLodScheduler()->streamingOptions()->popGeometryLodFunction()
            ? masterLodScheduler()->streamingOptions()->popGeometryLodFunction()(
                requiredLod._level,
                resource.maxLod,
                resource.fullPrecisionLod,
                surfaceInfo.weight,
                surfaceInfo.surface
            )
            : requiredLod._level;
    }

    const auto defaultPopGeometryError = float(masterLodScheduler()->streamingOptions()->popGeometryErrorToleranceThreshold());

    const auto popErrorBound = masterLodScheduler()->streamingOptions()->popGeometryErrorFunction()
        ? masterLodScheduler()->streamingOptions()->popGeometryErrorFunction()(defaultPopGeometryError, surfaceInfo.surface)
        : defaultPopGeometryError;

    const auto viewportHeight = _viewport.w > 0.f ? _viewport.w : 600.f;

    auto unitSize = std::abs(2.0f * std::tan(0.5f * _fov) * targetDistance / viewportHeight);

    requiredPrecisionLevel = math::log2(glm::length(worldMaxBound - worldMinBound) / (unitSize * (popErrorBound + 1)));

    auto ceiledRequiredPrecisionLevel = static_cast<int>(std::ceil(requiredPrecisionLevel));

    const auto& requiredLod = precisionLevelToClosestLod(resource, ceiledRequiredPrecisionLevel);

    return masterLodScheduler()->streamingOptions()->popGeometryLodFunction()
        ? masterLodScheduler()->streamingOptions()->popGeometryLodFunction() (
            requiredLod._level,
            resource.maxLod,
            resource.fullPrecisionLod,
            surfaceInfo.weight,
            surfaceInfo.surface
        )
        : requiredLod._level;
}

float
POPGeometryLodScheduler::computeLodPriority(const POPGeometryResourceInfo& 	resource,
                                            SurfaceInfo&                    surfaceInfo,
											int 							requiredLod,
											int 							activeLod,
                                            float                           time)
{
    const auto& lodPriorityFunction = masterLodScheduler()->streamingOptions()->popGeometryLodPriorityFunction();

    if (!lodPriorityFunction)
        throw std::runtime_error("popGeometryLodPriorityFunction not specified.");

    return lodPriorityFunction(
        activeLod,
        requiredLod,
        surfaceInfo.surface,
        surfaceInfo.surface->target()->data(),
        _sceneManager->target()->data(),
        _renderer ? &_renderer->target()->data() : nullptr
    );
}

bool
POPGeometryLodScheduler::findClosestValidLod(const POPGeometryResourceInfo&         resource,
											 int 							        lod,
											 const ProgressiveOrderedMeshLodInfo*&  result) const
{
	auto data = resource.base->data;

    const auto& lods = *resource.availableLods;

    if (lods.empty())
        return false;

    auto validLods = std::map<int, const ProgressiveOrderedMeshLodInfo*>();

    for (const auto& lod : lods)
    {
        if (lod.second.isValid())
            validLods.insert(std::make_pair(lod.first, &lod.second));
    }

    if (validLods.empty())
        return false;

    auto closestLodIt = validLods.lower_bound(lod);

    if (closestLodIt == validLods.end())
        result = validLods.rbegin()->second;
    else
        result = closestLodIt->second;

    return true;
}

bool
POPGeometryLodScheduler::findClosestLodByPrecisionLevel(const POPGeometryResourceInfo& 	        resource,
								 					    int 							        precisionLevel,
								 					    const ProgressiveOrderedMeshLodInfo*&   result) const
{
	auto data = resource.base->data;

    const auto& lods = *resource.availableLods;

    if (lods.empty())
        return false;

    for (const auto& precisionLevelToLodPair : lods)
    {
        const auto& lod = precisionLevelToLodPair.second;

        if (lod._precisionLevel >= precisionLevel)
        {
            result = &lod;

            return true;
        }
    }

    result = &lods.rbegin()->second;

    return true;
}

void
POPGeometryLodScheduler::updateClosestLods(POPGeometryResourceInfo& resource)
{
    const auto lowerLod = resource.minLod;
    const auto upperLod = resource.fullPrecisionLod + 1;

    for (auto lod = lowerLod; lod < upperLod; ++lod)
    {
        const ProgressiveOrderedMeshLodInfo* closestValidLod = nullptr;

        resource.lodToClosestValidLod[lod] = findClosestValidLod(resource, lod, closestValidLod)
            ? closestValidLod
            : &POPGeometryResourceInfo::defaultLodInfo;

        const ProgressiveOrderedMeshLodInfo* closestLodByPrecisionLevel = nullptr;

        resource.precisionLevelToClosestLod[lod] = findClosestLodByPrecisionLevel(resource, lod, closestLodByPrecisionLevel)
            ? closestLodByPrecisionLevel
            : &POPGeometryResourceInfo::defaultLodInfo;
    }
}

const ProgressiveOrderedMeshLodInfo&
POPGeometryLodScheduler::precisionLevelToClosestLod(const POPGeometryResourceInfo& resource, int precisionLevel) const
{
    return *resource.precisionLevelToClosestLod.at(math::clamp(precisionLevel, resource.minLod, resource.fullPrecisionLod));
}

const ProgressiveOrderedMeshLodInfo&
POPGeometryLodScheduler::lodToClosestValidLod(const POPGeometryResourceInfo& resource, int lod) const
{
    return *resource.lodToClosestValidLod.at(math::clamp(lod, resource.minLod, resource.fullPrecisionLod));
}

float
POPGeometryLodScheduler::distanceFromEye(const POPGeometryResourceInfo&  resource,
                                         SurfaceInfo&                    surfaceInfo,
                                         const math::vec3&               eyePosition)
{
    auto box = surfaceInfo.box;

    const auto distance = box->distance(eyePosition);

    return math::max(0.f, distance);
}

void
POPGeometryLodScheduler::requiredPrecisionLevelChanged(const POPGeometryResourceInfo&    resource,
                                                       SurfaceInfo&                      surfaceInfo)
{
}
