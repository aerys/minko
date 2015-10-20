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

#include "minko/render/DrawCallPool.hpp"

#include "minko/data/ResolvedBinding.hpp"

#ifdef MINKO_USE_SPARSE_HASH_MAP
# include "sparsehash/sparse_hash_map"
#endif

using namespace minko;
using namespace minko::render;

const std::vector<DrawCallPool::PropertyName> DrawCallPool::_zSortUsefulPropertyNames = {
    "modelToWorldMatrix",
    "material[${materialUuid}].priority",
    "material[${materialUuid}].zSorted",
    "geometry[${geometryUuid}].position"
};

const std::vector<DrawCallPool::PropertyName> DrawCallPool::_sortUsefulPropertyNames = {
    "material[${materialUuid}].priority",
    "material[${materialUuid}].zSorted",
    "material[${materialUuid}].target"
};

bool
DrawCallPool::SortPropertyKeyComparator::operator()(const SortPropertyTuple& left,
                                                    const SortPropertyTuple& right) const
{
    const auto leftPriority = std::get<0>(left);
    const auto rightPriority = std::get<0>(right);

    const bool samePriority = fabsf(leftPriority - rightPriority) < 1e-3f;

    if (samePriority)
    {
        const auto leftTargetId = std::get<1>(left);
        const auto rightTargetId = std::get<1>(right);

        return leftTargetId < rightTargetId;
    }

    return leftPriority > rightPriority;
}

DrawCallPool::DrawCallPool() :
    _batchId(0),
    _drawCalls(),
    _macroToDrawCalls(new MacroToDrawCallsMap()),
    _invalidDrawCalls(),
    _macroChangedSlot(new MacroToChangedSlotMap()),
    _propChangedSlot(new PropertyChangedSlotMap()),
    _drawCallToPropRebindFuncs(new PropertyRebindFuncMap()),
    _sortUsefulPropertyChangedSlot(new PropertyChangedSlotMap()),
    _zSortUsefulPropertyChangedSlot(new PropertyChangedSlotMap()),
    _mustZSort(false)
{
#ifdef MINKO_USE_SPARSE_HASH_MAP
    _macroToDrawCalls->set_deleted_key(MacroBindingKey("", nullptr, nullptr));
    _macroChangedSlot->set_deleted_key(MacroBindingKey("", nullptr, nullptr));
    _propChangedSlot->set_deleted_key(DrawCallKey(nullptr, nullptr));
    _sortUsefulPropertyChangedSlot->set_deleted_key(DrawCallKey(nullptr, nullptr));
    _zSortUsefulPropertyChangedSlot->set_deleted_key(DrawCallKey(nullptr, nullptr));
    _drawCallToPropRebindFuncs->set_deleted_key(nullptr);
#endif
}

DrawCallPool::~DrawCallPool()
{
    delete _macroToDrawCalls;
    delete _macroChangedSlot;
    delete _propChangedSlot;
    delete _drawCallToPropRebindFuncs;
}

uint
DrawCallPool::addDrawCalls(Effect::Ptr              effect,
                           const std::string&       techniqueName,
                           const EffectVariables&   variables,
                           data::Store&             rootData,
                           data::Store&             rendererData,
                           data::Store&             targetData)
{
    const auto& technique = effect->technique(techniqueName);

    _batchId++;
    for (const auto& pass : technique)
    {
        DrawCall* drawCall = new DrawCall(_batchId, pass, variables, rootData, rendererData, targetData);

        initializeDrawCall(*drawCall);

        // if the draw call is meant only for post-processing, then it should only exist once
        if (!pass->isForward())
        {
            auto seekedDrawCall = findDrawCall(
                [&](const DrawCall* d)
                {
                    return d->program() == drawCall->program();
                }
            );

            // FIXME: cumbersome and wasteful to completely init. a DrawCall just to discard it
            if (seekedDrawCall != nullptr)
            {
                seekedDrawCall->batchIDs().push_back(_batchId);
                delete drawCall;
                continue;
            }
        }

        addDrawCallToSortedBucket(drawCall);
    }

    return _batchId;
}

