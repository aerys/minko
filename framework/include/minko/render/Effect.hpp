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

#include "minko/Signal.hpp"
#include "minko/render/Pass.hpp"
#include "minko/data/Provider.hpp"
#include "minko/Uuid.hpp"
#include "minko/Flyweight.hpp"

namespace minko
{
	namespace render
	{
		class Effect :
            public Uuid::has_uuid
		{
		public:
			typedef std::shared_ptr<Effect>										Ptr;

		private:
			typedef std::shared_ptr<Pass>										PassPtr;
			typedef std::shared_ptr<VertexBuffer>								VertexBufferPtr;
			typedef std::shared_ptr<std::function<void(PassPtr)>>				OnPassFunctionPtr;
			typedef std::list<std::function<void(PassPtr)>>						OnPassFunctionList;
			typedef std::vector<PassPtr> 										Technique;
			typedef Signal<Ptr, const std::string&, const std::string&>::Ptr	TechniqueChangedSignalPtr;

		private:
            std::string                                     _name;

			std::unordered_map<std::string, Technique>		_techniques;
			std::unordered_map<std::string, std::string>	_fallback;
			std::shared_ptr<data::Provider>					_data;

			OnPassFunctionList								_uniformFunctions;
			OnPassFunctionList								_attributeFunctions;
            OnPassFunctionList                              _macroFunctions;

		public:
			inline static
			Ptr
            create(const std::string& name)
			{
				return std::shared_ptr<Effect>(new Effect(name));
			}

			inline static
			Ptr
            create(const std::string& name, std::vector<PassPtr>& passes)
			{
				auto effect = create(name);

				effect->_techniques["default"] = passes;

				return effect;
			}

            inline
            const std::string&
            uuid() const
            {
                return _data->uuid();
            }

            inline
            const std::string&
            name() const
            {
                return _name;
            }

            inline
			const std::unordered_map<std::string, Technique>&
			techniques() const
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
			technique(const std::string& techniqueName) const
			{
				if (!hasTechnique(techniqueName))
					throw std::invalid_argument("techniqueName = " + techniqueName);

				return _techniques.at(techniqueName);
			}

			inline
			const std::string&
			fallback(const std::string& techniqueName) const
			{
				auto foundFallbackIt = _fallback.find(techniqueName);

				if (foundFallbackIt == _fallback.end())
					throw std::invalid_argument("techniqueName = " + techniqueName);

				return foundFallbackIt->second;
			}

			inline
			bool
			hasTechnique(const std::string& techniqueName) const
			{
				return _techniques.count(techniqueName) != 0;
			}

			inline
			bool
			hasFallback(const std::string& techniqueName) const
			{
				return _fallback.count(techniqueName) != 0;
			}

            template <typename... T>
			void
			setUniform(const std::string& name, const T&... values)
			{
				_uniformFunctions.push_back(std::bind(
					&Effect::setUniformOnPass<T...>, std::placeholders::_1, name, values...
				));

				for (auto& technique : _techniques)
					for (auto& pass : technique.second)
						pass->setUniform(name, values...);
			}

		public:
			void
			setAttribute(const std::string& name, const VertexAttribute& attribute);

			void
			define(const std::string& macroName);

            template <typename T>
			inline
			void
			define(const std::string& macroName, T macroValue)
			{
                _macroFunctions.push_back(std::bind(
                    &Effect::defineOnPassWithValue<T>, std::placeholders::_1, macroName, macroValue
                ));

				for (auto& technique : _techniques)
					for (auto& pass : technique.second)
						pass->define(macroName, macroValue);
			}

            void
            addTechnique(const std::string& name, Technique& passes);

            void
            addTechnique(const std::string& name, Technique& passes, const std::string& fallback);

            void
            removeTechnique(const std::string& name);



			template <typename T = material::Material>
			std::shared_ptr<T>
			initializeMaterial(std::shared_ptr<T> material, const std::string& technique = "default")
			{
				fillMaterial(material, technique);

				return material;
			}

		private:
            Effect(const std::string& name);

			template <typename... T>
			static
			void
			setUniformOnPass(std::shared_ptr<Pass> pass, const std::string& name, const T&... values)
			{
				pass->setUniform(name, values...);
			}

			static
			void
			setVertexAttributeOnPass(std::shared_ptr<Pass> pass, const std::string& name, const VertexAttribute& attribute)
			{
				pass->setAttribute(name, attribute);
			}

            static
            void
            defineOnPass(std::shared_ptr<Pass> pass, const std::string& macroName)
            {
                pass->define(macroName);
            }

            template <typename T>
            static
            void
            defineOnPassWithValue(std::shared_ptr<Pass> pass, const std::string& macroName, T macroValue)
            {
                pass->define(macroName, macroValue);
            }

			void
			fillMaterial(std::shared_ptr<material::Material> 	material,
						 const std::string& 					technique);
		};
	}
}
