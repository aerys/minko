/*
Copyright (c) 2020 Aerys

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

#include "gtest/gtest.h"

#include "minko/file/JSON.hpp" //> minko::file::JSON::escapeNewlinesInStrings.

TEST(JSON_escapeNewlinesInStrings, JsonWithMultilineString)
{
    using namespace minko::file::JSON;
    
    std::string json_data = R"(
    {
        "string": "he
                   ll
                   o"
    })";
    
    std::string excepted_result = R"(
    {
        "string": "he\n                   ll\n                   o"
    })";
    
    escapeNewlinesInStrings(json_data);
    
    ASSERT_EQ(json_data, excepted_result);
}

TEST(JSON_escapeNewlinesInStrings, JsonWithMultilineString2)
{
    using namespace minko::file::JSON;
    
    std::string json_data = R"(
    {
        "string": "he
ll
                   o"
    })";
    
    std::string excepted_result = R"(
    {
        "string": "he\nll\n                   o"
    })";
    
    escapeNewlinesInStrings(json_data);
    
    ASSERT_EQ(json_data, excepted_result);
}

TEST(JSON_escapeNewlinesInStrings, JsonWithMultilineStringWithStrings)
{
    using namespace minko::file::JSON;
    
    std::string json_data = R"(
    {
        "value": true,
        "value2": 3,
        "vertexShader": "#pragma include \"foo.glsl\"
                         #pragma include \"test.glsl\"

                         void main(void)
                         {
                         }
        ",
        "value4": 5.14
    })";
    
    std::string excepted_result = R"(
    {
        "value": true,
        "value2": 3,
        "vertexShader": "#pragma include \"foo.glsl\"\n                         #pragma include \"test.glsl\"\n\n                         void main(void)\n                         {\n                         }\n        ",
        "value4": 5.14
    })";
    
    escapeNewlinesInStrings(json_data);
    
    ASSERT_EQ(json_data, excepted_result);
}