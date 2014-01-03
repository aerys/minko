#ifndef LUAGLUE_APPLYTUPLE_H_GUARD
#define LUAGLUE_APPLYTUPLE_H_GUARD

#include <cstdint>
#include <tuple>
#include <lua.hpp>
#include <typeinfo>

class LuaGlueBase;

template<class T>
LuaGlueClass<T> *getGlueClass(LuaGlueBase *g, lua_State *s, int idx)
{
	int ret = luaL_getmetafield(s, idx, LuaGlueClass<T>::METATABLE_CLASSIDX_FIELD);
	if(!ret)
	{
		LG_Error("typeid:%s", typeid(LuaGlueClass<T>).name());
		LG_Error("failed to get metafield for obj at idx %i", idx);
		return 0;
	}
	
	int id = luaL_checkint(s, -1);
	lua_pop(s, 1);
	
	//printf("getGlueClass: METATABLE_CLASSIDX_FIELD: %i\n", id);
	return (LuaGlueClass<T> *)g->lookupClass((uint32_t)id);
}

// FIXME: static objects need fixed again.
// new LuaGlueObject stuff needs a way to "own" a pointer, and know how to delete it.
template<class T>
struct stack {
	static T get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<static %s>: lud", typeid(T).name());
			return *(T*)lua_touserdata(s, idx);
		}
		
#ifdef LUAGLUE_TYPECHECK
		LuaGlueClass<T> *lgc = getGlueClass<T>(g, s, idx);
		if(lgc)
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<static %s>: mapped", typeid(T).name());
			LuaGlueObject<T> obj = *(LuaGlueObject<T> *)lua_touserdata(s, idx);
			return *obj;
#ifdef LUAGLUE_TYPECHECK
		}
#endif

		LG_Debug("stack::get<static %s>: failed to get a class instance for lua stack value at idx: %i", typeid(T).name(), idx);
		return T();
	}
	
	static void put(LuaGlueBase *g, lua_State *s, T v)
	{
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupClass(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			LG_Debug("stack::put<static1 %s>: mapped", typeid(T).name());
			lgc->pushInstance(s, new T(v), true);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T>: lud!\n");
		
		LG_Debug("stack::put<static1 %s>: lud", typeid(T).name());
		LuaGlueObject<T> *obj = new LuaGlueObject<T>(new T(v), 0, true);
		lua_pushlightuserdata(s, obj);
	}
	
	// for putting static types
	static void put(LuaGlueBase *g, lua_State *s, T *v)
	{
		//printf("stack<T>::put(T*)\n");
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupClass(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			LG_Debug("stack::put<static2 %s>: mapped", typeid(T).name());
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T>: lud!\n");
		LG_Debug("stack::put<static2 %s>: lud", typeid(T).name());
		LuaGlueObject<T> *obj = new LuaGlueObject<T>(new T(*v), 0, true);
		lua_pushlightuserdata(s, obj);
	}
};

template<class T>
struct stack<std::shared_ptr<T>> {
	static std::shared_ptr<T> get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			//printf("stack<shared_ptr<T>>::get: lud!\n");
			LG_Debug("stack::get<shared_ptr<%s>>: lud", typeid(T).name());
			return **(LuaGlueObject<std::shared_ptr<T>> *)lua_touserdata(s, idx);
		}
		
		//printf("stack<shared_ptr<T>>::get: name:%s\n", typeid(T).name());
#ifdef LUAGLUE_TYPECHECK
		LuaGlueClass<T> *lgc = getGlueClass<T>(g, s, idx);
		if(lgc)
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<shared_ptr<%s>>: mapped", typeid(T).name());
			return **(LuaGlueObject<std::shared_ptr<T>> *)lua_touserdata(s, idx);

#ifdef LUAGLUE_TYPECHECK
		}
#endif

		LG_Debug("stack::get<%s>: unk", typeid(T).name());
		//printf("stack::get<shared_ptr<T>>: failed to get a class instance for lua stack value at idx: %i\n", idx);
		return 0; // TODO: is this a valid thing? I can't imagine this is a good thing.
	}
	
	static void put(LuaGlueBase *g, lua_State *s, std::shared_ptr<T> v)
	{
		//printf("stack<T>::put(T)\n");
		
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupClass(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			//printf("stack<shared_ptr<T>>::put: name:%s\n", typeid(T).name());
			LG_Debug("stack::put<shared_ptr<%s>>: mapped", typeid(T).name());
			lgc->pushInstance(s, v);
			return;
		}

		if (!v)
		{
			lua_pushnil(s);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T>: lud!\n");
		LG_Debug("stack::put<shared_ptr<%s>>: lud", typeid(T).name());
		std::shared_ptr<T> *ptr = new std::shared_ptr<T>(v);
		LuaGlueObject<std::shared_ptr<T>> *obj = new LuaGlueObject<std::shared_ptr<T>>(ptr, nullptr, true);
		lua_pushlightuserdata(s, obj);
	}
};

