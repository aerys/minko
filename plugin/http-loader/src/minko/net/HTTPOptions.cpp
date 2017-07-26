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

#include "minko/net/HTTPOptions.hpp"

using namespace minko;
using namespace minko::file;
using namespace minko::net;

HTTPOptions::HTTPOptions() :
    Options(),
    _username(),
    _password(),
    _additionalHeaders(),
    _verifyPeer(true),
    _postFields()
{
}

HTTPOptions::HTTPOptions(const HTTPOptions& copy) :
    Options(copy),
    _username(copy._username),
    _password(copy._password),
    _additionalHeaders(copy._additionalHeaders),
    _verifyPeer(copy._verifyPeer),
    _postFields(copy._postFields)
{
}

HTTPOptions::HTTPOptions(const Options& copy) :
    Options(copy)
{
}

Options::Ptr
HTTPOptions::clone()
{
    auto copy = Ptr(new HTTPOptions(*this));

    copy->initialize();

    return copy;
}
