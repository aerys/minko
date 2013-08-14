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

#include "minko/render/Blending.hpp"
#include "minko/render/CompareMode.hpp"
#include "minko/render/TriangleCulling.hpp"

namespace minko
{
	namespace render
	{
		class Pass
		{
		public:
			typedef std::shared_ptr<Pass> Ptr;

		private:
            typedef std::shared_ptr<data::Binding>                          BindingPtr;
			typedef std::shared_ptr<Program>							    ProgramPtr;
            typedef std::unordered_map<std::string, render::SamplerState>   SamplerStatesMap;
			typedef std::shared_ptr<States>									StatesPtr;

		private:
			const std::string		            _name;
			ProgramPtr				            _programTemplate;
			data::BindingMap				    _attributeBindings;
			data::BindingMap				    _uniformBindings;
			data::BindingMap				    _stateBindings;
			data::BindingMap				    _macroBindings;
            StatesPtr           				_states;
			std::map<unsigned int, ProgramPtr>	_signatureToProgram;

		public:
			inline static
			Ptr
			create(const std::string&				name,
				   std::shared_ptr<render::Program>	program,
				   data::BindingMap&				attributeBindings,
				   data::BindingMap&				uniformBindings,
				   data::BindingMap&				stateBindings,
				   data::BindingMap&				macroBindings,
                   StatesPtr         				states)
			{
				return std::shared_ptr<Pass>(new Pass(
					name,
					program,
					attributeBindings,
					uniformBindings,
					stateBindings,
					macroBindings,
                    states
				));
			}

			inline
			const std::string&
			name()
			{
				return _name;
			}

			inline
			std::shared_ptr<Program>
			program()
			{
				return _programTemplate;
			}

			inline
			const data::BindingMap&
			attributeBindings() const
			{
				return _attributeBindings;
			}

			inline
			const data::BindingMap&
			uniformBindings() const
			{
				return _uniformBindings;
			}

			inline
			const data::BindingMap&
			stateBindings() const
			{
				return _stateBindings;
			}

			inline
			const BindingMap&
			macroBindings() const
			{
				return _macroBindings;
			}

			inline
			StatesPtr
			states() const
			{
				return _states;
			}

			std::shared_ptr<DrawCall>
			createDrawCall(std::shared_ptr<data::Container> data, std::shared_ptr<data::Container> rootData);

		private:
			Pass(const std::string&					name,
				 std::shared_ptr<render::Program>	program,
				 data::BindingMap&					attributeBindings,
				 data::BindingMap&					uniformBindings,
				 data::BindingMap&					stateBindings,
				 data::BindingMap&					macroBindings,
                 std::shared_ptr<States>            states);

			const unsigned int
			buildSignature(std::shared_ptr<data::Container> data, std::shared_ptr<data::Container> rootData);

			std::shared_ptr<Program>
			selectProgram(std::shared_ptr<data::Container> data, std::shared_ptr<data::Container> rootData);
		};
	}
}
