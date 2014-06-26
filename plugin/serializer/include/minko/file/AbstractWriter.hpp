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
#include "minko/file/Dependency.hpp"
#include "msgpack.hpp"

namespace minko
{
	namespace file
	{
		template <typename T>
		class AbstractWriter :
			public std::enable_shared_from_this<AbstractWriter<T>>
		{
		public:
			typedef std::shared_ptr<AbstractWriter> Ptr;

		private:
			typedef std::vector<msgpack::type::tuple<unsigned int, short, std::string>> SerializedDependency;

		protected :
			std::shared_ptr<Signal<Ptr>>	_complete;
			T								_data;
			std::shared_ptr<Dependency>		_parentDependencies;

			int								_magicNumber;

		public:
			inline
			std::shared_ptr<Signal<Ptr>>
			complete()
			{
				return _complete;
			}

			inline
			T
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
			void
			parentDependencies(std::shared_ptr<Dependency> parentDependencies)
			{
				_parentDependencies = parentDependencies;
			}

			void
			write(std::string&                          filename,
				  std::shared_ptr<AssetLibrary>         assetLibrary,
				  std::shared_ptr<Options>              options,
				  std::shared_ptr<WriterOptions>        writerOptions)
			{
				SerializedDependency includeDependency;

				write(filename, assetLibrary, options, writerOptions, includeDependency);
			}

			void
			write(std::string&                          filename,
				  std::shared_ptr<AssetLibrary>         assetLibrary,
				  std::shared_ptr<Options>              options,
                  std::shared_ptr<WriterOptions>        writerOptions,
				  SerializedDependency&					includeDependency)
			{
				std::ofstream file(filename, std::ios::out | std::ios::binary | std::ios::trunc);

				if (file)
				{
					Dependency::Ptr			dependencies			= Dependency::create();
					std::string				serializedData			= embed(assetLibrary, options, dependencies, writerOptions);
					SerializedDependency	serializedDependencies	= dependencies->serialize(assetLibrary, options, writerOptions);
					
					if (includeDependency.size() > 0)
						serializedDependencies.insert(serializedDependencies.begin(), includeDependency.begin(), includeDependency.end());

					std::vector<std::string> serializedDependenciesBufs;
					
					unsigned int dependenciesSize = 2;

					for (auto serializedDependency : serializedDependencies)
					{
						dependenciesSize += 4;

						std::stringstream sbuf;
						msgpack::pack(sbuf, serializedDependency);

						serializedDependenciesBufs.push_back(sbuf.str());

						dependenciesSize += sbuf.str().size();
					}

					msgpack::type::tuple<SerializedDependency> res(serializedDependencies);


					auto dataSize = serializedData.size();

					char* header = getHeader(dependenciesSize, dataSize);
					
					auto headerSize = MINKO_SCENE_HEADER_SIZE;

					file.write(header, headerSize);

					writeShort(file, serializedDependenciesBufs.size());

					for (auto& dependencyBuffer : serializedDependenciesBufs)
					{
						writeInt(file, dependencyBuffer.size());
						file.write(dependencyBuffer.c_str(), dependencyBuffer.size());

						dependencyBuffer.clear();
					}

					file.write(serializedData.c_str(), serializedData.size());
					file.close();
				}
				else
					std::cerr << "File " << filename << " can't be opened" << std::endl;

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
				auto version = ((MINKO_SCENE_VERSION_HI & 0xFF) << 24) | ((MINKO_SCENE_VERSION_LO << 8) & 0xFFFF) | (MINKO_SCENE_VERSION_BUILD & 0xFF);
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
			writeInt(std::ofstream& file, int i)
			{
				char data[4];

				data[0] = (i >> 24) & 0xFF;
				data[1] = (i >> 16) & 0xFF;
				data[2] = (i >> 8) & 0xFF;
				data[3] = i & 0xFF;

				file.write(data, 4);
			}

			void
			writeShort(std::ofstream& file, int s)
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
					 std::shared_ptr<WriterOptions> writerOptions)
			{
				SerializedDependency includeDependency;

				return embedAll(assetLibrary, options, writerOptions, includeDependency);
			}

            std::string
            embedAll(std::shared_ptr<AssetLibrary>  assetLibrary,
                     std::shared_ptr<Options>       options,
                     std::shared_ptr<WriterOptions> writerOptions,
					 SerializedDependency&			includeDependency)
            {
                // TODO
                // refactor with AbstractWriter::write by adding a tier private member function

				Dependency::Ptr			dependencies	= _parentDependencies;
				std::string				serializedData	= embed(assetLibrary, options, dependencies, writerOptions);
				SerializedDependency	serializedDependencies = Dependency::create()->serialize(assetLibrary,
                                                                                                 options,
                                                                                                 writerOptions);
				if (includeDependency.size() > 0)
					serializedDependencies.insert(serializedDependencies.begin(), includeDependency.begin(), includeDependency.end());

                msgpack::type::tuple<SerializedDependency> res(serializedDependencies);

				std::stringstream data;

				std::stringstream sbuf;
				msgpack::pack(sbuf, res);

				auto dependenciesSize = sbuf.str().size();
				auto sceneDataSize = serializedData.size();
				auto header = getHeader(dependenciesSize, sceneDataSize);

				auto headerSize = MINKO_SCENE_HEADER_SIZE;

				data.write(header, headerSize);
				data.write(sbuf.str().c_str(), dependenciesSize);
				data.write(serializedData.c_str(), sceneDataSize);

                complete()->execute(this->shared_from_this());

				sbuf.clear();
				serializedData.clear();
				serializedData.shrink_to_fit();
				free(header);

				return data.str();
            }

			virtual
			std::string
			embed(std::shared_ptr<AssetLibrary>		assetLibrary,
				  std::shared_ptr<Options>			options,
				  Dependency::Ptr					dependencies,
                  std::shared_ptr<WriterOptions>    writerOptions) = 0;

		protected:
			AbstractWriter() :
				_complete(Signal<Ptr>::create()),
				_parentDependencies(nullptr)
			{
			}
		};
	}
}
