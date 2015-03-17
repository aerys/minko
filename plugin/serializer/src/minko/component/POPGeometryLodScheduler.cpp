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
#include "minko/component/MasterLodScheduler.hpp"
#include "minko/component/POPGeometryLodScheduler.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/component/Surface.hpp"
#include "minko/data/Provider.hpp"
#include "minko/file/AssetLibrary.hpp"
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

POPGeometryLodScheduler::POPGeometryLodScheduler() :
    AbstractLodScheduler(),
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
POPGeometryLodScheduler::masterLodSchedulerSet(MasterLodSchedulerPtr masterLodScheduler)
{
    AbstractLodScheduler::masterLodSchedulerSet(masterLodScheduler);

    if (masterLodScheduler != nullptr)
        blendingRange(masterLodScheduler->streamingOptions()->popGeometryBlendingRange());
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

	auto resourceIt = _popGeometryResources.find(geometryData->uuid());

	POPGeometryResourceInfo* resource = nullptr;

	if (resourceIt == _popGeometryResources.end())
	{
        auto& resourceBase = registerResource(geometryData);

        auto newResourceIt = _popGeometryResources.insert(std::make_pair(
            resourceBase.uuid(),
            POPGeometryResourceInfo()
        ));

        auto& newResource = newResourceIt.first->second;

        newResource.base = &resourceBase;

        resource = &newResource;
	}
	else
	{
		resource = &resourceIt->second;
	}

    auto surfaceInfo = SurfaceInfo(surface);

    auto resourceData = resource->base->data;
    resource->fullPrecisionLod = surface->geometry()->data()->get<float>("popFullPrecisionLod");

	resource->surfaceToSurfaceInfoMap.insert(std::make_pair(surface, surfaceInfo));

	resource->modelToWorldMatrixChangedSlots.insert(std::make_pair(
		surfaceTarget,
		surfaceTarget->data().propertyChanged("modelToWorldMatrix").connect(
			[=](Store&          	store,
            	Provider::Ptr       provider,
				const data::Provider::PropertyName&)
        	{
        	    invalidateLodRequirement(*resource->base);
        	}
		)
	));

	const auto& availableLods = resourceData->get<std::map<int, ProgressiveOrderedMeshLodInfo>>("availableLods");

    resource->minLod = availableLods.begin()->second._level;

    auto maxLodIt = availableLods.rbegin();
    if (maxLodIt->second._level == resource->fullPrecisionLod)
        ++maxLodIt;

    resource->maxLod = maxLodIt->second._level;

	surface->geometry()->data()->set("popLod", 0.f);
	surface->geometry()->data()->set("popLodEnabled", true);

    if (blendingIsActive(*resource, surfaceInfo))
        blendingRangeChanged(*resource, surfaceInfo, _blendingRange);
}

void
POPGeometryLodScheduler::surfaceRemoved(Surface::Ptr surface)
{
    AbstractLodScheduler::surfaceRemoved(surface);

    // TODO
}

void
POPGeometryLodScheduler::viewPropertyChanged(const math::mat4&   worldToScreenMatrix,
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

    auto& popGeometryResource = _popGeometryResources.at(resource.uuid());

    if (popGeometryResource.minAvailableLod < 0)
        popGeometryResource.minAvailableLod = maxAvailableLod;
    else
        popGeometryResource.minAvailableLod = std::min(maxAvailableLod, popGeometryResource.minAvailableLod);

    if (maxAvailableLod < popGeometryResource.fullPrecisionLod)
        popGeometryResource.maxAvailableLod = std::max(maxAvailableLod, popGeometryResource.maxAvailableLod);
}

