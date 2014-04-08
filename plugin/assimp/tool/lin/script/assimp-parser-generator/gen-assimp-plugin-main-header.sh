#!/bin/bash

main_header="${MINKO_HOME}/plugin/assimp/include/minko/MinkoASSIMP.hpp"

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

#if (!defined ASSIMP_BUILD_NO_IMPORTER_INSTANCIATION)
# include \"minko/file/ASSIMPParser.hpp\"
#endif

" > ${main_header}

source assimp-parser-decl.sh

for loader in ${!loaders[@]}; do
    parser_name=${loader}

    parser_info=${loaders[${loader}]}
    parser_info_array=($parser_info)

    class_prefix=${parser_info_array[0]}

    header="minko/file/${class_prefix}${minko_class_suffix}.hpp"

    echo "#include \"${header}\"" >> ${main_header}
done
