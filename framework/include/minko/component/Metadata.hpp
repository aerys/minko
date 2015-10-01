/*
Copyright (c) 2015 Aerys

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

#include "minko/component/AbstractComponent.hpp"

namespace minko
{
	namespace component
	{
		class Metadata : public AbstractComponent
		{
        public:
            typedef std::shared_ptr<Metadata>                       Ptr;
            typedef std::unordered_map<std::string, std::string>    Data;

        private:
            Data    _data;

            Metadata(const Data& data) :
                _data(data)
            {
            }

        public:
            inline static
            Ptr
            create(const Data& data)
            {
                return Ptr(new Metadata(data));
            }

            inline
            const Data&
            data()
            {
                return _data;
            }

            inline
            const std::string&
            get(const std::string& propertyName)
            {
                return _data.at(propertyName);
            }

            inline
            bool
            has(const std::string& propertyName)
            {
                return _data.count(propertyName) != 0;
            }

            inline
            const std::string&
            operator[](const std::string& propertyName)
            {
                return get(propertyName);
            }
        };
    }
}
