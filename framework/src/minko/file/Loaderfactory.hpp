#pragma once

#include "minko/Common.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/APKLoader.hpp"

namespace minko
{
    namespace file
    {

        class Loaderfactory
        {
        public:
            inline static
            std::shared_ptr<AbstractLoader>
            create()
            {
#ifdef __ANDROID__
                return APKLoader::create();
#else
                return Loader::create();
#endif
            }
        };

    } /* namespace file */
} /* namespace minko */
