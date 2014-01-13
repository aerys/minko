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

template <typename T>
struct luaglue_remove_reference { typedef T type; };

template <typename T>
struct luaglue_remove_reference<T&> { typedef T type; };

template <typename T>
struct luaglue_remove_const { typedef T type; };

template <typename T>
struct luaglue_remove_const<const T> { typedef T type; };

template <typename T>
struct luaglue_remove_const_reference
{
	typedef typename luaglue_remove_const<typename luaglue_remove_reference<T>::type>::type type;
};

#else

template <typename T>
using luaglue_remove_reference<T> = std::remove_reference<T>;

template <typename T>
using luaglue_remove_const<T> std::remove_const<T>;

template <typename T>
struct luaglue_remove_const_reference
{
	typedef std::remove_const<typename std::remove_reference<T>::type>::type type;
};

#endif /* _MSC_VER */

#endif /* LUAGLUE_COMPAT_H_GUARD */
