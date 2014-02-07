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

#include "minko/async/Worker.hpp"

#if defined(EMSCRIPTEN)
# include "emscripten/emscripten.h"
#else
#endif

using namespace minko;
using namespace minko::async;

Worker::Worker(const std::string& name) :
	_progress(Signal<float>::create()),
	_complete(Signal<MessagePtr>::create()),
	_busy(false)
{
#if defined(EMSCRIPTEN)
	_handle = emscripten_create_worker("minko-worker-" + name + ".js");
#else
	_future = _promise.get_future();
#endif
}

void
Worker::start(const std::vector<char>& input)
{
	_busy = true;
	_input = std::make_shared<std::vector<char>>(input);

#if defined(EMSCRIPTEN)
	emscripten_call_worker(_handle, "minkoWorkerEntryPoint", input->begin(), input->size(), &messageHandler, this);
#else
	std::thread(&Worker::run, shared_from_this()).detach();
#endif
}

void
Worker::progress(float value)
{
#if defined(EMSCRIPTEN)
	emscripten_run_script("postMessage(" + std::to_string(value) + ")")
	// EM_ASM(
	// 	postMessage(0.5);
	// );
#else
	std::lock_guard<std::mutex> lock(_ratioMutex);
	_ratio = value;
	std::cout << "Worker::progress(): " << _ratio << std::endl;
#endif
}

void
Worker::update()
{
	if (!_busy)
		return;

#if !defined(EMSCRIPTEN)
	{
		std::lock_guard<std::mutex> lock(_ratioMutex);
		
		if (_ratio != _oldRatio)
		{
			_progress->execute(_ratio);
			_oldRatio = _ratio;
		}
	}
#endif

	if (_future.valid() && _future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		_complete->execute(_future.get());
		_busy = false;
	}
}

#if defined(EMSCRIPTEN)
void
Worker::messageHandler(char* data, int size, void* arg)
{
	Worker* worker = static_cast<Worker*>(arg);

	std::cout << "Worker::messageHandler(): " << size << std::endl;;

	if (size == sizeof(float))
	{
		_ratio = reinterpret_cast<float>(data);
	}
	else
	{
		auto output = std::make_shared<std::vector<char>>(data, data + size);

		worker->_promise.set_value(output);		
	}
}
#endif

Worker::~Worker()
{
	std::cout << "Worker::~Worker()" << std::endl;;
#if defined(EMSCRIPTEN)
	emscripten_destroy_worker(_handle);
#endif
}