#pragma once

#include <exception>
#include <string>

class DynamicLibrary
{
public:
    DynamicLibrary(const char *fileName);
    ~DynamicLibrary();

    void *getFunctionPtr(const char *name) const;

    DynamicLibrary(const DynamicLibrary &) = delete;
    DynamicLibrary & operator = (const DynamicLibrary &other) = delete;

private:
    void *libHandle;
};

class OpenLibFailedException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Failed to open Library";
    }
};

