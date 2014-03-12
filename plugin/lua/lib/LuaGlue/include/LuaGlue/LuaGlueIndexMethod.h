#ifndef LUAGLUE_INDEX_METHOD_H_GUARD
#define LUAGLUE_INDEX_METHOD_H_GUARD

#include <lua.hpp>
#include <string>
#include <tuple>
#include <utility>

#include "LuaGlue/LuaGlueObject.h"
#include "LuaGlue/LuaGlueApplyTuple.h"

#include "LuaGlue/LuaGlueBase.h"

template<typename _Class>
class LuaGlueClass;

template<typename _Value, typename _Class, typename _Key>
class LuaGlueIndexMethod : public LuaGlueMethodBase
{
	public:
		typedef _Class ClassType;
		typedef typename std::remove_reference<_Value>::type ValueType;
		typedef _Value (_Class::*MethodType)(_Key);
		
		LuaGlueIndexMethod(LuaGlueClass<_Class> *luaClass, const std::string &name, MethodType &&fn) : glueClass(luaClass), name_(name), fn(std::forward<MethodType>(fn))
		{ }
		
		~LuaGlueIndexMethod() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
		int invoke(lua_State *state)
		{
			ValueType ret;
			
			auto base = GetLuaUdata(state, 1, glueClass->name().c_str());
			if(base->isSharedPtr())
			{
				auto obj = *CastLuaGlueObjectShared(ClassType, base);
				lua_remove(state, 1); // hopefully remove table...
				ret = applyTuple(glueClass->luaGlue(), state, obj, fn, args);
			}
			else
			{
				auto obj = *CastLuaGlueObject(ClassType, base);
				lua_remove(state, 1); // hopefully remove table...
				ret = applyTuple(glueClass->luaGlue(), state, obj, fn, args);
			}
			
			lua_pop(state, 2);
			
			stack<ValueType>::put(glueClass->luaGlue(), state, ret);
			
			return 1;
		}
		
	private:
		LuaGlueClass<_Class> *glueClass;
		std::string name_;
		MethodType fn;
		std::tuple<_Key> args;
		
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueIndexMethod<_Value, _Class, _Value> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

#endif /* LUAGLUE_INDEX_METHOD_H_GUARD */
