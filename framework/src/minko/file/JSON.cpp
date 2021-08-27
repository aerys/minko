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

#include "minko/file/JSON.hpp"

namespace minko
{
    namespace file
    {
        namespace JSON
        {
            /// Tells if the given iterator corresponds to a string delimiter.
            /// WARNING: Do not use this function if the iterator corresponds to the beginning of a string.
            /// NOTE: It is used for the escapeNewlinesInStrings function.
            inline static bool isStringDelimiter(std::string::iterator it)
            {
                return *it == '"' && *(it - 1) != '\\';
            }

            void escapeNewlinesInStrings(std::string& json)
            {
                bool in_string = false;

                for (auto it = json.begin(); it != json.end(); ++it)
                {
                    if (!in_string && it != json.begin() && isStringDelimiter(it))
                    {
                        in_string = true;
                    }
                    else if (in_string && *it == '\n')
                    {
                        *it = '\\';
                        it = json.insert(it + 1, 'n');
                    }
                    else if (in_string && *it == '\r')
                    {
                        *it = '\\';
                        it = json.insert(it + 1, 'r');
                    }
                    else if (in_string && *it == '\t')
                    {
                        *it = '\\';
                        it = json.insert(it + 1, 't');
                    }
                    else if (in_string && it != json.begin() && isStringDelimiter(it))
                    {
                        in_string = false;
                    }
                }
            }

            //The goal of the following functions is to wrap the nlohmann JSON library and give it a behavior closer to the JsonCpp
            //library.
            json get(const json& object, const std::string& attribute)
            {
                if (object.contains(attribute))
                    return object[attribute];
                else
                    return json();
            }

            std::string as_string(const json& object)
            {
                if (object.is_string())
                    return object.get<std::string>();
                else if (object.is_number())
                    return std::to_string(object.get<int>());
                else if (object.is_boolean())
                {
                    bool obj_bool = object.get<bool>();
                    if (obj_bool)
                        return "true";
                    else
                        return "false";
                }
                else if (object.is_null())
                    return "";
                else
                    throw std::runtime_error("invalid json object type");
            }

            int as_int(const json& object)
            {
                if (object.is_number())
                    return object.get<int>();
                else if (object.is_boolean())
                {
                    bool obj_bool = object.get<bool>();
                    if (obj_bool)
                        return 1;
                    else
                        return 0;
                }
                else if (object.is_null())
                    return 0;
                else
                    throw std::runtime_error("invalid json object type");
            }

            float as_float(const json& object)
            {
                if (object.is_number())
                    return object.get<float>();
                else if (object.is_boolean())
                {
                    bool obj_bool = object.get<bool>();
                    if (obj_bool)
                        return 1.0f;
                    else
                        return 0.0f;
                }
                else if (object.is_null())
                    return 0.0f;
                else
                    throw std::runtime_error("invalid json object type");
            }

            bool as_bool(const json& object)
            {
                if (object.is_boolean())
                    return object.get<bool>();
                if (object.is_number())
                {
                    float obj_num = object.get<float>();
                    if (obj_num)
                        return true;
                    else
                        return false;
                }
                else if (object.empty())
                    return false;
                else
                    return true;
            }
        }
    }
}
