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

#include "minko/component/MasterAnimation.hpp"
#include "minko/component/Animation.hpp"

using namespace minko;
using namespace minko::component;

MasterAnimation::MasterAnimation(const std::vector<AnimationPtr>& animations, 
								 bool isLooping) :
	AbstractAnimation(isLooping),
	_animations(animations)
{
}

/*virtual*/
void
MasterAnimation::initialize()
{
	AbstractAnimation::initialize();
	
	_maxTime = 0;

	for (auto& animation : _animations)
	{
		animation->_master = std::static_pointer_cast<MasterAnimation>(shared_from_this());
		_maxTime = std::max(_maxTime, animation->_maxTime);
	}

	setPlaybackWindow(0, _maxTime)->seek(0)->play();
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::play()
{
	AbstractAnimation::play();

	for (auto& animation : _animations)
		animation->play();

	return std::static_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::stop()
{
	AbstractAnimation::stop();

	for (auto& animation : _animations)
		animation->stop();

	return std::static_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::addLabel(const std::string& name, uint time)
{
	AbstractAnimation::addLabel(name, time);

	for (auto& animation : _animations)
		animation->addLabel(name, time);

	return std::static_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::changeLabel(const std::string& name, const std::string& newName)
{
	AbstractAnimation::changeLabel(name, newName);

	for (auto& animation : _animations)
		animation->changeLabel(name, newName);

	return std::static_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::setTimeForLabel(const std::string& name, uint newTime)
{
	AbstractAnimation::setTimeForLabel(name, newTime);

	for (auto& animation : _animations)
		animation->setTimeForLabel(name, newTime);

	return std::static_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::removeLabel(const std::string& name)
{
	AbstractAnimation::removeLabel(name);

	for (auto& animation : _animations)
		animation->removeLabel(name);

	return std::static_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::setPlaybackWindow(uint beginTime, 
								   uint endTime, 
								   bool forceRestart)
{
	AbstractAnimation::setPlaybackWindow(beginTime, endTime, forceRestart);

	for (auto& animation : _animations)
		animation->setPlaybackWindow(beginTime, endTime, forceRestart);

	return std::static_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::setPlaybackWindow(const std::string&	beginLabelName, 
								   const std::string&	endLabelName, 
								   bool					forceRestart)
{
	AbstractAnimation::setPlaybackWindow(beginLabelName, endLabelName, forceRestart);

	for (auto& animation : _animations)
		animation->setPlaybackWindow(beginLabelName, endLabelName, forceRestart);

	return std::static_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::resetPlaybackWindow()
{
	AbstractAnimation::resetPlaybackWindow();

	for (auto& animation : _animations)
		animation->resetPlaybackWindow();

	return std::static_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
void
MasterAnimation::update()
{
	for (auto& animation : _animations)
	{
		animation->_currentTime = _currentTime;
		animation->update();
	}
}