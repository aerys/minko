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

namespace minko
{
	class Bindings
	{
	public:
		typedef std::shared_ptr<Bindings> Ptr;

    private:
        //std::vector<unsigned char>						_memory;
		std::list<std::string>							_bindings;
		std::unordered_map<std::string, std::string>	_attributeBindings;
		std::unordered_map<std::string, std::string>	_uniformBindings;

	public:
		inline static
		Ptr
		create()
		{
			return std::shared_ptr<Bindings>(new Bindings());
		}

		bool
		hasBinding(const std::string& bindingName)
		{
			return std::find(_bindings.begin(), _bindings.end(), bindingName) != _bindings.end();
		}

		bool
		hasVertexAttribute(const std::string& attributeName)
		{
			return _attributeBindings.count(attributeName);
		}

        void
        addVertexAttribute(const std::string& bindingName,
						   const std::string& attributeName)
        {
			_bindings.push_back(bindingName);
			_attributeBindings[attributeName] = bindingName;
        }

		/*
		void
        removeVertexAttribute(const std::string& bindingName)
        {
			_attributeBindings.erase(bindingName);
        }
		*/

		bool
		hasUniform(const std::string& uniformName)
		{
			return _uniformBindings.count(uniformName);
		}

		void
		addUniform(const std::string& bindingName,
				   const std::string& uniformName)
		{
			_bindings.push_back(bindingName);
			_attributeBindings[uniformName] = bindingName;
		}

		/*
		void
		removeUniform(const std::string& bindingName)
		{
			_attributeBindings.erase(bindingName);
		}
		*/

        void
        clear()
        {
			_attributeBindings.clear();
			_uniformBindings.clear();
        }

	private:
		Bindings()
		{
		}
	};
}
