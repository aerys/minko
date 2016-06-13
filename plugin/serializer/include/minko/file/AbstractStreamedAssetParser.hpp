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
#include "minko/deserialize/Unpacker.hpp"
#include "minko/data/Provider.hpp"
#include "minko/file/AbstractSerializerParser.hpp"

namespace minko
{
    namespace file
    {
        class AbstractStreamedAssetParser :
            public AbstractSerializerParser
        {
        public:
            typedef std::shared_ptr<AbstractStreamedAssetParser> Ptr;

        private:
            class ParsingJob :
                public component::JobManager::Job
            {
            public:
                typedef std::shared_ptr<ParsingJob>                             Ptr;

            private:
                std::function<void()>               _parsingFunction;
                std::function<void()>               _completeFunction;

                bool                                _complete;
                float                               _priority;

            public:
                inline
                static
                Ptr
                create(std::function<void()>    parsingFunction,
                       std::function<void()>    completeFunction,
                       float                    priority = 1.f)
                {
                    return Ptr(new ParsingJob(
                        parsingFunction,
                        completeFunction,
                        priority
                    ));
                }

			    bool
			    complete() override
                {
                    return _complete;
                }

			    void
			    beforeFirstStep() override
                {
                }

			    void
			    step() override
                {
                    if (_parsingFunction)
                        _parsingFunction();

                    _complete = true;
                }
			
			    float
			    priority() override
                {
                    return _priority;
                }
			
			    void
			    afterLastStep() override
                {
                    if (_completeFunction)
                        _completeFunction();
                }

            private:
                explicit
                ParsingJob(std::function<void()>    parsingFunction,
                           std::function<void()>    completeFunction,
                           float                    priority) :
                    _parsingFunction(parsingFunction),
                    _completeFunction(completeFunction),
                    _complete(false),
                    _priority(priority)
                {
                }
            };

        private:
            std::shared_ptr<AssetLibrary>														_assetLibrary;
            std::shared_ptr<Options>															_options;

            std::shared_ptr<StreamingOptions>													_streamingOptions;

            std::shared_ptr<LinkedAsset>														_linkedAsset;

            std::shared_ptr<component::JobManager>                                              _jobManager;

            std::string																			_filename;
            std::string																			_resolvedFilename;
            int																					_assetExtension;
            int																					_fileOffset;

            bool                                                                                _deferParsing;
            unsigned int                                                                        _dependencyId;

            bool																				_headerIsRead;
            bool                                                                                _readingHeader;

            int																					_previousLod;
            int																					_currentLod;

            int																					_nextLodOffset;
            int																					_nextLodSize;

            Signal<std::shared_ptr<LinkedAsset>, const Error&>::Slot                            _loaderErrorSlot;
            Signal<std::shared_ptr<LinkedAsset>, const std::vector<unsigned char>&>::Slot       _loaderCompleteSlot;

            bool																				_complete;

            std::shared_ptr<data::Provider>														_data;
            Signal<std::shared_ptr<data::Provider>, const data::Provider::PropertyName&>::Slot  _dataPropertyChangedSlot;

            int																					_requiredLod;
            float																				_priority;

            Signal<Ptr, float>::Ptr                                                             _beforePriorityChanged;
            Signal<Ptr, float>::Ptr                                                             _priorityChanged;
            Signal<Ptr>::Ptr                                                                    _lodRequestComplete;

            Signal<Ptr>::Ptr																	_ready;
            Signal<Ptr, float>::Ptr																_progress;

        public:
            void
            deferParsing(unsigned int dependencyId)
            {
                _deferParsing = true;

                _dependencyId = dependencyId;
            }

            inline
            Ptr
            data(std::shared_ptr<data::Provider> data)
            {
                _data = data;

                return std::static_pointer_cast<AbstractStreamedAssetParser>(shared_from_this());
            }

            inline
            void
            streamingOptions(std::shared_ptr<StreamingOptions> streamingOptions)
            {
                _streamingOptions = streamingOptions;
            }

            inline
            std::shared_ptr<LinkedAsset>
            linkedAsset() const
            {
                return _linkedAsset;
            }

            inline
            void
            linkedAsset(std::shared_ptr<LinkedAsset> linkedAsset)
            {
                _linkedAsset = linkedAsset;
            }

            inline
            void
            useJobBasedParsing(std::shared_ptr<component::JobManager> jobManager)
            {
                _jobManager = jobManager;
            }

            inline
            Signal<Ptr, float>::Ptr
            priorityChanged() const
            {
                return _priorityChanged;
            }

            inline
            Signal<Ptr, float>::Ptr
            beforePriorityChanged() const
            {
                return _beforePriorityChanged;
            }

            inline
            Signal<Ptr>::Ptr
            lodRequestComplete() const
            {
                return _lodRequestComplete;
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

            void
            parse(const std::string&                filename,
                  const std::string&                resolvedFilename,
                  std::shared_ptr<Options>          options,
                  const std::vector<unsigned char>& data,
                  std::shared_ptr<AssetLibrary>     assetLibrary);

            float
            priority();

            bool
            prepareForNextLodRequest();

            void
            getNextLodRequestInfo(int& offset, int& size);

            void
            lodRequestFetchingBegin();

            void
            lodRequestFetchingProgress(float progressRate);

            void
            lodRequestFetchingError(const Error& error);

            void
            lodRequestFetchingComplete(const std::vector<unsigned char>& data);

        protected:
            AbstractStreamedAssetParser();

            inline
            std::shared_ptr<data::Provider>
            data() const
            {
                return _data;
            }

            bool
            deferParsing() const
            {
                return _deferParsing;
            }

            unsigned int
            dependencyId() const
            {
                return _dependencyId;
            }

            virtual
            bool
            useDescriptor(const std::string&                filename,
                          std::shared_ptr<Options>          options,
                          const std::vector<unsigned char>& data,
                          std::shared_ptr<AssetLibrary>     assetLibrary) = 0;

            virtual
            void
            parsed(const std::string&                filename,
                   const std::string&                resolvedFilename,
                   std::shared_ptr<Options>          options,
                   const std::vector<unsigned char>& data,
                   std::shared_ptr<AssetLibrary>     assetLibrary) = 0;

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

            virtual
            void
            lodRangeFetchingBound(int  currentLod,
                                  int  requiredLod,
                                  int& lodRangeMinSize,
                                  int& lodRangeMaxSize,
                                  int& lodRangeRequestMinSize,
                                  int& lodRangeRequestMaxSize) = 0;

            virtual
            void
            lodRangeRequestByteRange(int lowerLod, int upperLod, int& offset, int& size) const = 0;

            virtual
            int
            lodLowerBound(int lod) const = 0;

            virtual
            int
            maxLod() const = 0;

            virtual
            void
            nextLod(int     previousLod,
                    int     requiredLod,
                    int&    nextLod,
                    int&    nextLodOffset,
                    int&    nextLodSize);

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
            parseLod(int                                previousLod,
                     int                                currentLod,
                     const std::vector<unsigned char>&  data,
                     std::shared_ptr<Options>           options);

            void
            parseHeader(const std::vector<unsigned char>&   data,
                        std::shared_ptr<Options>            options);

            void
            prepareNextLod();

            void
            terminate();

            void
            requiredLod(int requiredLod);

            void
            priority(float priority);

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

            void
            parseStreamedAssetHeader();
        };
    }
}
