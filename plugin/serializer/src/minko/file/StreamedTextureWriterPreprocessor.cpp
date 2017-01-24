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

#include "minko/component/Surface.hpp"
#include "minko/component/TextureLodScheduler.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/StreamedTextureWriterPreprocessor.hpp"
#include "minko/geometry/Geometry.hpp"
#include "minko/material/Material.hpp"
#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/render/AbstractTexture.hpp"
#include "minko/render/Texture.hpp"
#include "minko/render/TextureFormatInfo.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::data;
using namespace minko::file;
using namespace minko::geometry;
using namespace minko::material;
using namespace minko::render;
using namespace minko::scene;

StreamedTextureWriterPreprocessor::StreamedTextureWriterPreprocessor() :
    AbstractWriterPreprocessor<Node::Ptr>(),
    _statusChanged(StatusChangedSignal::create())
{
}

void
StreamedTextureWriterPreprocessor::process(Node::Ptr& node, AssetLibrary::Ptr assetLibrary)
{
    node->addComponent(TextureLodScheduler::create(assetLibrary));
}
