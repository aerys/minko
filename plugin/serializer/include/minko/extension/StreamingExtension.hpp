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
#include "minko/SerializerCommon.hpp"
#include "minko/StreamingCommon.hpp"
#include "minko/component/JobManager.hpp"
#include "minko/deserialize/Unpacker.hpp"
#include "minko/extension/AbstractExtension.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/file/StreamingOptions.hpp"

namespace minko
{
	namespace extension
	{
		class StreamingExtension :
			public AbstractExtension
		{
		public:
			typedef std::shared_ptr<StreamingExtension> Ptr;

		private:
			typedef unsigned char											uchar;
			typedef msgpack::type::tuple<std::string, uchar, uchar>			SerializeAttribute;
			typedef msgpack::type::tuple<unsigned int, short, std::string>	SerializedAsset;

            struct ParserEntry
            {
                Signal<std::shared_ptr<file::AbstractStreamedAssetParser>>::Slot          readySlot;
                std::list<Signal<std::shared_ptr<file::AbstractParser>>::Slot>            completeSlots;
                Signal<std::shared_ptr<file::AbstractStreamedAssetParser>, float>::Slot   progressSlot;

                float                                                                     progressRate;

                ParserEntry() :
                    progressRate(0.f)
                {
                }
            };

		private:
            std::shared_ptr<file::StreamingOptions>                             _streamingOptions;

            Signal<Ptr>::Ptr                                                    _sceneStreamingComplete;
            Signal<Ptr, float>::Ptr                                             _sceneStreamingProgress;
            Signal<Ptr>::Ptr                                                    _sceneStreamingActive;
            Signal<Ptr>::Ptr                                                    _sceneStreamingInactive;

            Signal<std::shared_ptr<file::StreamedAssetParserScheduler>>::Slot   _parserSchedulerActiveSlot;
            Signal<std::shared_ptr<file::StreamedAssetParserScheduler>>::Slot   _parserSchedulerInactiveSlot;

            std::unordered_map<
                std::shared_ptr<file::AbstractStreamedAssetParser>,
                ParserEntry
            >                                                                   _parsers;

            unsigned int                                                        _numActiveParsers;
            float                                                               _totalProgressRate;

            std::shared_ptr<file::StreamedAssetParserScheduler>                 _parserScheduler;

		public:
            void
            initialize(std::shared_ptr<file::StreamingOptions> streamingOptions);

			AbstractExtension::Ptr
			bind();

            void
            loadingContextDisposed();

			static
			inline
			Ptr
			create()
			{
				auto instance = Ptr(new StreamingExtension());

				return instance;
			}

            inline
            Signal<Ptr>::Ptr
            sceneStreamingComplete()
            {
                return _sceneStreamingComplete;
            }

            inline
            Signal<Ptr, float>::Ptr
            sceneStreamingProgress()
            {
                return _sceneStreamingProgress;
            }

            inline
            Signal<Ptr>::Ptr
            sceneStreamingActive()
            {
                return _sceneStreamingActive;
            }

            inline
            Signal<Ptr>::Ptr
            sceneStreamingInactive()
            {
                return _sceneStreamingInactive;
            }

			msgpack::type::tuple<uint, short, std::string>
			serializePOPGeometry(std::shared_ptr<file::Dependency>      dependency,
								 std::shared_ptr<file::AssetLibrary>	assetLibrary,
								 std::shared_ptr<geometry::Geometry>	geometry,
								 uint									resourceId,
								 std::shared_ptr<file::Options>			options,
                                 std::shared_ptr<file::WriterOptions>   writerOptions,
                                 std::vector<SerializedAsset>&  		includeDependencies);

			void
			deserializePOPGeometry(unsigned short											metaData,
								   std::shared_ptr<file::AssetLibrary>					    assets,
                                   std::shared_ptr<file::Options>                           options,
								   const std::string&									    completePath,
                                   const std::vector<unsigned char>&                        data,
								   std::shared_ptr<file::Dependency>						dependencies,
								   short													assetRef,
								   std::list<std::shared_ptr<component::JobManager::Job>>&	jobList);

			msgpack::type::tuple<uint, short, std::string>
			serializeStreamedTexture(std::shared_ptr<file::Dependency>          dependency,
									 std::shared_ptr<file::AssetLibrary>		assetLibrary,
									 const file::Dependency::TextureDependency& textureDependency,
									 std::shared_ptr<file::Options>				options,
                                     std::shared_ptr<file::WriterOptions>       writerOptions);

			void
			deserializeStreamedTexture(unsigned short											metaData,
									   std::shared_ptr<file::AssetLibrary>						assets,
									   std::shared_ptr<file::Options>							options,
									   const std::string&										completePath,
                                       const std::vector<unsigned char>&                        data,
									   std::shared_ptr<file::Dependency>						dependencies,
									   short													assetRef,
									   std::list<std::shared_ptr<component::JobManager::Job>>&	jobList);

        private:
            StreamingExtension();

            std::shared_ptr<file::StreamedAssetParserScheduler>
            parserScheduler(std::shared_ptr<file::Options>                          options,
                            std::list<std::shared_ptr<component::JobManager::Job>>&	jobList);

            inline
            void
            streamingOptions(std::shared_ptr<file::StreamingOptions> value)
            {
                _streamingOptions = value;
            }

            void
            registerPOPGeometryParser(std::shared_ptr<file::POPGeometryParser>  parser,
                                      std::shared_ptr<geometry::Geometry>       geometry);

            void
            registerStreamedTextureParser(std::shared_ptr<file::StreamedTextureParser>  parser,
                                          std::shared_ptr<render::AbstractTexture>      texture);

            ParserEntry&
            registerParser(std::shared_ptr<file::AbstractStreamedAssetParser> parser);

            bool
            getStreamedAssetHeader(unsigned short                       metadata,
                                   const std::vector<unsigned char>&    data,
                                   const std::string&                   filename,
                                   std::shared_ptr<file::Dependency>    dependency,
                                   std::shared_ptr<file::Options>       options,
                                   std::vector<unsigned char>&          streamedAssetHeaderData,
                                   bool&                                hasHeader,
                                   int&                                 streamedAssetHeaderSize,
                                   std::shared_ptr<file::LinkedAsset>&  linkedAsset);
		};
	}
}
