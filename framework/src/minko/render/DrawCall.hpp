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
#include "minko/data/Container.hpp"

namespace minko
{
	namespace render
	{
		class DrawCall :
            public std::enable_shared_from_this<DrawCall>
		{
		public:
			typedef std::shared_ptr<DrawCall> Ptr;

		private:
			typedef std::shared_ptr<AbstractContext>	AbsCtxPtr;
            typedef std::shared_ptr<data::Container>    ContainerPtr;

		private:
            static SamplerState                                         _defaultSamplerState;

			std::shared_ptr<Program>									_program;
			std::shared_ptr<data::Container>					        _data;
			const std::unordered_map<std::string, std::string>&	        _attributeBindings;
			const std::unordered_map<std::string, std::string>&	        _uniformBindings;
			const std::unordered_map<std::string, std::string>&	        _stateBindings;
            const std::unordered_map<std::string, SamplerState>&        _samplerStates;

			std::vector<std::function<void(AbsCtxPtr)>>			        _func;

            std::list<Signal<ContainerPtr, const std::string&>::Slot>   _propertyChangedSlots;

		public:
			static inline
			Ptr
			create(std::shared_ptr<Program>								program,
				   ContainerPtr						                    data,
				   const std::unordered_map<std::string, std::string>&	attributeBindings,
				   const std::unordered_map<std::string, std::string>&	uniformBindings,
				   const std::unordered_map<std::string, std::string>&	stateBindings,
                   const std::unordered_map<std::string, SamplerState>& samplerSates)
			{
                auto dc = std::shared_ptr<DrawCall>(new DrawCall(
					program, data, attributeBindings, uniformBindings, stateBindings, samplerSates
				));

                dc->bind();

				return dc;
			}

			void
			render(std::shared_ptr<AbstractContext> context);

			void
			initialize(ContainerPtr				                    data,
					   const std::map<std::string, std::string>&	inputNameToBindingName);

		private:
			DrawCall(std::shared_ptr<Program>								program,
					 ContainerPtr                   						data,
				     const std::unordered_map<std::string, std::string>&	attributeBindings,
				     const std::unordered_map<std::string, std::string>&	uniformBindings,
					 const std::unordered_map<std::string, std::string>&	stateBindings,
                     const std::unordered_map<std::string, SamplerState>&   samplerSates);

			void
			bind();

			void
			bindStates();

            template <typename T>
            T
            getDataProperty(const std::string& propertyName)
            {
                watchProperty(propertyName);

                return _data->get<T>(propertyName);
            }

			template <typename T>
            T
            getDataProperty(const std::string& propertyName, T defaultValue)
            {
				if (dataHasProperty(propertyName))
					return _data->get<T>(propertyName);

				return defaultValue;
            }

            bool
            dataHasProperty(const std::string& propertyName);

            void
            watchProperty(const std::string& propertyName);

            void
            boundPropertyChangedHandler(ContainerPtr        data,
                                        const std::string&  propertyName);
		};		
	}
}
