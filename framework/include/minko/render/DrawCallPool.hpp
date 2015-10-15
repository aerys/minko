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
#include "minko/Flyweight.hpp"
#include "minko/Hash.hpp"

#include "sparsehash/forward.h"

namespace minko
{
	namespace render
	{
		class DrawCallPool
		{
        private:
#ifdef MINKO_USE_SPARSE_HASH_MAP
            template <typename... H>
            using map = google::sparse_hash_map<H...>;
#else
            template <class K, typename... V>
            using map = std::unordered_map<K, V...>;
#endif

            typedef std::pair<const data::Binding*, const DrawCall*> DrawCallKey;
            typedef Hash<DrawCallKey> DrawCallKeyHash;
            /*struct DrawCallKeyHash
            {
                inline
                size_t
                operator()(const DrawCallKey& x) const
                {
                    size_t s;

                    Hash_combine(s, x.first);
                    Hash_combine(s, x.second);

                    return s;
                }
            };*/

            typedef std::tuple<float, int*>	SortPropertyTuple;

            struct SortPropertyKeyComparator
            {
                bool
                operator()(const SortPropertyTuple& left,
                           const SortPropertyTuple& right) const;
            };

            typedef std::vector<DrawCall*>	DrawCallList;

            typedef std::map<
                SortPropertyTuple,
                std::array<DrawCallList, 2u>,
                SortPropertyKeyComparator
            >                                                                   DrawCallContainer;
            typedef DrawCallList::iterator                                      DrawCallIterator;
            typedef data::Store::PropertyChangedSignal                          PropertyChanged;
            typedef std::pair<PropertyChanged::Slot, uint>                      ChangedSlot;
            typedef data::Store                                                 Store;
            typedef std::shared_ptr<data::Collection>                           CollectionPtr;
            typedef std::shared_ptr<data::Provider>                             ProviderPtr;
            typedef data::MacroBinding                                          MacroBinding;
            typedef Flyweight<std::string>                                      PropertyName;
            typedef std::tuple<PropertyName, const MacroBinding*, const Store*>	MacroBindingKey;
            typedef PropertyChanged::Callback                                   PropertyCallback;
            typedef map<DrawCall*, std::list<std::function<void(void)>>>	    PropertyRebindFuncMap;
            typedef Hash<MacroBindingKey>                                		MacroBindingKeyHash;
            typedef EqualTo<MacroBindingKey>                            		MacroBindingKeyEq;
            typedef map<MacroBindingKey, DrawCallList*, MacroBindingKeyHash>    MacroToDrawCallsMap;
            typedef map<MacroBindingKey, ChangedSlot, MacroBindingKeyHash>      MacroToChangedSlotMap;
            typedef map<DrawCallKey, PropertyChanged::Slot, DrawCallKeyHash>    PropertyChangedSlotMap;

        public:
            typedef std::pair<DrawCallIterator, DrawCallIterator>   DrawCallIteratorPair;

		private:
			uint							_batchId;
            DrawCallContainer             	_drawCalls;
            MacroToDrawCallsMap*            _macroToDrawCalls;
            std::unordered_map<
                DrawCall*,
                std::pair<bool, EffectVariables>
            >                               _invalidDrawCalls;
            std::unordered_set<DrawCall*>   _drawCallsToBeSorted;
            MacroToChangedSlotMap*          _macroChangedSlot;
            PropertyChangedSlotMap*         _propChangedSlot;
            PropertyChangedSlotMap*         _sortUsefulPropertyChangedSlot;
            static const std::vector<PropertyName> _sortUsefulPropertyNames;
            PropertyChangedSlotMap*         _zSortUsefulPropertyChangedSlot;
            static const std::vector<PropertyName> _zSortUsefulPropertyNames;
            bool                            _mustZSort;

			PropertyRebindFuncMap* 			_drawCallToPropRebindFuncs;

		public:
            DrawCallPool();

            ~DrawCallPool();

			inline
			const DrawCallContainer&
            drawCalls() const
            {
                return _drawCalls;
            }

            uint
            addDrawCalls(std::shared_ptr<Effect>    effect,
                         const std::string&         techniqueName,
                         const EffectVariables&     variables,
                         data::Store&               rootData,
                         data::Store&               rendererData,
                         data::Store&               targetData);

            void
            removeDrawCalls(uint batchId);

            void
            invalidateDrawCalls(uint batchId, const EffectVariables& variables);

            void
            update(bool forceSort = false, bool mustZSort = false);

			void
			clear();

            unsigned int
            numDrawCalls() const;

        private:
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
            macroPropertyAddedHandler(const data::MacroBinding&     macroBinding,
                                      const PropertyName&           propertyName,
                                      data::Store&                  store,
                                      const DrawCallList*           drawCalls);

            void
            macroPropertyRemovedHandler(const data::MacroBinding&   macroBinding,
                                        const PropertyName&         propertyName,
                                        data::Store&                store,
                                        const DrawCallList*         drawCalls);

            void
            macroPropertyChangedHandler(const data::MacroBinding& macroBinding, const DrawCallList* drawCalls);

            void
            initializeDrawCall(DrawCall& drawCall, bool forceRebind = false);

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
                                               const data::BindingMap&            uniformBindingMap,
                                               bool                               forceRebind = false);

            void
            stateBindingPropertyAddedHandler(const std::string&         stateName,
                                             DrawCall&                  drawCall,
                                             const data::BindingMap&    stateBindingMap,
                                             bool                       forceRebind);

            void
            samplerStatesBindingPropertyAddedHandler(DrawCall&                          drawCall,
                                                     const ProgramInputs::UniformInput& input,
                                                     const data::BindingMap&            uniformBindingMap);

            void
            addDrawCallToSortedBucket(DrawCall* drawCall);

            void
            removeDrawCallFromSortedBucket(DrawCall* drawCall);

            DrawCall*
            findDrawCall(const std::function<bool(DrawCall*)>& predicate);

            void
            foreachDrawCall(const std::function<void(DrawCall*)>& function);

			void
			bindDrawCall(DrawCall& 					drawCall,
						 std::shared_ptr<Pass> 		pass,
						 std::shared_ptr<Program> 	program,
						 bool						forceRebind);

			void
			unbindDrawCall(DrawCall& drawCall);

            static
            bool
            compareZSortedDrawCalls(DrawCall* a, DrawCall* b);

            void
            zSortDrawCalls();
        };
	}
}
