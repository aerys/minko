/*,
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

#include "minko/render/DrawCallZSorter.hpp"
#include "minko/data/ResolvedBinding.hpp"

#include "google/sparse_hash_map"

using namespace minko;
using namespace minko::render;

DrawCallPool::DrawCallPool() :
    _batchId(0),
    _drawCalls(),
    _macroToDrawCalls(new MacroToDrawCallsMap()),
    _invalidDrawCalls(),
    _macroChangedSlot(new MacroToChangedSlotMap()),
    _propChangedSlot(new PropertyChangedSlotMap()),
    _drawCallToPropRebindFuncs(new PropertyRebindFuncMap())
{
    _macroToDrawCalls->set_deleted_key(MacroBindingKey("", nullptr, nullptr));
    _macroChangedSlot->set_deleted_key(MacroBindingKey("", nullptr, nullptr));
    _propChangedSlot->set_deleted_key(DrawCallKey(nullptr, nullptr));
    _drawCallToPropRebindFuncs->set_deleted_key(nullptr);
}

DrawCallPool::~DrawCallPool()
{
    delete _macroToDrawCalls;
    delete _macroChangedSlot;
    delete _propChangedSlot;
    delete _drawCallToPropRebindFuncs;
}

uint
DrawCallPool::addDrawCalls(Effect::Ptr        effect,
                           const std::string& techniqueName,
                           const FStringMap&  variables,
                           data::Store&       rootData,
                           data::Store&       rendererData,
                           data::Store&       targetData)
{
    const auto& technique = effect->technique(techniqueName);

    _batchId++;
    for (const auto& pass : technique)
    {
        DrawCall* drawCall = new DrawCall(_batchId, pass, variables, rootData, rendererData, targetData);

        _drawCalls.push_back(drawCall);
        initializeDrawCall(*drawCall);
    }

    return _batchId;
}

void
DrawCallPool::removeDrawCalls(uint batchId)
{
    _drawCalls.remove_if([&](DrawCall* drawCall)
    {
        if (drawCall->batchId() == batchId)
        {
            unwatchProgramSignature(
                *drawCall,
                drawCall->pass()->macroBindings(),
                drawCall->rootData(),
                drawCall->rendererData(),
                drawCall->targetData()
            );
            unbindDrawCall(*drawCall);

            _invalidDrawCalls.erase(drawCall);

            return true;
        }

        return false;
    });
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
        auto& drawCalls = (*_macroToDrawCalls)[bindingKey];

        assert(std::find(drawCalls.begin(), drawCalls.end(), &drawCall) == drawCalls.end());

        drawCalls.push_back(&drawCall);

        if (macroBindings.types.at(macroName) != data::MacroBindingMap::MacroType::UNSET)
        {
            addMacroCallback(
                bindingKey,
                store.propertyChanged(propertyName),
                [&, this](data::Store&, data::Provider::Ptr, const data::Provider::PropertyName&)
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
                [&, hasProperty, this](data::Store& s, data::Provider::Ptr, const data::Provider::PropertyName& p)
                {
                    if (hasProperty)
                        macroPropertyRemovedHandler(macroBinding, p, s, drawCalls);
                    else
                        macroPropertyAddedHandler(macroBinding, p, s, drawCalls);
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
    if (_macroChangedSlot->count(key) == 0)
        (*_macroChangedSlot)[key] = ChangedSlot(signal.connect(callback, 10), 1);
    else
        (*_macroChangedSlot)[key].second++;
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
DrawCallPool::macroPropertyChangedHandler(const data::MacroBinding& macroBinding, const std::list<DrawCall*>& drawCalls)
{
    _invalidDrawCalls.insert(drawCalls.begin(), drawCalls.end());
}

void
DrawCallPool::macroPropertyAddedHandler(const data::MacroBinding&   macroBinding,
                                        const PropertyName&         propertyName,
                                        data::Store&                store,
                                        const std::list<DrawCall*>& drawCalls)
{
    MacroBindingKey key(propertyName, &macroBinding, &store);

    removeMacroCallback(key);
    addMacroCallback(
        key,
        store.propertyRemoved(propertyName),
        [&, this](data::Store& s, data::Provider::Ptr, const data::Provider::PropertyName& p)
        {
            macroPropertyRemovedHandler(macroBinding, p, s, drawCalls);
        }
    );

    macroPropertyChangedHandler(macroBinding, drawCalls);
}

void
DrawCallPool::macroPropertyRemovedHandler(const data::MacroBinding&     macroBinding,
                                          const PropertyName&           propertyName,
                                          data::Store&                  store,
                                          const std::list<DrawCall*>&   drawCalls)
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
        [&, this](data::Store& s, data::Provider::Ptr, const data::Provider::PropertyName& p)
        {
            macroPropertyAddedHandler(macroBinding, p, s, drawCalls);
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
        auto& drawCalls = (*_macroToDrawCalls)[bindingKey];

        drawCalls.remove(&drawCall);

        if (drawCalls.size() == 0)
            _macroToDrawCalls->erase(bindingKey);

        removeMacroCallback(bindingKey);
    }
}

void
DrawCallPool::initializeDrawCall(DrawCall& drawCall, bool forceRebind)
{
    auto pass = drawCall.pass();
    auto programAndSignature = pass->selectProgram(
        drawCall.variables(), drawCall.targetData(), drawCall.rendererData(), drawCall.rootData()
    );
    auto program = programAndSignature.first;

    if (program == drawCall.program())
        return;

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

    bindDrawCall(drawCall, pass, program, forceRebind);
    if (programAndSignature.second != nullptr)
        watchProgramSignature(
            drawCall,
            drawCall.pass()->macroBindings(),
            drawCall.rootData(),
            drawCall.rendererData(),
            drawCall.targetData()
        );
}

void
DrawCallPool::uniformBindingPropertyAddedHandler(DrawCall&                          drawCall,
                                                 const ProgramInputs::UniformInput& input,
                                                 const data::BindingMap&            uniformBindingMap,
                                                 bool                               forceRebind)
{
    if (!forceRebind && std::find(_invalidDrawCalls.begin(), _invalidDrawCalls.end(), &drawCall) != _invalidDrawCalls.end())
        return;

    data::ResolvedBinding* resolvedBinding = drawCall.bindUniform(
        input, uniformBindingMap.bindings, uniformBindingMap.defaultValues
    );

    if (resolvedBinding != nullptr)
    {
        auto& propertyName = resolvedBinding->propertyName;
        auto& signal = resolvedBinding->store.hasProperty(propertyName)
            ? resolvedBinding->store.propertyRemoved(propertyName)
            : resolvedBinding->store.propertyAdded(propertyName);

        (*_propChangedSlot)[{&resolvedBinding->binding, &drawCall}] = signal.connect(
            [&](data::Store&, data::Provider::Ptr, const data::Provider::PropertyName&)
            {
                (*_drawCallToPropRebindFuncs)[&drawCall].push_back(
                    [&, forceRebind, this]()
                    {
                        uniformBindingPropertyAddedHandler(drawCall, input, uniformBindingMap, forceRebind);
                    }
                );
            }
        );

        delete resolvedBinding;
    }

    if (input.type == ProgramInputs::Type::sampler2d)
    {
        samplerStatesBindingPropertyAddedHandler(drawCall, input, uniformBindingMap);
    }
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

            (*_propChangedSlot)[{&resolvedBinding->binding, &drawCall}] = signal.connect(
                [&](data::Store&, data::Provider::Ptr, const data::Provider::PropertyName&)
                {
                    (*_drawCallToPropRebindFuncs)[&drawCall].push_back(
                        [&, this]()
                        {
                            samplerStatesBindingPropertyAddedHandler(drawCall, input, uniformBindingMap);
                        }
                    );
                }
            );

            delete resolvedBinding;
        }
    }
}

void
DrawCallPool::update()
{
    for (auto* drawCallPtr : _invalidDrawCalls)
        initializeDrawCall(*drawCallPtr, true);
    _invalidDrawCalls.clear();

    for (auto drawCallPtrAndFuncList : *_drawCallToPropRebindFuncs)
        for (auto& func : drawCallPtrAndFuncList.second)
            func();
    _drawCallToPropRebindFuncs->clear();

    _drawCalls.sort(
        std::bind(
            &DrawCallPool::compareDrawCalls,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
}

void
DrawCallPool::invalidateDrawCalls(uint batchId, const FStringMap& variables)
{
    for (DrawCall* drawCall : _drawCalls)
    {
        if (drawCall->batchId() == batchId)
        {
            _invalidDrawCalls.insert(drawCall);
            drawCall->variables().clear();
            drawCall->variables().insert(variables.begin(), variables.end());
        }
    }
}

void
DrawCallPool::clear()
{
    _drawCalls.clear();
    _macroToDrawCalls->clear();
    _invalidDrawCalls.clear();
    _macroChangedSlot->clear();
    _propChangedSlot->clear();
    _drawCallToPropRebindFuncs->clear();
}

void
DrawCallPool::bindDrawCall(DrawCall& drawCall, Pass::Ptr pass, Program::Ptr program, bool forceRebind)
{
    drawCall.bind(program);

    // bind attributes
    // FIXME: like for uniforms, watch and swap default values / binding value
    for (const auto& input : program->inputs().attributes())
        drawCall.bindAttribute(input, pass->attributeBindings().bindings, pass->attributeBindings().defaultValues);
    // bind uniforms
    for (const auto& input : program->inputs().uniforms())
        uniformBindingPropertyAddedHandler(drawCall, input, pass->uniformBindings(), forceRebind);
    // bind states
    drawCall.bindStates(pass->stateBindings().bindings, pass->stateBindings().defaultValues);
    // bind index buffer
    if (!pass->isPostProcessing())
        drawCall.bindIndexBuffer();
}

void
DrawCallPool::unbindDrawCall(DrawCall& drawCall)
{
    std::list<PropertyChangedSlotMap::key_type> toRemove;
    for (auto& bindingDrawCallPairAndSlot : *_propChangedSlot)
        if (bindingDrawCallPairAndSlot.first.second == &drawCall)
            toRemove.push_front(bindingDrawCallPairAndSlot.first);

    for (const auto& key : toRemove)
        _propChangedSlot->erase(key);

    _drawCallToPropRebindFuncs->erase(&drawCall);
}

bool
DrawCallPool::compareDrawCalls(DrawCall* a, DrawCall* b)
{
    const float aPriority = a->priority();
    const float bPriority = b->priority();
    const bool samePriority = fabsf(aPriority - bPriority) < 1e-3f;

    if (samePriority)
    {
        if (a->target().id == b->target().id)
        {
            if (a->zSorted() && b->zSorted())
            {
                auto aPosition = a->getEyeSpacePosition();
                auto bPosition = b->getEyeSpacePosition();

                return aPosition.z > bPosition.z;
            }
        }

        return a->target().id < b->target().id;
    }

    return aPriority > bPriority;
}
