#ifndef LUAGLUE_DEBUG_H_GUARD
#define LUAGLUE_DEBUG_H_GUARD

#ifdef LUAGLUE_DEBUG

#include <cstring>
static inline void LG_Debug_(const char *FILE, const char *FUNCTION, int LINE, const char *t, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	int msg_len = vsnprintf(0, 0, format, ap);
	va_end(ap);
	
	char *msg = new char[msg_len+1];
	if(!msg)
		return;
	
	msg[msg_len] = 0;
	
	va_start(ap, format);
	vsnprintf(msg, (size_t)msg_len+1, format, ap);
	va_end(ap);
	
	const char *fptr = strrchr(FILE, '/');
	if(!fptr)
		fptr = strrchr(FILE, '\\');
	
	if(!fptr)
		fptr = FILE;
	else
		fptr++;
	
	printf("%s:%s:%i: %s: %s\n", fptr, FUNCTION, LINE, t, msg);
	
	delete[] msg;
}

#define LG_Debug(a, b...) LG_Debug_(__FILE__, __PRETTY_FUNCTION__, __LINE__, "D", a, ##b)
#define LG_Warn(a, b...) LG_Debug_(__FILE__, __PRETTY_FUNCTION__, __LINE__, "W", a, ##b)
#define LG_Error(a, b...) LG_Debug_(__FILE__, __PRETTY_FUNCTION__, __LINE__, "E", a, ##b)

#else

#define LG_Debug(...) {}
#define LG_Warn(...) {}
#define LG_Error(...) {}

#endif /* !defined DEBUG */

#endif /* LUAGLUE_DEBUG_H_GUARD */