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
#include "minko/render/States.hpp"

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
 			typedef std::shared_ptr<Program>											ProgramPtr;
			typedef std::shared_ptr<VertexBuffer>										VertexBufferPtr;
            typedef std::unordered_map<std::string, SamplerState>						SamplerStatesMap;
			typedef std::shared_ptr<States>												StatesPtr;
			typedef std::unordered_map<ProgramSignature, ProgramPtr>					SignatureProgramMap;
			typedef std::shared_ptr<std::function<void(ProgramPtr)>>					OnProgramFunctionPtr;
			typedef std::list<std::function<void(ProgramPtr)>>							OnProgramFunctionList;	
			typedef std::unordered_map<std::string, data::MacroBinding>					MacroBindingsMap;
			typedef std::unordered_map<std::string, std::string>						StringToStringMap;
			typedef std::function<std::string(const std::string&, StringToStringMap&)>	FormatFunction;

		private:
			const std::string						_name;
			ProgramPtr								_programTemplate;
			data::BindingMap						_attributeBindings;
			data::BindingMap						_uniformBindings;
			data::BindingMap						_stateBindings;
			data::MacroBindingMap					_macroBindings;
            StatesPtr								_states;
			std::string								_fallback;
			SignatureProgramMap						_signatureToProgram;

			OnProgramFunctionList					_uniformFunctions;
			OnProgramFunctionList					_attributeFunctions;
			OnProgramFunctionPtr					_indexFunction;
			std::set<std::string>					_undefinedMacros;
			std::set<std::string>					_definedBoolMacros;
			std::unordered_map<std::string, int>	_definedIntMacros;

		public:
			inline static
			Ptr
			create(const std::string&				name,
				   std::shared_ptr<render::Program>	program,
				   const data::BindingMap&			attributeBindings,
				   const data::BindingMap&			uniformBindings,
				   const data::BindingMap&			stateBindings,
				   const data::MacroBindingMap&		macroBindings,
                   StatesPtr         				states,
				   const std::string&				fallback)
			{
				return std::shared_ptr<Pass>(new Pass(
					name,
					program,
					attributeBindings,
					uniformBindings,
					stateBindings,
					macroBindings,
                    states,
					fallback
				));
			}

			inline static
			Ptr
			create(Ptr pass, float, bool deepCopy = false)
			{
				auto p = create(
					pass->_name,
					deepCopy ? Program::create(pass->_programTemplate, deepCopy) : pass->_programTemplate,
					pass->_attributeBindings,
					pass->_uniformBindings,
					pass->_stateBindings,
					pass->_macroBindings,
					deepCopy ? States::create(pass->_states) : pass->_states,
					pass->_fallback
				);

				p->_signatureToProgram = pass->_signatureToProgram;

				p->_uniformFunctions = pass->_uniformFunctions;
				p->_attributeFunctions = pass->_attributeFunctions;
				p->_indexFunction = pass->_indexFunction;
	
				if (pass->_programTemplate->isReady())
				{
					for (auto& f : p->_uniformFunctions)
						f(pass->_programTemplate);
					for (auto& f : p->_attributeFunctions)
						f(pass->_programTemplate);
					if (p->_indexFunction)
						p->_indexFunction->operator()(pass->_programTemplate);
				}

				return p;
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
			const data::MacroBindingMap&
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

			inline
			const std::string&
			fallback()
			{
				return _fallback;
			}

			std::shared_ptr<Program>
			selectProgram(std::shared_ptr<render::DrawCall>		drawCall,
						  std::shared_ptr<data::Container>		targetData,
						  std::shared_ptr<data::Container>		rendererData,
						  std::shared_ptr<data::Container>		rootData,
						  std::list<data::ContainerProperty>&	booleanMacros,
						  std::list<data::ContainerProperty>&	integerMacros,
						  std::list<data::ContainerProperty>&	incorrectIntegerMacros);
			
			template <typename... T>
			void
			setUniform(const std::string& name, const T&... values)
			{
				_uniformFunctions.push_back(std::bind(
					&Pass::setUniformOnProgram<T...>, std::placeholders::_1, name, values...
				));

				if (_programTemplate->isReady())
					_programTemplate->setUniform(name, values...);
				for (auto signatureAndProgram : _signatureToProgram)
					signatureAndProgram.second->setUniform(name, values...);
			}

			inline
			void
			setVertexAttribute(const std::string& name, unsigned int attributeSize, const std::vector<float>& data)
			{
				_attributeFunctions.push_back(std::bind(
					&Pass::setVertexAttributeOnProgram, std::placeholders::_1, name, attributeSize, data
				));

				if (_programTemplate->isReady())
					_programTemplate->setVertexAttribute(name, attributeSize, data);
				for (auto signatureAndProgram : _signatureToProgram)
					signatureAndProgram.second->setVertexAttribute(name, attributeSize, data);
			}

			inline
			void
			setIndexBuffer(const std::vector<unsigned short>& indices)
			{
				_indexFunction = std::make_shared<std::function<void(ProgramPtr)>>(std::bind(
					&Pass::setIndexBufferOnProgram, std::placeholders::_1, indices
				));
					
				if (_programTemplate->isReady())
					_programTemplate->setIndexBuffer(indices);
				for (auto signatureAndProgram : _signatureToProgram)
					signatureAndProgram.second->setIndexBuffer(indices);
			}

			inline
			void
			define(const std::string& macroName)
			{
				if (!macroName.empty())
					_definedBoolMacros.insert(macroName);
			}

			inline
			void
			define(const std::string& macroName, int macroValue)
			{
				if (!macroName.empty())
					_definedIntMacros[macroName] = macroValue;
			}

			inline
			void
			undefine(const std::string& macroName)
			{
				if (!macroName.empty())
					_undefinedMacros.insert(macroName);
			}

			void
			getExplicitDefinitions(std::unordered_map<std::string, data::MacroBindingDefault>& macroNameToValue) const;

			inline
			bool
			isExplicitlyUndefined(const std::string& macroName) const
			{
				return _undefinedMacros.find(macroName) != _undefinedMacros.end();
			}

		private:
			Pass(const std::string&					name,
				 std::shared_ptr<render::Program>	program,
				 const data::BindingMap&			attributeBindings,
				 const data::BindingMap&			uniformBindings,
				 const data::BindingMap&			stateBindings,
				 const data::MacroBindingMap&		macroBindings,
                 std::shared_ptr<States>            states,
				 const std::string&					fallback);

			template <typename... T>
			static
			void
			setUniformOnProgram(std::shared_ptr<Program> program, const std::string& name, const T&... values)
			{
				program->setUniform(name, values...);
			}

			static
			void
			setVertexAttributeOnProgram(std::shared_ptr<Program> program, const std::string& name, unsigned int attributeSize, const std::vector<float>& data)
			{
				program->setVertexAttribute(name, attributeSize, data);
			}

			static 
			void
			setIndexBufferOnProgram(std::shared_ptr<Program> program, const std::vector<unsigned short>& indices)
			{
				program->setIndexBuffer(indices);
			}

			ProgramPtr
			finalizeProgram(ProgramPtr program);
		};

		template <>
		inline
		void
		Pass::setUniform(const std::string& name, const math::vec2& v)
		{
			setUniform(name, v.x, v.y);
		};

		template <>
		inline
		void
		Pass::setUniform(const std::string& name, const math::vec3& v)
		{
			setUniform(name, v.x, v.y, v.z);
		};

		template <>
		inline
		void
		Pass::setUniform(const std::string& name, const math::vec4& v)
		{
			setUniform(name, v.x, v.y, v.z, v.w);
		};

		template <>
		inline
		void
		Pass::setUniform(const std::string& name, const math::mat4& v)
		{
			setUniform(name, 1, false, math::value_ptr(v));
		};
	}
}