template<class T>
struct stack<LuaGlueObject<T>> {
	static T get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<LuaGlueObject<%s>>: lud", typeid(T).name());
			return *(LuaGlueObject<T> *)lua_touserdata(s, idx);
		}
		
		//printf("stack<shared_ptr<T>>::get: name:%s\n", typeid(T).name());
#ifdef LUAGLUE_TYPECHECK
		LuaGlueClass<T> *lgc = getGlueClass<T>(g, s, idx);
		if(lgc)
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<LuaGlueObject<%s>>: mapped", typeid(T).name());
			return **(LuaGlueObject<T> *)lua_touserdata(s, idx);

#ifdef LUAGLUE_TYPECHECK
		}
#endif

		LG_Debug("stack::get<LuaGlueObject<%s>>: unk", typeid(T).name());
		return T(); // TODO: is this a valid thing? I can't imagine this is a good thing.
	}
	
	static void put(LuaGlueBase *g, lua_State *s, const LuaGlueObject<T> &v)
	{
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupClass(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			LG_Debug("stack::put<LuaGlueObject<%s>>: mapped", typeid(T).name());
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		LG_Debug("stack::put<LuaGlueObject<%s>>: lud", typeid(T).name());
		LuaGlueObject<T> *obj = new LuaGlueObject<T>(v);
		lua_pushlightuserdata(s, obj);
	}
};

template<>
struct stack<int> {
	static int get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkint(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, int v)
	{
		lua_pushinteger(s, v);
	}
};

template<>
struct stack<long int> : public stack<int> {};
template<>
struct stack<unsigned int> : public stack<int> {};
template<>
struct stack<long unsigned int> : public stack<int> {};

template<>
struct stack<float> {
	static float get(LuaGlueBase *, lua_State *s, int idx)
	{
		return (float)luaL_checknumber(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, float v)
	{
		lua_pushnumber(s, v);
	}
};

template<>
struct stack<double> {
	static double get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, double v)
	{
		lua_pushnumber(s, v);
	}
};

template<>
struct stack<bool> {
	static bool get(LuaGlueBase *, lua_State *s, int idx)
	{
		return lua_toboolean(s, idx) != 0;
	}
	
	static void put(LuaGlueBase *, lua_State *s, bool v)
	{
		lua_pushboolean(s, v);
	}
};

template<>
struct stack<const char *> {
	static const char *get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}
};

template<>
struct stack<std::string> {
	static std::string get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, std::string v)
	{
		lua_pushstring(s, v.c_str());
	}
};

template<class T>
struct stack<T *> {
	static T *get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<%s *>: lud", typeid(T).name());
			return (T*)lua_touserdata(s, idx);
		}
		
#ifdef LUAGLUE_TYPECHECK
		typedef typename std::remove_pointer<T>::type TC;
		LuaGlueClass<TC> *lgc = getGlueClass<TC>(g, s, idx);
		if(lgc)
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<%s *>: mapped", typeid(T).name());
			LuaGlueObject<T> obj = *(LuaGlueObject<T> *)lua_touserdata(s, idx);
			return obj.ptr();
#ifdef LUAGLUE_TYPECHECK
		}
#endif
		
		LG_Debug("stack::get<%s *>: unk", typeid(T).name());
		return 0;
	}
	
	static void put(LuaGlueBase *g, lua_State *s, T *v)
	{
		// first look for a class we support

		typedef typename std::remove_pointer<T>::type TC;
		LuaGlueClass<TC> *lgc = (LuaGlueClass<TC> *)g->lookupClass(typeid(LuaGlueClass<TC>).name(), true);
		//printf("stack<T*>::put(T): %s %p lgc:%p\n", typeid(LuaGlueClass<T>).name(), v, lgc);
		if(lgc)
		{
			LG_Debug("stack::put<%s *>: mapped", typeid(T).name());
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T*>: lud!\n");
		LG_Debug("stack::put<%s *>: lud", typeid(T).name());
		lua_pushlightuserdata(s, v);
	}
};

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
template < int N >
struct apply_obj_func
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlueBase *g, lua_State *s, T* pObj,
                          R (T::*f)( ArgsF... ),
                          const std::tuple<ArgsT...> &t,
                          Args... args )
	{
		const static int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_obj_func<N-1>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t), stack<ltype>::get(g, s, -(argCount-N+1)), args... );
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
  static R applyTuple(LuaGlueBase *, lua_State *, T* pObj,
                          R (T::*f)( ArgsF... ),
                          const std::tuple<ArgsT...> &/* t */,
                          Args... args )
	{
		LG_Debug("applyTuple callon: %s", typeid(T).name()); 
		return (pObj->*f)( args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * Object Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename T, typename R, typename... ArgsF, typename... ArgsT >
R applyTuple(LuaGlueBase *g, lua_State *s, T* pObj,
                 R (T::*f)( ArgsF... ),
                 const std::tuple<ArgsT...> &t )
{
	return apply_obj_func<sizeof...(ArgsT)>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t) );
}


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
template < int N >
struct apply_obj_constfunc
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlueBase *g, lua_State *s, T* pObj,
                          R (T::*f)( ArgsF... ) const,
                          const std::tuple<ArgsT...> &t,
                          Args... args )
	{
		const static int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_obj_func<N-1>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t), stack<ltype>::get(g, s, -(argCount-N+1)), args... );
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
struct apply_obj_constfunc<0>
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlueBase *, lua_State *, T* pObj,
                          R (T::*f)( ArgsF... ) const,
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
R applyTuple(LuaGlueBase *g, lua_State *s, T* pObj,
                 R (T::*f)( ArgsF... ) const,
                 const std::tuple<ArgsT...> &t )
{
	return apply_obj_constfunc<sizeof...(ArgsT)>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t) );
}



