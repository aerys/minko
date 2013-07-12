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

namespace minko
{
	namespace render
	{
		class Pass
		{
		public:
			typedef std::shared_ptr<Pass> Ptr;

		private:
			typedef const std::unordered_map<std::string, std::string>	BindingMap;
			typedef std::shared_ptr<Program>							ProgramPtr;

		private:
			const std::string					_name;
			ProgramPtr							_programTemplate;
			BindingMap							_attributeBindings;
			BindingMap							_uniformBindings;
			BindingMap							_stateBindings;
			BindingMap							_macroBindings;
			const float							_priority;
			Blending::Source					_blendingSourceFactor;
			Blending::Destination				_blendingDestinationFactor;
			bool								_depthMask;
			CompareMode							_depthFunc;

			std::map<unsigned int, ProgramPtr>	_signatureToProgram;

		public:
			inline static
			Ptr
			create(const std::string&				name,
				   std::shared_ptr<render::Program>	program,
				   BindingMap&						attributeBindings,
				   BindingMap&						uniformBindings,
				   BindingMap&						stateBindings,
				   BindingMap&						macroBindings,
				   const float						priority					= 0.f,
				   Blending::Source					blendingSourceFactor		= Blending::Source::ONE,
				   Blending::Destination			blendingDestinationFactor	= Blending::Destination::ZERO,
				   bool								depthMask					= true,
				   CompareMode						depthFunc					= CompareMode::LESS)
			{
				return std::shared_ptr<Pass>(new Pass(
					name,
					program,
					attributeBindings,
					uniformBindings,
					stateBindings,
					macroBindings,
					priority,
					blendingSourceFactor,
					blendingDestinationFactor,
					depthMask,
					depthFunc
				));
			}

			inline
			const std::string&
			name()
			{
				return _name;
			}

			inline
			const BindingMap&
			attributeBindings()
			{
				return _attributeBindings;
			}

			inline
			const BindingMap&
			uniformBindings()
			{
				return _uniformBindings;
			}

			inline
			const BindingMap&
			stateBindings()
			{
				return _stateBindings;
			}

			std::shared_ptr<DrawCall>
			createDrawCall(std::shared_ptr<data::Container> data);

		private:
			Pass(const std::string&					name,
				 std::shared_ptr<render::Program>	program,
				 BindingMap&						attributeBindings,
				 BindingMap&						uniformBindings,
				 BindingMap&						stateBindings,
				 BindingMap&						macroBindings,
				 const float						priority,
				 Blending::Source					blendingSourceFactor,
				 Blending::Destination				blendingDestinationFactor,
				 bool								depthMask,
				 CompareMode						depthFunc);

			const unsigned int
			buildSignature(std::shared_ptr<data::Container> data);

			std::shared_ptr<Program>
			selectProgram(std::shared_ptr<data::Container> data);
		};
	}
}
