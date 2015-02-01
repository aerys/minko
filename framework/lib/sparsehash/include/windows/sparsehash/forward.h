#pragma once

#include <hash_map>
#include <sparsehash/internal/sparseconfig.h>

namespace google
{
    template<class T>
    class libc_allocator_with_realloc;

    template<class Key, class T,
    class HashFcn = SPARSEHASH_HASH<Key>,
    class EqualKey = std::equal_to<Key>,
    class Alloc = libc_allocator_with_realloc<std::pair<const Key, T>>>
    class sparse_hash_map;
}
