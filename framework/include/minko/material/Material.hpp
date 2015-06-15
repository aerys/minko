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

#include "minko/data/Provider.hpp"
#include "minko/Uuid.hpp"

namespace minko
{
	namespace material
	{
		class Material :
            public std::enable_shared_from_this<Material>,
            public Uuid::has_uuid
		{
		public:
			typedef std::shared_ptr<Material>	Ptr;

        private:
            std::shared_ptr<data::Provider> _provider;

		public:
            ~Material()
            {
                _provider = nullptr;
            }

			inline static
			Ptr
			create(const std::string& name = "material")
			{
                auto instance = Ptr(new Material(name));

                instance->initialize();

                return instance;
			}

			inline static
			Ptr
			create(Ptr source)
			{
				auto mat = create();

				mat->_provider->copyFrom(source->_provider);

				return mat;
			}

            inline
            const std::string&
            uuid() const
            {
                return _provider->uuid();
            }

            inline
            const std::string&
            name() const
            {
                return _provider->get<std::string>("name");
            }

            inline
            std::shared_ptr<data::Provider>
            data() const
            {
                return _provider;
            }

            Ptr
            set(std::initializer_list<data::Provider::ValueType> values)
            {
                _provider->set(values);

                return shared_from_this();
            }

		protected:
			Material(const std::string& name) :
                _provider(data::Provider::create())
            {
                _provider->set("name", name);
                _provider->set("uuid", _provider->uuid());
            }

            Material(const std::string& name,
                     const data::Provider::DefaultValueMap& values) :
                _provider(data::Provider::create(values))
            {
                _provider->set("name", name);
                _provider->set("uuid", _provider->uuid());
            }

            virtual
            void
            initialize()
            {
            }
		};
	}
}
