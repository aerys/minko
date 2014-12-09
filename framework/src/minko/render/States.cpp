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

#include "minko/render/States.hpp"

using namespace minko;
using namespace minko::render;

const std::string           States::PROPERTY_PRIORITY               = "priority";
const std::string           States::PROPERTY_ZSORTED                = "zSorted";
const std::string           States::PROPERTY_BLENDING_SOURCE        = "blendingSource";
const std::string           States::PROPERTY_BLENDING_DESTINATION   = "blendingDestination";
const std::string           States::PROPERTY_COLOR_MASK             = "colorMask";
const std::string           States::PROPERTY_DEPTH_MASK             = "depthMask";
const std::string           States::PROPERTY_DEPTH_FUNCTION         = "depthFunction";
const std::string           States::PROPERTY_TRIANGLE_CULLING       = "triangleCulling";
const std::string           States::PROPERTY_STENCIL_FUNCTION       = "stencilFunction";
const std::string           States::PROPERTY_STENCIL_REFERENCE      = "stencilReference";
const std::string           States::PROPERTY_STENCIL_MASK           = "stencilMask";
const std::string           States::PROPERTY_STENCIL_FAIL_OP        = "stencilFailOperation";
const std::string           States::PROPERTY_STENCIL_ZFAIL_OP       = "stencilZFailOperation";
const std::string           States::PROPERTY_STENCIL_ZPASS_OP       = "stencilZPassOperation";
const std::string           States::PROPERTY_SCISSOR_TEST           = "scissorTest";
const std::string           States::PROPERTY_SCISSOR_BOX            = "scissorBox";
const std::string           States::PROPERTY_TARGET                 = "target";

const float				    States::DEFAULT_PRIORITY                = 0.f;
const bool					States::DEFAULT_ZSORTED                 = false;
const Blending::Source		States::DEFAULT_BLENDING_SOURCE         = Blending::Source::ONE;
const Blending::Destination States::DEFAULT_BLENDING_DESTINATION    = Blending::Destination::ZERO;
const bool					States::DEFAULT_COLOR_MASK              = true;
const bool					States::DEFAULT_DEPTH_MASK              = true;
const CompareMode		    States::DEFAULT_DEPTH_FUNCTION          = CompareMode::LESS;
const TriangleCulling       States::DEFAULT_TRIANGLE_CULLING        = TriangleCulling::BACK;
const CompareMode			States::DEFAULT_STENCIL_FUNCTION        = CompareMode::ALWAYS;
const int					States::DEFAULT_STENCIL_REFERENCE       = 0;
const uint					States::DEFAULT_STENCIL_MASK            = 1;
const StencilOperation		States::DEFAULT_STENCIL_FAIL_OP         = StencilOperation::KEEP;
const StencilOperation		States::DEFAULT_STENCIL_ZFAIL_OP        = StencilOperation::KEEP;
const StencilOperation		States::DEFAULT_STENCIL_ZPASS_OP        = StencilOperation::KEEP;
const bool					States::DEFAULT_SCISSOR_TEST            = false;
const math::ivec4			States::DEFAULT_SCISSOR_BOX             = math::ivec4();
const States::AbsTexturePtr States::DEFAULT_TARGET                  = nullptr;

const std::array<std::string, 17> States::PROPERTY_NAMES = {
    PROPERTY_PRIORITY,
    PROPERTY_ZSORTED,
    PROPERTY_BLENDING_SOURCE,
    PROPERTY_BLENDING_DESTINATION,
    PROPERTY_COLOR_MASK,
    PROPERTY_DEPTH_MASK,
    PROPERTY_DEPTH_FUNCTION,
    PROPERTY_TRIANGLE_CULLING,
    PROPERTY_STENCIL_FUNCTION,
    PROPERTY_STENCIL_REFERENCE,
    PROPERTY_STENCIL_MASK,
    PROPERTY_STENCIL_FAIL_OP,
    PROPERTY_STENCIL_ZFAIL_OP,
    PROPERTY_STENCIL_ZPASS_OP,
    PROPERTY_SCISSOR_TEST,
    PROPERTY_SCISSOR_BOX,
    PROPERTY_TARGET
};

States::States(float					priority,
               bool						zSorted,
               Blending::Source			blendingSourceFactor,
               Blending::Destination	blendingDestinationFactor,
               bool						colorMask,
               bool						depthMask,
               CompareMode				depthFunc,
               TriangleCulling          triangleCulling,
               CompareMode				stencilFunction,
               int						stencilRef,
               uint						stencilMask,
               StencilOperation			stencilFailOp,
               StencilOperation			stencilZFailOp,
               StencilOperation			stencilZPassOp,
               bool						scissorTest,
               const math::ivec4&		scissorBox,
               AbsTexturePtr		    target) :
    _data(data::Provider::create())
{
    resetDefaultValues();

    // FIXME: set the state values
}

States::States(std::shared_ptr<data::Provider> data) :
    _data(data)
{}

States::States(const States& states) :
    _data(data::Provider::create(states._data))
{
}

void
States::resetDefaultValues()
{
    _data->set(PROPERTY_PRIORITY, DEFAULT_PRIORITY);
    _data->set(PROPERTY_ZSORTED, DEFAULT_ZSORTED);
    _data->set(PROPERTY_BLENDING_SOURCE, DEFAULT_BLENDING_SOURCE);
    _data->set(PROPERTY_BLENDING_DESTINATION, DEFAULT_BLENDING_DESTINATION);
    _data->set(PROPERTY_COLOR_MASK, DEFAULT_COLOR_MASK);
    _data->set(PROPERTY_DEPTH_MASK, DEFAULT_DEPTH_MASK);
    _data->set(PROPERTY_DEPTH_FUNCTION, DEFAULT_DEPTH_FUNCTION);
    _data->set(PROPERTY_TRIANGLE_CULLING, DEFAULT_TRIANGLE_CULLING);
    _data->set(PROPERTY_STENCIL_FUNCTION, DEFAULT_STENCIL_FUNCTION);
    _data->set(PROPERTY_STENCIL_REFERENCE, DEFAULT_STENCIL_REFERENCE);
    _data->set(PROPERTY_STENCIL_MASK, DEFAULT_STENCIL_MASK);
    _data->set(PROPERTY_STENCIL_FAIL_OP, DEFAULT_STENCIL_FAIL_OP);
    _data->set(PROPERTY_STENCIL_ZFAIL_OP, DEFAULT_STENCIL_ZFAIL_OP);
    _data->set(PROPERTY_STENCIL_ZPASS_OP, DEFAULT_STENCIL_ZPASS_OP);
    _data->set(PROPERTY_SCISSOR_TEST, DEFAULT_SCISSOR_TEST);
    _data->set(PROPERTY_SCISSOR_BOX, DEFAULT_SCISSOR_BOX);
    _data->unset(PROPERTY_TARGET);
}
