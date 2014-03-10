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

#pragma once

#include "minko/Common.hpp"

#include "minko/Signal.hpp"

#if EMSCRIPTEN
#include "emscripten/emscripten.h"
#endif

// Possible implementations:
// - Web Worker-based (web, sync)
// - Callback-based (web, async eg. xhr)
// - Thread-based (native)
// - Fork-based (native)

namespace minko
{
	namespace async
	{
		class Worker :
			public std::enable_shared_from_this<Worker>
		{
		public:
			typedef std::shared_ptr<Worker>				Ptr;
			typedef std::shared_ptr<std::vector<char>>	MessagePtr;
			typedef float								Progress;

		protected:
			std::shared_ptr<Signal<Ptr, float>>			_progress;
			std::shared_ptr<Signal<Ptr, MessagePtr>>	_complete;
			bool										_busy;
			bool										_finished;

#if defined(EMSCRIPTEN)
			int											_handle;
#else
			std::shared_future<MessagePtr>				_future;
			std::promise<MessagePtr>					_promise;
			float										_ratio;
			float										_oldRatio;
			std::mutex									_ratioMutex;
#endif

		public:
			void
			start();

			void
			update();

			bool
			busy() const
			{
				return _busy;
			}

			virtual
			void
			run() = 0;

			void
			progress(float value);

			inline
			Signal<Ptr, float>::Ptr
			progress()
			{
				return _progress;
			}

			inline
			Signal<Ptr, MessagePtr>::Ptr
			complete()
			{
				return _complete;
			}

			virtual
			~Worker();

			MessagePtr
			input()
			{
				return _input;
			}

			void
			input(MessagePtr value)
			{
				_input = value;
			}

			MessagePtr
			output()
			{
				return _output;
			}

		protected:
			Worker(const std::string& name);

			void
			output(MessagePtr value)
			{
				_output = value;
			}

		private:
			MessagePtr _input;
			MessagePtr _output;

#if defined(EMSCRIPTEN)
			static
			void
			messageHandler(char* data, int size, void* arg);
#endif
		};
	}
}

#if defined(EMSCRIPTEN)

# define MINKO_WORKER(Name, Class, Code)										\
void minkoWorkerEntryPoint(char* data, int size)								\
{																				\
	auto worker = Class ::create();												\
	worker->input(std::make_shared<std::vector<char>>(data, data + size));		\
	worker->run();																\
}																				\
void Class ::run()																\
{																				\
	auto code = [this]() Code;													\
	code();																		\
	emscripten_worker_respond(&*output()->begin(), output()->size());			\
}

#else

# define MINKO_WORKER(Name, Class, Code)										\
void Class ::run()																\
{																				\
	auto code = [this]() Code;													\
	code();																		\
	_promise.set_value(output());												\
}

#endif
