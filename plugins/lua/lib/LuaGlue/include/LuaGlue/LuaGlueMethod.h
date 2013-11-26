#ifndef LUAGLUE_METHOD_H_GUARD
#define LUAGLUE_METHOD_H_GUARD

#include <lua.hpp>
#include <string>
#include <tuple>
#include <utility>

#include "LuaGlue/LuaGlueMethodBase.h"
#include "LuaGlue/LuaGlueApplyTuple.h"
#include "LuaGlue/LuaGlueUtils.h"

class LuaGlue;

template<typename _Class>
class LuaGlueClass;

template<typename _Ret, typename _Class, typename... _Args>
class LuaGlueMethod : public LuaGlueMethodBase
{
	private:
		template <typename... T>
		using tuple = std::tuple<typename std::remove_const<typename std::remove_reference<T>::type>::type...>;
	
	public:
		typedef _Class ClassType;
		typedef _Ret ReturnType;
		typedef _Ret (_Class::*MethodType)( _Args... );
		
		LuaGlueMethod(LuaGlueClass<_Class> *luaClass, const std::string &name, MethodType &&fn) : glueClass(luaClass), name_(name), fn(std::forward<decltype(fn)>(fn))
		{ }
		
		~LuaGlueMethod() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlue *luaGlue)
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
		tuple<_Args...> args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
		
	public:
		int invoke(lua_State *state)
		{
			//printf("invoker: %s::%s\n", typeid(*glueClass).name(), name_.c_str());
#ifdef LUAGLUE_TYPECHECK
			ClassType *obj = *(ClassType **)luaL_checkudata(state, 1, glueClass->name().c_str());
#else
			ClassType *obj = *(ClassType **)lua_touserdata(state, 1);
#endif
			ReturnType ret = applyTuple(glueClass->luaGlue(), state, (_Class *)obj, fn, args);
			if(Arg_Count_) lua_pop(state, (int)Arg_Count_);
			
			returnValue(glueClass->luaGlue(), state, ret);
			return 1;
		}
		
	private:
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueMethod<_Ret, _Class, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

template<typename _Class, typename... _Args>
class LuaGlueMethod<void, _Class, _Args...> : public LuaGlueMethodBase
{
	private:
		template <typename... T>
		using tuple = std::tuple<typename std::remove_const<typename std::remove_reference<T>::type>::type...>;

	public:
		typedef _Class ClassType;
		typedef void (_Class::*MethodType)(_Args...);
		
		LuaGlueMethod(LuaGlueClass<_Class> *luaClass, const std::string &name, MethodType &&fn) : glueClass(luaClass), name_(name), fn(std::forward<decltype(fn)>(fn))
		{ }
		
		~LuaGlueMethod() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlue *luaGlue)
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
		tuple<_Args...> args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
	
	public:
		int invoke(lua_State *state)
		{
			//printf("invokev: %s::%s\n", typeid(*glueClass).name(), name_.c_str());
#ifdef LUAGLUE_TYPECHECK
			ClassType *obj = *(ClassType **)luaL_checkudata(state, 1, glueClass->name().c_str());
#else
			ClassType *obj = *(ClassType **)lua_touserdata(state, 1);
#endif
			//printf("obj: %p\n", obj);
			applyTuple(glueClass->luaGlue(), state, (_Class *)obj, fn, args);
			if(Arg_Count_) lua_pop(state, (int)Arg_Count_);
			return 0;
		}
		
	private:
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueMethod<void, _Class, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

#endif /* LUAGLUE_METHOD_H_GUARD */