POPGeometryLodScheduler::LodInfo
POPGeometryLodScheduler::lodInfo(ResourceInfo& resource)
{
	auto lodInfo = LodInfo();

	auto& popGeometryResource = _popGeometryResources.at(resource.uuid());

    auto maxRequiredLod = 0;
    auto maxPriority = 0.f;

	for (auto& surfaceToActiveLodPair : popGeometryResource.surfaceToSurfaceInfoMap)
	{
		auto surface = surfaceToActiveLodPair.first;
        auto& surfaceInfo = surfaceToActiveLodPair.second;

		const auto previousActiveLod = surfaceInfo.activeLod;

        auto activeLod = previousActiveLod;

        auto requiredPrecisionLevel = 0.f;
		const auto requiredLod = computeRequiredLod(popGeometryResource, surfaceInfo, requiredPrecisionLevel);

	    auto lod = ProgressiveOrderedMeshLodInfo();
        if (findClosestValidLod(popGeometryResource, requiredLod, lod))
            activeLod = lod._level;

        if (previousActiveLod != activeLod)
        {
            surfaceInfo.activeLod = activeLod;

            activeLodChanged(popGeometryResource, surfaceInfo, previousActiveLod, activeLod, requiredPrecisionLevel);
        }

        if (surfaceInfo.requiredPrecisionLevel != requiredPrecisionLevel)
        {
            surfaceInfo.requiredPrecisionLevel = requiredPrecisionLevel;

            requiredPrecisionLevelChanged(popGeometryResource, surfaceInfo);
        }

        if (blendingIsActive(popGeometryResource, surfaceInfo))
            updateBlendingLod(popGeometryResource, surfaceInfo);

        maxRequiredLod = std::max(requiredLod, maxRequiredLod);

        const auto priority = computeLodPriority(popGeometryResource, surfaceInfo, requiredLod, activeLod);

        maxPriority = std::max(priority, maxPriority);
	}

    lodInfo.requiredLod = maxRequiredLod;
    lodInfo.priority = maxPriority;

	return lodInfo;
}

void
POPGeometryLodScheduler::activeLodChanged(POPGeometryResourceInfo&   resource,
                             			  SurfaceInfo&               surfaceInfo,
                             			  int                        previousLod,
                             			  int                        lod,
                                          float                      requiredPrecisionLevel)
{
	auto provider = resource.base->data;

	const auto& availableLods = provider->get<std::map<int, ProgressiveOrderedMeshLodInfo>>("availableLods");

	const auto& activeLod = availableLods.at(lod);

	const auto numIndices = static_cast<unsigned int>(
		(activeLod._indexOffset + activeLod._indexCount)
	);

    // TODO use setter from Surface

    surfaceInfo.surface->geometry()->data()->set("numIndices", numIndices);

    surfaceInfo.surface->geometry()->data()->set("popLod", float(activeLod._precisionLevel));
}

int
POPGeometryLodScheduler::computeRequiredLod(const POPGeometryResourceInfo&  resource,
											SurfaceInfo& 				    surfaceInfo,
                                            float&                          requiredPrecisionLevel)
{
    if (masterLodScheduler()->streamingOptions()->popGeometryLodFunction())
        return masterLodScheduler()->streamingOptions()->popGeometryLodFunction()(surfaceInfo.surface);

    auto target = surfaceInfo.surface->target();

    auto box = target->component<BoundingBox>()->box();

    const auto worldMinBound = box->bottomLeft();
    const auto worldMaxBound = box->topRight();

    const auto targetDistance = distanceFromEye(resource, surfaceInfo, _eyePosition);

    if (targetDistance <= 0)
    {
        const auto maxPrecisionLevel = std::numeric_limits<int>::max();

        requiredPrecisionLevel = maxPrecisionLevel;

        auto lod = ProgressiveOrderedMeshLodInfo();
        auto requiredLod = findClosestLodByPrecisionLevel(resource, maxPrecisionLevel, lod) ? lod._level : DEFAULT_LOD;

        return requiredLod;
    }

    const auto popErrorBound = masterLodScheduler()->streamingOptions()->popGeometryErrorToleranceThreshold();

    const auto viewportHeight = _viewport.w > 0.f ? _viewport.w : 600.f;

    auto unitSize = std::abs(2.0f * std::tan(0.5f * _fov) * targetDistance / viewportHeight);

    requiredPrecisionLevel = std::log2(glm::length(worldMaxBound - worldMinBound) / (unitSize * popErrorBound));

    auto ceiledRequiredPrecisionLevel = static_cast<int>(std::ceil(requiredPrecisionLevel));

    auto lod = ProgressiveOrderedMeshLodInfo();
    auto requiredLod = findClosestLodByPrecisionLevel(resource, ceiledRequiredPrecisionLevel, lod) ? lod._level : DEFAULT_LOD;

    return requiredLod;
}

float
POPGeometryLodScheduler::computeLodPriority(const POPGeometryResourceInfo& 	resource,
                                            SurfaceInfo&                    surfaceInfo,
											int 							requiredLod,
											int 							activeLod)
{
    // TODO make abstraction for priority contribution factors
    // apply frustum culling factor
    // apply occlusion culling factor (BSP-tree?)

    if (activeLod >=  requiredLod)
        return 0.f;

    // TODO provide streaming API priority computing function

    auto priority = 0.f;

    const auto distanceFromEye = this->distanceFromEye(resource, surfaceInfo, _eyePosition);
    const auto distanceFactor = (1000.f - distanceFromEye) * 0.001f;

    if (activeLod < 4)
    {
        priority += 11.f + distanceFactor;
    }
    else if (activeLod < 7)
    {
        priority += 9.f + distanceFactor;
    }
    else
    {
        priority += 1.f + distanceFactor;
    }

    return priority;
}

