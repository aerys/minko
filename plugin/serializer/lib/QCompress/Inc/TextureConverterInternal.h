/**
 *  Need this header file so that we can make TextureConverter.h not dependent on C++
 *
 */

#ifndef _TEXTURECONVERTER_INTERNAL_H
#define _TEXTURECONVERTER_INTERNAL_H

#include <map>

using std::map;

// Key: texture-format Value: expected interediate-format
DllImport extern map<unsigned int, unsigned int> expectedIntermediateFormat;     

#endif