/*
Copyright (c) 2022 Aerys

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

#include "BasicMaterialTest.hpp"

#include "minko/MinkoTests.hpp"

using namespace minko;
using namespace minko::material;
using namespace minko::render;

TEST_F(BasicMaterialTest, Create)
{
    try
    {
        auto m = BasicMaterial::create();
    }
    catch (...)
    {
        ASSERT_TRUE(false);
    }
}

TEST_F(BasicMaterialTest, PriorityDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->priority(), States::DEFAULT_PRIORITY);
    ASSERT_EQ(mat->data()->get<float>(States::PROPERTY_PRIORITY), States::DEFAULT_PRIORITY);
}

TEST_F(BasicMaterialTest, ZSortedDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->zSorted(), States::DEFAULT_ZSORTED);
    ASSERT_EQ(mat->data()->get<bool>(States::PROPERTY_ZSORTED), States::DEFAULT_ZSORTED);
}

TEST_F(BasicMaterialTest, BlendingSourceFactorDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->blendingSourceFactor(), States::DEFAULT_BLENDING_SOURCE);
    ASSERT_EQ(mat->data()->get<Blending::Source>(States::PROPERTY_BLENDING_SOURCE), States::DEFAULT_BLENDING_SOURCE);
}

TEST_F(BasicMaterialTest, BlendingDestinationFactorDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->blendingDestinationFactor(), States::DEFAULT_BLENDING_DESTINATION);
    ASSERT_EQ(mat->data()->get<Blending::Destination>(States::PROPERTY_BLENDING_DESTINATION), States::DEFAULT_BLENDING_DESTINATION);
}

TEST_F(BasicMaterialTest, ColorMaskDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->colorMask(), States::DEFAULT_COLOR_MASK);
    ASSERT_EQ(mat->data()->get<bool>(States::PROPERTY_COLOR_MASK), States::DEFAULT_COLOR_MASK);
}

TEST_F(BasicMaterialTest, DepthMaskDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->depthMask(), States::DEFAULT_DEPTH_MASK);
    ASSERT_EQ(mat->data()->get<bool>(States::PROPERTY_DEPTH_MASK), States::DEFAULT_DEPTH_MASK);
}

TEST_F(BasicMaterialTest, DepthFunctionDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->depthFunction(), States::DEFAULT_DEPTH_FUNCTION);
    ASSERT_EQ(mat->data()->get<CompareMode>(States::PROPERTY_DEPTH_FUNCTION), States::DEFAULT_DEPTH_FUNCTION);
}

TEST_F(BasicMaterialTest, TriangleCullingDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->triangleCulling(), States::DEFAULT_TRIANGLE_CULLING);
    ASSERT_EQ(mat->data()->get<TriangleCulling>(States::PROPERTY_TRIANGLE_CULLING), States::DEFAULT_TRIANGLE_CULLING);
}

TEST_F(BasicMaterialTest, StencilFunctionDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->stencilFunction(), States::DEFAULT_STENCIL_FUNCTION);
    ASSERT_EQ(mat->data()->get<CompareMode>(States::PROPERTY_STENCIL_FUNCTION), States::DEFAULT_STENCIL_FUNCTION);
}

TEST_F(BasicMaterialTest, StencilReferenceDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->stencilReference(), States::DEFAULT_STENCIL_REFERENCE);
    ASSERT_EQ(mat->data()->get<int>(States::PROPERTY_STENCIL_REFERENCE), States::DEFAULT_STENCIL_REFERENCE);
}

TEST_F(BasicMaterialTest, StencilMaskDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->stencilMask(), States::DEFAULT_STENCIL_MASK);
    ASSERT_EQ(mat->data()->get<uint>(States::PROPERTY_STENCIL_MASK), States::DEFAULT_STENCIL_MASK);
}

TEST_F(BasicMaterialTest, StencilFailOperationDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->stencilFailOperation(), States::DEFAULT_STENCIL_FAIL_OPERATION);
    ASSERT_EQ(mat->data()->get<StencilOperation>(States::PROPERTY_STENCIL_FAIL_OPERATION), States::DEFAULT_STENCIL_FAIL_OPERATION);
}

TEST_F(BasicMaterialTest, StencilZFailOperationDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->stencilZFailOperation(), States::DEFAULT_STENCIL_ZFAIL_OPERATION);
    ASSERT_EQ(mat->data()->get<StencilOperation>(States::PROPERTY_STENCIL_ZFAIL_OPERATION), States::DEFAULT_STENCIL_ZFAIL_OPERATION);
}

TEST_F(BasicMaterialTest, StencilZPassOperationDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->stencilZPassOperation(), States::DEFAULT_STENCIL_ZPASS_OPERATION);
    ASSERT_EQ(mat->data()->get<StencilOperation>(States::PROPERTY_STENCIL_ZPASS_OPERATION), States::DEFAULT_STENCIL_ZPASS_OPERATION);
}

TEST_F(BasicMaterialTest, NoTargetProperty)
{
    auto mat = BasicMaterial::create();

    ASSERT_FALSE(mat->data()->hasProperty(States::PROPERTY_TARGET));
}
