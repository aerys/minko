#ifndef LUAGLUE_BASE_H_GUARD
#define LUAGLUE_BASE_H_GUARD

struct lua_State;
class LuaGlueClassBase;
template<typename T> class LuaGlueSymTab;

class LuaGlueBase
{
	public:
		virtual ~LuaGlueBase() {}
		virtual lua_State *state() = 0;
		virtual bool glue() = 0;
		virtual LuaGlueClassBase *lookupClass(const char *name, bool internal_name = false) = 0;
		virtual LuaGlueClassBase *lookupClass(uint32_t idx) = 0;
		virtual LuaGlueSymTab<LuaGlueClassBase *> &getSymTab() = 0;
};

#endif /* LUAGLUE_BASE_H_GUARD */
