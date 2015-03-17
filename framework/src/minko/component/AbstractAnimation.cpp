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

#include <climits>

#include "minko/component/AbstractAnimation.hpp"
#include "minko/component/MasterAnimation.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/SceneManager.hpp"
#include "minko/CloneOption.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::scene;

AbstractAnimation::AbstractAnimation(bool isLooping):
	AbstractComponent(),
	_maxTime(0),
	_loopMinTime(0),
	_loopMaxTime(0),
	_loopTimeRange(0),
	_currentTime(0),
	_previousTime(0),
	_previousGlobalTime(0),
	_isPlaying(false),
	_isLooping(isLooping),
	_isReversed(false),
	_mustUpdateOnce(false),
	_clockStart(clock()),
	_timeFunction(),
	_labels(),
	_labelNameToIndex(),
	_nextLabelIds(),
	_sceneManager(nullptr),
	_started(Signal<Ptr>::create()),
	_looped(Signal<Ptr>::create()),
	_stopped(Signal<Ptr>::create()),
	_labelHit(Signal<Ptr, std::string, uint>::create()),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr),
	_frameBeginSlot(nullptr)
{
	_timeFunction = [](uint t) -> uint
	{
		return t;
	};
}

AbstractAnimation::AbstractAnimation(const AbstractAnimation& absAnimation, const CloneOption& option) :
	AbstractComponent(absAnimation._isLooping),
	_maxTime(absAnimation._maxTime),
	_loopMinTime(absAnimation._loopMinTime),
	_loopMaxTime(absAnimation._loopMaxTime),
	_loopTimeRange(absAnimation._loopTimeRange),
	_currentTime(0),
	_previousTime(0),
	_previousGlobalTime(0),
	_isPlaying(false),
	_isLooping(absAnimation._isLooping),
	_isReversed(absAnimation._isReversed),
	_mustUpdateOnce(absAnimation._mustUpdateOnce),
	_clockStart(clock()),
	_timeFunction(),
	_labels(),
	_labelNameToIndex(),
	_nextLabelIds(),
	_sceneManager(nullptr),
	_started(Signal<Ptr>::create()),
	_looped(Signal<Ptr>::create()),
	_stopped(Signal<Ptr>::create()),
	_labelHit(Signal<Ptr, std::string, uint>::create()),
	_targetAddedSlot(nullptr),
	_targetRemovedSlot(nullptr),
	_addedSlot(nullptr),
	_removedSlot(nullptr),
	_frameBeginSlot(nullptr)
{
	if (option == CloneOption::DEEP)
	{
		_currentTime = absAnimation._currentTime;
		_previousTime = absAnimation._previousTime;
		_previousGlobalTime = absAnimation._previousGlobalTime;
		_isPlaying = absAnimation._isPlaying;
	}
	_timeFunction = [](uint t) -> uint
	{
		return t;
	};
}

void
AbstractAnimation::initialize()
{
}

void
AbstractAnimation::targetAdded(Node::Ptr node)
{
	_addedSlot = node->added().connect(std::bind(
		&AbstractAnimation::addedHandler,
		std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_removedSlot = node->removed().connect(std::bind(
		&AbstractAnimation::removedHandler,
		std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this()),
		std::placeholders::_1,
		std::placeholders::_2,
		std::placeholders::_3
	));

	_target = node;
}

void
AbstractAnimation::targetRemoved(Node::Ptr node)
{
	_addedSlot = nullptr;
	_removedSlot = nullptr;
}

/*virtual*/
void
AbstractAnimation::addedHandler(Node::Ptr node,
								Node::Ptr target,
								Node::Ptr parent)
{
	findSceneManager();
}

/*virtual*/
void
AbstractAnimation::removedHandler(Node::Ptr node,
								  Node::Ptr target,
								  Node::Ptr parent)
{
	findSceneManager();
}

void
AbstractAnimation::findSceneManager()
{
	NodeSet::Ptr roots = NodeSet::create(target())
		->roots()
		->where([](NodePtr node)
		{
			return node->hasComponent<SceneManager>();
		});

	if (roots->nodes().size() > 1)
		throw std::logic_error("Renderer cannot be in two separate scenes.");
	else if (roots->nodes().size() == 1)
		setSceneManager(roots->nodes()[0]->component<SceneManager>());
	else
		setSceneManager(nullptr);
}

void
AbstractAnimation::setSceneManager(SceneManager::Ptr sceneManager)
{
	if (sceneManager && sceneManager != _sceneManager)
	{
		_frameBeginSlot = sceneManager->frameBegin()->connect(std::bind(
			&AbstractAnimation::frameBeginHandler,
			std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this()),
			std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3
		));

		if (_sceneManager == nullptr)
			_previousGlobalTime = _timeFunction(uint(sceneManager->time()));
	}
	else if (_frameBeginSlot && sceneManager == nullptr)
	{
		stop();
		_frameBeginSlot = nullptr;
	}

	_sceneManager = sceneManager;
}

