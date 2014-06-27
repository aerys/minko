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

		protected:
			std::shared_ptr<Signal<Ptr>>	                    _complete;
            std::shared_ptr<Signal<Ptr, const WriterError&>>    _error;
			T								                    _data;
			std::shared_ptr<Dependency>		                    _parentDependencies;

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
                try
                {
                    std::ofstream file(filename, std::ios::out | std::ios::binary | std::ios::trunc);

                    if (file)
                    {
                        Dependency::Ptr			dependencies = Dependency::create();
                        std::string				serializedData = embed(assetLibrary, options, dependencies, writerOptions);
                        SerializedDependency	serializedDependencies = dependencies->serialize(assetLibrary, options, writerOptions);

                        if (includeDependency.size() > 0)
                            serializedDependencies.insert(serializedDependencies.begin(), includeDependency.begin(), includeDependency.end());

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
                catch (const WriterError& exception)
                {
                    if (error()->numCallbacks() > 0)
                        error()->execute(this->shared_from_this(), exception);
                    else
                        throw exception;
                }
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
                try
                {
                    Dependency::Ptr			dependencies = _parentDependencies;
                    std::string				serializedData = embed(assetLibrary, options, dependencies, writerOptions);
                    SerializedDependency	serializedDependencies = Dependency::create()->serialize(assetLibrary,
                                                                                                     options,
                                                                                                     writerOptions);
                    if (includeDependency.size() > 0)
                        serializedDependencies.insert(serializedDependencies.begin(), includeDependency.begin(), includeDependency.end());

                    msgpack::type::tuple<SerializedDependency, std::string> res(serializedDependencies, serializedData);

                    std::stringstream sbuf;
                    msgpack::pack(sbuf, res);

                    complete()->execute(this->shared_from_this());

                    return sbuf.str();
                }
                catch (const WriterError& exception)
                {
                    if (error()->numCallbacks() > 0)
                        error()->execute(this->shared_from_this(), exception);
                    else
                        throw exception;
                }

                return std::string();
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
                _error(Signal<Ptr, const WriterError&>::create()),
				_parentDependencies(nullptr)
			{
			}
		};
	}
}
