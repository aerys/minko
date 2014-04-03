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

#include "minko/input/Finger.hpp"

using namespace minko;
using namespace minko::input;

Finger::Finger(std::shared_ptr<AbstractCanvas> canvas) :
	_canvas(canvas),
    _x(0),
    _y(0),
    _dx(0),
    _dy(0),
	_fingerMotion(Signal<Ptr, float, float>::create()),
    _fingerDown(Signal<Ptr, float, float>::create()),
    _fingerUp(Signal<Ptr, float, float>::create()),
	_fingerId(0)
{
    _slots.push_front(_fingerDown->connect([&](Ptr finger, float x, float y)
    {
        _fingerIsDown = true;
    }));
    
	_slots.push_front(_fingerUp->connect([&](Ptr finger, float x, float y)
    {
        _fingerIsDown = false;
    }));
}