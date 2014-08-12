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

#include "ProgramSignatureTest.hpp"

#include "minko/data/MacroBinding.hpp"

using namespace minko;
using namespace minko::render;

void
ProgramSignatureTest::SetUp()
{
    _targetProvider = data::Provider::create();
    _targetData.addProvider(_targetProvider);
    _targetProvider->set("foo", 42);
    _targetProvider->set("foo2", 24);
    _targetProvider->set("bar", true);
    auto targetCollection = data::Collection::create("target");
    auto targetProvider = data::Provider::create();
    targetCollection->pushBack(targetProvider);
    targetProvider->set("foo", 4242);
    _targetData.addCollection(targetCollection);
    _variables["targetId"] = "0";
    
    _rendererProvider = data::Provider::create();
    _rendererData.addProvider(_rendererProvider);
    _rendererProvider->set("foo", 4242);
    _rendererProvider->set("bar", true);

    _rootProvider = data::Provider::create();
    auto rootCollection = data::Collection::create("root");
    rootCollection->pushBack(_rootProvider);
    _rootData.addCollection(rootCollection);
    _rootProvider->set("foo", 424242);
    _rootProvider->set("bar", true);
    _variables["rootId"] = "0";
}

TEST_F(ProgramSignatureTest, TargetDefinedIntegerValue)
{
    data::MacroBindingMap macroBindings;

    macroBindings["FOO"] = {
        "foo",
        data::BindingSource::TARGET,
        data::MacroBinding::State::UNDEFINED,
        data::MacroBinding::Value()
    };

    ProgramSignature signature(macroBindings, _variables, _targetData, _rendererData, _rootData);

    ASSERT_EQ(signature.mask(), 0x00000001);
    ASSERT_EQ(signature.values().size(), 1);
    ASSERT_EQ(signature.values()[0], 42);
    ASSERT_EQ(signature.states().size(), 1);
    ASSERT_EQ(signature.states()[0], data::MacroBinding::State::DEFINED_INTEGER_VALUE);
}

TEST_F(ProgramSignatureTest, TargetDefinedIntegerValueFromCollection)
{
    data::MacroBindingMap macroBindings;

    macroBindings["FOO"] = {
        "target[${targetId}].foo",
        data::BindingSource::TARGET,
        data::MacroBinding::State::UNDEFINED,
        data::MacroBinding::Value()
    };

    ProgramSignature signature(macroBindings, _variables, _targetData, _rendererData, _rootData);

    ASSERT_EQ(signature.mask(), 0x00000001);
    ASSERT_EQ(signature.values().size(), 1);
    ASSERT_EQ(signature.values()[0], 4242);
    ASSERT_EQ(signature.states().size(), 1);
    ASSERT_EQ(signature.states()[0], data::MacroBinding::State::DEFINED_INTEGER_VALUE);
}

TEST_F(ProgramSignatureTest, TargetDefined)
{
    data::MacroBindingMap macroBindings;

    macroBindings["BAR"] = {
        "bar",
        data::BindingSource::TARGET,
        data::MacroBinding::State::UNDEFINED,
        data::MacroBinding::Value()
    };

    ProgramSignature signature(macroBindings, _variables, _targetData, _rendererData, _rootData);

    ASSERT_EQ(signature.mask(), 0x00000001);
    ASSERT_EQ(signature.values().size(), 0);
    ASSERT_EQ(signature.states().size(), 1);
    ASSERT_EQ(signature.states()[0], data::MacroBinding::State::DEFINED);
}

TEST_F(ProgramSignatureTest, TargetUndefined)
{
    data::MacroBindingMap macroBindings;

    macroBindings["BAR"] = {
        "fooBar",
        data::BindingSource::TARGET,
        data::MacroBinding::State::UNDEFINED,
        data::MacroBinding::Value()
    };

    ProgramSignature signature(macroBindings, _variables, _targetData, _rendererData, _rootData);

    ASSERT_EQ(signature.mask(), 0x00000000);
    ASSERT_EQ(signature.values().size(), 0);
    ASSERT_EQ(signature.states().size(), 0);
}

TEST_F(ProgramSignatureTest, RootDefinedIntegerValueFromCollection)
{
    data::MacroBindingMap macroBindings;

    macroBindings["FOO"] = {
        "root[${rootId}].foo",
        data::BindingSource::ROOT,
        data::MacroBinding::State::UNDEFINED,
        data::MacroBinding::Value()
    };

    ProgramSignature signature(macroBindings, _variables, _targetData, _rendererData, _rootData);

    ASSERT_EQ(signature.mask(), 0x00000001);
    ASSERT_EQ(signature.values().size(), 1);
    ASSERT_EQ(signature.values()[0], 424242);
    ASSERT_EQ(signature.states().size(), 1);
    ASSERT_EQ(signature.states()[0], data::MacroBinding::State::DEFINED_INTEGER_VALUE);
}
