#include "minko/render/VertexAttribute.hpp"

using namespace minko;
using namespace minko::render;

const VertexAttribute::Type VertexAttribute::NONE =     0;
const VertexAttribute::Type VertexAttribute::POSITION = 1 << 0;
const VertexAttribute::Type VertexAttribute::UV =       1 << 1;
const VertexAttribute::Type VertexAttribute::NORMAL =   1 << 2;
const VertexAttribute::Type VertexAttribute::TANGENT =  1 << 3;
const VertexAttribute::Type VertexAttribute::COLOR =    1 << 4;
const VertexAttribute::Type VertexAttribute::ANY =
    VertexAttribute::POSITION |
    VertexAttribute::UV |
    VertexAttribute::NORMAL |
    VertexAttribute::TANGENT |
    VertexAttribute::COLOR;
