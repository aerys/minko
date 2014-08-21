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

        initializeDrawCall(*drawCall);

        _drawCalls.push_back(drawCall);
    }

    return std::pair<std::list<DrawCall*>::iterator, std::list<DrawCall*>::iterator>(
        std::prev(_drawCalls.end(), technique.size()),
        _drawCalls.end()
    );
}

void
DrawCallPool::removeDrawCalls(const DrawCallIteratorPair& iterators)
{
    for (auto it = iterators.first; it != iterators.second; ++it)
        delete *it;

    _drawCalls.erase(iterators.first, iterators.second);
}

void
DrawCallPool::watchProgramSignature(DrawCall*                       drawCall,
                                    const ProgramSignature&         signature,
                                    const data::MacroBindingMap&    macroBindings,
                                    data::Container&                rootData,
                                    data::Container&                rendererData,
                                    data::Container&                targetData)
{
    for (const auto& macroName : signature.macros())
    {
        const auto& macro = macroBindings.at(macroName);
        auto& drawCalls = _macroToDrawCalls[macro];
        auto container = macro.source == data::BindingSource::ROOT ? rootData
            : macro.source == data::BindingSource::RENDERER ? rendererData
            : targetData;

        drawCalls.push_back(drawCall);
        if (_macroChangedSlot.count(macro) == 0)
        {
            auto propertyName = data::Container::getActualPropertyName(drawCall->variables(), macro.propertyName);
            auto& signal = container.propertyChanged(propertyName);

            _macroChangedSlot[macro] = signal.connect(std::bind(
                &DrawCallPool::macroPropertyChangedHandler,
                this,
                drawCalls
            ));
        }
    }
}

void
DrawCallPool::unwatchProgramSignature(DrawCall*                       drawCall,
                                      const ProgramSignature&         signature,
                                      const data::MacroBindingMap&    macroBindings)
{
    for (const auto& macroName : signature.macros())
    {
        const auto& macro = macroBindings.at(macroName);

        // FIXME
    }
}

void
DrawCallPool::macroPropertyChangedHandler(const std::list<DrawCall*>&  drawCalls)
{
    _changedDrawCalls.insert(drawCalls.begin(), drawCalls.end());
}

void
DrawCallPool::initializeDrawCall(DrawCall& drawCall)
{
    auto pass = drawCall.pass();
    auto programAndSignature = pass->selectProgram(
        drawCall.variables(), drawCall.targetData(), drawCall.rendererData(), drawCall.rootData()
    );

    drawCall.bind(
        programAndSignature.first,
        pass->attributeBindings(),
        pass->uniformBindings(),
        pass->stateBindings()
    );

    // FIXME: avoid const_cast
    if (programAndSignature.second != nullptr)
        watchProgramSignature(
        &drawCall,
        *programAndSignature.second,
        pass->macroBindings(),
        const_cast<data::Container&>(drawCall.rootData()),
        const_cast<data::Container&>(drawCall.rendererData()),
        const_cast<data::Container&>(drawCall.targetData())
    );
}

void
DrawCallPool::update()
{
    for (auto* drawCall : _changedDrawCalls)
        initializeDrawCall(*drawCall);

    _changedDrawCalls.clear();

    // FIXME: sort draw calls back-to-front and according to their priority
    /* _drawCalls.sort([](const DrawCall& a, const DrawCall& b)
    {
        return a.priority > b.priority;
    );*/
}
