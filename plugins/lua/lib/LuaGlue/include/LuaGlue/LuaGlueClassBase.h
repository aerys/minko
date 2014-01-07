#ifndef LUAGLUE_CLASS_BASE_H
#define LUAGLUE_CLASS_BASE_H

class LuaGlueBase;

class LuaGlueClassBase
{
	public:
		virtual bool glue(LuaGlueBase *) = 0;
		virtual ~LuaGlueClassBase() { }
};

#endif /* LUAGLUE_CLASS_BASE_H */
