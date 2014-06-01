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

#include <android/log.h>

namespace minko
{
	namespace log
	{
		class AndroidStreambuf : public std::streambuf
		{
		public:
			enum { bufsize = 2048 }; // ... or some other suitable buffer size

			AndroidStreambuf() { this->setp(_buffer, _buffer + bufsize - 1); }

		private:
			int overflow(int c)
			{
				if (c == traits_type::eof())
				{
					*this->pptr() = traits_type::to_char_type(c);
					this->sbumpc();
				}
				return this->sync() ? traits_type::eof() : traits_type::not_eof(c);
			}

			int sync()
			{
				if (this->pbase() != this->pptr())
				{
					std::string buf(this->pbase(), this->pptr() - this->pbase());
					__android_log_print(ANDROID_LOG_INFO, "minko", buf.c_str());
					this->setp(_buffer, _buffer + bufsize - 1);
				}
				return 0;
			}

			char _buffer[bufsize];
		};
	}
}