bool 
POPGeometryLodScheduler::findClosestValidLod(const POPGeometryResourceInfo& resource,
											 int 							lod,
											 ProgressiveOrderedMeshLodInfo& result) const
{
	auto data = resource.base->data;

    const auto& lods = data->get<std::map<int, ProgressiveOrderedMeshLodInfo>>("availableLods");

    if (lods.empty())
        return false;

    auto validLods = std::map<int, ProgressiveOrderedMeshLodInfo>();

    for (const auto& lod : lods)
    {
        if (lod.second.isValid())
            validLods.insert(std::make_pair(lod.first, lod.second));
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
POPGeometryLodScheduler::findClosestLodByPrecisionLevel(const POPGeometryResourceInfo& 	resource,
								 					    int 							precisionLevel,
								 					    ProgressiveOrderedMeshLodInfo& 	result) const
{
	auto data = resource.base->data;

    const auto& lods = data->get<std::map<int, ProgressiveOrderedMeshLodInfo>>("availableLods");

    if (lods.empty())
        return false;

    for (const auto& precisionLevelToLodPair : lods)
    {
        const auto& lod = precisionLevelToLodPair.second;
        
        if (lod._precisionLevel >= precisionLevel)
        {
            result = lod;

            return true;
        }
    }

    result = lods.rbegin()->second;

    return true;
}

float
POPGeometryLodScheduler::distanceFromEye(const POPGeometryResourceInfo&  resource,
                                         SurfaceInfo&                    surfaceInfo,
                                         const math::vec3&               eyePosition)
{
    auto target = surfaceInfo.surface->target();
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

void
POPGeometryLodScheduler::requiredPrecisionLevelChanged(const POPGeometryResourceInfo&    resource,
                                                       SurfaceInfo&                      surfaceInfo)
{
}

bool
POPGeometryLodScheduler::blendingIsActive(const POPGeometryResourceInfo&    resource,
                                          SurfaceInfo&                      surfaceInfo)
{
    return _blendingRange > 0.f;
}

void
POPGeometryLodScheduler::updateBlendingLod(const POPGeometryResourceInfo&    resource,
                                           SurfaceInfo&                      surfaceInfo)
{
    surfaceInfo.surface->geometry()->data()->set("popBlendingLod", blendingLod(resource, surfaceInfo));
}

void
POPGeometryLodScheduler::blendingRange(float value)
{
    if (_blendingRange == value)
        return;

    _blendingRange = value;

    for (auto& uuidToResourcePair : _popGeometryResources)
    {
        auto& resource = uuidToResourcePair.second;

        for (auto& surfaceToSurfaceInfoPair : resource.surfaceToSurfaceInfoMap)
        {
            auto& surfaceInfo = surfaceToSurfaceInfoPair.second;

            blendingRangeChanged(resource, surfaceInfo, _blendingRange);
        }
    }
}

void
POPGeometryLodScheduler::blendingRangeChanged(const POPGeometryResourceInfo&    resource,
                                              SurfaceInfo&                      surfaceInfo,
                                              float                             blendingRange)
{
    auto geometry = surfaceInfo.surface->geometry();

    if (_blendingRange > 0.f)
    {
        geometry->data()->set("popBlendingLod", blendingLod(resource, surfaceInfo));
        geometry->data()->set("popBlendingEnabled", true);
    }
    else
    {
        geometry->data()->unset("popBlendingEnabled");
    }
}

float
POPGeometryLodScheduler::blendingLod(const POPGeometryResourceInfo&    resource,
                                     SurfaceInfo&                      surfaceInfo) const
{
    const auto requiredPrecisionLevel = surfaceInfo.requiredPrecisionLevel;

    auto blendingLod = std::max<float>(resource.minAvailableLod, requiredPrecisionLevel);

    blendingLod = std::min<float>(resource.maxAvailableLod, blendingLod);

    if (requiredPrecisionLevel >= float(resource.maxLod + 1))
        blendingLod = resource.fullPrecisionLod;

    return blendingLod;
}
