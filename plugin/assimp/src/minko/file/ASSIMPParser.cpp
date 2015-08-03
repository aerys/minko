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

#include "minko/file/ASSIMPParser.hpp"

#include "assimp/Importer.hpp"

using namespace minko;
using namespace file;

ASSIMPParser::Ptr ASSIMPParser::create()
{
    return ASSIMPParser::Ptr(new ASSIMPParser());
}

void ASSIMPParser::provideLoaders(Assimp::Importer& importer)
{
}

#if !defined (ASSIMP_BUILD_NO_IMPORTER_INSTANCIATION)
std::set<std::string>
ASSIMPParser::getSupportedFileExtensions()
{
  Assimp::Importer importer;

  std::string list;
  std::set<std::string> result;

  importer.GetExtensionList(list);

  auto pos = list.find_first_of(";");
  while (pos != std::string::npos)
  {
    result.insert(list.substr(2, pos - 2));
    list = list.substr(pos + 1);
    pos = list.find_first_of(";");
  }
  if (!list.empty())
    result.insert(list.substr(2));

  return result;
}
#endif // ! ASSIMP_BUILD_NO_IMPORTER_INSTANCIATION
