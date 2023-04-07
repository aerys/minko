/*
Copyright (c) 2023 Aerys

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

#include "MaterialTest.hpp"

#include "minko/MinkoTests.hpp"
#include "minko/render/Priority.hpp"

using namespace minko;
using namespace minko::material;
using namespace minko::render;

void
assertDefaultValues(const Material::Ptr m)
{
    ASSERT_FALSE(m->data()->hasProperty(States::targetPropertyName()));

    ASSERT_EQ(States::priorityDefaultValue(), m->priority());
    ASSERT_EQ(States::zSortedDefaultValue(), m->zSorted());
    ASSERT_EQ(States::blendingSourceDefaultValue(), m->blendingSource());
    ASSERT_EQ(States::blendingDestinationDefaultValue(), m->blendingDestination());
    ASSERT_EQ(States::colorMaskDefaultValue(), m->colorMask());
    ASSERT_EQ(States::depthMaskDefaultValue(), m->depthMask());
    ASSERT_EQ(States::depthFunctionDefaultValue(), m->depthFunction());
    ASSERT_EQ(States::triangleCullingDefaultValue(), m->triangleCulling());
    ASSERT_EQ(States::stencilFunctionDefaultValue(), m->stencilFunction());
    ASSERT_EQ(States::stencilReferenceDefaultValue(), m->stencilReference());
    ASSERT_EQ(States::stencilMaskDefaultValue(), m->stencilMask());
    ASSERT_EQ(States::stencilFailOperationDefaultValue(), m->stencilFailOperation());
    ASSERT_EQ(States::stencilZFailOperationDefaultValue(), m->stencilZFailOperation());
    ASSERT_EQ(States::stencilZPassOperationDefaultValue(), m->stencilZPassOperation());

    ASSERT_EQ(States::priorityDefaultValue(), m->data()->get<float>(States::priorityPropertyName()));
    ASSERT_EQ(States::zSortedDefaultValue(), m->data()->get<bool>(States::zSortedPropertyName()));
    ASSERT_EQ(States::blendingSourceDefaultValue(), m->data()->get<Blending::Source>(States::blendingSourcePropertyName()));
    ASSERT_EQ(States::blendingDestinationDefaultValue(), m->data()->get<Blending::Destination>(States::blendingDestinationPropertyName()));
    ASSERT_EQ(States::colorMaskDefaultValue(), m->data()->get<bool>(States::colorMaskPropertyName()));
    ASSERT_EQ(States::depthMaskDefaultValue(), m->data()->get<bool>(States::depthMaskPropertyName()));
    ASSERT_EQ(States::depthFunctionDefaultValue(), m->data()->get<CompareMode>(States::depthFunctionPropertyName()));
    ASSERT_EQ(States::triangleCullingDefaultValue(), m->data()->get<TriangleCulling>(States::triangleCullingPropertyName()));
    ASSERT_EQ(States::stencilFunctionDefaultValue(), m->data()->get<CompareMode>(States::stencilFunctionPropertyName()));
    ASSERT_EQ(States::stencilReferenceDefaultValue(), m->data()->get<int>(States::stencilReferencePropertyName()));
    ASSERT_EQ(States::stencilMaskDefaultValue(), m->data()->get<uint>(States::stencilMaskPropertyName()));
    ASSERT_EQ(States::stencilFailOperationDefaultValue(), m->data()->get<StencilOperation>(States::stencilFailOperationPropertyName()));
    ASSERT_EQ(States::stencilZFailOperationDefaultValue(), m->data()->get<StencilOperation>(States::stencilZFailOperationPropertyName()));
    ASSERT_EQ(States::stencilZPassOperationDefaultValue(), m->data()->get<StencilOperation>(States::stencilZPassOperationPropertyName()));
    ASSERT_EQ(States::scissorTestDefaultValue(), m->data()->get<bool>(States::scissorTestPropertyName()));
    ASSERT_EQ(States::scissorBoxDefaultValue(), m->data()->get<math::ivec4>(States::scissorBoxPropertyName()));
}

TEST_F(MaterialTest, Create)
{
    try
    {
        const auto m = Material::create();

        assertDefaultValues(m);
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }
}

TEST_F(MaterialTest, CreateCopy)
{
    try
    {
        const auto m1 = Material::create();
        const auto m2 = Material::create(m1);

        assertDefaultValues(m1);
        assertDefaultValues(m2);
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }

    try
    {
        const auto p = States::priorityDefaultValue() + 1000.0f;
        const auto m1 = Material::create()
            ->priority(p);
        const auto m2 = Material::create(m1);

        ASSERT_EQ(p, m1->priority());
        ASSERT_EQ(p, m2->priority());
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }
}

