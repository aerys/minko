#ifndef LUAGLUE_H_GUARD
#define LUAGLUE_H_GUARD

#include <lua.hpp>
#include <string>
#include <map>
#include <typeinfo>
#include <memory>

#include "LuaGlue/LuaGlueBase.h"
#include "LuaGlue/LuaGlueClassBase.h"
#include "LuaGlue/LuaGlueFunction.h"
#include "LuaGlue/LuaGlueFunctionRef.h"
#include "LuaGlue/LuaGlueSymTab.h"
#include "LuaGlue/LuaGlueEnum.h"

template<typename _Class>
class LuaGlueClass;

class LuaGlue : public LuaGlueBase
{
	public:
		
		LuaGlue(lua_State *s = 0) : state_(s) { }
		~LuaGlue() { if(state_) lua_close(state_); }
		
		LuaGlue &open(lua_State *s) { state_ = s; return *this; }
		LuaGlue &open()
		{
			state_ = luaL_newstate();
			luaL_openlibs(state_);
			return *this;
		}
		
		template<typename _Class>
		LuaGlueClass<_Class> &Class(const std::string &name)
		{
			//printf("glue.Class(\"%s\")\n", name.c_str());
			auto new_class = new LuaGlueClass<_Class>(this, name);
			classes.addSymbol(name.c_str(), new_class);
			return *new_class;
		}

		template<typename _Class>
		LuaGlueEnum<_Class> &Enum(const std::string &name)
		{
			//printf("glue.Class(\"%s\")\n", name.c_str());
			auto new_class = new LuaGlueEnum<_Class>(this, name);
			classes.addSymbol(name.c_str(), new_class);
			return *new_class;
		}
		
		template<typename _Ret, typename... _Args>
		LuaGlue &func(const std::string &name, _Ret (*fn)(_Args...))
		{
			auto new_func = new LuaGlueFunction<_Ret, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			functions.addSymbol(name.c_str(), new_func);
			return *this;
		}
		
		template<typename _Ret, typename... _Args>
		_Ret invokeFunction(const std::string &name, _Args... args)
		{
			const unsigned int Arg_Count_ = sizeof...(_Args);
			
			lua_getglobal(state_, name.c_str());
			applyTuple(this, state_, args...);
			lua_call(state_, Arg_Count_, 1);
			return stack<_Ret>::get(this, state_, -1);
		}
		
		template<typename... _Args>
		void invokeVoidFunction(const std::string &name, _Args... args)
		{
			const unsigned int Arg_Count_ = sizeof...(_Args);
			
			lua_getglobal(state_, name.c_str());
			applyTuple(this, state_, args...);
			lua_call(state_, Arg_Count_, 0);
		}
		
		lua_State *state() { return state_; }
		
		bool glue()
		{
			//printf("LuaGlue.glue()\n");
			for(auto &c: classes)
			{
				if(!c.ptr->glue(this))
					return false;
			}
			
			for(auto &c: functions)
			{
				if(!c.ptr->glue(this))
					return false;
			}
			
			return true;
		}
		
		bool doFile(const std::string &path)
		{
			bool success = !luaL_dofile(state_, path.c_str());
			if(!success)
			{
				const char *err = luaL_checkstring(state_, -1);
				last_error = std::string(err);
			}
			else
			{
				last_error = std::string("success");
			}
			
			return success;
		}
		
		bool doString(const std::string &script)
		{
			bool success = !luaL_dostring(state_, script.c_str());
			if(!success)
			{
				const char *err = luaL_checkstring(state_, -1);
				last_error = std::string(err);
			}
			else
			{
				last_error = std::string("success");
			}
			
			return success;
		}
		
		LuaGlueClassBase *lookupClass(const char *name, bool internal_name = false)
		{
			return classes.lookup(name, internal_name);
		}
		
		//LuaGlueClassBase *lookupClass(const std::string &name);
		LuaGlueClassBase *lookupClass(uint32_t idx)
		{
			return classes.lookup(idx);
		}
		
		LuaGlueSymTab<LuaGlueClassBase *> &getSymTab() { return classes; }
		
		const std::string &lastError() { return last_error; }
	private:
		lua_State *state_;
		LuaGlueSymTab<LuaGlueClassBase *> classes;
		LuaGlueSymTab<LuaGlueFunctionBase *> functions;
		
		std::string last_error;
};

#include "LuaGlue/LuaGlueClass.h"
#include "LuaGlue/LuaGlueConstant.h"
#include "LuaGlue/LuaGlueCtorMethod.h"
#include "LuaGlue/LuaGlueDtorMethod.h"
#include "LuaGlue/LuaGlueMethod.h"
#include "LuaGlue/LuaGlueFunction.h"
#include "LuaGlue/LuaGlueStaticMethod.h"
#include "LuaGlue/LuaGlueIndexMethod.h"
#include "LuaGlue/LuaGlueNewIndexMethod.h"
#include "LuaGlue/LuaGlueProperty.h"

#endif /* LUAGLUE_H_GUARD */
