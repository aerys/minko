#ifndef LUAGLUE_FUNCTION_REF_H_GUARD
#define LUAGLUE_FUNCTION_REF_H_GUARD

#include <lua.hpp>

#include "LuaGlue/LuaGlueApplyTuple.h"

class LuaGlueFunctionRef
{
public:
	typedef std::shared_ptr<LuaGlueFunctionRef> Ptr;

private:	
	int 			_ref;
	LuaGlueBase* 	_state;

private:
	LuaGlueFunctionRef(LuaGlueBase* s, int ref) :
		_state(s),
		_ref(ref)
	{}

public:
	static inline
	Ptr
	create(LuaGlueBase* s, int ref)
	{
		return std::shared_ptr<LuaGlueFunctionRef>(new LuaGlueFunctionRef(s, ref));
	}

	inline
	int
	ref()
	{
		return _ref;
	}

	template<typename _Ret, typename... _Args>
	_Ret invoke(_Args... args)
	{
		const unsigned int Arg_Count_ = sizeof...(_Args);
		
		lua_rawgeti(_state->state(), LUA_REGISTRYINDEX, _ref);
		applyTuple(_state, _state->state(), args...);
		lua_call(_state->state(), Arg_Count_, 1);

		auto result = stack<_Ret>::get(_state, _state->state(), -1);

		lua_pop(_state->state(), 1);

		return result;
	}

	template<typename... _Args>
	void invokeVoid(_Args... args)
	{
		const unsigned int Arg_Count_ = sizeof...(_Args);
		
		lua_rawgeti(_state->state(), LUA_REGISTRYINDEX, _ref);
		applyTuple(_state, _state->state(), args...);
		lua_call(_state->state(), Arg_Count_, 0);
	}

	~LuaGlueFunctionRef()
	{
		luaL_unref(_state->state(), LUA_REGISTRYINDEX, _ref);
	}
};

template<>
struct stack<LuaGlueFunctionRef::Ptr> {
	static LuaGlueFunctionRef::Ptr get(LuaGlueBase *g, lua_State *s, int idx)
	{
		auto ptr = LuaGlueFunctionRef::create(g, luaL_ref(s, LUA_REGISTRYINDEX));
		lua_rawgeti(s, LUA_REGISTRYINDEX, ptr->ref());

		return ptr;
	}
	
	static void put(LuaGlueBase *, lua_State *s, LuaGlueFunctionRef::Ptr v)
	{
		lua_rawgeti(s, LUA_REGISTRYINDEX, v->ref());
	}
};

#endif /* LUAGLUE_FUNCTION_REF_H_GUARD */
