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
#include "minko/component/PerspectiveCamera.hpp"
#include "minko/component/ScreenAreaLodPriorityModifier.hpp"
#include "minko/component/Surface.hpp"
#include "minko/component/Transform.hpp"
#include "minko/math/Box.hpp"

using namespace minko;
using namespace minko::component;

ScreenAreaLodPriorityModifier::ScreenAreaLodPriorityModifier() :
    AbstractLodPriorityModifier(),
    _boxVertices()
{
}

void
ScreenAreaLodPriorityModifier::doInitialize(Surface::Ptr surface)
{
    auto box = surface->target()->component<BoundingBox>()->box();

    const auto bottomLeft = box->bottomLeft();
    const auto topRight = box->topRight();

    const auto width = box->width();
    const auto height = box->height();
    const auto depth = box->depth();

	_boxVertices = std::array<math::vec3, 8>
	{{
		math::vec3(bottomLeft),
		math::vec3(bottomLeft.x + width, bottomLeft.y, bottomLeft.z),
		math::vec3(bottomLeft.x + width, bottomLeft.y + height, bottomLeft.z),
		math::vec3(bottomLeft.x, bottomLeft.y + height, bottomLeft.z),

		math::vec3(bottomLeft.x, bottomLeft.y, bottomLeft.z + depth),
		math::vec3(bottomLeft.x + width, bottomLeft.y, bottomLeft.z + depth),
		math::vec3(bottomLeft.x + width, bottomLeft.y + height, bottomLeft.z + depth),
		math::vec3(bottomLeft.x, bottomLeft.y + height, bottomLeft.z + depth),
	}};
}

float
ScreenAreaLodPriorityModifier::accurateScreenArea(const math::mat4& modelToWorld,
                                                  const math::vec3& eyePosition,
                                                  const math::vec4& viewport,
                                                  const math::mat4& worldToScreenMatrix,
                                                  const math::mat4& viewMatrix) const
{
    // ref: http://www.cg.tuwien.ac.at/research/publications/1999/Fuhr-1999-Conc/TR-186-2-99-05Paper.pdf

    static const int hullVertexData[64][7] =
    {
        0, -1, -1, -1, -1, -1, -1,
        4, 0,  4,  7,  3,  -1, -1,
        4, 1,  2,  6,  5,  -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        4, 0,  1,  5,  4,  -1, -1,
        6, 0,  1,  5,  4,  7,  3,
        6, 0,  1,  2,  6,  5,  4,
        0, -1, -1, -1, -1, -1, -1,
        4, 2,  3,  7,  6,  -1, -1,
        6, 4,  7,  6,  2,  3,  0,
        6, 2,  3,  7,  6,  5,  1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        4,  0, 3,  2,  1,  -1, -1,
        6,  0, 4,  7,  3,  2,  1,
        6,  0, 3,  2,  6,  5,  1,
        0, -1, -1, -1, -1, -1, -1,
        6, 0,  3,  2,  1,  5,  4,
        6, 2,  1,  5,  4,  7,  3,
        6, 0,  3,  2,  6,  5,  4,
        0, -1, -1, -1, -1, -1, -1,
        6, 0,  3,  7,  6,  2,  1,
        6, 0,  4,  7,  6,  2,  1,
        6, 0,  3,  7,  6,  5,  1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        4, 4,  5,  6,  7,  -1, -1,
        6, 4,  5,  6,  7,  3,  0,
        6, 1,  2,  6,  7,  4,  5,
        0, -1, -1, -1, -1, -1, -1,
        6, 0,  1,  5,  6,  7,  4,
        6, 0,  1,  5,  6,  7,  3,
        6, 0,  1,  2,  6,  7,  4,
        0, -1, -1, -1, -1, -1, -1,
        6, 2,  3,  7,  4,  5,  6,
        6, 0,  4,  5,  6,  2,  3,
        6, 1,  2,  3,  7,  4,  5,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1,
        0, -1, -1, -1, -1, -1, -1
    };

    const auto viewportWidth = viewport.z;
    const auto viewportHeight = viewport.w;

    auto projectedVertices = std::array<math::vec3, 8>();

    auto area = 0.f;

    const auto position =
        ((eyePosition.x < _boxVertices[0].x)) +
        ((eyePosition.x > _boxVertices[6].x) << 1) +
        ((eyePosition.y < _boxVertices[0].y) << 2) +
        ((eyePosition.y > _boxVertices[6].y) << 3) +
        ((eyePosition.z < _boxVertices[0].z) << 4) +
        ((eyePosition.z > _boxVertices[6].z) << 5);

    if (position == 0)
        return viewportWidth * viewportHeight;

    const auto numVertices = hullVertexData[position][0];

    if (numVertices == 0)
        return 0.f;

    for (auto i = 0u; i < numVertices; ++i)
    {
        const auto projectedVertex = PerspectiveCamera::project(
            _boxVertices[hullVertexData[position][i + 1]],
            viewportWidth,
            viewportHeight,
            viewMatrix,
            worldToScreenMatrix
        );

        projectedVertices[i] = math::clamp(
            projectedVertex,
            math::vec3(viewport.x, viewport.y, 0.f),
            math::vec3(viewport.z, viewport.w, 0.f)
        );
    }

    for (auto i = 0u; i < numVertices; ++i)
        area += (projectedVertices[i].x - projectedVertices[(i + 1) % numVertices].x) *
                (projectedVertices[i].y + projectedVertices[(i + 1) % numVertices].y);

    return math::abs(area) * 0.5f;
}

