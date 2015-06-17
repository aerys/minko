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

#include "minko/component/AbstractLodPriorityModifier.hpp"
#include "minko/component/Surface.hpp"

using namespace minko;
using namespace minko::component;

AbstractLodPriorityModifier::AbstractLodPriorityModifier() :
    _refreshRate(0.f),
    _refreshTime(0.f),
    _previousValue(0.f),
    _targetValue(0.f),
    _previousTime(0.f)
{
}

void
AbstractLodPriorityModifier::initialize(Surface::Ptr surface)
{
    doInitialize(surface);
}

float
AbstractLodPriorityModifier::value(Surface::Ptr        surface,
                                   const math::vec3&   eyePosition,
                                   const math::vec4&   viewport,
                                   const math::mat4&   worldToScreenMatrix,
                                   const math::mat4&   viewMatrix,
                                   float               time)
{
    if (refreshRate() <= 0.f)
    {
        _previousValue = _targetValue;
        
        _targetValue = computeValue(surface, eyePosition, viewport, worldToScreenMatrix, viewMatrix, time);

        return _targetValue;
    }

    auto elapsedTime = time - _previousTime;

    if (elapsedTime >= _refreshTime)
    {
        elapsedTime = elapsedTime - _refreshTime;

        _previousTime = time;

        _previousValue = _targetValue;

        _targetValue = computeValue(surface, eyePosition, viewport, worldToScreenMatrix, viewMatrix, time);
    }

    return math::mix(_previousValue, _targetValue, elapsedTime / _refreshTime);
}