void
DrawCallPool::removeDrawCalls(uint batchId)
{
    for (auto& priorityAndTargetIdToDrawCalls : _drawCalls)
    {
        for (auto& drawCalls : priorityAndTargetIdToDrawCalls.second)
        {
            drawCalls.erase(std::remove_if(
                drawCalls.begin(),
                drawCalls.end(),
                [&](DrawCall* drawCall)
                {
                    auto& batchIDs = drawCall->batchIDs();
                    auto it = std::find(batchIDs.begin(), batchIDs.end(), batchId);

                    if (it != batchIDs.end())
                    {
                        batchIDs.erase(it);

                        if (batchIDs.size() != 0)
                            return false;

                        unwatchProgramSignature(
                            *drawCall,
                            drawCall->pass()->macroBindings(),
                            drawCall->rootData(),
                            drawCall->rendererData(),
                            drawCall->targetData()
                        );
                        unbindDrawCall(*drawCall);

                        _invalidDrawCalls.erase(drawCall);
                        _drawCallsToBeSorted.erase(drawCall);

                        delete drawCall;

                        assert(_drawCallToPropRebindFuncs->count(drawCall) == 0);
                        for (auto it = _propChangedSlot->begin(); it != _propChangedSlot->end(); ++it)
                            assert(it->first.second != drawCall);

                        return true;
                    }

                    return false;
                }
            ), drawCalls.end());
        }
    }
}

void
DrawCallPool::watchProgramSignature(DrawCall&                       drawCall,
                                    const data::MacroBindingMap&    macroBindings,
                                    data::Store&                    rootData,
                                    data::Store&                    rendererData,
                                    data::Store&                    targetData)
{
    for (const auto& macroNameAndBinding : macroBindings.bindings)
    {
        const auto& macroName = macroNameAndBinding.first;
        const auto& macroBinding = macroNameAndBinding.second;
        auto& store = macroBinding.source == data::Binding::Source::ROOT ? rootData
            : macroBinding.source == data::Binding::Source::RENDERER ? rendererData
            : targetData;
        auto propertyName = Store::getActualPropertyName(drawCall.variables(), macroBinding.propertyName);
        auto bindingKey = MacroBindingKey(propertyName, &macroBinding, &store);

        DrawCallList* drawCalls = _macroToDrawCalls->count(bindingKey) != 0
            ? (*_macroToDrawCalls)[bindingKey]
            : (*_macroToDrawCalls)[bindingKey] = new DrawCallList();

        assert(std::find(drawCalls->begin(), drawCalls->end(), &drawCall) == drawCalls->end());

        drawCalls->push_back(&drawCall);

        if (macroBindings.types.at(macroName) != data::MacroBindingMap::MacroType::UNSET)
        {
            addMacroCallback(
                bindingKey,
                store.propertyChanged(propertyName),
                [&, drawCalls, this](data::Store&, data::Provider::Ptr, const data::Provider::PropertyName&)
                {
                    macroPropertyChangedHandler(macroBinding, drawCalls);
                }
            );
        }
        else
        {
            auto hasProperty = store.hasProperty(propertyName);

            addMacroCallback(
                bindingKey,
                hasProperty ? store.propertyRemoved(propertyName) : store.propertyAdded(propertyName),
                [&, hasProperty, propertyName, drawCalls, this](data::Store& s, data::Provider::Ptr, const data::Provider::PropertyName& p)
                {
                    if (hasProperty)
                        macroPropertyRemovedHandler(macroBinding, propertyName, s, drawCalls);
                    else
                        macroPropertyAddedHandler(macroBinding, propertyName, s, drawCalls);
                }
            );
        }
    }
}

