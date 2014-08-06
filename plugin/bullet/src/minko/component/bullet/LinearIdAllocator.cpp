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

#include "LinearIdAllocator.hpp"

using namespace minko;
using namespace component;

bullet::LinearIdAllocator::LinearIdAllocator(uint maxUid):
    _uids(),
    _uidToIndex(),
    _numUsedUids(0),
    _MAX_UID(maxUid)
{
    _uids.resize(_MAX_UID);
    _uidToIndex.resize(_MAX_UID);

    for (uint i = 0; i < _MAX_UID; ++i)
        _uids[i] = _uidToIndex[i] = i;
}

uint
bullet::LinearIdAllocator::allocate()
{
    if (_numUsedUids == _MAX_UID)
        throw std::logic_error("failed to allocate a new unique id (max number of ids reached).");

    const uint ret = _uids[_numUsedUids];

    ++_numUsedUids;

    return ret;
}

void
bullet::LinearIdAllocator::free(uint uid)
{
    if (uid > _MAX_UID || _uidToIndex[uid] >= _numUsedUids)
        throw std::invalid_argument("uid");

    const unsigned int pos    = _uidToIndex[uid];

    const unsigned int pos2 = _numUsedUids-1;
    const unsigned int uid2 = _uids[pos2];

    _uids[_numUsedUids-1]    = uid;
    _uidToIndex[uid]        = _numUsedUids-1;

    _uids[pos]                = uid2;
    _uidToIndex[uid2]        = pos;

    --_numUsedUids;
}