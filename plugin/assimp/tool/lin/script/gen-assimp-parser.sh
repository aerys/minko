#! /bin/bash

function args_error
{
	exit 1
}

if [ ! -z $1 ]; then
	parser_name=$1
else
	parser_name="ParserNamePlaceHolder"
	args_error
fi

if [ ! -z $2 ]; then
	assimp_file_name=$2
else
	assimp_file_name="AssimpClassPlaceHolder"
	args_error
fi

if [ ! -z $3 ]; then
	include_path=$3
else
	include_path="."
	args_error
fi

if [ ! -z $4 ]; then
	src_path=$4
else
	src_path="."
	args_error
fi


file_name_prefix="${parser_name}ASSIMPParser"
header_file_name="${file_name_prefix}.hpp"
source_file_name="${file_name_prefix}.cpp"

assimp_class_name="${parser_name}Importer"

# Ugly exceptions...
# assimp class names do not respect consistent convention
# remapping parser name to specific assimp class name
if [[ ${parser_name} == "Collada" ]]; then
 	assimp_class_name="ColladaLoader"
fi

if [[ ${parser_name} == "Discreet3DS" ]]; then
    assimp_class_name="Discreet3DSImporter"
fi

if [[ ${parser_name} == "BVH" ]]; then
 	assimp_class_name="BVHLoader"
fi

parser_type_name="${file_name_prefix}"

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
    class "${assimp_class_name}";
}

namespace minko
{
    namespace file
    {
        template <>
	class AnyASSIMPParser<Assimp::"${assimp_class_name}"> : public AbstractASSIMPParser
        {
        public:

            typedef std::shared_ptr<AnyASSIMPParser<Assimp::"${assimp_class_name}">> Ptr;

        public:

            virtual ~AnyASSIMPParser() { }

            static
            Ptr
            create();

            virtual void provideLoaders(Assimp::Importer& importer);

        private:

            AnyASSIMPParser() { }
        };

        using "${parser_type_name}" = AnyASSIMPParser<Assimp::"${assimp_class_name}">;
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
#include \"../code/"${assimp_file_name}".h\"

using namespace minko;
using namespace file;

"${parser_type_name}"::Ptr "${parser_type_name}"::create()
{
    return "${parser_type_name}"::Ptr(new "${parser_type_name}"());
}

void "${parser_type_name}"::provideLoaders(Assimp::Importer& importer)
{
    importer.RegisterLoader(new Assimp::"${assimp_class_name}"());
}" > ${src_path}/${source_file_name}