//-----------------------------------------------------------------------------

/**
 * LuaGlueObject Function Tuple Argument Unpacking
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @tparam N Number of tuple arguments to unroll
 *
 * @ingroup g_util_tuple
 */
template < int N >
struct apply_glueobj_func
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlueBase *g, lua_State *s, LuaGlueObject<T> pObj,
                          R (T::*f)( ArgsF... ),
                          const std::tuple<ArgsT...> &t,
                          Args... args )
	{
		const static int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_glueobj_func<N-1>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t), stack<ltype>::get(g, s, -(argCount-N+1)), args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * LuaGlueObject Function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <>
struct apply_glueobj_func<0>
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlueBase *, lua_State *, LuaGlueObject<T> pObj,
                          R (T::*f)( ArgsF... ),
                          const std::tuple<ArgsT...> &/* t */,
                          Args... args )
	{
		LG_Debug("glueobj call!");
		return (pObj.ptr()->*f)( args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * LuaGlueObject Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename T, typename R, typename... ArgsF, typename... ArgsT >
R applyTuple(LuaGlueBase *g, lua_State *s, LuaGlueObject<T> pObj,
                 R (T::*f)( ArgsF... ),
                 const std::tuple<ArgsT...> &t )
{
	return apply_glueobj_func<sizeof...(ArgsT)>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t) );
}


/**
 * LuaGlueObject<shared_ptr> Function Tuple Argument Unpacking
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @tparam N Number of tuple arguments to unroll
 *
 * @ingroup g_util_tuple
 */
template < int N >
struct apply_glueobj_sptr_func
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlueBase *g, lua_State *s, LuaGlueObject<std::shared_ptr<T>> pObj,
                          R (T::*f)( ArgsF... ),
                          const std::tuple<ArgsT...> &t,
                          Args... args )
	{
		const static int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_glueobj_sptr_func<N-1>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t), stack<ltype>::get(g, s, -(argCount-N+1)), args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * LuaGlueObject<shared_ptr> Function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <>
struct apply_glueobj_sptr_func<0>
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlueBase *, lua_State *, LuaGlueObject<std::shared_ptr<T>> pObj,
                          R (T::*f)( ArgsF... ),
                          const std::tuple<ArgsT...> &/* t */,
                          Args... args )
	{
		LG_Debug("glueobj<shared_ptr> call!");
		return (pObj.ptr()->*f)( args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * LuaGlueObject<shared_ptr> Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename T, typename R, typename... ArgsF, typename... ArgsT >
R applyTuple(LuaGlueBase *g, lua_State *s, LuaGlueObject<std::shared_ptr<T>> pObj,
                 R (T::*f)( ArgsF... ),
                 const std::tuple<ArgsT...> &t )
{
	return apply_glueobj_sptr_func<sizeof...(ArgsT)>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t) );
}


//-----------------------------------------------------------------------------

/**
 * LuaGlueObject<shared_ptr> Function Tuple Argument Unpacking
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @tparam N Number of tuple arguments to unroll
 *
 * @ingroup g_util_tuple
 */
template < int N >
struct apply_glueobj_sptr_constfunc
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlueBase *g, lua_State *s, LuaGlueObject<std::shared_ptr<T>> pObj,
                          R (T::*f)( ArgsF... ) const,
                          const std::tuple<ArgsT...> &t,
                          Args... args )
	{
		const static int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_glueobj_sptr_constfunc<N-1>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t), stack<ltype>::get(g, s, -(argCount-N+1)), args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * LuaGlueObject<shared_ptr> Function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <>
