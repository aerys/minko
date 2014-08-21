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
#include "minko/render/Effect.hpp"
#include "minko/render/DrawCall.hpp"
#include "minko/data/Container.hpp"

namespace minko
{
	namespace render
	{
		class DrawCallPool :
			public std::enable_shared_from_this<DrawCallPool>
		{
        private:
            typedef std::list<DrawCall*>::iterator                  DrawCallIterator;
            typedef data::Container::PropertyChangedSignal::Slot    PropertyChangedSlot;

        public:
            typedef std::pair<DrawCallIterator, DrawCallIterator>   DrawCallIteratorPair;

		private:
            std::list<DrawCall*>                                    _drawCalls;
            std::set<std::string>                                   _watchedProperties;
            std::unordered_map<data::Binding, std::list<DrawCall*>> _macroToDrawCalls;
            std::unordered_set<DrawCall*>                           _changedDrawCalls;

            std::unordered_map<data::Binding, PropertyChangedSlot>  _macroChangedSlot;

		public:
            ~DrawCallPool()
            {

            }

			const std::list<DrawCall*>&
            drawCalls()
            {
                return _drawCalls;
            }
			
            DrawCallIteratorPair
            addDrawCalls(std::shared_ptr<Effect>                                effect,
                         const std::unordered_map<std::string, std::string>&    variables,
                         const std::string&                                     techniqueName,
                         data::Container&                                       rootData,
                         data::Container&                                       rendererData,
                         data::Container&                                       targetData);

            void
            removeDrawCalls(const DrawCallIteratorPair& iterators);

            void
            update();

        private:
            void
            watchProgramSignature(DrawCall*                     drawCall,
                                  const ProgramSignature&       signature,
                                  const data::MacroBindingMap&  macroBindings,
                                  data::Container&              rootData,
                                  data::Container&              rendererData,
                                  data::Container&              targetData);

            void
            unwatchProgramSignature(DrawCall*                     drawCall,
                                    const ProgramSignature&       signature,
                                    const data::MacroBindingMap&  macroBindings);

            void
            macroPropertyChangedHandler(const std::list<DrawCall*>& drawCalls);

            void
            initializeDrawCall(DrawCall& drawCall);
		};
	}
}

