#ifndef LUAGLUE_APPLYTUPLE_H_GUARD
#define LUAGLUE_APPLYTUPLE_H_GUARD

#include <cstdint>
#include <tuple>
#include <lua.hpp>
#include <typeinfo>

class LuaGlue;

template<typename T>
T getValue(LuaGlue &, lua_State *, unsigned int);

template<>
int getValue<int>(LuaGlue &, lua_State *state, unsigned int idx)
{
	int v = luaL_checkint(state, idx);
	//printf("getValue<int>: v=%d\n", v);
	return v;
}

template<>
double getValue<double>(LuaGlue &, lua_State *state, unsigned int idx)
{
	return luaL_checknumber(state, idx);
}

template<>
const char *getValue<const char *>(LuaGlue &, lua_State *state, unsigned int idx)
{
	return luaL_checkstring(state, idx);
}

template<class T>
LuaGlueClass<T> *getGlueClass(LuaGlue &g, lua_State *state, unsigned int idx)
{
	int ret = luaL_getmetafield(state, idx, LuaGlueClass<T>::METATABLE_CLASSIDX_FIELD);
	if(!ret)
	{
		printf("getGlueClassPtr: typeid:%s\n", typeid(LuaGlueClass<T>).name());
		printf("getGlueClassPtr: failed to get metafield for obj at idx %i\n", idx);
		return 0;
	}
	
	int id = luaL_checkint(state, -1);
	lua_pop(state, 1);
	
	//printf("getGlueClass: METATABLE_CLASSIDX_FIELD: %i\n", id);
	return (LuaGlueClass<T> *)g.lookupClass(id);
}

template<class T>
T getValue_(LuaGlue &g, lua_State *state, unsigned int idx, std::true_type)
{
	//printf("getValuePtr: idx:%i\n", idx);
	if(lua_islightuserdata(state, idx))
	{
		//printf("getValue: lud!\n");
		return (T)lua_touserdata(state, idx);
	}
	
#ifdef LUAGLUE_TYPECHECK
	typedef typename std::remove_pointer<T>::type TC;
	LuaGlueClass<TC> *lgc = getGlueClass<TC>(g, state, idx);
	if(lgc)
	{
#else
		(void)g;
#endif
		T v = *(T *)lua_touserdata(state, idx);
		return v;
#ifdef LUAGLUE_TYPECHECK
	}
#endif
	
	printf("getValuePtr: failed to get a class instance for lua stack value at idx: %i\n", idx);
	return 0;
}

template<class T>
T getValue_(LuaGlue &g, lua_State *state, unsigned int idx, std::false_type)
{
	if(lua_islightuserdata(state, idx))
	{
		//printf("getValue: lud!\n");
		return *(T*)lua_touserdata(state, idx);
	}

#ifdef LUAGLUE_TYPECHECK
	LuaGlueClass<T> *lgc = getGlueClass<T>(g, state, idx);
	if(lgc)
	{
#else
		(void)g;
#endif
		return **(T **)lua_touserdata(state, idx);
#ifdef LUAGLUE_TYPECHECK
	}
#endif

	printf("getValue: failed to get a class instance for lua stack value at idx: %i\n", idx);
	return T();
}

template<class T>
T getValue(LuaGlue &g, lua_State *state, unsigned int idx)
{
	return getValue_<T>(g, state, idx, std::is_pointer<T>());
}

template<typename T>
void returnValue(LuaGlue &, lua_State *, T);

template<>
void returnValue(LuaGlue &, lua_State *state, bool v)
{
	//printf("returnValue: v=%d\n", v);
	lua_pushboolean(state, v);
}

template<>
void returnValue(LuaGlue &, lua_State *state, int v)
{
	//printf("returnValue: v=%d\n", v);
	lua_pushinteger(state, v);
}

template<>
void returnValue(LuaGlue &, lua_State *state, unsigned int v)
{
	//printf("returnValue: v=%d\n", v);
	lua_pushinteger(state, v);
}

template<>
void returnValue(LuaGlue &, lua_State *state, float v)
{
	//printf("returnValue: v=%d\n", v);
	lua_pushnumber(state, v);
}

