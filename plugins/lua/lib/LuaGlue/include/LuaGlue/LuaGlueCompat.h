#ifndef LUAGLUE_COMPAT_H_GUARD
#define LUAGLUE_COMPAT_H_GUARD

#if defined(_MSC_VER)

// TODO: at some point MSVC will fully support constexpr.
//  http://herbsutter.com/2013/11/18/visual-c-compiler-november-2013-ctp/
//  It seems the November 2013 CTP release supports /some/ uses of constexpr.
//  But I am not aware of how much it supports, or when it will be included in the release msvc.
//  For now, just blanket define constexpr, replace it with a version check later.

# ifndef constexpr
#  define constexpr const
# endif

#endif /* _MSC_VER */

#endif /* LUAGLUE_COMPAT_H_GUARD */
