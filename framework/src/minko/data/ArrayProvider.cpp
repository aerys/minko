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

#include "minko/data/ArrayProvider.hpp"
#include "minko/Signal.hpp"

using namespace minko;
using namespace minko::data;

ArrayProvider::ArrayProvider(const std::string& name, uint index) :
	_name(name),
	_index(index),
	_indexChanged(IndexChangedSignal::create())
{
	if (_name.find(NO_STRUCT_SEP) != std::string::npos)
		throw new std::invalid_argument("The name of a ArrayProvider cannot contain the following character sequence: " + NO_STRUCT_SEP);
}

void
ArrayProvider::index(unsigned int index)
{
	if (_index == index)
		return;

	_index = index;

	for (auto& formattedPropertyName : propertyNames())
	{
		auto propertyName				= unformatPropertyName(formattedPropertyName);
		auto newFormattedPropertyName	= formatPropertyName(propertyName);

		swap(formattedPropertyName, newFormattedPropertyName, true);
	}

	_indexChanged->execute(std::dynamic_pointer_cast<ArrayProvider>(shared_from_this()), _index);
}

std::string
ArrayProvider::formatPropertyName(const std::string& propertyName) const
{
#ifndef MINKO_NO_GLSL_STRUCT

	return _name + "[" + std::to_string(_index) + "]." + propertyName;

#else

	return _name + NO_STRUCT_SEP + propertyName + "[" + std::to_string(_index) + "]";

#endif // MINKO_NO_GLSL_STRUCT
}

std::string
ArrayProvider::unformatPropertyName(const std::string& formattedPropertyName) const
{
	if (formattedPropertyName.substr(0, _name.size()) != _name)
		return Provider::unformatPropertyName(formattedPropertyName);

#ifndef MINKO_NO_GLSL_STRUCT

	std::size_t pos = formattedPropertyName.rfind("].");

	if (pos == std::string::npos)
		return Provider::unformatPropertyName(formattedPropertyName);

	return formattedPropertyName.substr(pos + 2);

#else

	std::size_t pos1 = formattedPropertyName.find_first_of(NO_STRUCT_SEP);
	std::size_t pos2 = formattedPropertyName.find_first_of('[');

	if (pos1 == std::string::npos || pos2 == std::string::npos)
		return Provider::unformatPropertyName(formattedPropertyName);

	pos1 += NO_STRUCT_SEP.size();

	return formattedPropertyName.substr(pos1, pos2 - pos1);

#endif // MINKO_NO_GLSL_STRUCT
}