template<>
void returnValue(LuaGlue &, lua_State *state, double v)
{
	lua_pushnumber(state, v);
}

template<>
void returnValue(LuaGlue &, lua_State *state, const char *v)
{
	lua_pushstring(state, v);
}

template<class T>
void returnValue(LuaGlue &g, lua_State *state, T *v)
{
	//printf("returnValue begin!\n");
	// first look for a class we support
	
	typedef typename std::remove_pointer<T>::type TC;
	LuaGlueClass<TC> *lgc = (LuaGlueClass<TC> *)g.lookupClass(typeid(LuaGlueClass<TC>).name(), true);
	//printf("returnValuePtr: %s %p lgc:%p\n", typeid(LuaGlueClass<T>).name(), v, lgc);
	if(lgc)
	{
		lgc->pushInstance(state, v);
		return;
	}
	
	// otherwise push onto stack as light user data
	//printf("returnValue: lud!\n");
	lua_pushlightuserdata(state, v);
}

// this is currently a source of memory leaks :(
// need to tell the code that lua owns it somehow
template<class T>
void returnValue(LuaGlue &g, lua_State *state, T v)
{
	LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g.lookupClass(typeid(LuaGlueClass<T>).name(), true);
	if(lgc)
	{
		lgc->pushInstance(state, new T(v));
		return;
	}
	
	// otherwise push onto stack as light user data
	//printf("returnValue: lud!\n");
	lua_pushlightuserdata(state, new T(v));
}

template<typename T>
void putValue(LuaGlue &, lua_State *, T);

template<class T>
void putValue(LuaGlue &g, lua_State *state, T v)
{
	//printf("returnValue begin!\n");
	// first look for a class we support
	
	typedef typename std::remove_pointer<T>::type TC;
	LuaGlueClass<TC> *lgc = (LuaGlueClass<TC> *)g.lookupClass(typeid(LuaGlueClass<TC>).name(), true);
	//printf("returnValuePtr: %s %p lgc:%p\n", typeid(LuaGlueClass<T>).name(), v, lgc);
	if(lgc)
	{
		lgc->pushInstance(state, v);
		return;
	}
	
	// otherwise push onto stack as light user data
	//printf("returnValue: lud!\n");
	lua_pushlightuserdata(state, v);
}

template<>
void putValue(LuaGlue &, lua_State *state, int v)
{
	//printf("returnValue: v=%d\n", v);
	lua_pushinteger(state, v);
}

template<>
void putValue(LuaGlue &, lua_State *state, double v)
{
	lua_pushnumber(state, v);
}

template<>
void putValue(LuaGlue &, lua_State *state, const char *v)
{
	lua_pushstring(state, v);
}

//template<class T>
//void returnValue(LuaGlue &g, lua_State *state, T v)
//{
//	return returnValue_<T>(g, state, v, std::is_pointer<T>());
//}

// original apply tuple code:
// http://stackoverflow.com/questions/687490/how-do-i-expand-a-tuple-into-variadic-template-functions-arguments


//-----------------------------------------------------------------------------

/**
 * Object Function Tuple Argument Unpacking
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @tparam N Number of tuple arguments to unroll
 *
 * @ingroup g_util_tuple
 */
