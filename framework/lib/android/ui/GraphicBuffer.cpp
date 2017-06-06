#include "GraphicBuffer.h"

#include <string>
#include <cstdlib>
#include <iostream>

using std::string;

const int GRAPHICBUFFER_SIZE = 1024;

template<typename Func>
void setFuncPtr (Func*& funcPtr, const DynamicLibrary& lib, const string& symname)
{
    funcPtr = reinterpret_cast<Func*>(lib.getFunctionPtr(symname.c_str()));
}

#if defined(__aarch64__)
#	define CPU_ARM_64
#elif defined(__arm__) || defined(__ARM__) || defined(__ARM_NEON__) || defined(ARM_BUILD)
#	define CPU_ARM
#elif defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
#	define CPU_X86_64
#elif defined(__i386__) || defined(_M_X86) || defined(_M_IX86) || defined(X86_BUILD)
#	define CPU_X86
#else
#	warning "target CPU does not support ABI"
#endif

template <typename RT, typename T1, typename T2, typename T3, typename T4>
RT* callConstructor4 (void (*fptr)(), void* memory, T1 param1, T2 param2, T3 param3, T4 param4)
{
#if defined(CPU_ARM)
    // C1 constructors return pointer
    typedef RT* (*ABIFptr)(void*, T1, T2, T3, T4);
    (void)((ABIFptr)fptr)(memory, param1, param2, param3, param4);
    return reinterpret_cast<RT*>(memory);
#elif defined(CPU_ARM_64)
    // C1 constructors return void
    typedef void (*ABIFptr)(void*, T1, T2, T3, T4);
    ((ABIFptr)fptr)(memory, param1, param2, param3, param4);
    return reinterpret_cast<RT*>(memory);
#elif defined(CPU_X86) || defined(CPU_X86_64)
    // ctor returns void
    typedef void (*ABIFptr)(void*, T1, T2, T3, T4);
    ((ABIFptr)fptr)(memory, param1, param2, param3, param4);
    return reinterpret_cast<RT*>(memory);
#else
    return nullptr;
#endif
}

template <typename T>
void callDestructor (void (*fptr)(), T* obj)
{
#if defined(CPU_ARM)
    // D1 destructor returns ptr
    typedef void* (*ABIFptr)(T* obj);
    (void)((ABIFptr)fptr)(obj);
#elif defined(CPU_ARM_64)
    // D1 destructor returns void
    typedef void (*ABIFptr)(T* obj);
    ((ABIFptr)fptr)(obj);
#elif defined(CPU_X86) || defined(CPU_X86_64)
    // dtor returns void
    typedef void (*ABIFptr)(T* obj);
    ((ABIFptr)fptr)(obj);
#endif
}

template<typename T1, typename T2>
T1* pointerToOffset (T2* ptr, size_t bytes)
{
    return reinterpret_cast<T1*>((uint8_t *)ptr + bytes);
}

static android::android_native_base_t* getAndroidNativeBase (android::GraphicBuffer* gb)
{
    return pointerToOffset<android::android_native_base_t>(gb, 2 * sizeof(void *));
}

GraphicBuffer::GraphicBuffer(uint32_t width, uint32_t height, PixelFormat format, uint32_t usage):
    library("libui.so")
{
    setFuncPtr(functions.constructor, library, "_ZN7android13GraphicBufferC1Ejjij");
    setFuncPtr(functions.destructor, library, "_ZN7android13GraphicBufferD1Ev");
    setFuncPtr(functions.getNativeBuffer, library, "_ZNK7android13GraphicBuffer15getNativeBufferEv");
    setFuncPtr(functions.lock, library, "_ZN7android13GraphicBuffer4lockEjPPv");
    setFuncPtr(functions.unlock, library, "_ZN7android13GraphicBuffer6unlockEv");
    setFuncPtr(functions.initCheck, library, "_ZNK7android13GraphicBuffer9initCheckEv");

    // allocate memory for GraphicBuffer object
    void *const memory = malloc(GRAPHICBUFFER_SIZE);
    if (memory == nullptr) {
        std::cerr << "Could not alloc for GraphicBuffer" << std::endl;
        return;
    }

    try {
        android::GraphicBuffer* const gb = callConstructor4<android::GraphicBuffer, uint32_t, uint32_t, PixelFormat, uint32_t>(
                functions.constructor,
                memory,
                width,
                height,
                format,
                usage
                );
        android::android_native_base_t* const base = getAndroidNativeBase(gb);
        status_t ctorStatus = functions.initCheck(gb);

        if (ctorStatus) {
            // ctor failed
            callDestructor<android::GraphicBuffer>(functions.destructor, gb);
            std::cerr << "GraphicBuffer ctor failed, initCheck returned "  << ctorStatus << std::endl;
        }

        // check object layout
        if (base->magic != 0x5f626672u) // "_bfr"
            std::cerr << "GraphicBuffer layout unexpected" << std::endl;

        // check object version
        const uint32_t expectedVersion = sizeof(void *) == 4 ? 96 : 168;
        if (base->version != expectedVersion)
            std::cerr << "GraphicBuffer version unexpected" << std::endl;

        base->incRef(base);
        impl = gb;
    } catch (...) {
        free(memory);
        throw;
    }
}

GraphicBuffer::~GraphicBuffer()
{
    if (impl) {
        android::android_native_base_t* const base = getAndroidNativeBase(impl);
        base->decRef(base);
        free(impl);
    }
}

status_t GraphicBuffer::lock(uint32_t usage, void** vaddr)
{
    return functions.lock(impl, usage, vaddr);
}

status_t GraphicBuffer::unlock()
{
    return functions.unlock(impl);
}

ANativeWindowBuffer *GraphicBuffer::getNativeBuffer() const
{
    return functions.getNativeBuffer(impl);
}

uint32_t GraphicBuffer::getStride() const
{
    return ((android::android_native_buffer_t*)getNativeBuffer())->stride;
}
