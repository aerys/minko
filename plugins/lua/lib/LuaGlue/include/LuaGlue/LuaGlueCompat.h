#pragma once

#if _MSC_VER

// Visual Studio does not support constexpr
# ifndef constexpr
#  define constexpr const
# endif

// Visual Studio does not support uint
# ifndef uint
#  define uint unsigned int
# endif

#endif
