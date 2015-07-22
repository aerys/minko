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
#include "minko/data/MacroBinding.hpp"
#include "minko/data/BindingMap.hpp"

namespace minko
{
	namespace render
	{
		class ProgramSignature
		{
        public:
            typedef std::uint64_t                           MaskType;

		private:
            static const uint                               _maxNumMacros;

			MaskType				                    	_mask;
            std::vector<Any>	                    		_values;
            std::vector<data::MacroBindingMap::MacroType>   _types;
            std::vector<Flyweight<std::string>>             _macros;

		public:
			ProgramSignature(const data::MacroBindingMap&   macroBindings,
                             const EffectVariables&         variables,
						     const data::Store&             targetData,
                             const data::Store&		        rendererData,
                             const data::Store&             rootData);

            ProgramSignature(const ProgramSignature& signature);

            ~ProgramSignature()
            {
                _values.clear();
                _types.clear();
                _macros.clear();
            }

			bool
			operator==(const ProgramSignature&) const;

            void
            updateProgram(Program& program) const;

            inline
			MaskType
			mask() const
			{
				return _mask;
			}

			inline
			const std::vector<Any>&
			values() const
			{
				return _values;
			}

            inline
            const std::vector<data::MacroBindingMap::MacroType>&
            types()
            {
                return _types;
            }

            inline
            const std::vector<Flyweight<std::string>>&
            macros() const
            {
                return _macros;
            }

        private:
            Any
            getValueFromStore(const data::Store&        				store,
                              const std::string&        				propertyName,
							  const data::MacroBindingMap::MacroType&   type);
		};
	}
}
//
//namespace std
//{
//	template<>
//    struct hash<minko::render::ProgramSignature>
//	{
//		inline
//		size_t
//		operator()(const minko::render::ProgramSignature& x) const
//		{
//			size_t seed = std::hash<minko::uint>()(x.mask());
//            auto i = 0u;
//
//            for (auto value : x.values())
//                hash_combine(seed, (x.mask() >> i) != 0 ? value : 0);
//
//			return seed;
//		}
//	};
//}
