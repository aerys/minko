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

#include "minko/data/HalfEdge.hpp"

using namespace minko;
using namespace minko::data;

HalfEdge::HalfEdge(unsigned int startNodeId,
                   unsigned int endNodeId,
                   unsigned int edgeId) :
    _startNodeId(startNodeId),
    _endNodeId(endNodeId),
    _edgeId(edgeId),
    _marked(false),
    _adjacent(nullptr),
    _next(nullptr),
    _prec(nullptr)
{
}

bool
HalfEdge::indiceInEdge(unsigned int indice)
{
    return indice == _startNodeId || indice == _endNodeId;
}

bool
HalfEdge::indiceInFace(unsigned int indice)
{
    return indice == _face[0]->startNodeId() || indice == _face[1]->startNodeId() || indice == _face[2]->startNodeId();
}

unsigned int
HalfEdge::getThirdVertex()
{
    for (int i = 0; i < 3; i++)
    {
        if (_face[i]->startNodeId() != _startNodeId && _face[i]->startNodeId() != _endNodeId)
            return _face[i]->startNodeId();
    }

    return 0;
}

void
HalfEdge::setFace(HalfEdgePtr he1,
                  HalfEdgePtr he2,
                  HalfEdgePtr he3)
{
    _face.push_back(he1);
    _face.push_back(he2);
    _face.push_back(he3);
}

namespace minko
{
    namespace data
    {
        std::ostream &operator<<(std::ostream &flux, const HalfEdge& halfEdge)
        {

            flux << "HalfEdge " << halfEdge.edgeId() << " : " << halfEdge.startNodeId() << " - " << halfEdge.endNodeId() << std::endl;
            flux << "  triangle : " << halfEdge.face()[0]->startNodeId() << ", " << halfEdge.face()[1]->startNodeId() << ", " << halfEdge.face()[2]->startNodeId() << std::endl;
            flux <<    "  next     : " << halfEdge.next()->startNodeId() << " - " << halfEdge.next()->endNodeId() << std::endl;
            flux <<    "  prec     : " << halfEdge.prec()->startNodeId() << " - " << halfEdge.prec()->endNodeId() << std::endl;
            if (halfEdge.adjacent() != nullptr)
                flux << "  adjacent : " << halfEdge.adjacent()->startNodeId() << " - " << halfEdge.adjacent()->endNodeId() << std::endl;

            return flux;
        }

        std::ostream &operator<<(std::ostream &flux, std::shared_ptr<HalfEdge> halfEdge)
        {
            const HalfEdge& he = *halfEdge;

            return flux << he;
        }
    }
}