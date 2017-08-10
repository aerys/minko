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

#include "minko/file/Options.hpp"

namespace minko
{
    namespace net
    {
        class HTTPOptions :
            public file::Options
        {
        public:
            typedef std::shared_ptr<HTTPOptions> Ptr;

        private:
            std::string                                     _username;
            std::string                                     _password;

            std::unordered_map<std::string, std::string>    _additionalHeaders;

            bool                                            _verifyPeer;

        public:
            inline
            static
            Ptr
            create()
            {
                auto instance = Ptr(new HTTPOptions());

                instance->initialize();

                return instance;
            };

            inline
            static
            Ptr
            create(file::Options::Ptr copy)
            {
                auto httpOptions = std::dynamic_pointer_cast<HTTPOptions>(copy);
                Ptr instance = nullptr;

                if (!httpOptions)
                    instance = Ptr(new HTTPOptions(*copy));
                else
                    instance = std::static_pointer_cast<HTTPOptions>(httpOptions->clone());

                instance->initialize();

                return instance;
            };

            file::Options::Ptr
            clone();

            inline
            const std::string&
            username() const
            {
                return _username;
            }

            inline
            Ptr
            username(const std::string& value)
            {
                _username = value;

                return std::static_pointer_cast<HTTPOptions>(shared_from_this());
            }

            inline
            const std::string&
            password() const
            {
                return _password;
            }

            inline
            Ptr
            password(const std::string& value)
            {
                _password = value;

                return std::static_pointer_cast<HTTPOptions>(shared_from_this());
            }

            inline
            std::unordered_map<std::string, std::string>&
            additionalHeaders()
            {
                return _additionalHeaders;
            }

            inline
            bool
            verifyPeer() const
            {
                return _verifyPeer;
            }

            inline
            Ptr
            verifyPeer(bool value)
            {
                _verifyPeer = value;

                return std::static_pointer_cast<HTTPOptions>(shared_from_this());
            }

        protected:
            HTTPOptions(const HTTPOptions& copy);

        private:
            HTTPOptions();

            HTTPOptions(const file::Options& copy);
        };
    }
}
