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

DrawCallPool::DrawCallPool()
{
}

void
DrawCallPool::addDrawCalls(std::shared_ptr<Effect>                                effect,
                           const std::unordered_map<std::string, std::string>&    variables,
                           const std::string&                                     techniqueName,
                           std::shared_ptr<data::Container>                       rootData,
                           std::shared_ptr<data::Container>                       rendererData,
                           std::shared_ptr<data::Container>                       targetData)
{
    const auto& technique = effect->technique(techniqueName);
                
    for (const auto& pass : technique)
    {
        auto program = pass->selectProgram(variables, targetData, rendererData, rootData);
        auto* drawCall = new DrawCall(pass->macroBindings());

        drawCall->bind(
            program,
            variables,
            rootData,
            rendererData,
            targetData,
            pass->attributeBindings(),
            pass->uniformBindings(),
            pass->stateBindings()
        );

        _drawCalls.push_back(drawCall);
    }
}
