#ifndef LUAGLUE_METHOD_BASE_H_GUARD
#define LUAGLUE_METHOD_BASE_H_GUARD

class LuaGlue;

class LuaGlueMethodBase
{
	public:
		LuaGlueMethodBase() { }
		virtual ~LuaGlueMethodBase() { }
		virtual bool glue(LuaGlue *luaGlue) = 0;
};

#endif /* LUAGLUE_METHOD_BASE_H_GUARD */
