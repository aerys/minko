#ifndef LUAGLUE_STATIC_METHOD_H_GUARD
#define LUAGLUE_STATIC_METHOD_H_GUARD

#include <lua.hpp>
#include <string>
#include <tuple>
#include <utility>

#include "LuaGlue/LuaGlueObject.h"
#include "LuaGlue/LuaGlueApplyTuple.h"
#include "LuaGlue/LuaGlueBase.h"

template<typename _Class>
class LuaGlueClass;

template<typename _Ret, typename _Class, typename... _Args>
class LuaGlueStaticMethod : public LuaGlueMethodBase
{
	typedef std::tuple<typename std::remove_const<typename std::remove_reference<_Args>::type>::type...> ArgsTuple;

	public:
		typedef _Class ClassType;
		typedef _Ret ReturnType;
		typedef _Ret (*MethodType)( _Args... );
		
		LuaGlueStaticMethod(LuaGlueClass<_Class> *luaClass, const std::string &name, MethodType &&fn) :
			glueClass(luaClass), name_(name), fn(std::forward<decltype(fn)>(fn))
		{ }
		
		~LuaGlueStaticMethod() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
	private:
		LuaGlueClass<_Class> *glueClass;
		std::string name_;
		MethodType fn;
		ArgsTuple args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
		
		int invoke(lua_State *state)
		{
			ReturnType ret = applyTuple(glueClass->luaGlue(), state, fn, args);
			if(Arg_Count_) lua_pop(state, (int)Arg_Count_);
			stack<_Ret>::put(glueClass->luaGlue(), state, ret);
			return 1;
		}
		
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueStaticMethod<_Ret, _Class, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

template<typename _Class, typename... _Args>
class LuaGlueStaticMethod<void, _Class, _Args...> : public LuaGlueMethodBase
{
	private:
		typedef std::tuple<typename std::remove_const<typename std::remove_reference<_Args>::type>::type...> ArgsTuple;

	public:
		typedef _Class ClassType;
		typedef void ReturnType;
		typedef void (*MethodType)( _Args... );
		
		LuaGlueStaticMethod(LuaGlueClass<_Class> *luaClass, const std::string &name, MethodType &&fn) :
			glueClass(luaClass), name_(name), fn(std::forward<decltype(fn)>(fn))
		{ }
		
		~LuaGlueStaticMethod() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
	private:
		LuaGlueClass<_Class> *glueClass;
		std::string name_;
		MethodType fn;
		ArgsTuple args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
		
		int invoke(lua_State *state)
		{
			applyTuple(glueClass->luaGlue(), state, fn, args);
			if(Arg_Count_) lua_pop(state, (int)Arg_Count_);
			return 0;
		}
		
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueStaticMethod<void, _Class, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

#endif /* LUAGLUE_STATIC_METHOD_H_GUARD */
