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

#include "minko/component/JobManager.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/SceneManager.hpp"

using namespace minko;
using namespace minko::component;

JobManager::Job::Job()
{
	_running = false;
}

JobManager::JobManager(unsigned int loadingFramerate):
	_loadingFramerate(loadingFramerate)
{
	_frameTime = 1.f / loadingFramerate;
}

JobManager::Ptr
JobManager::pushJob(Job::Ptr Job)
{
	float JobPriority	= Job->priority();
	bool inserted		= false;
	uint i				= 0;
	
	for (; i < _jobs.size() && inserted; ++i)
	{
		if (_jobs[i]->priority() > JobPriority)
			inserted = true;
	}

	_jobs.insert(_jobs.begin() + i, Job);

	return std::dynamic_pointer_cast<JobManager>(shared_from_this());
}

void
JobManager::update(NodePtr target)
{
	_frameStartTime = std::clock();
}

void
JobManager::end(NodePtr target)
{
	if (_jobs.size() == 0)
		return;

	float consumeTime	= (float(std::clock() - _frameStartTime) / CLOCKS_PER_SEC);
	Job::Ptr currentJob	= nullptr;

	while (consumeTime < _frameTime)
	{
		if (currentJob == nullptr)
		{
			currentJob = _jobs.back();
			if (!currentJob->running())
			{
				currentJob->_jobManager = std::dynamic_pointer_cast<JobManager>(shared_from_this());
				currentJob->running(true);
				currentJob->beforeFirstStep();
			}
		}

		currentJob->step();

		if (currentJob->complete())
		{
			_jobs.pop_back();
			currentJob->afterLastStep();
			currentJob = nullptr;
			if (_jobs.size() == 0)
				return;
		}

		consumeTime = (float(std::clock() - _frameStartTime) / CLOCKS_PER_SEC);
	}
}