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

using namespace minko;
using namespace minko::render;

std::pair<std::list<DrawCall*>::iterator, std::list<DrawCall*>::iterator>
DrawCallPool::addDrawCalls(std::shared_ptr<Effect>                              effect,
                           const std::unordered_map<std::string, std::string>&  variables,
                           const std::string&                                   techniqueName,
                           data::Container&                                     rootData,
                           data::Container&                                     rendererData,
                           data::Container&                                     targetData)
{
    const auto& technique = effect->technique(techniqueName);
    
    for (const auto& pass : technique)
    {
        auto* drawCall = new DrawCall(pass, variables, rootData, rendererData, targetData);

        initializeDrawCall(drawCall);

        _drawCalls.push_back(drawCall);
    }

    return std::pair<std::list<DrawCall*>::iterator, std::list<DrawCall*>::iterator>(
        std::prev(_drawCalls.end(), technique.size()),
        std::prev(_drawCalls.end())
    );
}

void
DrawCallPool::removeDrawCalls(const DrawCallIteratorPair& iterators)
{
    auto end = std::next(iterators.second);

    for (auto it = iterators.first; it != end; ++it)
    {
        DrawCall* drawCall = *it;

        // FIXME: avoid const_cast
        unwatchProgramSignature(
            drawCall,
            drawCall->pass()->macroBindings(),
            const_cast<data::Container&>(drawCall->rootData()),
            const_cast<data::Container&>(drawCall->rendererData()),
            const_cast<data::Container&>(drawCall->targetData())
        );

        _invalidDrawCalls.erase(drawCall);
        delete drawCall;
    }

    _drawCalls.erase(iterators.first, end);
}

void
DrawCallPool::watchProgramSignature(DrawCall*                       drawCall,
                                    const data::MacroBindingMap&    macroBindings,
                                    data::Container&                rootData,
                                    data::Container&                rendererData,
                                    data::Container&                targetData)
{
    for (const auto& macroNameAndBinding : macroBindings)
    {
        const auto& macroBinding = macroNameAndBinding.second;
        auto& container = macroBinding.source == data::BindingSource::ROOT ? rootData
            : macroBinding.source == data::BindingSource::RENDERER ? rendererData
            : targetData;
        auto bindingKey = MacroBindingKey(&macroBinding, &container);
        auto& drawCalls = _macroToDrawCalls[bindingKey];

        drawCalls.push_back(drawCall);

        if (macroBinding.isInteger)
        {
            addMacroCallback(
                { &container, &container.propertyChanged() },
                container,
                std::bind(&DrawCallPool::macroPropertyChangedHandler, this, drawCalls)
            );
        }
        else
        {
            auto propertyName = Container::getActualPropertyName(drawCall->variables(), macroBinding.propertyName);
            
            if (container.hasProperty(propertyName))
            {
                addMacroCallback(
                    { &container, &container.propertyRemoved() },
                    container,
                    std::bind(&DrawCallPool::macroPropertyRemovedHandler, this, std::ref(container), drawCalls)
                );
            }
            else
            {
                addMacroCallback(
                    { &container, &container.propertyAdded() },
                    container,
                    std::bind(&DrawCallPool::macroPropertyAddedHandler, this, std::ref(container), drawCalls)
                );
            }

        }
    }
}

void
DrawCallPool::addMacroCallback(const ContainerKey&  key,
                               data::Container&     container,
                               const MacroCallback& callback)
{
    if (_macroChangedSlot.count(key) == 0)
        _macroChangedSlot.emplace(key, ChangedSlot(container.propertyAdded().connect(callback), 1));
    else
        _macroChangedSlot[key].second++;
}

void
DrawCallPool::removeMacroCallback(const ContainerKey& key)
{
    _macroChangedSlot[key].second--;
    if (_macroChangedSlot[key].second == 0)
        _macroChangedSlot.erase(key);
}