float
ScreenAreaLodPriorityModifier::aabbApproxScreenArea(const math::mat4& modelToWorld,
                                                    const math::vec3& eyePosition,
                                                    const math::vec4& viewport,
                                                    const math::mat4& worldToScreenMatrix,
                                                    const math::mat4& viewMatrix) const
{
    const auto viewportWidth = viewport.z;
    const auto viewportHeight = viewport.w;

    auto projectedVertices = std::array<math::vec3, 8>();

    auto zSignChanged = -1;
    auto positiveZ = true;

    for (auto i = 0u; i < 8u; ++i)
    {
        const auto projectedVertex = PerspectiveCamera::project(
            _boxVertices[i],
            viewportWidth,
            viewportHeight,
            viewMatrix,
            worldToScreenMatrix
        );

        projectedVertices[i] = math::clamp(
            projectedVertex,
            math::vec3(viewport.x, viewport.y, projectedVertex.z),
            math::vec3(viewport.z, viewport.w, projectedVertex.z)
        );

        if (zSignChanged == -1)
        {
            positiveZ = projectedVertices[i].z >= 0.f;

            ++zSignChanged;
        }
        else
        {
            const auto localPositiveZ = projectedVertices[i].z >= 0.f;

            if (positiveZ != localPositiveZ)
                ++zSignChanged;
        }
    }

    if (zSignChanged > 0)
    {
        return viewportWidth * viewportHeight;
    }

    if (!positiveZ)
    {
        return 0.f;
    }

    auto boxScreenBottomLeft = math::vec2(std::numeric_limits<float>::max());
    auto boxScreenTopRight = math::vec2(-std::numeric_limits<float>::max());

    for (const auto& vertex : projectedVertices)
    {
        boxScreenBottomLeft = math::min(boxScreenBottomLeft, vertex.xy());
        boxScreenTopRight = math::max(boxScreenTopRight, vertex.xy());
    }

    const auto boxScreenSize = boxScreenTopRight - boxScreenBottomLeft;

    return boxScreenSize.x * boxScreenSize.y;
}

float
ScreenAreaLodPriorityModifier::computeValue(Surface::Ptr        surface,
                                            const math::vec3&   eyePosition,
                                            const math::vec4&   viewport,
                                            const math::mat4&   worldToScreenMatrix,
                                            const math::mat4&   viewMatrix,
                                            float               time)
{

    const auto& modelToWorldMatrix = surface->target()->component<Transform>()->modelToWorldMatrix(true);

    const auto screenArea = this->aabbApproxScreenArea(
        modelToWorldMatrix,
        eyePosition,
        viewport,
        worldToScreenMatrix,
        viewMatrix
    );

    const auto maxScreenArea = viewport.z * viewport.w;

    const auto screenAreaFactor = screenArea / maxScreenArea;

    return screenAreaFactor;
}
