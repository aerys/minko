#ifndef LUAGLUE_PROPERTY_BASE_H_GUARD
#define LUAGLUE_PROPERTY_BASE_H_GUARD

class LuaGlueBase;

class LuaGluePropertyBase
{
	public:
		LuaGluePropertyBase() { }
		virtual ~LuaGluePropertyBase() { }
		virtual bool glue(LuaGlueBase *luaGlue) = 0;
};

#endif /* LUAGLUE_PROPERTY_BASE_H_GUARD */