void
DrawCallPool::macroPropertyChangedHandler(const std::list<DrawCall*>& drawCalls)
{
    _invalidDrawCalls.insert(drawCalls.begin(), drawCalls.end());
}

void
DrawCallPool::macroPropertyAddedHandler(data::Container&            container,
                                        const std::list<DrawCall*>& drawCalls)
{
    removeMacroCallback({ &container, &container.propertyAdded() });

    addMacroCallback(
        { &container, &container.propertyRemoved() },
        container,
        std::bind(&DrawCallPool::macroPropertyRemovedHandler, this, container, drawCalls)
    );

    macroPropertyChangedHandler(drawCalls);
}

void
DrawCallPool::macroPropertyRemovedHandler(data::Container&            container,
                                          const std::list<DrawCall*>& drawCalls)
{
    removeMacroCallback({ &container, &container.propertyRemoved() });

    addMacroCallback(
        { &container, &container.propertyAdded() },
        container,
        std::bind(&DrawCallPool::macroPropertyAddedHandler, this, container, drawCalls)
    );

    macroPropertyChangedHandler(drawCalls);
}

void
DrawCallPool::unwatchProgramSignature(DrawCall*                       drawCall,
                                      const data::MacroBindingMap&    macroBindings,
                                      data::Container&                rootData,
                                      data::Container&                rendererData,
                                      data::Container&                targetData)
{
    for (const auto& macroNameAndBinding : macroBindings)
    {
        const auto& macroBinding = macroNameAndBinding.second;
        auto& container = macroBinding.source == data::BindingSource::ROOT ? rootData
            : macroBinding.source == data::BindingSource::RENDERER ? rendererData
            : targetData;
        auto bindingKey = MacroBindingKey(&macroBinding, &container);
        auto& drawCalls = _macroToDrawCalls.at(bindingKey);

        drawCalls.remove(drawCall);

        if (drawCalls.size() == 0)
            _macroToDrawCalls.erase(bindingKey);
        
        if (macroBinding.isInteger)
            removeMacroCallback({ &container, &container.propertyChanged() });
        else
        {
            auto propertyName = Container::getActualPropertyName(drawCall->variables(), macroBinding.propertyName);

            if (container.hasProperty(propertyName))
                removeMacroCallback({ &container, &container.propertyRemoved() });
            else
                removeMacroCallback({ &container, &container.propertyAdded() });
        }
    }
}

void
DrawCallPool::initializeDrawCall(DrawCall* drawCall)
{
    auto pass = drawCall->pass();
    auto programAndSignature = pass->selectProgram(
        drawCall->variables(), drawCall->targetData(), drawCall->rendererData(), drawCall->rootData()
    );

    if (programAndSignature.first == drawCall->program())
        return;

    drawCall->bind(
        programAndSignature.first,
        pass->attributeBindings(),
        pass->uniformBindings(),
        pass->stateBindings()
    );

    // FIXME: avoid const_cast
    if (programAndSignature.second != nullptr)
        watchProgramSignature(
            drawCall,
            pass->macroBindings(),
            const_cast<data::Container&>(drawCall->rootData()),
            const_cast<data::Container&>(drawCall->rendererData()),
            const_cast<data::Container&>(drawCall->targetData())
        );
}

void
DrawCallPool::update()
{
    for (auto* drawCall : _invalidDrawCalls)
        initializeDrawCall(drawCall);

    _invalidDrawCalls.clear();

    // FIXME: sort draw calls back-to-front and according to their priority
    /* _drawCalls.sort([](const DrawCall& a, const DrawCall& b)
    {
        return a.priority > b.priority;
    );*/
}

void
DrawCallPool::invalidateDrawCalls(const DrawCallIteratorPair& iterators)
{
    auto end = std::next(iterators.second);

    for (auto it = iterators.first; it != end; ++it)
        _invalidDrawCalls.insert(*it);
}