AbstractAnimation::Ptr
AbstractAnimation::play()
{
	_previousGlobalTime = _timeFunction(_sceneManager ? uint(_sceneManager->time()) : 0);
	_isPlaying	 		= true;
	_started->execute(std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this()));
	checkLabelHit(_currentTime, _currentTime);

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

AbstractAnimation::Ptr
AbstractAnimation::stop()
{
	if (_isPlaying)
	{
		updateNextLabelIds(_currentTime);
		checkLabelHit(_currentTime, _currentTime);
	}

	_isPlaying			= false;
	_stopped->execute(std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this()));
	_mustUpdateOnce		= true;
	_previousGlobalTime = _timeFunction(_sceneManager ? uint(_sceneManager->time()) : 0);

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

AbstractAnimation::Ptr
AbstractAnimation::seek(const std::string& labelName)
{
	auto masterAnim = std::dynamic_pointer_cast<MasterAnimation>(shared_from_this());

	return masterAnim ? masterAnim->seek(labelName) : seek(labelTime(labelName));
}

AbstractAnimation::Ptr
AbstractAnimation::seek(uint currentTime)
{
	if (!isInPlaybackWindow(currentTime))
		throw std::logic_error("Provided time value is outside of playback window. In order to reset playback window, call resetPlaybackWindow().");

	_currentTime = currentTime;

	updateNextLabelIds(_currentTime);

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

bool
AbstractAnimation::hasLabel(const std::string& name) const
{
	return _labelNameToIndex.count(name) > 0;
}

AbstractAnimation::Ptr
AbstractAnimation::addLabel(const std::string& name, uint time)
{
	if (hasLabel(name))
		throw std::logic_error("A label called '" + name + "' already exists.");

	_labelNameToIndex[name] = _labels.size();
	_labels.push_back(Label(name, time));

    updateNextLabelIds(_currentTime);

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

AbstractAnimation::Ptr
AbstractAnimation::changeLabel(const std::string& name, const std::string& newName)
{
	const auto foundLabelIt = _labelNameToIndex.find(name);
	if (foundLabelIt == _labelNameToIndex.end())
		throw std::logic_error("No label called '" + name + "' currently exists.");

	const uint	labelId	= foundLabelIt->second;
	auto&		label	= _labels[labelId];

	_labelNameToIndex.erase(foundLabelIt);
	label.name = newName;
	_labelNameToIndex[newName] = labelId;

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

AbstractAnimation::Ptr
AbstractAnimation::setTimeForLabel(const std::string& name, uint newTime)
{
	const auto foundLabelIt = _labelNameToIndex.find(name);
	if (foundLabelIt == _labelNameToIndex.end())
		throw std::logic_error("No label called '" + name + "' currently exists.");

	const uint	labelId	= foundLabelIt->second;
	auto&		label	= _labels[labelId];

	label.time = newTime;

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

AbstractAnimation::Ptr
AbstractAnimation::removeLabel(const std::string& name)
{
	const auto foundLabelIt = _labelNameToIndex.find(name);
	if (foundLabelIt == _labelNameToIndex.end())
		throw std::logic_error("No label called '" + name + "' currently exists.");

	const uint			labelId			= foundLabelIt->second;
	const std::string	lastLabelName	= _labels.back().name;

	// swap with last and pop back
	std::swap(_labels[labelId], _labels.back());
	_labelNameToIndex[lastLabelName]	= labelId;
	_labels.pop_back();

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

uint
AbstractAnimation::labelTime(const std::string& name) const
{
	const auto foundLabelIt = _labelNameToIndex.find(name);
	if (foundLabelIt == _labelNameToIndex.end())
		throw std::logic_error("No label called '" + name + "' currently exists.");

	return labelTime(foundLabelIt->second);
}

AbstractAnimation::Ptr
AbstractAnimation::setPlaybackWindow(uint beginTime,
									 uint endTime,
									 bool forceRestart)
{
	_loopMinTime = beginTime;
	_loopMaxTime = endTime;

	if (_loopMinTime > _loopMaxTime)
		std::swap(_loopMinTime, _loopMaxTime);

	_loopTimeRange = _loopMaxTime - _loopMinTime + 1;

	if (!isInPlaybackWindow(_currentTime) || forceRestart)
		_currentTime = loopStartTime();

	updateNextLabelIds(_currentTime);

	return std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this());
}

AbstractAnimation::Ptr
AbstractAnimation::setPlaybackWindow(const std::string& beginLabelName,
									 const std::string& endLabelName,
									 bool forceRestart)
{
	return setPlaybackWindow(
		labelTime(beginLabelName),
		labelTime(endLabelName),
		forceRestart
	);
}


AbstractAnimation::Ptr
AbstractAnimation::resetPlaybackWindow()
{
	return setPlaybackWindow(0, _maxTime);
}

void
AbstractAnimation::updateNextLabelIds(uint time)
{
	_nextLabelIds.clear();

	if (_labels.empty())
		return;

	_nextLabelIds.reserve(_labels.size());

	uint nextLabelTime = !_isReversed ? UINT_MAX : 0;

	for (uint labelId = 0; labelId < _labels.size(); ++labelId)
	{
		const uint labelTime = _labels[labelId].time;

		if (!isInPlaybackWindow(labelTime))
			continue;

		if (!_isReversed && time < labelTime)
		{
			if (labelTime < nextLabelTime)
			{
				nextLabelTime = labelTime;

				_nextLabelIds.clear();
				_nextLabelIds.push_back(labelId);
			}
			else if (labelTime == nextLabelTime)
				_nextLabelIds.push_back(labelId);
		}
		else if (_isReversed && labelTime < time)
		{
			if (nextLabelTime < labelTime)
			{
				nextLabelTime = labelTime;

				_nextLabelIds.clear();
				_nextLabelIds.push_back(labelId);
			}
			else if (labelTime == nextLabelTime)
				_nextLabelIds.push_back(labelId);
		}
	}

	if (_nextLabelIds.empty())
	{
		if (time != loopStartTime())
			updateNextLabelIds(loopStartTime());
	}
	else if (_isLooping && nextLabelTime == loopEndTime())
	{
		for (uint labelId = 0; labelId < _labels.size(); ++labelId)
		{
			if (_labels[labelId].time == loopStartTime() &&
				std::find(_nextLabelIds.begin(), _nextLabelIds.end(), labelId) == _nextLabelIds.end())
				_nextLabelIds.push_back(labelId);
		}
	}
}

void
AbstractAnimation::checkLabelHit(uint previousTime, uint newTime)
{
	if (!_isPlaying || _nextLabelIds.empty())
		return;

	const Label&	nextLabel		= _labels[_nextLabelIds.front()];
	const uint		nextLabelTime	= nextLabel.time;

	assert(isInPlaybackWindow(nextLabelTime));

	bool trigger = false;

	if (!_isReversed)
	{
		if (previousTime <= newTime)
		{
			if ((newTime == nextLabelTime)
				|| (previousTime < nextLabelTime && nextLabelTime <= newTime))
				trigger = true;
		}
		else // newTime < previousTime
		{
			if (previousTime < nextLabelTime)
				trigger = true;
			else if (nextLabelTime < newTime)
				trigger = true;
		}
	}
	else // reversed animation
	{
		if (newTime <= previousTime)
		{
			if ((newTime == nextLabelTime)
				|| (newTime <= nextLabelTime && nextLabelTime < previousTime))
				trigger = true;
		}
		else // previousTime < newTime
		{
			if (nextLabelTime < previousTime)
				trigger = true;
			else if (newTime < nextLabelTime)
				trigger = true;
		}
	}

	if (trigger)
	{
        auto nextLabelIds = _nextLabelIds;

		for (auto labelId : nextLabelIds)
		{
			const auto& label = _labels[labelId];

			_labelHit->execute(
				std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this()),
				label.name,
				label.time
			);
		}

		updateNextLabelIds(getNewLoopTime(_currentTime, !_isReversed ? 1 : -1));
	}
}

bool
AbstractAnimation::isInPlaybackWindow(uint time) const
{
	assert(_loopMinTime <= _loopMaxTime);

	return _loopMinTime <= time && time <= _loopMaxTime;
}


void
AbstractAnimation::frameBeginHandler(SceneManager::Ptr sceneManager, float time, float)
{
	update(uint(time));
}

/*virtual*/
bool
AbstractAnimation::update(uint rawGlobalTime)
{
	if (!_isPlaying && !_mustUpdateOnce)
		return false;

	_mustUpdateOnce = false;

	const uint	globalTime		= _timeFunction(rawGlobalTime);
	const uint	globalDeltaTime	= globalTime - _previousGlobalTime;
	const int	deltaTime		= !_isReversed
		? int(globalDeltaTime)
		: - int(globalDeltaTime);

	_previousTime		= _currentTime;
	if (_isPlaying)
		_currentTime	= getNewLoopTime(_currentTime, deltaTime);
	_previousGlobalTime	= globalTime;

	const bool looped	=
		(!_isReversed && _currentTime < _previousTime) ||
		(_isReversed && _previousTime < _currentTime);

	if (looped)
	{
		if (_isLooping)
			_looped->execute(std::dynamic_pointer_cast<AbstractAnimation>(shared_from_this()));
		else
		{
			_currentTime = loopEndTime();
			stop();
		}
	}

	update();

	checkLabelHit(_previousTime, _currentTime);

	return _isPlaying || _mustUpdateOnce;
}

uint
AbstractAnimation::getNewLoopTime(uint time, int deltaTime) const
{
	const int	relTime		= int(time - _loopMinTime) + deltaTime;
	const uint	timeOffset	= uint((relTime + _loopTimeRange) % _loopTimeRange);

	return _loopMinTime + timeOffset;
}
