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
#include "minko/render/Effect.hpp"
#include "minko/render/DrawCall.hpp"

namespace minko
{
	namespace render
	{

		class DrawCallPool :
			public std::enable_shared_from_this<DrawCallPool>
		{
		public:
			typedef std::shared_ptr<DrawCallPool>   Ptr;

		private:
            std::list<DrawCall>     _drawCalls;
            std::set<std::string>   _watchedProperties;

		public:
			inline static
			Ptr
			create()
			{
				Ptr ptr = std::shared_ptr<DrawCallPool>(new DrawCallPool());

				return ptr;
			}

			const std::list<DrawCall>&
			drawCalls();
			
		private:
			explicit
			DrawCallPool();

			void
			initialize();

            void
            addDrawCalls(std::shared_ptr<Effect>                                effect,
                         const std::unordered_map<std::string, std::string>&    variables,
                         const std::string&                                     techniqueName,
                         std::shared_ptr<data::Container>                       rootData,
                         std::shared_ptr<data::Container>                       rendererData,
                         std::shared_ptr<data::Container>                       targetData)
            {
                const auto& technique = effect->technique(techniqueName);
                
                for (const auto& pass : technique)
                {
                    auto program = pass->selectProgram(
                        translateMacroPropertyNames(pass->macroBindings(), variables),
                        targetData,
                        rendererData,
                        rootData
                    );
                }
            }

            std::unordered_map<std::string, std::string>
            translateMacroPropertyNames(const data::MacroBindingMap&                        macroBindings,
                                        const std::unordered_map<std::string, std::string>& variables)
            {
                std::unordered_map<std::string, std::string> formattedNames;

                for (const auto& macroBinding : macroBindings)
                {
                    for (const auto& variable : variables)
                    {
                        auto pos = macroBinding.first.find(variable.first);

                        if (pos != std::string::npos)
                            formattedNames[macroBinding.first] = macroBinding.first.substr(0, pos)
                            + variable.second + macroBinding.first.substr(pos + variable.second.size());
                    }
                }

                return formattedNames;
            }
		};
	}
}

