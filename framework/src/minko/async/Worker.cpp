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
#include "minko/AbstractCanvas.hpp"

#if defined(EMSCRIPTEN)
# include "emscripten/emscripten.h"
#else
#endif

using namespace minko;
using namespace minko::async;

Worker::Worker(const std::string& name) :
	_progress(Signal<float>::create()),
	_complete(Signal<MessagePtr>::create()),
	_busy(false),
	_finished(false)
{
#if defined(EMSCRIPTEN)
	std::string path = "minko-worker-" + name + ".js";
	_handle = emscripten_create_worker(path.c_str());
#else
	_ratio = 0;
	_oldRatio = 0;
	_future = _promise.get_future();
#endif
}

void
Worker::start()
{
	std::cout << "Worker::start()" << std::endl;;

	_busy = true;

#if defined(EMSCRIPTEN)
	emscripten_call_worker(_handle, "minkoWorkerEntryPoint", &*_input->begin(), _input->size(), &messageHandler, this);
#else
	std::thread(&Worker::run, shared_from_this()).detach();
#endif
}

void
Worker::progress(float value)
{
#if defined(EMSCRIPTEN)
	std::string script = "postMessage(" + std::to_string(value) + ")";
	emscripten_run_script(script.c_str());
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
	if (_finished)
		return;

	std::cout << "Worker::update()" << std::endl;;

	if (!_busy)
		start();

#if !defined(EMSCRIPTEN)
	std::lock_guard<std::mutex> lock(_ratioMutex);
	
	if (_ratio != _oldRatio)
	{
		std::cout << "Worker::update(): progress execute" << std::endl;
		_progress->execute(_ratio);
		_oldRatio = _ratio;
	}

	if (_future.valid() && _future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		std::cout << "Worker::update(): complete execute" << std::endl;
		_busy = false;
		_finished = true;
		_complete->execute(_future.get());
	}
#endif
}

#if defined(EMSCRIPTEN)
void
Worker::messageHandler(char* data, int size, void* arg)
{
	Worker* worker = static_cast<Worker*>(arg);

	std::cout << "Worker::messageHandler(): " << size << std::endl;;

	if (size == sizeof(float))
	{
		std::cout << "Worker::messageHandler(): progress execute" << std::endl;
		float ratio = reinterpret_cast<float*>(data)[0];
		worker->progress()->execute(ratio);

	}
	else
	{
		std::cout << "Worker::messageHandler(): complete execute" << std::endl;
		auto output = std::make_shared<std::vector<char>>(data, data + size);
		worker->complete()->execute(output);
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