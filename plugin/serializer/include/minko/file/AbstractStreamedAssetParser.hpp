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

#pragma once

#include "minko/Common.hpp"
#include "minko/StreamingCommon.hpp"
#include "minko/component/JobManager.hpp"
#include "minko/deserialize/Unpacker.hpp"
#include "minko/file/AbstractSerializerParser.hpp"

namespace minko
{
    namespace file
    {
        class AbstractStreamedAssetParser :
            public AbstractSerializerParser,
            public component::JobManager::Job
        {
        public:
            typedef std::shared_ptr<AbstractStreamedAssetParser> Ptr;

        private:
            std::shared_ptr<AssetLibrary>                                                   _assetLibrary;
            std::shared_ptr<Options>                                                        _options;

            std::shared_ptr<StreamingOptions>                                               _streamingOptions;

            std::shared_ptr<LinkedAsset>                                                    _linkedAsset;

            std::string                                                                     _filename;
            std::string                                                                     _resolvedFilename;
            int                                                                             _assetExtension;
            int                                                                             _fileOffset;

            bool                                                                            _headerIsRead;

            int                                                                             _previousLod;
            int                                                                             _currentLod;
            bool                                                                            _busy;

            int                                                                             _nextLodOffset;
            int                                                                             _nextLodSize;

            Signal<std::shared_ptr<LinkedAsset>, const Error&>::Slot                        _loaderErrorSlot;
            Signal<std::shared_ptr<LinkedAsset>, const std::vector<unsigned char>&>::Slot   _loaderCompleteSlot;

            bool                                                                            _complete;

            std::shared_ptr<data::Provider>                                                 _data;
            Signal<std::shared_ptr<data::Provider>, const std::string&>::Slot               _dataPropertyChangedSlot;

            int                                                                             _requiredLod;
            float                                                                           _priority;

            Signal<Ptr>::Ptr                                                                _ready;
            Signal<Ptr, float>::Ptr                                                         _progress;
            Signal<Ptr>::Ptr                                                                _active;
            Signal<Ptr>::Ptr                                                                _inactive;

        public:
            inline
            void
            streamingOptions(std::shared_ptr<StreamingOptions> streamingOptions)
            {
                _streamingOptions = streamingOptions;
            }

            inline
            void
            linkedAsset(std::shared_ptr<LinkedAsset> linkedAsset)
            {
                _linkedAsset = linkedAsset;
            }

            inline
            Signal<Ptr>::Ptr
            ready() const
            {
                return _ready;
            }

            inline
            Signal<Ptr, float>::Ptr
            progress() const
            {
                return _progress;
            }

            inline
            Signal<Ptr>::Ptr
            active() const
            {
                return _active;
            }

            inline
            Signal<Ptr>::Ptr
            inactive() const
            {
                return _inactive;
            }

            void
            parse(const std::string&                filename,
                  const std::string&                resolvedFilename,
                  std::shared_ptr<Options>          options,
                  const std::vector<unsigned char>& data,
                  std::shared_ptr<AssetLibrary>     assetLibrary);

            bool
            complete();

            void
            beforeFirstStep();

            void
            step();

            float
            priority();

            void
            afterLastStep();

        protected:
            AbstractStreamedAssetParser(std::shared_ptr<data::Provider> data);

            inline
            std::shared_ptr<data::Provider>
            data() const
            {
                return _data;
            }

            virtual
            void
            parsed(const std::string&                filename,
                   const std::string&                resolvedFilename,
                   std::shared_ptr<Options>          options,
                   const std::vector<unsigned char>& data,
                   std::shared_ptr<AssetLibrary>     assetLibrary) = 0;

            virtual
            void
            nextLod(int     previousLod,
                    int     requiredLod,
                    int&    nextLod,
                    int&    nextLodOffset,
                    int&    nextLodSize) = 0;

            virtual
            void
            headerParsed(const std::vector<unsigned char>&   data,
                         std::shared_ptr<Options>            options,
                         unsigned int&                       linkedAssetId) = 0;

            virtual
            void
            lodParsed(int                                previousLod,
                      int                                currentLod,
                      const std::vector<unsigned char>&  data,
                      std::shared_ptr<Options>           options) = 0;

            virtual
            bool
            complete(int currentLod) = 0;

            virtual
            void
            completed() = 0;

            inline
            void
            assetExtension(int value)
            {
                _assetExtension = value;
            }

            inline
            std::shared_ptr<StreamingOptions>
            streamingOptions() const
            {
                return _streamingOptions;
            }

        private:
            void
            loadRange(const std::string&        filename,
                      std::shared_ptr<Options>  options);

            void
            loadRangeComplete(const std::vector<unsigned char>&     data,
                              std::shared_ptr<Options>              options);

            void
            parseLod(int                                previousLod,
                     int                                currentLod,
                     const std::vector<unsigned char>&  data,
                     std::shared_ptr<Options>           options);

            void
            parseHeader(const std::vector<unsigned char>&   data,
                        std::shared_ptr<Options>            options);

            void
            prepareNextLod();

            bool
            nextLodIsReady();

            void
            fetchNextLod(std::shared_ptr<Options> options);

            void
            terminate();

            void
            requiredLod(int requiredLod);

            void
            priority(float priority);

            void
            busy(bool value);

            inline
            int
            assetHeaderOffset() const
            {
                return 0;
            }

            inline
            int
            streamedAssetHeaderOffset() const
            {
                return assetHeaderOffset() + MINKO_SCENE_HEADER_SIZE + _dependencySize;
            }
        };
    }
}
