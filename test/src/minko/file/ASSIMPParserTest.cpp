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

#include "ASSIMPParserTest.hpp"

#include "minko/MinkoTests.hpp"
#include "minko/MinkoASSIMP.hpp"

#include "minko/scene/Node.hpp"
#include "minko/scene/NodeSet.hpp"
#include "minko/component/Surface.hpp"
#include "minko/material/Material.hpp"
#include "minko/data/Provider.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/Options.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::file;

scene::Node::Ptr
ASSIMPParserTest::loadModel(const std::string& filename)
{
    auto lib = AssetLibrary::create(MinkoTests::canvas()->context());

    // Register the single ASSIMP entry point for every format under test so
    // the loader dispatches the file to minko::file::ASSIMPParser by extension.
    static const std::vector<std::string> extensions = {
        "dae", "obj", "fbx", "3ds", "stl", "ifc",
        "lwo", "x", "dxf", "ase", "blend", "glb", "gltf"
    };

    auto options = lib->loader()->options();

    options->loadAsynchronously(false);

    for (const auto& extension : extensions)
        options->registerParser<file::ASSIMPParser>(extension);

    // Some sample assets reference sidecar textures (e.g. an .mtl or an LWO
    // surface map) that we deliberately do not ship. A missing dependency must
    // not abort the load: swallow it here (as example/assimp does) so the
    // geometry/material check below can still run. Without an error callback the
    // Loader rethrows the error.
    auto errorSlot = lib->loader()->error()->connect(
        [](Loader::Ptr, const Error&) { /* tolerate missing sidecar deps */ });

    lib->loader()->queue(filename);
    lib->loader()->load();

    return lib->symbol(filename);
}

ASSIMPParserTest::SceneStats
ASSIMPParserTest::collectStats(scene::Node::Ptr root)
{
    SceneStats stats;

    if (!root)
        return stats;

    auto surfaceNodes = scene::NodeSet::create(root)
        ->descendants(true)
        ->where([](scene::Node::Ptr n) { return n->hasComponent<Surface>(); });

    for (auto node : surfaceNodes->nodes())
    {
        for (auto surface : node->components<Surface>())
        {
            ++stats.surfaceCount;

            if (surface->geometry() != nullptr)
                ++stats.geometryCount;

            auto material = surface->material();

            if (material != nullptr)
            {
                ++stats.materialCount;

                auto data = material->data();

                // The parser only sets "diffuseColor" when it builds a material
                // from an aiMaterial; a non-black RGB means the value came from
                // the file rather than the (0,0,0) default.
                if (data->hasProperty("diffuseColor") &&
                    data->get<math::vec4>("diffuseColor").rgb() != math::zero<math::vec3>())
                    ++stats.fileDiffuseColorCount;
            }
        }
    }

    return stats;
}

// ---------------------------------------------------------------------------
// AC#2 — no regression: each per-format sample yields a populated scene.
// ---------------------------------------------------------------------------

namespace
{
    void
    assertPopulated(const std::string& filename)
    {
        auto root = file::ASSIMPParserTest::loadModel(filename);

        ASSERT_NE(root, nullptr) << "ASSIMPParser produced no symbol for " << filename;

        auto stats = file::ASSIMPParserTest::collectStats(root);

        ASSERT_GE(stats.surfaceCount, 1u) << "no Surface in " << filename;
        ASSERT_GE(stats.geometryCount, 1u) << "no Geometry in " << filename;
    }
}

TEST_F(ASSIMPParserTest, LoadCollada)     { assertPopulated("model/cube_triangulate.dae"); }
// The engine's own shipped Collada example asset, guarding the real-world path.
TEST_F(ASSIMPParserTest, LoadColladaPirate) { assertPopulated("model/pirate.dae"); }
TEST_F(ASSIMPParserTest, LoadObj)         { assertPopulated("model/box.obj"); }
TEST_F(ASSIMPParserTest, LoadFbx)         { assertPopulated("model/box.fbx"); }
TEST_F(ASSIMPParserTest, Load3ds)         { assertPopulated("model/RotatingCube.3ds"); }
TEST_F(ASSIMPParserTest, LoadStl)         { assertPopulated("model/triangle.stl"); }
TEST_F(ASSIMPParserTest, LoadIfc)         { assertPopulated("model/AC14-FZK-Haus.ifc"); }
TEST_F(ASSIMPParserTest, LoadLwo)         { assertPopulated("model/sphere_with_mat_gloss_10pc.lwo"); }
TEST_F(ASSIMPParserTest, LoadX)           { assertPopulated("model/test_cube_text.x"); }
TEST_F(ASSIMPParserTest, LoadDxf)         { assertPopulated("model/PinkEggFromLW.dxf"); }
TEST_F(ASSIMPParserTest, LoadAse)         { assertPopulated("model/ThreeCubesGreen.ase"); }
TEST_F(ASSIMPParserTest, LoadBlend)       { assertPopulated("model/box.blend"); }

// ---------------------------------------------------------------------------
// AC#3 — glTF2: a non-draco self-contained .glb yields geometry AND material.
// ---------------------------------------------------------------------------

TEST_F(ASSIMPParserTest, LoadGltf2BinaryHasGeometryAndMaterial)
{
    auto root = loadModel("model/BoxTextured.glb");

    ASSERT_NE(root, nullptr);

    auto stats = collectStats(root);

    ASSERT_GE(stats.geometryCount, 1u) << "glTF2 .glb produced no geometry";
    ASSERT_GE(stats.materialCount, 1u) << "glTF2 .glb produced no material";

    // BoxTextured.glb's only material is a textured PBR material whose
    // base-color factor is white. AbstractASSIMPParser::createMaterial() always
    // returns a non-null material (a default fallback when nothing is imported),
    // so a mere material!=nullptr count cannot tell an imported material from
    // the engine default. Assert a property that ONLY an imported glTF2 material
    // carries: a non-black diffuse/base-color read from the file. This fails if
    // the material is dropped and replaced by the default fallback.
    ASSERT_GE(stats.fileDiffuseColorCount, 1u)
        << "glTF2 material was not imported (no file-sourced diffuse color)";
}
