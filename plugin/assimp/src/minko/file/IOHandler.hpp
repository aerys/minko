/*
Copyright(c) 2014 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "minko/Common.hpp"
#include "minko/file/AssetLibrary.hpp"
#include "minko/file/IOStream.hpp"
#include "minko/file/Loader.hpp"
#include "minko/file/Options.hpp"
#include "minko/log/Logger.hpp"

#include "assimp/IOStream.hpp"
#include "assimp/IOSystem.hpp"

namespace minko
{
    namespace file
    {
        class IOHandler :
            public Assimp::IOSystem
        {
            typedef std::shared_ptr<Loader>     LoaderPtr;

            typedef std::function<void(
                IOHandler&,
                const std::string&,
                const Error&)>                  ErrorFunction;

        private:
            std::shared_ptr<file::Options>                                          _options;
            std::shared_ptr<file::AssetLibrary>                                     _assets;
            ErrorFunction                                                           _errorFunction;
            std::unordered_map<LoaderPtr, Signal<LoaderPtr>::Slot>                  _loaderCompleteSlots;
            std::unordered_map<LoaderPtr, Signal<LoaderPtr, const Error&>::Slot>    _loaderErrorSlots;

        public:
            IOHandler(std::shared_ptr<file::Options> options, std::shared_ptr<file::AssetLibrary> assets) :
                _options(options),
                _assets(assets)
            {

            }

            void
            errorFunction(ErrorFunction errorFunction)
            {
                _errorFunction = errorFunction;
            }

            void
            Close(Assimp::IOStream* pFile)
            {

            }

            bool
            Exists(const char*  pFile) const
            {
                std::ifstream f(pFile);

                return (bool)f;
            }

            char
            getOsSeparator() const
            {
#ifdef _WIN32
                return '\\';
#else
                return '/';
#endif
            }

            Assimp::IOStream*
            Open(const char* pFile, const char* pMode = "rb")
            {
                const auto filename = std::string(pFile);

                auto loader = Loader::create();

                loader->options(_options);

                _options
                    ->loadAsynchronously(false)
                    ->storeDataIfNotParsed(false)
                    ->parserFunction([](const std::string&) -> AbstractParser::Ptr
                    {
                        return nullptr;
                    });

                Assimp::IOStream* stream = nullptr;

                _loaderCompleteSlots[loader] = loader->complete()->connect([&](Loader::Ptr loaderThis)
                {
                    _loaderErrorSlots.erase(loader);
                    _loaderCompleteSlots.erase(loader);

                    stream = new minko::file::IOStream(loaderThis->files().at(filename)->data());
                });

                _loaderErrorSlots[loader] = loader->error()->connect([&](Loader::Ptr, const Error& error)
                {
                    if (_errorFunction)
                        _errorFunction(*this, filename, error);
                    else
                        throw error;
                });

                loader
                    ->queue(filename)
                    ->load();

                return stream;
            }
        };
    }
}