#include "Matrix4x4TimelineTest.hpp"
#include "minko/MinkoTests.hpp"

using namespace minko;
using namespace minko::math;
using namespace minko::animation;

bool areMatricesEqual(const mat4& m1, const mat4& m2) {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (!epsilonEqual(m1[i][j], m2[i][j], 1.0e-5f))
                return false;
    return true;
}

TEST_F(Matrix4x4TimelineTest, InterpolateTranslation)
{
    const auto ident = mat4(1.0f);
    auto timeline = Matrix4x4Timeline::create(
        "test",
        4,
        { 0, 4 },
        {
            ident,
            translate(ident, vec3(1.0f, 2.0f, 3.0f)),
        },
        true
    );

    ASSERT_TRUE(areMatricesEqual(timeline->interpolate(0), ident));
    ASSERT_TRUE(areMatricesEqual(timeline->interpolate(1), translate(ident, vec3(0.25f, 0.5f, 0.75f))));
    ASSERT_TRUE(areMatricesEqual(timeline->interpolate(2), translate(ident, vec3(0.5f, 1.0f, 1.5f))));
    ASSERT_TRUE(areMatricesEqual(timeline->interpolate(3), translate(ident, vec3(0.75f, 1.5f, 2.25f))));
    ASSERT_TRUE(areMatricesEqual(timeline->interpolate(4), translate(ident, vec3(1.0f, 2.0f, 3.0f))));
}

TEST_F(Matrix4x4TimelineTest, InterpolateRotation)
{
    const auto ident = mat4(1.0f);
    auto timeline = Matrix4x4Timeline::create(
        "test",
        4,
        { 0, 4 },
        {
            ident,
            rotate(ident, radians(180.0f), vec3(1.0f, 0.0f, 0.0f)),
        },
        true
    );

    ASSERT_TRUE(areMatricesEqual(timeline->interpolate(0), ident));
    ASSERT_TRUE(areMatricesEqual(timeline->interpolate(1), mat4({
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, -0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    })));
    ASSERT_TRUE(areMatricesEqual(timeline->interpolate(2), mat4({
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    })));
    ASSERT_TRUE(areMatricesEqual(timeline->interpolate(3), mat4({
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f, -0.0f, 0.0f, 0.0f, 0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    })));
    ASSERT_TRUE(areMatricesEqual(timeline->interpolate(4), mat4({
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    })));
}
