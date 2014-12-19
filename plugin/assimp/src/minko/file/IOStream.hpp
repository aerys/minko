/*
Copyright(c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "minko/Common.hpp"

#include "assimp/IOStream.hpp"

namespace minko
{
    namespace file
    {
        class IOStream :
            public Assimp::IOStream
        {
        private:
            const std::vector<unsigned char>    _data;
            size_t                                _position;

        public:
            IOStream(const std::vector<unsigned char>& data) :
                _data(data),
                _position(0)
            {

            }

            size_t
            Read(void* pvBuffer, size_t pSize, size_t pCount)
            {
                auto size = std::min(_data.size() - _position, pSize * pCount);

                memcpy(pvBuffer, (void*)&_data[_position], pSize * pCount);

                return size;
            }

            size_t
            Write(const void* pvBuffer, size_t pSize, size_t pCount)
            {
                throw;
            }

            aiReturn
            Seek(size_t pOffset, aiOrigin pOrigin)
            {
                if (pOrigin == aiOrigin::aiOrigin_CUR)
                    _position = _position + pOffset;
                else if (pOrigin == aiOrigin::aiOrigin_END)
                    _position = _data.size() - _position;
                else if (pOrigin == aiOrigin::aiOrigin_SET)
                    _position = pOffset;

                if (_position > _data.size())
                    return aiReturn_FAILURE;

                return aiReturn_SUCCESS;
            }

            size_t
            Tell() const
            {
                return _position;
            }

            size_t
            FileSize() const
            {
                return _data.size();
            }

            void
            Flush()
            {
                throw;
            }

        };
    }
}
