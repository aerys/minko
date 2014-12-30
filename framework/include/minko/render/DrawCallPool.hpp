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
#include "minko/data/Store.hpp"

namespace minko
{
	namespace render
	{
		class DrawCallPool
		{
        private:
            typedef std::list<DrawCall>                                                                 DrawCallList;
            typedef std::list<DrawCall*>                                                                DrawCallPtrList;
            typedef DrawCallList::iterator                                                              DrawCallIterator;
            typedef data::Store::PropertyChangedSignal                                                  PropertyChanged;
            typedef std::pair<PropertyChanged::Slot, uint>                                              ChangedSlot;
            typedef data::Store                                                                         Store;
            typedef std::shared_ptr<data::Collection>                                                   CollectionPtr;
            typedef std::shared_ptr<data::Provider>                                                     ProviderPtr;
            typedef data::MacroBinding                                                                  MacroBinding;
            typedef std::pair<const MacroBinding*, const Store*>                                        MacroBindingKey;
            typedef PropertyChanged::Callback                                                           PropertyCallback;

            typedef std::pair_hash<const MacroBinding*, const Store*>                                   BindingHash;
            typedef std::pair_eq<const MacroBinding*, const Store*>                                     BindingEq;
            typedef std::unordered_map<MacroBindingKey, DrawCallPtrList, BindingHash, BindingEq>        MacroToDrawCallsMap;
            typedef std::unordered_map<MacroBindingKey, ChangedSlot, BindingHash, BindingEq>            MacroToChangedSlotMap;

            typedef std::pair_hash<const data::Binding*, const DrawCall*>                               DrawCallHash;
            typedef std::pair_eq<const data::Binding*, const DrawCall*>                                 DrawCallEq;
            typedef std::pair<const data::Binding*, const DrawCall*>                                    DrawCallKey;
            typedef std::unordered_map<DrawCallKey, PropertyChanged::Slot, DrawCallHash, DrawCallEq>    PropertyChangedSlotMap;

        public:
            typedef std::pair<DrawCallIterator, DrawCallIterator>   DrawCallIteratorPair;

		private:
            DrawCallList                    _drawCalls;
            MacroToDrawCallsMap             _macroToDrawCalls;
            std::unordered_set<DrawCall*>   _invalidDrawCalls;
            MacroToChangedSlotMap           _macroChangedSlot;
            PropertyChangedSlotMap          _propChangedSlot;

		public:
            DrawCallPool();

            ~DrawCallPool()
            {
            }

			const DrawCallList&
            drawCalls()
            {
                return _drawCalls;
            }

            DrawCallIteratorPair
            addDrawCalls(const scene::Layout*                                   layout,
						 std::shared_ptr<Effect>                                effect,
                         const std::string&                                     techniqueName,
                         const std::unordered_map<std::string, std::string>&    variables,
                         data::Store&                                           rootData,
                         data::Store&                                           rendererData,
                         data::Store&                                           targetData);

            void
            removeDrawCalls(const DrawCallIteratorPair& iterators);

            void
            invalidateDrawCalls(const DrawCallIteratorPair&                         iterators,
                                const std::unordered_map<std::string, std::string>& variables);

            void
            update();

        private:
            bool
            compareDrawCalls(DrawCall& a, DrawCall& b);

            math::vec3
            getDrawcallEyePosition(DrawCall& drawcall);

            void
            bindUniforms(data::BindingMap& uniformBindings, Program::Ptr program, DrawCall& drawCall);

            void
            watchProgramSignature(DrawCall&                     drawCall,
                                  const data::MacroBindingMap&  macroBindings,
                                  data::Store&                  rootData,
                                  data::Store&                  rendererData,
                                  data::Store&                  targetData);

            void
            unwatchProgramSignature(DrawCall&                       drawCall,
                                    const data::MacroBindingMap&    macroBindings,
                                    data::Store&                    rootData,
                                    data::Store&                    rendererData,
                                    data::Store&                    targetData);

            void
            macroPropertyAddedHandler(const data::MacroBinding&   macroBinding,
                                      data::Store&                  store,
                                      const std::list<DrawCall*>&   drawCalls);

            void
            macroPropertyRemovedHandler(const data::MacroBinding&   macroBinding,
                                        data::Store&                store,
                                        const std::list<DrawCall*>& drawCalls);

            void
            macroPropertyChangedHandler(const data::MacroBinding& macroBinding, const std::list<DrawCall*>& drawCalls);

            void
            initializeDrawCall(DrawCall& drawCall);

            void
            addMacroCallback(const MacroBindingKey&     key,
                             PropertyChanged&           signal,
                             const PropertyCallback&    callback);

            void
            removeMacroCallback(const MacroBindingKey& key);

            bool
            hasMacroCallback(const MacroBindingKey& key);

            void
            uniformBindingPropertyAddedHandler(DrawCall&                          drawCall,
                                               const ProgramInputs::UniformInput& input,
                                               const data::BindingMap&            uniformBindingMap);
        };
	}
}
