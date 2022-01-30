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

    ASSERT_EQ(mat->priority(), States::priorityDefaultValue());
    ASSERT_EQ(mat->data()->get<float>(States::priorityPropertyName()), States::priorityDefaultValue());
}

TEST_F(BasicMaterialTest, ZSortedDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->zSorted(), States::zSortedDefaultValue());
    ASSERT_EQ(mat->data()->get<bool>(States::zSortedPropertyName()), States::zSortedDefaultValue());
}

TEST_F(BasicMaterialTest, BlendingSourceDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->blendingSource(), States::blendingSourceDefaultValue());
    ASSERT_EQ(mat->data()->get<Blending::Source>(States::blendingSourcePropertyName()), States::blendingSourceDefaultValue());
}

TEST_F(BasicMaterialTest, BlendingDestinationDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->blendingDestination(), States::blendingDestinationDefaultValue());
    ASSERT_EQ(mat->data()->get<Blending::Destination>(States::blendingDestinationPropertyName()), States::blendingDestinationDefaultValue());
}

TEST_F(BasicMaterialTest, ColorMaskDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->colorMask(), States::colorMaskDefaultValue());
    ASSERT_EQ(mat->data()->get<bool>(States::colorMaskPropertyName()), States::colorMaskDefaultValue());
}

TEST_F(BasicMaterialTest, DepthMaskDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->depthMask(), States::depthMaskDefaultValue());
    ASSERT_EQ(mat->data()->get<bool>(States::depthMaskPropertyName()), States::depthMaskDefaultValue());
}

TEST_F(BasicMaterialTest, DepthFunctionDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->depthFunction(), States::depthFunctionDefaultValue());
    ASSERT_EQ(mat->data()->get<CompareMode>(States::depthFunctionPropertyName()), States::depthFunctionDefaultValue());
}

TEST_F(BasicMaterialTest, TriangleCullingDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->triangleCulling(), States::triangleCullingDefaultValue());
    ASSERT_EQ(mat->data()->get<TriangleCulling>(States::triangleCullingPropertyName()), States::triangleCullingDefaultValue());
}

TEST_F(BasicMaterialTest, StencilFunctionDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->stencilFunction(), States::stencilFunctionDefaultValue());
    ASSERT_EQ(mat->data()->get<CompareMode>(States::stencilFunctionPropertyName()), States::stencilFunctionDefaultValue());
}

TEST_F(BasicMaterialTest, StencilReferenceDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->stencilReference(), States::stencilReferenceDefaultValue());
    ASSERT_EQ(mat->data()->get<int>(States::stencilReferencePropertyName()), States::stencilReferenceDefaultValue());
}

TEST_F(BasicMaterialTest, StencilMaskDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->stencilMask(), States::stencilMaskDefaultValue());
    ASSERT_EQ(mat->data()->get<uint>(States::stencilMaskPropertyName()), States::stencilMaskDefaultValue());
}

TEST_F(BasicMaterialTest, StencilFailOperationDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->stencilFailOperation(), States::stencilFailOperationDefaultValue());
    ASSERT_EQ(mat->data()->get<StencilOperation>(States::stencilFailOperationPropertyName()), States::stencilFailOperationDefaultValue());
}

TEST_F(BasicMaterialTest, StencilZFailOperationDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->stencilZFailOperation(), States::stencilZFailOperationDefaultValue());
    ASSERT_EQ(mat->data()->get<StencilOperation>(States::stencilZFailOperationPropertyName()), States::stencilZFailOperationDefaultValue());
}

TEST_F(BasicMaterialTest, StencilZPassOperationDefaultValue)
{
    auto mat = BasicMaterial::create();

    ASSERT_EQ(mat->stencilZPassOperation(), States::stencilZPassOperationDefaultValue());
    ASSERT_EQ(mat->data()->get<StencilOperation>(States::stencilZPassOperationPropertyName()), States::stencilZPassOperationDefaultValue());
}

TEST_F(BasicMaterialTest, NoTargetProperty)
{
    auto mat = BasicMaterial::create();

    ASSERT_FALSE(mat->data()->hasProperty(States::targetPropertyName()));
}