struct apply_glueobj_sptr_constfunc<0>
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlueBase *, lua_State *, LuaGlueObject<std::shared_ptr<T>> pObj,
                          R (T::*f)( ArgsF... ) const,
                          const std::tuple<ArgsT...> &/* t */,
                          Args... args )
	{
		LG_Debug("glueobj<shared_ptr> call!");
		return (pObj.ptr()->*f)( args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * LuaGlueObject<shared_ptr> Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename T, typename R, typename... ArgsF, typename... ArgsT >
R applyTuple(LuaGlueBase *g, lua_State *s, LuaGlueObject<std::shared_ptr<T>> pObj,
                 R (T::*f)( ArgsF... ) const,
                 const std::tuple<ArgsT...> &t )
{
	return apply_glueobj_sptr_constfunc<sizeof...(ArgsT)>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t) );
}


//-----------------------------------------------------------------------------


/**
 * LuaGlueObject Const Function Tuple Argument Unpacking
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @tparam N Number of tuple arguments to unroll
 *
 * @ingroup g_util_tuple
 */
template < int N >
struct apply_glueobj_constfunc
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlueBase *g, lua_State *s, LuaGlueObject<T> pObj,
                          R (T::*f)( ArgsF... ) const,
                          const std::tuple<ArgsT...> &t,
                          Args... args )
	{
		const static int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_glueobj_constfunc<N-1>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t), stack<ltype>::get(g, s, -(argCount-N+1)), args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * LuaGlueObject Function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <>
struct apply_glueobj_constfunc<0>
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlueBase *, lua_State *, LuaGlueObject<T> pObj,
                          R (T::*f)( ArgsF... ) const,
                          const std::tuple<ArgsT...> &/* t */,
                          Args... args )
	{
		return (pObj.ptr()->*f)( args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * LuaGlueObject Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename T, typename R, typename... ArgsF, typename... ArgsT >
R applyTuple(LuaGlueBase *g, lua_State *s, LuaGlueObject<T> pObj,
                 R (T::*f)( ArgsF... ) const,
                 const std::tuple<ArgsT...> &t )
{
	return apply_glueobj_constfunc<sizeof...(ArgsT)>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t) );
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
template < int N >
struct apply_func
{
	template < typename R, typename... ArgsF, typename... ArgsT, typename... Args >
	static R applyTuple(	LuaGlueBase *g, lua_State *s, R (*f)( ArgsF... ),
									const std::tuple<ArgsT...>& t,
									Args... args )
	{
		const static int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_func<N-1>::applyTuple( g, s, f, std::forward<decltype(t)>(t), stack<ltype>::get(g, s, -(argCount-N+1)), args... );
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
	static R applyTuple(	LuaGlueBase *, lua_State *, R (*f)( ArgsF... ),
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
R applyTuple( LuaGlueBase *g, lua_State *s, R (*f)(ArgsF...),
                 const std::tuple<ArgsT...> & t )
{
	return apply_func<sizeof...(ArgsT)>::applyTuple( g, s, f, std::forward<decltype(t)>(t) );
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
template <class C, int N >
struct apply_ctor_func
{
	template < typename... ArgsT, typename... Args >
	static C *applyTuple(	LuaGlueBase *g, lua_State *s, const std::tuple<ArgsT...>& t,
								Args... args )
	{
		const static int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_ctor_func<C, N-1>::applyTuple( g, s, std::forward<decltype(t)>(t), stack<ltype>::get(g, s, -(argCount-N+1)), args... );
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
	static C *applyTuple(	LuaGlueBase *, lua_State *, const std::tuple<ArgsT...>& /* t */,
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
C *applyTuple( LuaGlueBase *g, lua_State *s, const std::tuple<ArgsT...> & t )
{
	return apply_ctor_func<C, sizeof...(ArgsT)>::applyTuple( g, s, std::forward<decltype(t)>(t) );
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
template < int N >
struct apply_lua_func
{
	template < typename... ArgsT, typename... Args >
	static void applyTuple(	LuaGlueBase *g, lua_State *s, const std::tuple<ArgsT...>& t,
								Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		const static unsigned int argIdx = (argCount-N);
		
		typedef typename std::remove_reference<decltype(std::get<argIdx>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		stack<ltype>::put(g, s, std::get<argIdx>(t));
		
		apply_lua_func<N-1>::applyTuple( g, s, std::forward<decltype(t)>(t), std::get<argIdx>(t), args... );
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
	static void applyTuple(	LuaGlueBase *, lua_State *, const std::tuple<ArgsT...>& /* t */,
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
void applyTuple( LuaGlueBase *g, lua_State *s, Args... args )
{
	std::tuple<Args...> t(args...);
	apply_lua_func<sizeof...(Args)>::applyTuple( g, s, std::forward<decltype(t)>(t) );
}


#endif /* LUAGLUE_APPLYTUPLE_H_GUARD */
