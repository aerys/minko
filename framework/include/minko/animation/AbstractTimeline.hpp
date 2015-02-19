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

namespace minko
{
	namespace animation
	{
		class AbstractTimeline
		{
		public:
			typedef std::shared_ptr<AbstractTimeline>	Ptr;

		protected:
			typedef std::shared_ptr<data::Store>	UpdateTargetPtr;

		protected:
			std::string	_propertyName;
			uint		_duration;
			bool		_isLocked;

		public:
			virtual
			AbstractTimeline::Ptr
			clone()
			{
				throw std::logic_error("Missing clone function for a component.");
				return nullptr;
			}

			inline
			const std::string&
			propertyName() const
			{
				return _propertyName;
			}

			inline
			void
			propertyName(const std::string& value)
			{
				_propertyName = value;
			}

			inline
			uint
			duration() const
			{
				return _duration;
			}

			inline
			void
			duration(uint value)
			{
				_duration = value;
			}

			inline
			bool 
			isLocked()
			{
				return _isLocked;
			}

			inline
			void
			isLocked(bool value)
			{
				_isLocked = value;
			}

			virtual
			void
			update(uint time, data::Store& data, bool skipPropertyNameFormatting = true) = 0;

		protected:
			AbstractTimeline(const std::string& propertyName, uint duration);
		};
	}
}