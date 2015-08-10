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
#include "minko/component/Skinning.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"

using namespace minko;
using namespace minko::scene;
using namespace minko::component;

MasterAnimation::MasterAnimation(bool isLooping) :
	AbstractAnimation(isLooping),
	_animations()
{
	/*
	_maxTime = 0;

	for (auto& animation : _animations)
	{
		animation->_master = std::dynamic_pointer_cast<MasterAnimation>(shared_from_this());
		_maxTime = std::max(_maxTime, animation->_maxTime);
	}

	setPlaybackWindow(0, _maxTime)->seek(0)->play();
	*/
}

MasterAnimation::MasterAnimation(const MasterAnimation& masterAnim, const CloneOption& option) :
	AbstractAnimation(masterAnim, option),
	_animations()
{
}

AbstractComponent::Ptr
MasterAnimation::clone(const CloneOption& option)
{
	auto anim = std::shared_ptr<MasterAnimation>(new MasterAnimation(*this, option));

	return anim;
}

void
MasterAnimation::targetAdded(Node::Ptr target)
{
	_addedSlot = target->added().connect(std::bind(
		&MasterAnimation::addedHandler,
		std::dynamic_pointer_cast<MasterAnimation>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_removedSlot = target->removed().connect(std::bind(
		&MasterAnimation::removedHandler,
		std::dynamic_pointer_cast<MasterAnimation>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_target = target;
	
	initAnimations();
}

void
MasterAnimation::initAnimations()
{
    auto target = this->target();
    auto targetParent = target->parent();

    auto rootNode = targetParent != nullptr ? targetParent : target;

	auto descendants = NodeSet::create(rootNode)->descendants(true);
	for (auto descendant : descendants->nodes())
	{
        for (auto skinning : descendant->components<Skinning>())
            _animations.push_back(skinning);

        for (auto animation : descendant->components<Animation>())
            _animations.push_back(animation);
	}

	_maxTime = 0;

	for (auto& animation : _animations)
	{
		_maxTime = std::max(_maxTime, animation->maxTime());
	}

	setPlaybackWindow(0, _maxTime)->seek(0)->play();
}

void
MasterAnimation::targetRemoved(Node::Ptr target)
{
}

/*virtual*/
void
MasterAnimation::addedHandler(Node::Ptr node,
								Node::Ptr target,
								Node::Ptr parent)
{
	AbstractAnimation::addedHandler(node, target, parent);
}

/*virtual*/
void
MasterAnimation::removedHandler(Node::Ptr node,
									Node::Ptr target,
									Node::Ptr parent)
{
	AbstractAnimation::removedHandler(node, target, parent);
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::play()
{
	AbstractAnimation::play();

	for (auto& animation : _animations)
		animation->play();

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::stop()
{
	AbstractAnimation::stop();

	for (auto& animation : _animations)
		animation->stop();

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

AbstractAnimation::Ptr
MasterAnimation::seek(uint time)
{
    AbstractAnimation::seek(time);

	for (auto& animation : _animations)
		animation->seek(time);

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

AbstractAnimation::Ptr
MasterAnimation::seek(const std::string& labelName)
{
    return seek(labelTime(labelName));
}


/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::addLabel(const std::string& name, uint time)
{
	AbstractAnimation::addLabel(name, time);

	for (auto& animation : _animations)
    {
		animation->addLabel(name, time);
    }

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::changeLabel(const std::string& name, const std::string& newName)
{
	AbstractAnimation::changeLabel(name, newName);

	for (auto& animation : _animations)
		animation->changeLabel(name, newName);

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::setTimeForLabel(const std::string& name, uint newTime)
{
	AbstractAnimation::setTimeForLabel(name, newTime);

	for (auto& animation : _animations)
		animation->setTimeForLabel(name, newTime);

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::removeLabel(const std::string& name)
{
	AbstractAnimation::removeLabel(name);

	for (auto& animation : _animations)
		animation->removeLabel(name);

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
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

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
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

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
AbstractAnimation::Ptr
MasterAnimation::resetPlaybackWindow()
{
	AbstractAnimation::resetPlaybackWindow();

	for (auto& animation : _animations)
		animation->resetPlaybackWindow();

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

/*virtual*/
void
MasterAnimation::update()
{
	for (auto& animation : _animations)
	{
		auto anim = std::dynamic_pointer_cast<Animation>(animation);
		if (anim != nullptr) {
			anim->_currentTime = _currentTime;
			anim->update();
		}		
	}
}

void
MasterAnimation::rebindDependencies(std::map<AbstractComponent::Ptr, AbstractComponent::Ptr>& componentsMap, std::map<NodePtr, NodePtr>& nodeMap, CloneOption option)
{
	/*
	var newAnimations : Vector.<AnimationController> = new Vector.<AnimationController>();

	for (var i : int = 0; i < _animations.length; ++i)
	{
		var newController : AbstractAnimationController = controllerMap[_animations[i]] as AbstractAnimationController;
		if (newController)
			newAnimations.push(newController);
	}

	_animations = newAnimations;
	*/

	std::vector<AbstractAnimationPtr> newAnimations;

	for (auto animation : _animations) 
	{
		std::map<AbstractComponent::Ptr, AbstractComponent::Ptr>::iterator it;

		it = componentsMap.find(animation);
	
		if (it != componentsMap.end())
			newAnimations.push_back(animation);
	}

	_animations = newAnimations;
}