void
DrawCallPool::addMacroCallback(const MacroBindingKey&   key,
                               PropertyChanged&         signal,
                               const PropertyCallback&  callback)
{
    //if (_macroChangedSlot->count(key) == 0)
        (*_macroChangedSlot)[key] = ChangedSlot(signal.connect(callback), 1);
    /*else
        (*_macroChangedSlot)[key].second++;*/
}

void
DrawCallPool::removeMacroCallback(const MacroBindingKey& key)
{
    if (!_macroChangedSlot->count(key))
        return;

    assert((*_macroChangedSlot)[key].second != 0);

    (*_macroChangedSlot)[key].second--;
    if ((*_macroChangedSlot)[key].second == 0)
        _macroChangedSlot->erase(key);
}

bool
DrawCallPool::hasMacroCallback(const MacroBindingKey& key)
{
    return _macroChangedSlot->count(key) != 0;
}

void
DrawCallPool::macroPropertyChangedHandler(const data::MacroBinding& macroBinding, const DrawCallList* drawCalls)
{
    for (auto* drawCall : *drawCalls)
    {
        _invalidDrawCalls.emplace(drawCall, std::make_pair(false, EffectVariables()));
    }
}

void
DrawCallPool::macroPropertyAddedHandler(const data::MacroBinding&   macroBinding,
                                        const PropertyName&         propertyName,
                                        data::Store&                store,
                                        const DrawCallList*         drawCalls)
{
    MacroBindingKey key(propertyName, &macroBinding, &store);

    removeMacroCallback(key);
    addMacroCallback(
        key,
        store.propertyRemoved(propertyName),
        [&, propertyName, drawCalls, this](data::Store& s, data::Provider::Ptr, const data::Provider::PropertyName& p)
        {
            macroPropertyRemovedHandler(macroBinding, propertyName, s, drawCalls);
        }
    );

    macroPropertyChangedHandler(macroBinding, drawCalls);
}

void
DrawCallPool::macroPropertyRemovedHandler(const data::MacroBinding&     macroBinding,
                                          const PropertyName&           propertyName,
                                          data::Store&                  store,
                                          const DrawCallList*           drawCalls)
{
    // If the store still has the property, it means that it was not really removed
    // but that one of the copies of the properties was removed (ie same material added multiple
    // times to the same store). Thus the macro state should not be affected.
    if (store.hasProperty(propertyName))
        return;

    MacroBindingKey key(propertyName, &macroBinding, &store);

    removeMacroCallback(key);
    addMacroCallback(
        key,
        store.propertyAdded(propertyName),
        [&, propertyName, drawCalls, this](data::Store& s, data::Provider::Ptr, const data::Provider::PropertyName& p)
        {
            macroPropertyAddedHandler(macroBinding, propertyName, s, drawCalls);
        }
    );

    macroPropertyChangedHandler(macroBinding, drawCalls);
}

void
DrawCallPool::unwatchProgramSignature(DrawCall&                     drawCall,
                                      const data::MacroBindingMap&  macroBindings,
                                      data::Store&                  rootData,
                                      data::Store&                  rendererData,
                                      data::Store&                  targetData)
{
    for (const auto& macroNameAndBinding : macroBindings.bindings)
    {
        const auto& macroBinding = macroNameAndBinding.second;
        auto& store = macroBinding.source == data::Binding::Source::ROOT ? rootData
            : macroBinding.source == data::Binding::Source::RENDERER ? rendererData
            : targetData;
        auto propertyName = Store::getActualPropertyName(drawCall.variables(), macroBinding.propertyName);
        auto bindingKey = MacroBindingKey(propertyName, &macroBinding, &store);

        auto* drawCalls = (*_macroToDrawCalls)[bindingKey];

        drawCalls->erase(std::remove(drawCalls->begin(), drawCalls->end(), &drawCall), drawCalls->end());

        if (drawCalls->size() == 0)
        {
            delete drawCalls;
            _macroToDrawCalls->erase(bindingKey);
        }

        removeMacroCallback(bindingKey);
    }
}

