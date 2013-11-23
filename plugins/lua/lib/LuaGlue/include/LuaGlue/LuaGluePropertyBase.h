#ifndef LUAGLUE_PROPERTY_BASE_H_GUARD
#define LUAGLUE_PROPERTY_BASE_H_GUARD

class LuaGlue;

class LuaGluePropertyBase
{
	public:
		LuaGluePropertyBase() { }
		virtual ~LuaGluePropertyBase() { }
		virtual bool glue(LuaGlue *luaGlue) = 0;
};

#endif /* LUAGLUE_PROPERTY_BASE_H_GUARD */