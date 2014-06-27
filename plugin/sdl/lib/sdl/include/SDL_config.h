/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _SDL_config_h
#define _SDL_config_h

#include "SDL_platform.h"

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#if _MSC_VER
# include "SDL_config_windows.h"
#elif __ANDROID__
# include "SDL_config_android.h"
#elif TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
# include "SDL_config_iphoneos.h"
#elif TARGET_OS_MAC
# include "SDL_config_macosx.h"
#elif LINUX || __unix__
# include "SDL_config_linux.h"
#elif EMSCRIPTEN
# error "Emscripten has its own set of headers"
#else
# error "unsupported platform"
#endif

#endif /* _SDL_config_h */
