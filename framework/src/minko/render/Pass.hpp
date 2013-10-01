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
#include "minko/render/ProgramSignature.hpp"
#include "minko/render/Program.hpp"

namespace minko
{
	namespace render
	{
		class Pass :
			public std::enable_shared_from_this<Pass>
		{
		public:
			typedef std::shared_ptr<Pass> Ptr;

		private:
 			typedef std::shared_ptr<Program>							ProgramPtr;
            typedef std::unordered_map<std::string, SamplerState>		SamplerStatesMap;
			typedef std::shared_ptr<States>								StatesPtr;
			typedef std::unordered_map<ProgramSignature, ProgramPtr>	SignatureProgramMap;
			typedef std::function<void(ProgramPtr)>						SetUniformFct;
			typedef std::unordered_map<std::string, SetUniformFct>		UniformFctMap;

		private:
			const std::string	_name;
			ProgramPtr			_programTemplate;
			data::BindingMap	_attributeBindings;
			data::BindingMap	_uniformBindings;
			data::BindingMap	_stateBindings;
			data::BindingMap	_macroBindings;
            StatesPtr           _states;
			SignatureProgramMap	_signatureToProgram;

			UniformFctMap		_uniformFunctions;

		public:
			inline static
			Ptr
			create(const std::string&				name,
				   std::shared_ptr<render::Program>	program,
				   const data::BindingMap&			attributeBindings,
				   const data::BindingMap&			uniformBindings,
				   const data::BindingMap&			stateBindings,
				   const data::BindingMap&			macroBindings,
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
			const data::BindingMap&
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

			std::shared_ptr<Program>
			selectProgram(std::shared_ptr<data::Container> 	data,
						  std::shared_ptr<data::Container> 	rootData,
						  std::list<std::string>&			bindingDefines,
						  std::list<std::string>&			bindingValues);

			void
			finalizeProgram(ProgramPtr program);

			template <typename... T>
			void
			setUniform(const std::string& name, const T&... values)
			{
				_uniformFunctions[name] = std::bind(
					&Pass::setUniformOnProgram, shared_from_this(), std::placeholders::_1, name, values...
				);

				for (auto signatureAndProgram : _signatureToProgram)
					signatureAndProgram.second->setUniform(name, values...);
			}

			template <typename... T>
			void
			setUniformOnProgram(std::shared_ptr<Program> program, const std::string& name, const T&... values)
			{
				program->setUniform(name, values...);
			}

		private:
			Pass(const std::string&					name,
				 std::shared_ptr<render::Program>	program,
				 const data::BindingMap&			attributeBindings,
				 const data::BindingMap&			uniformBindings,
				 const data::BindingMap&			stateBindings,
				 const data::BindingMap&			macroBindings,
                 std::shared_ptr<States>            states);
		};
	}
}