void
DrawCallPool::initializeDrawCall(DrawCall& drawCall, bool forceRebind)
{
    auto invalidDrawCallIt = _invalidDrawCalls.find(&drawCall);
    const auto variablesChanged = invalidDrawCallIt != _invalidDrawCalls.end() &&
                                  invalidDrawCallIt->second.first;
    auto newVariables = EffectVariables();

    if (variablesChanged)
        newVariables = invalidDrawCallIt->second.second;

    const auto& variables = variablesChanged
        ? newVariables
        : drawCall.variables();

    auto pass = drawCall.pass();

    auto programAndSignature = pass->selectProgram(
        variables, drawCall.targetData(), drawCall.rendererData(), drawCall.rootData()
    );

    auto program = programAndSignature.first;

    if (program == drawCall.program())
    {
        if (variablesChanged)
            drawCall.variables().assign(variables.begin(), variables.end());

        return;
    }

    if (drawCall.program())
    {
        unwatchProgramSignature(
            drawCall,
            drawCall.pass()->macroBindings(),
            drawCall.rootData(),
            drawCall.rendererData(),
            drawCall.targetData()
        );
        unbindDrawCall(drawCall);
    }

    if (variablesChanged)
        drawCall.variables().assign(variables.begin(), variables.end());

    bindDrawCall(drawCall, pass, program, forceRebind);

    if (programAndSignature.second != nullptr)
    {
        watchProgramSignature(
            drawCall,
            drawCall.pass()->macroBindings(),
            drawCall.rootData(),
            drawCall.rendererData(),
            drawCall.targetData()
        );
    }
}

void
DrawCallPool::uniformBindingPropertyAddedHandler(DrawCall&                          drawCall,
                                                 const ProgramInputs::UniformInput& input,
                                                 const data::BindingMap&            uniformBindingMap,
                                                 bool                               forceRebind)
{
    if (!forceRebind && _invalidDrawCalls.find(&drawCall) != _invalidDrawCalls.end())
        return;

    data::ResolvedBinding* resolvedBinding = drawCall.bindUniform(
        input, uniformBindingMap.bindings, uniformBindingMap.defaultValues
    );

    if (resolvedBinding != nullptr)
    {
        auto& propertyName = resolvedBinding->propertyName;
        auto bindingPtr = &resolvedBinding->binding;
        auto propertyExist = resolvedBinding->store.hasProperty(propertyName);
        auto& signal = propertyExist
            ? resolvedBinding->store.propertyRemoved(propertyName)
            : resolvedBinding->store.propertyAdded(propertyName);

        _propChangedSlot->insert(std::make_pair(
            std::make_pair(bindingPtr, &drawCall),
            signal.connect(
                [&, bindingPtr](data::Store&, data::Provider::Ptr, const data::Provider::PropertyName&)
                {
                    _propChangedSlot->erase({ bindingPtr, &drawCall });
                    (*_drawCallToPropRebindFuncs)[&drawCall].push_back(
                        [&, forceRebind, this]()
                        {
                            uniformBindingPropertyAddedHandler(drawCall, input, uniformBindingMap, forceRebind);
                        }
                    );
                }
            )
        ));

        // If this draw call needs to be sorted
        // => we listen to the useful properties
        if (propertyExist && drawCall.zSorted())
        {
            auto propertyRelatedToZSortIt = std::find_if(
                _zSortUsefulPropertyNames.begin(),
                _zSortUsefulPropertyNames.end(),
                [&](const PropertyName& zSortUsefulPropertyName) -> bool
                {
                    return
                        data::Store::getActualPropertyName(drawCall.variables(), zSortUsefulPropertyName) ==
                        propertyName;
                }
            );

            if (propertyRelatedToZSortIt != _zSortUsefulPropertyNames.end())
            {
                // Bind the signal to request a Z-sorting if one of these properties changed
                _zSortUsefulPropertyChangedSlot->insert(
                    std::make_pair(
                        std::make_pair(bindingPtr, &drawCall),
                        resolvedBinding->store.propertyChanged(propertyName).connect(
                            [&](data::Store&, data::Provider::Ptr, const data::Provider::PropertyName&)
                            {
                                _mustZSort = true;
                            }
                        )
                    )
                );
            }
        }

        delete resolvedBinding;
    }

    if (input.type == ProgramInputs::Type::sampler2d || input.type == ProgramInputs::Type::samplerCube)
        samplerStatesBindingPropertyAddedHandler(drawCall, input, uniformBindingMap);
}

