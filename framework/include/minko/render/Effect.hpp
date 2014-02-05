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
#include "minko/render/Pass.hpp"

namespace minko
{
	namespace render
	{
		class Effect :
			public std::enable_shared_from_this<Effect>
		{
		public:
			typedef std::shared_ptr<Effect>	Ptr;

		private:
			typedef std::shared_ptr<Pass>										PassPtr;
			typedef std::shared_ptr<VertexBuffer>								VertexBufferPtr;
			typedef std::shared_ptr<std::function<void(PassPtr)>>				OnPassFunctionPtr;
			typedef std::list<std::function<void(PassPtr)>>						OnPassFunctionList;	
			typedef std::vector<PassPtr> 										Technique;
			typedef Signal<Ptr, const std::string&, const std::string&>::Ptr	TechniqueChangedSignalPtr;

		private:
			std::unordered_map<std::string, Technique>		_techniques;
			std::unordered_map<std::string, std::string>	_fallback;
			std::shared_ptr<data::Provider>					_data;

			OnPassFunctionList								_uniformFunctions;
			OnPassFunctionList								_attributeFunctions;
			OnPassFunctionPtr								_indexFunction;

		public:
			inline static
			Ptr
			create()
			{
				return std::shared_ptr<Effect>(new Effect());
			}

			inline static
			Ptr
			create(std::vector<PassPtr>& passes)
			{
				auto effect = create();

				effect->_techniques["default"] = passes;

				return effect;
			}

			inline
			const std::unordered_map<std::string, Technique>&
			techniques()
			{
				return _techniques;
			}

			inline
			std::shared_ptr<data::Provider>
			data() const
			{
				return _data;
			}

			inline
			const Technique&
			technique(const std::string& techniqueName)
			{
				if (!hasTechnique(techniqueName))
					throw std::invalid_argument("techniqueName = " + techniqueName);

				return _techniques[techniqueName];
			}

			inline
			const std::string&
			fallback(const std::string& techniqueName)
			{
				return _fallback[techniqueName];
			}

			inline
			bool
			hasTechnique(const std::string& techniqueName)
			{
				return _techniques.count(techniqueName) != 0;
			}

			inline
			bool
			hasFallback(const std::string& techniqueName)
			{
				return _fallback.count(techniqueName) != 0;
			}

            template <typename... T>
			void
			setUniform(const std::string& name, const T&... values)
			{
#if defined(EMSCRIPTEN)
				auto that = shared_from_this();
				_uniformFunctions.push_back([=](std::shared_ptr<Pass> pass) {
					that->setUniformOnPass<T...>(pass, name, values...);
				});
#else
				_uniformFunctions.push_back(std::bind(
					&Effect::setUniformOnPass<T...>, std::placeholders::_1, name, values...
				));
#endif
				for (auto& technique : _techniques)
					for (auto& pass : technique.second)
						pass->setUniform(name, values...);
			}

			void
			setIndexBuffer(const std::vector<unsigned short>& indices)
			{
				_indexFunction = std::make_shared<std::function<void(PassPtr)>>(std::bind(
					&Effect::setIndexBufferOnPass, std::placeholders::_1, indices
				));

				for (auto& technique : _techniques)
					for (auto& pass : technique.second)
						pass->setIndexBuffer(indices);
			}

			void
			setVertexAttribute(const std::string& name, unsigned int attributeSize, const std::vector<float>& data)
			{
				_attributeFunctions.push_back(std::bind(
					&Effect::setVertexAttributeOnPass, std::placeholders::_1, name, attributeSize, data
				));

				for (auto& technique : _techniques)
					for (auto& pass : technique.second)
						pass->setVertexAttribute(name, attributeSize, data);
			}

            void
            addTechnique(const std::string& name, Technique& passes);

            void
            addTechnique(const std::string& name, Technique& passes, const std::string& fallback);

            void
            removeTechnique(const std::string& name);

		private:
			Effect();

			template <typename... T>
			static 
			void
			setUniformOnPass(std::shared_ptr<Pass> pass, const std::string& name, const T&... values)
			{
				pass->setUniform(name, values...);
			}

			static 
			void
			setVertexAttributeOnPass(std::shared_ptr<Pass> pass, const std::string& name, unsigned int attributeSize, const std::vector<float>& data)
			{
				pass->setVertexAttribute(name, attributeSize, data);
			}

			static
			void
			setIndexBufferOnPass(std::shared_ptr<Pass> pass, const std::vector<unsigned short>& indices)
			{
				pass->setIndexBuffer(indices);
			}
		};		
	}
}
