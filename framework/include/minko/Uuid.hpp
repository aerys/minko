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
	class Uuid
	{
	private:
		static bool	_randSeeded;

    public:
        class has_uuid
        {
        public:
            virtual
            const std::string&
            uuid() const = 0;
        };

        class enable_uuid
            : public has_uuid
        {
        protected:
            const std::string _uuid;

        protected:
            enable_uuid() :
                _uuid(Uuid::getUuid())
            {
            }

            enable_uuid(const std::string& uuid) :
                _uuid(uuid)
            {
            }

        public:
            virtual
            ~enable_uuid()
            {}

            inline
            const std::string&
            uuid() const
            {
                return _uuid;
            }
        };

	public:
		static
		std::string
		getUuid()
		{
			return s4() + s4() + "-" + s4() + "-" + s4() + "-" + s4() + "-" + s4() + s4() + s4();
		}

	private:
		static
		std::string
		s4();
	};
}
