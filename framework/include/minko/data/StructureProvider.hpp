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

#include "minko/data/Provider.hpp"

namespace minko
{
    namespace data
    {
        class StructureProvider :
            public Provider
        {
        public:
            typedef std::shared_ptr<StructureProvider>            Ptr;

        private:
            std::string    _structureName;

        public:
            inline static
            Ptr
            create(const std::string& structureName)
            {
                return std::shared_ptr<StructureProvider>(new StructureProvider(structureName));
            }

            inline
            const std::string&
            structureName() const
            {
                return _structureName;
            }

            inline
            Ptr
            copyFrom(Ptr source)
            {
                Provider::copyFrom(std::static_pointer_cast<Provider>(source));

                _structureName = source->_structureName;

                return std::static_pointer_cast<StructureProvider>(shared_from_this());
            }

            inline
            Ptr
            clone()
            {
                auto that = std::static_pointer_cast<StructureProvider>(shared_from_this());

                return StructureProvider::create(_structureName)->copyFrom(that);
            }

        protected:
            inline explicit
            StructureProvider(const std::string& name) :
                _structureName(name)
            {
                if (_structureName.find(NO_STRUCT_SEP) != std::string::npos)
                    throw std::invalid_argument("The name of a StructureProvider cannot contain the following character sequence: " + NO_STRUCT_SEP);
            }

            inline
            std::string
            formatPropertyName(const std::string& propertyName) const
            {
                return _structureName + '.' + propertyName;
            }

            inline
            std::string
            unformatPropertyName(const std::string& formattedPropertyName) const
            {
                std::size_t pos = formattedPropertyName.find_first_of('.');
                if (pos == std::string::npos || formattedPropertyName.substr(0, pos) != _structureName)
                    return Provider::unformatPropertyName(formattedPropertyName);
                ++pos;
                return formattedPropertyName.substr(pos);
            }
        };
    }
}
