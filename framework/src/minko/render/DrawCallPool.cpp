/*,
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
        auto programAndSignature = pass->selectProgram(variables, targetData, rendererData, rootData);
        auto* drawCall = new DrawCall(pass, rootData, rendererData, targetData);

        drawCall->bind(
            programAndSignature.first,
            variables,
            pass->attributeBindings(),
            pass->uniformBindings(),
            pass->stateBindings()
        );

        _drawCalls.push_back(drawCall);

        if (programAndSignature.second != nullptr)
            watchProgramSignature(
                drawCall,
                *programAndSignature.second,
                pass->macroBindings(),
                rootData,
                rendererData,
                targetData
            );
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

        _macroToDrawCalls[macro].push_back(drawCall);

        rootData.propertyChanged(macroName).connect(std::bind(
            &DrawCallPool::macroPropertyChangedHandler,
            this,
            macro
        ));
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
DrawCallPool::macroPropertyChangedHandler(const data::Binding&  binding)
{
    const auto& drawCalls = _macroToDrawCalls[binding];

    _changedDrawCalls.insert(drawCalls.begin(), drawCalls.end());
}

void
DrawCallPool::update()
{
    for (const auto* drawCall : _changedDrawCalls)
    {
        /*auto programAndSignature = drawCall->pass()->selectProgram(variables, targetData, rendererData, rootData);

        drawCall->bind(
            programAndSignature.first,
            variables,
            pass->attributeBindings(),
            pass->uniformBindings(),
            pass->stateBindings()
        );*/
    }

    _changedDrawCalls.clear();

   /* _drawCalls.sort([](const DrawCall& a, const DrawCall& b)
    {
        return a.priority > b.priority;
    );*/
}