void
DrawCallPool::samplerStatesBindingPropertyAddedHandler(DrawCall&                          drawCall,
                                                       const ProgramInputs::UniformInput& input,
                                                       const data::BindingMap&            uniformBindingMap)
{
    auto resolvedBindings = drawCall.bindSamplerStates(
        input, uniformBindingMap.bindings, uniformBindingMap.defaultValues
    );

    for (auto resolvedBinding : resolvedBindings)
    {
        if (resolvedBinding != nullptr)
        {
            auto& propertyName = resolvedBinding->propertyName;
            auto& signal = resolvedBinding->store.hasProperty(propertyName)
                ? resolvedBinding->store.propertyRemoved(propertyName)
                : resolvedBinding->store.propertyAdded(propertyName);

            _propChangedSlot->insert(std::make_pair(
                std::make_pair(&resolvedBinding->binding, &drawCall),
                signal.connect(
                    [&](data::Store&, data::Provider::Ptr, const data::Provider::PropertyName&)
                    {
                        (*_drawCallToPropRebindFuncs)[&drawCall].push_back(
                            [&, this]()
                            {
                                samplerStatesBindingPropertyAddedHandler(drawCall, input, uniformBindingMap);
                            }
                        );
                    }
                )
            ));

            delete resolvedBinding;
        }
    }
}

void
DrawCallPool::stateBindingPropertyAddedHandler(const std::string&       stateName,
                                               DrawCall&                drawCall,
                                               const data::BindingMap&  stateBindingMap,
                                               bool                     forceRebind)
{
    if (!forceRebind && _invalidDrawCalls.find(&drawCall) != _invalidDrawCalls.end())
        return;

    auto resolvedBinding = drawCall.bindState(stateName, stateBindingMap.bindings, stateBindingMap.defaultValues);

    if (resolvedBinding != nullptr)
    {
        auto bindingPtr = &resolvedBinding->binding;
        auto& propertyName = resolvedBinding->propertyName;
        auto& signal = resolvedBinding->store.hasProperty(propertyName)
            ? resolvedBinding->store.propertyRemoved(propertyName)
            : resolvedBinding->store.propertyAdded(propertyName);

        _propChangedSlot->insert(
            std::make_pair(
                std::make_pair(&resolvedBinding->binding, &drawCall),
                signal.connect(
                [&, bindingPtr](data::Store&, data::Provider::Ptr, const data::Provider::PropertyName&)
                {
                    _propChangedSlot->erase({ bindingPtr, &drawCall });

                    (*_drawCallToPropRebindFuncs)[&drawCall].push_back(
                    [&, forceRebind, this]()
                    {
                        stateBindingPropertyAddedHandler(stateName, drawCall, stateBindingMap, forceRebind);
                    });
                })
            )
        );

        auto propertyRelatedToSortIt = std::find_if(
            _sortUsefulPropertyNames.begin(),
            _sortUsefulPropertyNames.end(),
            [&](const PropertyName& sortUsefulPropertyName) -> bool
            {
                return
                    data::Store::getActualPropertyName(drawCall.variables(), sortUsefulPropertyName) ==
                    propertyName;
            }
        );

        if (propertyRelatedToSortIt != _sortUsefulPropertyNames.end())
        {
            _sortUsefulPropertyChangedSlot->insert(
                std::make_pair(
                    std::make_pair(bindingPtr, &drawCall),
                    resolvedBinding->store.propertyChanged(propertyName).connect(
                        [&](data::Store&, data::Provider::Ptr, const data::Provider::PropertyName&)
                        {
                            _drawCallsToBeSorted.insert(&drawCall);
                        }
                    )
                )
            );
        }

        delete resolvedBinding;
    }
}

