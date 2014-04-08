#!/bin/bash

function args_error
{
    exit 1
}

if [ ! -z $1 ]; then
    parser_name=$1
else
    args_error
fi

if [ ! -z $2 ]; then
    minko_class=$2
else
    args_error
fi

if [ ! -z $3 ]; then
    assimp_header_file=$3
else
    args_error
fi

if [ ! -z $4 ]; then
    assimp_class=$4
else
    args_error
fi

if [ ! -z $5 ]; then
    include_path=$5
else
    args_error
fi

if [ ! -z $6 ]; then
    src_path=$6
else
    args_error
fi

header_file_name=${minko_class}.hpp
source_file_name=${minko_class}.cpp

echo "/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the \"Software\"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include \"minko/file/AnyASSIMPParser.hpp\"

namespace Assimp
{
    class "${assimp_class}";
}

namespace minko
{
    namespace file
    {
        template <>
	class AnyASSIMPParser<Assimp::"${assimp_class}"> : public AbstractASSIMPParser
        {
        public:

            typedef std::shared_ptr<AnyASSIMPParser<Assimp::"${assimp_class}">> Ptr;

        public:

            virtual ~AnyASSIMPParser() { }

            static
            Ptr
            create();

            virtual
            void
            provideLoaders(Assimp::Importer& importer);

        private:

            AnyASSIMPParser() { }
        };

        using "${minko_class}" = AnyASSIMPParser<Assimp::"${assimp_class}">;
    }
}" > ${include_path}/${header_file_name}

echo "/*
Copyright (c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the \"Software\"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include \"minko/file/"${header_file_name}"\"

#include \"../code/AssimpPCH.h\"
#include \"assimp/Importer.hpp\"
#include \"../code/"${assimp_header_file}"\"

using namespace minko;
using namespace file;

"${minko_class}"::Ptr "${minko_class}"::create()
{
    return "${minko_class}"::Ptr(new "${minko_class}"());
}

void "${minko_class}"::provideLoaders(Assimp::Importer& importer)
{
    importer.RegisterLoader(new Assimp::"${assimp_class}"());
}" > ${src_path}/${source_file_name}
