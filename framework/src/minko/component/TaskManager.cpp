/*
Copyright (c) 2013 Aerys

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

#include "minko/component/TaskManager.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/SceneManager.hpp"

using namespace minko;
using namespace minko::component;

TaskManager::Task::Task()
{
	_running = false;
}

TaskManager::TaskManager(unsigned int loadingFramerate):
	_loadingFramerate(loadingFramerate)
{
	_frameTime = 1.f / loadingFramerate;
}

TaskManager::Ptr
TaskManager::pushTask(Task::Ptr task)
{
	float taskPriority	= task->priority();
	bool inserted		= false;
	uint i				= 0;
	
	for (; i < _tasks.size() && inserted; ++i)
	{
		if (_tasks[i]->priority() > taskPriority)
			inserted = true;
	}

	_tasks.insert(_tasks.begin() + i, task);

	return std::dynamic_pointer_cast<TaskManager>(shared_from_this());
}

void
TaskManager::update(NodePtr target)
{
	_frameStartTime = std::clock();
}

void
TaskManager::end(NodePtr target)
{
	if (_tasks.size() == 0)
		return;

	float consumeTime		= (float(std::clock() - _frameStartTime) / CLOCKS_PER_SEC);
	Task::Ptr currentTask	= nullptr;

	while (consumeTime < _frameTime)
	{
		if (currentTask == nullptr)
		{
			currentTask = _tasks.back();
			if (!currentTask->running())
			{
				currentTask->_taskManager = std::dynamic_pointer_cast<TaskManager>(shared_from_this());
				currentTask->running(true);
				currentTask->beforeFirstStep();
			}
		}

		currentTask->step();

		if (currentTask->complete())
		{
			_tasks.pop_back();
			currentTask->afterLastStep();
			currentTask = nullptr;
			if (_tasks.size() == 0)
				return;
		}

		consumeTime = (float(std::clock() - _frameStartTime) / CLOCKS_PER_SEC);
	}
}