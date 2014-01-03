#ifndef LUAGLUE_FUNCTION_H_GUARD
#define LUAGLUE_FUNCTION_H_GUARD

#include <lua.hpp>
#include <string>
#include <tuple>
#include <utility>

#include "LuaGlue/LuaGlueObject.h"
#include "LuaGlue/LuaGlueApplyTuple.h"
#include "LuaGlue/LuaGlueFunctionBase.h"
#include "LuaGlue/LuaGlueBase.h"

template<typename _Ret, typename... _Args>
class LuaGlueFunction : public LuaGlueFunctionBase
{
	private:
		typedef std::tuple<typename std::remove_const<typename std::remove_reference<_Args>::type>::type...> ArgsTuple;

	public:
		typedef _Ret ReturnType;
		typedef _Ret (*MethodType)( _Args... );
		
		LuaGlueFunction(LuaGlueBase *lg, const std::string &n, MethodType &&fn) :
			g(lg), name_(n), fn_(std::forward<decltype(fn)>(fn))
		{ }
		
		~LuaGlueFunction() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			//printf("add function: %s\n", name_.c_str());
			lua_setglobal(luaGlue->state(), name_.c_str());
			return true;
		}
		
	private:
		LuaGlueBase *g;
		std::string name_;
		MethodType fn_;
		ArgsTuple args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
		
		int invoke(lua_State *state)
		{
			ReturnType ret = applyTuple(g, state, fn_, args);
			lua_pop(state, (int)Arg_Count_);
			stack<_Ret>::put(g, state, ret);
			return 1;
		}
		
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueFunction<_Ret, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

template<typename... _Args>
class LuaGlueFunction<void, _Args...> : public LuaGlueFunctionBase
{
	private:
		typedef std::tuple<typename std::remove_const<typename std::remove_reference<_Args>::type>::type...> ArgsTuple;

	public:
		typedef void ReturnType;
		typedef void (*MethodType)( _Args... );
		
		LuaGlueFunction(LuaGlueBase *lg, const std::string &n, MethodType &&fn) :
			g(lg), name_(n), fn_(std::forward<decltype(fn)>(fn))
		{ }
		
		~LuaGlueFunction() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			//printf("add vfunction: %s\n", name_.c_str());
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			lua_setglobal(luaGlue->state(), name_.c_str());
			return true;
		}
		
	private:
		LuaGlueBase *g;
		std::string name_;
		MethodType fn_;
		ArgsTuple args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
		
		int invoke(lua_State *state)
		{
			applyTuple(g, state, fn_, args);
			if(Arg_Count_) lua_pop(state, (int)Arg_Count_);
			return 0;
		}
		
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueFunction<void, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

#endif /* LUAGLUE_FUNCTION_H_GUARD */
