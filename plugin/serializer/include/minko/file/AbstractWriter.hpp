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
#include "minko/Signal.hpp"
#include "minko/file/AbstractWriterPreprocessor.hpp"
#include "minko/file/Dependency.hpp"
#include "minko/log/Logger.hpp"
#include "msgpack.hpp"

namespace minko
{
	namespace file
	{
        class WriterError :
            public std::runtime_error
        {
        private:
            std::string _type;

        public:
            explicit
            WriterError(const std::string& message) :
                std::runtime_error(message),
                _type()
            {
            }

            WriterError(const std::string& type, const std::string& message) :
                std::runtime_error(message),
                _type(type)
            {
            }

            inline
            const std::string&
            type() const
            {
                return _type;
            }
        };

		template <typename T>
		class AbstractWriter :
			public std::enable_shared_from_this<AbstractWriter<T>>
		{
		public:
			typedef std::shared_ptr<AbstractWriter> Ptr;

		private:
			typedef std::vector<msgpack::type::tuple<unsigned int, short, std::string>> SerializedDependency;

            typedef std::shared_ptr<AbstractWriterPreprocessor<T>> PreprocessorPtr;

		protected:
			std::shared_ptr<Signal<Ptr>>	                    _complete;

            std::shared_ptr<Signal<Ptr, const WriterError&>>    _error;
			T								                    _data;

            std::list<PreprocessorPtr>                          _preprocessors;

            int                                                 _magicNumber;

		public:
			inline
			std::shared_ptr<Signal<Ptr>>
			complete()
			{
				return _complete;
			}

            inline
            std::shared_ptr<Signal<Ptr, const WriterError&>>
            error()
            {
                return _error;
            }

            inline
            const T&
            data() const
            {
                return _data;
            }

            inline
            T&
            data()
            {
                return _data;
            }

            inline
            void
            data(const T& data)
            {
                _data = data;
            }

            inline
            Ptr
            registerPreprocessor(PreprocessorPtr preprocessor)
            {
                _preprocessors.push_back(preprocessor);

                return this->shared_from_this();
            }

            inline
            Ptr
            unregisterPreprocessor(PreprocessorPtr preprocessor)
            {
                _preprocessors.remove(preprocessor);

                return this->shared_from_this();
            }

            void
            write(const std::string&                    filename,
                  std::shared_ptr<AssetLibrary>         assetLibrary,
                  std::shared_ptr<Options>              options,
                  std::shared_ptr<WriterOptions>        writerOptions)
            {
                write(
                    filename,
                    assetLibrary,
                    options,
                    writerOptions,
                    nullptr
                );
            }

            void
            write(const std::string&                    filename,
                  std::shared_ptr<AssetLibrary>         assetLibrary,
                  std::shared_ptr<Options>              options,
                  std::shared_ptr<WriterOptions>        writerOptions,
                  Dependency::Ptr                       dependency)
            {
                auto embeddedHeaderData = std::vector<unsigned char>();

                write(
                    filename,
                    assetLibrary,
                    options,
                    writerOptions,
                    dependency,
                    {},
                    embeddedHeaderData
                );
            }

            void
            write(const std::string&                    filename,
                  std::shared_ptr<AssetLibrary>         assetLibrary,
                  std::shared_ptr<Options>              options,
                  std::shared_ptr<WriterOptions>        writerOptions,
                  Dependency::Ptr                       dependency,
                  const SerializedDependency&	        userDefinedDependency,
                  std::vector<unsigned char>&           embeddedHeaderData)
            {
                std::ofstream file(filename, std::ios::out | std::ios::binary | std::ios::trunc);

                doWrite(
                    filename,
                    assetLibrary,
                    options,
                    writerOptions,
                    dependency,
                    userDefinedDependency,
                    embeddedHeaderData,
                    file
                );

                complete()->execute(this->shared_from_this());
            }

			char *
			getHeader(unsigned int dependenciesSize, unsigned int dataSize)
			{
				auto headerSize = MINKO_SCENE_HEADER_SIZE;

				char *header = (char*)(malloc(headerSize));

				//MAGIC NUMBER
				writeInt(header, _magicNumber, 0);

				//VERSION
				auto version = ((MINKO_SCENE_VERSION_MAJOR & 0xFF) << 24) | ((MINKO_SCENE_VERSION_MINOR << 8) & 0xFFFF) | (MINKO_SCENE_VERSION_PATCH & 0xFF);
				writeInt(header, version, 4);

				auto fileSize = headerSize + dependenciesSize + dataSize;

				//FILE SIZE
				writeInt(header, fileSize, 8);

				//HEADER SIZE
				writeShort(header, headerSize, 12);

				//DEPENDENCIES SIZE
				writeInt(header, dependenciesSize, 14);

				//DATA SIZE
				writeInt(header, dataSize, 18);

				//RESERVED FOR FUTURE USE
				writeInt(header, 0x00000000, 22);
				writeInt(header, 0x00000000, 26);

				return header;
			}

			void
			writeInt(std::ostream& file, int i)
			{
				char data[4];

				data[0] = (i >> 24) & 0xFF;
				data[1] = (i >> 16) & 0xFF;
				data[2] = (i >> 8) & 0xFF;
				data[3] = i & 0xFF;

				file.write(data, 4);
			}

			void
			writeShort(std::ostream& file, int s)
			{
				char data[2];

				data[0] = (s >> 8) & 0xFF;
				data[1] = s & 0xFF;

				file.write(data, 2);
			}

			void
			writeInt(char *data, int i, int offset)
			{
				data[offset] = (i >> 24) & 0xFF;
				data[offset + 1] = (i >> 16) & 0xFF;
				data[offset + 2] = (i >> 8) & 0xFF;
				data[offset + 3] = i & 0xFF;
			}

			void
			writeShort(char *data, int s, int offset)
			{
				data[offset] = (s >> 8) & 0xFF;
				data[offset + 1] = s & 0xFF;
			}

			std::string
			embedAll(std::shared_ptr<AssetLibrary>  assetLibrary,
					 std::shared_ptr<Options>       options,
					 std::shared_ptr<WriterOptions> writerOptions,
                     Dependency::Ptr                dependency)
			{
				return embedAll(assetLibrary, options, writerOptions, dependency, SerializedDependency());
			}

            std::string
            embedAll(std::shared_ptr<AssetLibrary>  assetLibrary,
                     std::shared_ptr<Options>       options,
                     std::shared_ptr<WriterOptions> writerOptions,
                     Dependency::Ptr                dependency,
                     const SerializedDependency&	userDefinedDependency)
            {
                std::stringstream data;

                auto embeddedHeaderdata = std::vector<unsigned char>();

                doWrite(
                    {},
                    assetLibrary,
                    options,
                    writerOptions,
                    dependency,
                    userDefinedDependency,
                    embeddedHeaderdata,
                    data
                );

                complete()->execute(this->shared_from_this());

                return data.str();
            }

			virtual
			std::string
			embed(std::shared_ptr<AssetLibrary>		assetLibrary,
				  std::shared_ptr<Options>			options,
                  Dependency::Ptr                   dependency,
                  std::shared_ptr<WriterOptions>    writerOptions,
                  std::vector<unsigned char>&       embededHeaderData) = 0;

		protected:
			AbstractWriter() :
				_complete(Signal<Ptr>::create()),
                _error(Signal<Ptr, const WriterError&>::create())
            {
            }

        private:
            inline
            void
            preprocess(T& data, std::shared_ptr<file::AssetLibrary> assetLibrary)
            {
                for (auto preprocessor : _preprocessors)
                {
                    auto statusChangedSlot = preprocessor->statusChanged()->connect(
                        [](typename AbstractWriterPreprocessor<T>::Ptr preprocessor, const std::string& status)
                        {
                            const auto progressRate = static_cast<unsigned int>(preprocessor->progressRate() * 100.f);

                            LOG_INFO(status << " ( " << progressRate << "% )");
                        }
                    );

                    preprocessor->process(data, assetLibrary);
                }
            }

            inline
            void
            doWrite(const std::string&                    filename,
                    std::shared_ptr<AssetLibrary>         assetLibrary,
                    std::shared_ptr<Options>              options,
                    std::shared_ptr<WriterOptions>        writerOptions,
                    Dependency::Ptr                       dependency,
                    const SerializedDependency&	          userDefinedDependency,
                    std::vector<unsigned char>&           embeddedHeaderData,
                    std::ostream&                         result)
            {
                preprocess(data(), assetLibrary);

                auto localDependency = Dependency::Ptr();
                auto globalDependency = Dependency::Ptr();

                if (dependency == nullptr)
                {
                    globalDependency = Dependency::create();
                    localDependency = globalDependency;
                }
                else
                {
                    globalDependency = dependency;
                    localDependency = Dependency::create();
                }

                try
                {
                    const auto serializedData = embed(
                        assetLibrary,
                        options,
                        globalDependency,
                        writerOptions,
                        embeddedHeaderData
                    );

                    auto internalLinkedAssets = std::vector<std::vector<unsigned char>>();
                    auto serializedDependency = localDependency->serialize(
                        filename,
                        assetLibrary,
                        options,
                        writerOptions,
                        internalLinkedAssets
                    );

                    if (userDefinedDependency.size() > 0)
                    {
                        serializedDependency.insert(
                            serializedDependency.begin(),
                            userDefinedDependency.begin(),
                            userDefinedDependency.end()
                        );
                    }

                    std::vector<std::string> serializedDependencyBufs;

                    unsigned int dependencySize = 2;

                    for (auto serializedDependencyEntry : serializedDependency)
                    {
                        dependencySize += 4;

                        std::stringstream sbuf;
                        msgpack::pack(sbuf, serializedDependencyEntry);

                        serializedDependencyBufs.push_back(sbuf.str());

                        dependencySize += sbuf.str().size();
                    }

                    msgpack::type::tuple<SerializedDependency> res(serializedDependency);

                    auto dataSize = serializedData.size();

                    char* header = getHeader(dependencySize, dataSize);

                    auto headerSize = MINKO_SCENE_HEADER_SIZE;

                    embeddedHeaderData.insert(embeddedHeaderData.begin(), header, header + headerSize);

                    const auto numDependencies = short(serializedDependencyBufs.size());

                    embeddedHeaderData.push_back((numDependencies & 0xff00) >> 8);
                    embeddedHeaderData.push_back((numDependencies & 0x00ff));

                    result.write(
                        reinterpret_cast<const char*>(embeddedHeaderData.data()),
                        embeddedHeaderData.size()
                    );

                    for (auto& dependencyBuffer : serializedDependencyBufs)
                    {
                        writeInt(result, dependencyBuffer.size());
                        result.write(dependencyBuffer.c_str(), dependencyBuffer.size());

                        dependencyBuffer.clear();
                    }

                    result.write(serializedData.c_str(), serializedData.size());

                    for (const auto& internalLinkedAsset : internalLinkedAssets)
                    {
                        result.write(
                            reinterpret_cast<const char*>(internalLinkedAsset.data()),
                            internalLinkedAsset.size()
                        );
                    }
                }
                catch (const WriterError& exception)
                {
                    if (error()->numCallbacks() > 0)
                        error()->execute(this->shared_from_this(), exception);
                    else
                        throw exception;
                }
            }
        };
    }
}
