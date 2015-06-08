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

const unsigned int JobManager::_defaultMinimumNumStepsPerFrame = 1u;

JobManager::Job::Job() :
    _jobManager(),
    _running(false),
    _priorityChanged(Signal<float>::create())
{
}

JobManager::JobManager(unsigned int loadingFramerate):
    _loadingFramerate(loadingFramerate),
    _sortingNeeded(false)
{
    _frameTime = 1.f / loadingFramerate;
}

JobManager::Ptr
JobManager::pushJob(Job::Ptr job)
{
    _jobPriorityChangedSlots.insert(std::make_pair(
        job,
        job->priorityChanged()->connect([=](float priority) -> void 
        {
            _sortingNeeded = true;
        }))
    );

    job->_jobManager = std::static_pointer_cast<JobManager>(shared_from_this());

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

    auto numStepPerformed = 0u;

    while (consumeTime < _frameTime || numStepPerformed < _defaultMinimumNumStepsPerFrame)
    {
        if (_sortingNeeded)
        {
            _sortingNeeded = false;

            currentJob = nullptr;

            _jobs.sort(Job::PriorityComparator());
        }

        if (!hasPendingJob())
            break;

        if (currentJob == nullptr)
        {
            currentJob = _jobs.back();

            if (!currentJob->running())
            {
                currentJob->running(true);
                currentJob->beforeFirstStep();
            }
        }

        auto currentJobComplete = currentJob->complete();

        if (!currentJobComplete)
        {
            currentJob->step();

            currentJobComplete |= currentJob->complete();
        }

        if (currentJobComplete)
        {
            _jobs.pop_back();
            currentJob->afterLastStep();
            _jobPriorityChangedSlots.erase(currentJob);
            currentJob = nullptr;
        }

        ++numStepPerformed;

        consumeTime = (float(std::clock() - _frameStartTime) / CLOCKS_PER_SEC);
    }
}

void
JobManager::insertJob(Job::Ptr job)
{
    _jobs.push_back(job);

    _sortingNeeded = true;
}

bool
JobManager::hasPendingJob() const
{
    return !_jobs.empty() && _jobs.back()->priority() > 0.f;
}
