/*
Copyright (c) 2013 Aerys

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

#pragma once

#include "minko/Common.hpp"

#include "minko/Signal.hpp"
#include "minko/file/Options.hpp"
#include "minko/file/AbstractParser.hpp"
#include "minko/file/EffectParser.hpp"
#include "minko/file/AbstractLoader.hpp"
#include "minko/file/AssetLibrary.hpp"

namespace minko
{
    namespace file
    {
        class BatchLoader :
            public AbstractLoader
        {
        public:
            typedef std::shared_ptr<BatchLoader>        Ptr;

        private:
            typedef std::shared_ptr<AbstractParser>                                         AbsParserPtr;
            typedef std::shared_ptr<AbstractLoader>                                         AbsLoaderPtr;
            typedef std::unordered_map<std::string, std::shared_ptr<Options>>               FilenameToOptionsMap;
            typedef std::unordered_map<std::string, std::shared_ptr<AbstractSingleLoader>>  FilenameToLoaderMap;
            typedef std::vector<Signal<std::shared_ptr<AbstractLoader>>::Slot>              LoaderSlots;

        protected:
            std::list<std::string>	_filesQueue;
            std::list<std::string>	_loading;
            FilenameToOptionsMap	_filenameToOptions;
            FilenameToLoaderMap	    _filenameToLoader;

            LoaderSlots             _loaderSlots;

        public:
            inline static
            Ptr
            create()
            {
                return std::shared_ptr<BatchLoader>(new BatchLoader());
            }
            
            inline static
            Ptr
            create(Ptr loader)
            {
                auto copy = BatchLoader::create();

                copy->_options = loader->_options;

                return copy;
            }

            inline
			const std::list<std::string>&
			filesQueue()
			{
				return _filesQueue;
			}

			inline
			bool
			loading() const
			{
				return _filesQueue.size() > 0 || _loading.size() > 0;
			}

            Ptr
			queue(const std::string& filename);

			Ptr
			queue(const std::string& filename, std::shared_ptr<Options> options);

			Ptr
			queue(const std::string&				    filename,
				  std::shared_ptr<Options>			    options,
				  std::shared_ptr<AbstractSingleLoader>	loader);

			void
			load();

        protected:
            void
            loaderErrorHandler(std::shared_ptr<AbstractLoader> loader);

			void
            loaderCompleteHandler(std::shared_ptr<AbstractLoader> loader);

            void
			finalize(const std::string& filename);
        };
    }
}
