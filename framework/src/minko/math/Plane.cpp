#include "minko/math/Plane.hpp"
#include "minko/math/Ray.hpp"

using namespace minko;
using namespace minko::math;

Plane::Plane(const math::vec3& point, const math::vec3& normal) :
    AbstractShape(),
    _point(point),
    _normal(normal)
{
}

bool
Plane::cast(std::shared_ptr<Ray> ray, float& distance)
{
    const auto denom = math::dot(_normal, ray->direction());
    if (math::abs(denom) > 1e-6f)
    {
        const auto v = _point - ray->origin();
        distance = math::dot(v, _normal) / denom;

        return distance >= 0.f;
    }

    return false;
}

ShapePosition
Plane::testBoundingBox(std::shared_ptr<math::Box> box)
{
    throw std::runtime_error("Unimplemented");
}

void
Plane::updateFromMatrix(const math::mat4& matrix)
{
    throw std::runtime_error("Unimplemented");
}
