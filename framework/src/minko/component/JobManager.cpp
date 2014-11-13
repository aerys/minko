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

#include "minko/component/JobManager.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/SceneManager.hpp"

using namespace minko;
using namespace minko::component;

JobManager::Job::Job() :
    _jobManager(),
    _running(false),
    _oneStepPerFrame(false),
    _priorityChanged(Signal<Job::Ptr, float>::create())
{
}

JobManager::JobManager(unsigned int loadingFramerate):
    _loadingFramerate(loadingFramerate)
{
    _frameTime = 1.f / loadingFramerate;
}

JobManager::Ptr
JobManager::pushJob(Job::Ptr job)
{
    _jobPriorityChangedSlots.insert(std::make_pair(
        job,
        job->priorityChanged()->connect([this](Job::Ptr job, float priority) -> void 
        {
            auto jobIt = std::find(_jobs.begin(), _jobs.end(), job);

            if (jobIt != _jobs.end())
            {
                _jobs.erase(jobIt);
            }

            insertJob(job);
        }))
    );

    insertJob(job);

    return std::static_pointer_cast<JobManager>(shared_from_this());
}

void
JobManager::update(NodePtr target)
{
    _frameStartTime = std::clock();
}

void
JobManager::end(NodePtr target)
{
    if (_jobs.empty())
        return;

    auto consumeTime        = float(std::clock() - _frameStartTime) / CLOCKS_PER_SEC;
    Job::Ptr currentJob     = nullptr;

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

        consumeTime = (float(std::clock() - _frameStartTime) / CLOCKS_PER_SEC);

        if (currentJob->complete())
        {
            _jobs.pop_back();
            currentJob->afterLastStep();
            _jobPriorityChangedSlots.erase(currentJob);
            currentJob = nullptr;

            if (_jobs.empty())
                return;
        }
        else
        {
            if (currentJob->oneStepPerFrame())
            {
                _jobs.push_back(currentJob);
                currentJob = nullptr;
                consumeTime = _frameTime;
            }
        }
    }
}

void
JobManager::insertJob(Job::Ptr job)
{
    auto jobPosition = std::lower_bound(
        _jobs.begin(),
        _jobs.end(),
        job,
        [&](Job::Ptr left, Job::Ptr right) -> bool
        {
            return left->priority() > right->priority();
        }
    );

    _jobs.insert(jobPosition, job);
}
