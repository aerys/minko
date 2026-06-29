/*
Copyright (c) 2025 Aerys

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

#include "minko/Minko.hpp"
#include "gtest/gtest.h"

namespace minko
{
    namespace file
    {
        // Headless load-verification of the Assimp 6.x plugin through the real
        // minko::file::ASSIMPParser path (AssetLibrary + Loader + ASSIMPParser),
        // exactly like example/assimp but assertion-driven.
        class ASSIMPParserTest :
            public ::testing::Test
        {
        public:
            struct SceneStats
            {
                unsigned int surfaceCount = 0;
                unsigned int geometryCount = 0;
                unsigned int materialCount = 0;

                // Materials whose imported diffuse/base-color RGB is non-black,
                // i.e. read from the file rather than left at the parser's
                // (0,0,0) default. A default-fallback material has no
                // "diffuseColor" property at all.
                unsigned int fileDiffuseColorCount = 0;
            };

            // Loads `filename` through the ASSIMP plugin and returns the parsed
            // scene root Node (the AssetLibrary symbol), or nullptr on failure.
            static
            scene::Node::Ptr
            loadModel(const std::string& filename);

            // Walks the Node subtree and counts Surface components together with
            // the Geometry/Material they carry.
            static
            SceneStats
            collectStats(scene::Node::Ptr root);
        };
    }
}
