#ifndef LUAGLUE_PROPERTY_H_GUARD
#define LUAGLUE_PROPERTY_H_GUARD

#include <lua.hpp>
#include <string>

#include "LuaGlue/LuaGlueObject.h"
#include "LuaGlue/LuaGlueApplyTuple.h"
#include "LuaGlue/LuaGluePropertyBase.h"
#include "LuaGlue/LuaGlueBase.h"

template<typename _Type, typename _Class>
class LuaGlueDirectProperty : public LuaGluePropertyBase
{
	public:
		typedef _Type _Class::*PropType;
		
		LuaGlueDirectProperty(LuaGlueClass<_Class> *luaClass, const std::string &name, PropType prop) : name_(name), prop_(prop), glueClass(luaClass)
		{
			
		}
		
		~LuaGlueDirectProperty() { }
		
		std::string name() { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_access, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
	private:
		std::string name_;
		_Type _Class::*prop_;
		LuaGlueClass<_Class> *glueClass;
		
		int access(lua_State *state)
		{
			//printf("access: %s::%s\n", glueClass->name().c_str(), name_.c_str());
			return accessImpl(state, std::is_class<typename std::remove_pointer<_Type>::type>());
		}
		
		void getReturnVal(lua_State *state, _Class *obj, std::true_type)
		{
			//printf("getReturnVal:pointer\n");
			_Type val = (obj->*prop_);
			stack<_Type>::put(glueClass->luaGlue(), state, val);
		}
		
		void getReturnVal(lua_State *state, _Class *obj, std::false_type)
		{
			//printf("getReturnVal:!pointer\n");
			_Type *val = &(obj->*prop_);
			stack<_Type>::put(glueClass->luaGlue(), state, val);
		}
		
		void getReturnVal(lua_State *state, _Class *obj)
		{
			//printf("getProp: %s::%s\n", glueClass->name().c_str(), name_.c_str());
			getReturnVal(state, obj, std::is_pointer<_Type>());
		}
		
		void setProp(lua_State *state, _Class *obj, std::true_type)
		{
			(obj->*prop_) = stack<_Type>::get(glueClass->luaGlue(), state, 2);
		}
		
		void setProp(lua_State *state, _Class *obj, std::false_type)
		{
			(obj->*prop_) = *stack<_Type *>::get(glueClass->luaGlue(), state, 2);
		}
		
		void setProp(lua_State *state, _Class *obj)
		{
			//printf("setProp: %s::%s\n", glueClass->name().c_str(), name_.c_str());
			setProp(state, obj, std::is_pointer<_Type>());
		}
		
		int accessImpl(lua_State *state, std::true_type)
		{
			//printf("accessImpl: class\n");
			int nargs = lua_gettop(state);

			_Class *ptr = nullptr;
			auto base = GetLuaUdata(state, 1, glueClass->name().c_str());
			if(base->isSharedPtr())
			{
				auto obj = *CastLuaGlueObjectShared(_Class, base);
				ptr = obj.ptr();
			}
			else
			{
				auto obj = *CastLuaGlueObject(_Class, base);
				ptr = obj.ptr();
			}
			
			if(nargs == 2)
			{
				// get
				getReturnVal(state, ptr);
				return 1;
			}
			else if(nargs == 3)
			{
				// set
				setProp(state, ptr);
				return 0;
			}
			
			return 0;
		}
		
		int accessImpl(lua_State *state, std::false_type)
		{
			int nargs = lua_gettop(state);
			
			_Class *ptr = nullptr;
			auto base = GetLuaUdata(state, 1, glueClass->name().c_str());
			if(base->isSharedPtr())
			{
				auto obj = *CastLuaGlueObjectShared(_Class, base);
				ptr = obj.ptr();
			}
			else
			{
				auto obj = *CastLuaGlueObject(_Class, base);
				ptr = obj.ptr();
			}
			
			//printf("accessImpl: %p pod nargs:%i '%s'\n", obj, nargs, lua_tostring(state, -1));
			
			if(nargs == 2)
			{
				// get
				//printf("type: %s\n", typeid(decltype((obj->*prop_))).name());
				_Type val = (ptr->*prop_);
				stack<_Type>::put(glueClass->luaGlue(), state, val);
				return 1;
			}
			else if(nargs == 3)
			{
				// set
				_Type val = stack<_Type>::get(glueClass->luaGlue(), state, 3);
				(ptr->*prop_) = val;
				//printf("set prop to %d\n", (obj->*prop_));
				return 0;
			}
			
			return 0;
		}
		
		static int lua_access(lua_State *state)
		{
			auto pimp = (LuaGlueDirectProperty<_Type, _Class> *)lua_touserdata(state, lua_upvalueindex(1));
			return pimp->access(state);
		}
};

template<typename _Type, typename _Class>
class LuaGlueProperty : public LuaGluePropertyBase
{
	public:
		typedef _Type (_Class::*GetterType)();
		typedef void (_Class::*SetterType)(_Type);
		
		LuaGlueProperty(LuaGlueClass<_Class> *luaClass, const std::string &name, GetterType getter, SetterType setter) : name_(name), getter(getter), setter(setter), glueClass(luaClass)
		{
			
		}
		
		~LuaGlueProperty() { }
		
		std::string name() { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_access, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
	private:
		std::string name_;
		GetterType getter;
		SetterType setter;
		LuaGlueClass<_Class> *glueClass;
		
		int access(lua_State *state)
		{
			int nargs = lua_gettop(state);

			_Class *ptr = nullptr;
			auto base = GetLuaUdata(state, 1, glueClass->name().c_str());
			if(base->isSharedPtr())
			{
				auto obj = *CastLuaGlueObjectShared(_Class, base);
				ptr = obj.ptr();
			}
			else
			{
				auto obj = *CastLuaGlueObject(_Class, base);
				ptr = obj.ptr();
			}
			
			if(nargs == 2)
			{
				// get
				_Type ret = (ptr->*getter)();
				stack<_Type>::put(glueClass->luaGlue(), state, ret);
				return 1;
			}
			else if(nargs == 3)
			{
				// set
				_Type arg = stack<_Type>::get(glueClass->luaGlue(), state, -1);
				(ptr->*setter)(arg);
				return 0;
			}
			
			return 0;
		}
		
		static int lua_access(lua_State *state)
		{
			auto pimp = (LuaGlueProperty<_Type, _Class> *)lua_touserdata(state, lua_upvalueindex(1));
			return pimp->access(state);
		}
};

#endif /* LUAGLUE_PROPERTY_H_GUARD */