void
DrawCallPool::update(bool forceSort, bool mustZSort)
{
    for (auto& invalidDrawCall : _invalidDrawCalls)
    {
        auto* drawCallPtr = invalidDrawCall.first;

        initializeDrawCall(*drawCallPtr, true);
    }
    _invalidDrawCalls.clear();

    for (auto drawCallPtrAndFuncList : *_drawCallToPropRebindFuncs)
    {
        for (auto& func : drawCallPtrAndFuncList.second)
            func();
    }

    _drawCallToPropRebindFuncs->clear();
#ifdef MINKO_USE_SPARSE_HASH_MAP
    _drawCallToPropRebindFuncs->resize(0);
#endif

    for (auto drawCall : _drawCallsToBeSorted)
    {
        removeDrawCallFromSortedBucket(drawCall);
        addDrawCallToSortedBucket(drawCall);
    }

    _drawCallsToBeSorted.clear();

    const auto finalMustZSort = forceSort || _mustZSort || mustZSort;

    if (finalMustZSort)
    {
        _mustZSort = false;

        zSortDrawCalls();
    }
}

void
DrawCallPool::invalidateDrawCalls(uint batchId, const EffectVariables& variables)
{
    foreachDrawCall(
        [&](DrawCall* drawCall)
        {
            auto& batchIDs = drawCall->batchIDs();
            auto it = std::find(batchIDs.begin(), batchIDs.end(), batchId);

            if (it != batchIDs.end())
            {
                _invalidDrawCalls[drawCall] = std::make_pair(true, variables);
            }
        }
    );
}

void
DrawCallPool::clear()
{
    _drawCalls.clear();
    _macroToDrawCalls->clear();
#ifdef MINKO_USE_SPARSE_HASH_MAP
    _macroToDrawCalls->resize(0);
#endif
    _invalidDrawCalls.clear();
    _macroChangedSlot->clear();
#ifdef MINKO_USE_SPARSE_HASH_MAP
    _macroChangedSlot->resize(0);
#endif
    _propChangedSlot->clear();
#ifdef MINKO_USE_SPARSE_HASH_MAP
    _propChangedSlot->resize(0);
#endif
    _sortUsefulPropertyChangedSlot->clear();
#ifdef MINKO_USE_SPARSE_HASH_MAP
    _sortUsefulPropertyChangedSlot->resize(0);
#endif
    _zSortUsefulPropertyChangedSlot->clear();
#ifdef MINKO_USE_SPARSE_HASH_MAP
    _zSortUsefulPropertyChangedSlot->resize(0);
#endif
    _drawCallToPropRebindFuncs->clear();
#ifdef MINKO_USE_SPARSE_HASH_MAP
    _drawCallToPropRebindFuncs->resize(0);
#endif
    _drawCallsToBeSorted.clear();
}

void
DrawCallPool::addDrawCallToSortedBucket(DrawCall* drawCall)
{
    const auto priority = drawCall->priority();
    const auto targetId = drawCall->target().id;
    const auto zSortedIndex = drawCall->zSorted() ? 1u : 0u;

    _drawCalls[SortPropertyTuple(priority, targetId)][zSortedIndex].push_back(drawCall);
}

void
DrawCallPool::removeDrawCallFromSortedBucket(DrawCall* drawCall)
{
    for (auto& sortPropertiesToDrawCalls : _drawCalls)
    {
        for (auto& drawCalls : sortPropertiesToDrawCalls.second)
        {
            drawCalls.erase(
                std::remove(drawCalls.begin(), drawCalls.end(), drawCall),
                drawCalls.end()
            );
        }
    }
}

