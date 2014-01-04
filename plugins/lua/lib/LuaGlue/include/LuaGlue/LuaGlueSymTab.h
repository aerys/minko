#ifndef LUAGLUE_SYMTAB_H_GUARD
#define LUAGLUE_SYMTAB_H_GUARD

#include <cstring>
#include <vector>
#include <typeinfo>

#include "LuaGlue/LuaGlueCompat.h"

#pragma warning(push)
// strcpy is considered as deprecated/unsafe by MSVC
#pragma warning(disable: 4996)

// NOTE: hashing algorithm used is FNV-1a

// FNV-1a constants
class LuaGlueHash {
	private:
		static constexpr unsigned long long basis = 14695981039346656037ULL;
		static constexpr unsigned long long prime = 1099511628211ULL;

		// compile-time hash helper function
		constexpr static unsigned long long hash_one(unsigned char c, const unsigned char* remain, unsigned long long value)
		{
			return c == 0 ? value : hash_one(remain[0], remain + 1, (value ^ c) * prime);
		}

	public:
		// compile-time hash
		constexpr static unsigned long long hash(const unsigned char* str)
		{
			return hash_one(str[0], str + 1, basis);
		}

		// run-time hash
		static unsigned long long hash_rt(const unsigned char* str)
		{
			unsigned long long hash = basis;
			while (*str != 0) {
				hash ^= str[0];
				hash *= prime;
				++str;
			}
			return hash;
		}
};

template<class T>
class LuaGlueSymTab
{
	private:
		
		struct Symbol {
			const char *name;
			const char *typeid_name;
			T ptr; int idx;
			
			Symbol(const char *n = nullptr, const char *tn = nullptr, T p = nullptr, int i = -1)
				: name(n ? strcpy(new char[strlen(n) + 1], n) : nullptr), typeid_name(tn), ptr(p), idx(i)
			{
				//printf("new Symbol(\"%s\", \"%s\", %p, %i)\n", n, tn, p, idx);
			}
		};
		
	public:
		LuaGlueSymTab() { }
		~LuaGlueSymTab()
		{
			for (auto &i : items)
			{
				delete i.name;
				delete i.ptr;
			}
		}
		
		template<class C>
		void addSymbol(const char *name, C *ptr)
		{
			const Symbol &sym = findSym(name);
			if(sym.name)
				return;
			
			items.push_back(Symbol(name, typeid(C).name(), ptr, items.size()));
		}
		
		/*
		T &operator[]( const char *key )
		{
			Symbol &sym = findSym(key);
			if(!sym.name)
			{
				items.push_back(Symbol(key, nullptr, items.size()));
				Symbol &new_sym = items.back();
				new_sym.name = strdup(key);
				return new_sym.ptr;
			}
			
			return sym.ptr;
		}
		
		T &operator[]( const std::string &key )
		{
			const char *ckey = key.c_str();
			Symbol &sym = findSym(ckey);
			if(!sym.name)
			{
				items.push_back(Symbol(ckey, nullptr, items.size()));
				Symbol &new_sym = items.back();
				new_sym.name = strdup(ckey);
				return new_sym.ptr;
			}
			
			return sym.ptr;
		}
		*/
		
		typename std::vector<Symbol>::iterator begin()
		{
			return items.begin();
		}

		typename std::vector<Symbol>::iterator end()
		{
			return items.end();
		}
		
		bool exists(const char *key, bool internal_name = false)
		{
			if(internal_name)
				return findSym_int(key).name != nullptr;
			
			return findSym(key).name != nullptr;
		}
		
		T lookup(const char *key, bool internal_name = false)
		{
			if(internal_name)
				return findSym_int(key).ptr;
			
			return findSym(key).ptr;
		}
		
		T lookup(uint32_t idx)
		{
			return findSym(idx).ptr;
		}
		
		const Symbol &findSym(const char *name)
		{
			for(auto &sym: items)
			{
				//printf("findSym: %s <=> %s\n", sym.name, name);
				if(strcmp(sym.name, name) == 0)
					return sym;
			}
			
			return nullSymbol;
		}
		
	private:
		
		static const Symbol nullSymbol;
		std::vector<Symbol> items;
		
		const Symbol &findSym_int(const char *name)
		{
			for(auto &sym: items)
			{
				//printf("findSym_int: %s <=> %s\n", sym.typeid_name, name);
				if(strcmp(sym.typeid_name, name) == 0)
					return sym;
			}
			
			return nullSymbol;
		}
		
		const Symbol &findSym(uint32_t idx)
		{
			if(idx > items.size())
			{
				//printf("findSym(%i): not found\n", idx);
				return nullSymbol;
			}
			
			//printf("findSym(%i): %s\n", idx, items[idx].name);
			return items[idx];
		}
};

template<typename T>
const typename LuaGlueSymTab<T>::Symbol LuaGlueSymTab<T>::nullSymbol;

#pragma warning(pop)

#endif /* LUAGLUE_SYMTAB_H_GUARD */
