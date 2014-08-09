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

#include "minko/component/Animation.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/animation/AbstractTimeline.hpp"
#include "minko/scene/Node.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::animation;

Animation::Animation(const std::vector<AbstractTimeline::Ptr>& timelines, 
					 bool                                      isLooping):
	AbstractAnimation(isLooping),
	_timelines(timelines),
	_master(nullptr)
{
	_maxTime = 0;
	for (auto& timeline : _timelines)
		_maxTime = std::max(_maxTime, timeline->duration());

	setPlaybackWindow(0, _maxTime)->seek(0);
}

void
Animation::update()
{
	for (auto& timeline : _timelines)
        timeline->update(_currentTime % (timeline->duration() + 1), target()->data());
}