DrawCall*
DrawCallPool::findDrawCall(const std::function<bool(DrawCall*)>& predicate)
{
    for (const auto& sortPropertiesToDrawCalls : _drawCalls)
    {
        for (const auto& drawCalls : sortPropertiesToDrawCalls.second)
        {
            for (auto drawCall : drawCalls)
            {
                if (predicate(drawCall))
                    return drawCall;
            }
        }
    }

    return nullptr;
}

void
DrawCallPool::foreachDrawCall(const std::function<void(DrawCall*)>& function)
{
    for (const auto& sortPropertiesToDrawCalls : _drawCalls)
    {
        for (const auto& drawCalls : sortPropertiesToDrawCalls.second)
        {
            for (auto drawCall : drawCalls)
            {
                function(drawCall);
            }
        }
    }
}

unsigned int
DrawCallPool::numDrawCalls() const
{
    auto numDrawCalls = 0u;

    for (const auto& sortPropertiesToDrawCalls : _drawCalls)
    {
        for (const auto& drawCalls : sortPropertiesToDrawCalls.second)
        {
            numDrawCalls += drawCalls.size();
        }
    }

    return numDrawCalls;
}

void
DrawCallPool::bindDrawCall(DrawCall& drawCall, Pass::Ptr pass, Program::Ptr program, bool forceRebind)
{
    drawCall.bind(program);

    // bind attributes
    // FIXME: like for uniforms, watch and swap default values / binding value
    for (const auto& input : program->inputs().attributes())
        drawCall.bindAttribute(input, pass->attributeBindings().bindings, pass->attributeBindings().defaultValues);

    // bind states
    for (const auto& stateName : States::PROPERTY_NAMES)
        stateBindingPropertyAddedHandler(stateName, drawCall, pass->stateBindings(), forceRebind);

    // bind uniforms
    for (const auto& input : program->inputs().uniforms())
        uniformBindingPropertyAddedHandler(drawCall, input, pass->uniformBindings(), forceRebind);

    // bind index buffer
    if (!!pass->isForward())
        drawCall.bindIndexBuffer();
}

void
DrawCallPool::unbindDrawCall(DrawCall& drawCall)
{
    for (auto it = _propChangedSlot->begin(); it != _propChangedSlot->end();)
    {
        if (it->first.second == &drawCall)
            _propChangedSlot->erase(it++);
        else
            ++it;
    }
    //_propChangedSlot->clear();

    for (auto it = _sortUsefulPropertyChangedSlot->begin(); it != _sortUsefulPropertyChangedSlot->end();)
    {
        if (it->first.second == &drawCall)
            _sortUsefulPropertyChangedSlot->erase(it++);
        else
            ++it;
    }

    for (auto it = _zSortUsefulPropertyChangedSlot->begin(); it != _zSortUsefulPropertyChangedSlot->end();)
    {
        if (it->first.second == &drawCall)
            _zSortUsefulPropertyChangedSlot->erase(it++);
        else
            ++it;
    }

    _drawCallToPropRebindFuncs->erase(&drawCall);
    //_drawCallToPropRebindFuncs->clear();
}

bool
DrawCallPool::compareZSortedDrawCalls(DrawCall* a, DrawCall* b)
{
    const auto aPosition = a->getEyeSpacePosition();
    const auto bPosition = b->getEyeSpacePosition();

    return aPosition.z > bPosition.z;
}

void
DrawCallPool::zSortDrawCalls()
{
    for (auto& sortPropertiesToDrawCalls : _drawCalls)
    {
        auto& drawCalls = sortPropertiesToDrawCalls.second.at(1u);

        std::sort(
            drawCalls.begin(),
            drawCalls.end(),
            compareZSortedDrawCalls
        );
    }
}
