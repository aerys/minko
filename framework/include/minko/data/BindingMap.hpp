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

#include "minko/data/Binding.hpp"
#include "minko/data/MacroBinding.hpp"
#include "minko/data/Store.hpp"

namespace minko
{
    namespace data
    {
        template <typename T = data::Binding>
        struct BindingMapBase
        {
            std::map<std::string, T> bindings;
            data::Store defaultValues;
        };

        typedef BindingMapBase<data::Binding>       BindingMap;
        typedef BindingMapBase<data::MacroBinding>  MacroBindingMap;
    }
}

ArrayProvider::ArrayProvider(const ArrayProvider& provider) :
_name(provider._name),
_indexChanged(IndexChangedSignal::create())
{
	if (_name.find(NO_STRUCT_SEP) != std::string::npos)
		throw std::invalid_argument("The name of a ArrayProvider cannot contain the following character sequence: " + NO_STRUCT_SEP);
}