template < uint32_t N >
struct apply_obj_func
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlue &g, lua_State *state, T* pObj,
                          R (T::*f)( ArgsF... ),
                          const std::tuple<ArgsT...> &t,
                          Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_obj_func<N-1>::applyTuple(g, state, pObj, f, std::forward<decltype(t)>(t), getValue<ltype>(g, state, -(argCount-N+1)), args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * Object Function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <>
struct apply_obj_func<0>
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlue &, lua_State *, T* pObj,
                          R (T::*f)( ArgsF... ),
                          const std::tuple<ArgsT...> &/* t */,
                          Args... args )
	{
		return (pObj->*f)( args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * Object Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename T, typename R, typename... ArgsF, typename... ArgsT >
R applyTuple(LuaGlue &g, lua_State *state, T* pObj,
                 R (T::*f)( ArgsF... ),
                 const std::tuple<ArgsT...> &t )
{
	return apply_obj_func<sizeof...(ArgsT)>::applyTuple(g, state, pObj, f, std::forward<decltype(t)>(t) );
}

//-----------------------------------------------------------------------------

/**
 * Static Function Tuple Argument Unpacking
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @tparam N Number of tuple arguments to unroll
 *
 * @ingroup g_util_tuple
 */
template < uint N >
struct apply_func
{
	template < typename R, typename... ArgsF, typename... ArgsT, typename... Args >
	static R applyTuple(	LuaGlue &g, lua_State *state, R (*f)( ArgsF... ),
									const std::tuple<ArgsT...>& t,
									Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_func<N-1>::applyTuple( g, state, f, std::forward<decltype(t)>(t), getValue<ltype>(g, state, -(argCount-N+1)), args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * Static Function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <>
struct apply_func<0>
{
	template < typename R, typename... ArgsF, typename... ArgsT, typename... Args >
	static R applyTuple(	LuaGlue &, lua_State *, R (*f)( ArgsF... ),
									const std::tuple<ArgsT...>& /* t */,
									Args... args )
	{
		return f( args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * Static Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename R, typename... ArgsF, typename... ArgsT >
R applyTuple( LuaGlue &g, lua_State *state, R (*f)(ArgsF...),
                 const std::tuple<ArgsT...> & t )
{
	return apply_func<sizeof...(ArgsT)>::applyTuple( g, state, f, std::forward<decltype(t)>(t) );
}

//-----------------------------------------------------------------------------

// object constructor tuple unpack

/**
 * Ctor Function Tuple Argument Unpacking
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @tparam N Number of tuple arguments to unroll
 *
 * @ingroup g_util_tuple
 */
template <class C, uint N >
struct apply_ctor_func
{
	template < typename... ArgsT, typename... Args >
	static C *applyTuple(	LuaGlue &g, lua_State *state, const std::tuple<ArgsT...>& t,
								Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_ctor_func<C, N-1>::applyTuple( g, state, std::forward<decltype(t)>(t), getValue<ltype>(g, state, -(argCount-N+1)), args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * ctor Function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <class C>
struct apply_ctor_func<C, 0>
{
	template < typename... ArgsT, typename... Args >
	static C *applyTuple(	LuaGlue &, lua_State *, const std::tuple<ArgsT...>& /* t */,
								Args... args )
	{
		return new C( args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * ctor Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename C, typename... ArgsT >
C *applyTuple( LuaGlue &g, lua_State *state, const std::tuple<ArgsT...> & t )
{
	return apply_ctor_func<C, sizeof...(ArgsT)>::applyTuple( g, state, std::forward<decltype(t)>(t) );
}


// lua function tuple unpack

/**
 * Lua Function Tuple Argument Unpacking
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @tparam N Number of tuple arguments to unroll
 *
 * @ingroup g_util_tuple
 */
template < uint N >
struct apply_lua_func
{
	template < typename... ArgsT, typename... Args >
	static void applyTuple(	LuaGlue &g, lua_State *state, const std::tuple<ArgsT...>& t,
								Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		const static unsigned int argIdx = (argCount-N);
		
		typedef typename std::remove_reference<decltype(std::get<argIdx>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		putValue<ltype>(g, state, std::get<argIdx>(t));
		
		apply_lua_func<N-1>::applyTuple( g, state, std::forward<decltype(t)>(t), std::get<argIdx>(t), args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * Lua Function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <>
struct apply_lua_func<0>
{
	template < typename... ArgsT, typename... Args >
	static void applyTuple(	LuaGlue &, lua_State *, const std::tuple<ArgsT...>& /* t */,
								Args... /*args*/ )
	{
		// nada
	}
};

//-----------------------------------------------------------------------------

/**
 * Lua Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename... Args >
void applyTuple( LuaGlue &g, lua_State *state, Args... args )
{
	std::tuple<Args...> t(args...);
	apply_lua_func<sizeof...(Args)>::applyTuple( g, state, std::forward<decltype(t)>(t) );
}


#endif /* LUAGLUE_APPLYTUPLE_H_GUARD */
