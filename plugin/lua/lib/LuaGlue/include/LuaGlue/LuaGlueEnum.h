#ifndef LUAGLUE_ENUM_H_GUARD
#define LUAGLUE_ENUM_H_GUARD

#include <vector>
#include <string>
#include <map>
#include <new>

#include "LuaGlue/LuaGlueObject.h"
#include "LuaGlue/LuaGlueClassBase.h"
#include "LuaGlue/LuaGlueConstant.h"
#include "LuaGlue/LuaGlueMethodBase.h"
#include "LuaGlue/LuaGluePropertyBase.h"
#include "LuaGlue/LuaGlueSymTab.h"

#include "LuaGlue/LuaGlueUtils.h"
#include "LuaGlue/LuaGlueDebug.h"

#include "LuaGlue/LuaGlueBase.h"

#include "LuaGlueProperty.h"

template<class _Class>
class LuaGlueObject;

class LuaGlue;

template<typename _Class>
class LuaGlueEnum : public LuaGlueClassBase
{
	public:
		typedef _Class ClassType;
		
		LuaGlueEnum(LuaGlueBase *luaGlue, const std::string &name) : luaGlue_(luaGlue), name_(name)
		{ }
		
		~LuaGlueEnum()
		{ 	}
		
		const std::string &name() { return name_; }
				
		template<typename T>
		LuaGlueEnum<_Class> &constant(const std::string &name, T v)
		{
			//printf("constant(%s)\n", name.c_str());
			auto impl = new LuaGlueConstant(name, v);
			constants_.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		LuaGlueEnum<_Class> &constants(const std::vector<LuaGlueConstant> &c)
		{
			for(unsigned int i = 0; i < c.size(); i++)
			{
				//printf("constant(%s)\n", c[i].name().c_str());
				auto impl = new LuaGlueConstant(c[i]);
				constants_.addSymbol(impl->name().c_str(), impl);
			}
			
			return *this;
		}
		
		LuaGlue &end() { return *(LuaGlue*)luaGlue_; }
		LuaGlueBase *luaGlue() { return luaGlue_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_createtable(luaGlue->state(), 0, 0);
			//int lib_id = lua_gettop(luaGlue->state());
						
			for(auto &constant: constants_)
			{
				//printf("Glue constant: %s::%s\n", name_.c_str(), constant.first.c_str());
				if(!constant.ptr->glue(luaGlue))
					return false;
			}
						
			lua_pushvalue(luaGlue->state(), -1);
			lua_setglobal(luaGlue->state(), name_.c_str());
			
			lua_pop(luaGlue->state(), 1);

			//printf("done.\n");
			return true;
		}
				
	private:
		LuaGlueBase *luaGlue_;
		std::string name_;
		
		LuaGlueSymTab<LuaGlueConstant *> constants_;
};

#endif /* LUAGLUE_ENUM_H_GUARD */
