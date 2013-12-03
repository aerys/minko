#ifndef LUAGLUE_METHOD_BASE_H_GUARD
#define LUAGLUE_METHOD_BASE_H_GUARD

class LuaGlueBase;

class LuaGlueMethodBase
{
	public:
		LuaGlueMethodBase() { }
		virtual ~LuaGlueMethodBase() { }
		virtual bool glue(LuaGlueBase *luaGlue) = 0;
};

#endif /* LUAGLUE_METHOD_BASE_H_GUARD */
