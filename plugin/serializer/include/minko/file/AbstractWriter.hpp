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
				std::ofstream file(filename, std::ios::out | std::ios::binary | std::ios::trunc);

				if (file)
				{
					Dependency::Ptr			dependencies			= Dependency::create();
					std::string				serializedData			= embed(assetLibrary, options, dependencies, writerOptions);
					SerializedDependency	serializedDependencies	= dependencies->serialize(assetLibrary, options, writerOptions);

					msgpack::type::tuple<SerializedDependency, std::string> res(serializedDependencies, serializedData);

					std::stringstream sbuf;
					msgpack::pack(sbuf, res);

					file.write(sbuf.str().c_str(), sbuf.str().size());
					file.close();
				}
				else
					std::cerr << "File " << filename << " can't be opened" << std::endl;

				complete()->execute(this->shared_from_this());
			}

            std::string
            embedAll(std::shared_ptr<AssetLibrary>  assetLibrary,
                     std::shared_ptr<Options>       options,
                     std::shared_ptr<WriterOptions> writerOptions)
            {
                // TODO
                // refactor with AbstractWriter::write by adding a tier private member function

				Dependency::Ptr			dependencies	= _parentDependencies;
				std::string				serializedData	= embed(assetLibrary, options, dependencies, writerOptions);
				SerializedDependency	serializedDependencies = Dependency::create()->serialize(assetLibrary,
                                                                                                 options,
                                                                                                 writerOptions);

                msgpack::type::tuple<SerializedDependency, std::string> res(serializedDependencies, serializedData);

                std::stringstream sbuf;
                msgpack::pack(sbuf, res);

                complete()->execute(this->shared_from_this());

                return sbuf.str();
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
