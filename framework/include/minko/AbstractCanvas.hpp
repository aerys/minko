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

#pragma once

#include "minko/Common.hpp"

#include "minko/Signal.hpp"

namespace minko
{
	class AbstractCanvas
	{
	public:
		typedef std::shared_ptr<AbstractCanvas>							Ptr;
		typedef std::function<std::shared_ptr<async::Worker> ()>		WorkerHandler;

	public:
		virtual
		uint
		x() = 0;

		virtual
		uint
		y() = 0;

		virtual
		uint
		width() = 0;

		virtual
		uint
		height() = 0;

		virtual
		std::shared_ptr<input::Mouse>
		mouse() = 0;

        virtual
        std::shared_ptr<input::Keyboard>
        keyboard() = 0;

		virtual
		std::shared_ptr<input::Joystick>
		joystick(uint id) = 0;

		virtual
		uint
		numJoysticks() = 0;

        virtual
		Signal<Ptr, uint, uint>::Ptr
		resized() = 0;

		virtual
		Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::Ptr
		joystickAdded() = 0;

		virtual
		Signal<AbstractCanvas::Ptr, std::shared_ptr<input::Joystick>>::Ptr
		joystickRemoved() = 0;

		virtual
		std::shared_ptr<async::Worker>
		getWorker(const std::string& name) = 0;

		// Current frame execution time in milliseconds.
		virtual
		float
		frameDuration() const = 0;

		// Time in milliseconds since application started.
		virtual
		float
		relativeTime() const = 0;

		virtual
		bool
		isWorkerRegistered(const std::string& name) = 0;

		template <typename T>
		void
		registerWorker(const std::string& type)
		{
			std::string key(type);

			std::transform(key.begin(), key.end(), key.begin(), ::tolower);

			_workers[key] = T::create;
		}

		static
		std::shared_ptr<AbstractCanvas>
		defaultCanvas()
		{
			return _defaultCanvas;
		}

		static
		void
		defaultCanvas(std::shared_ptr<AbstractCanvas> value)
		{
			_defaultCanvas = value;
		}

	protected:
		static
		std::unordered_map<std::string, WorkerHandler>		_workers;
		
		static
		std::shared_ptr<AbstractCanvas>						_defaultCanvas;
	};
}
