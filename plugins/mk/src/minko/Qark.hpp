//
//  Created by Warren Seine on Oct 1, 2011.
//  Copyright (c) 2011 Aerys. All rights reserved.
//

#pragma once

#include <iostream>
#include <list>
#include <map>
#include <typeinfo>
#include <vector>
#include <sstream>

#include "minko/Any.hpp"


namespace minko
{
	class Qark
	{
	public:
		static const int                  MAGIC                   = 0x3121322b;

		static const int                  FLAG_NONE               = 0;
		static const int                  FLAG_GZIP               = 1;
		static const int                  FLAG_DEFLATE            = 2;

		static const int                  TYPE_CUSTOM             = 0;
		static const int                  TYPE_OBJECT             = 1;
		static const int                  TYPE_ARRAY              = 2;
		static const int                  TYPE_INT                = 3;
		static const int                  TYPE_UINT               = 4;
		static const int                  TYPE_FLOAT              = 5;
		static const int                  TYPE_STRING             = 6;
		static const int                  TYPE_BYTES              = 7;
		static const int                  TYPE_BOOLEAN            = 8;
		static const int                  TYPE_BITMAP_DATA        = 9;

		typedef std::vector<char>         ByteArray;
		typedef minko::Any								Object;
		typedef std::string               String;
		typedef std::vector<Object>       Array;
		typedef std::map<String, Object>  Map;

		typedef void (*Encoder)(std::stringstream&, const Object&);
		typedef void (*Decoder)(std::stringstream&, Object&);

	public:
		static 
		ByteArray
		encode(const Object& source)
		{
			std::stringstream stream;

			int magic = MAGIC;
			write(stream, magic);
			encodeRecursive(stream, source);

			std::string buffer = stream.str();
			const char* data = buffer.data();
			std::size_t size = buffer.size();

			return ByteArray(data, data + size);
		}

		static 
		Object 
		decode(const ByteArray& source)
		{
			std::stringstream stream;

			stream.write(&*source.begin(), source.size());

			int magic = 0;
			read(stream, magic);

			if (magic != MAGIC)
				return Object();

			Object result;
			decodeRecursive(stream, result);
			return result;
		}

	private:
		static 
		int 
		getType(const Object& source)
		{
			if (source.type() == typeid(int))
				return TYPE_INT;
			if (source.type() == typeid(unsigned int))
				return TYPE_UINT;
			if (source.type() == typeid(float))
				return TYPE_FLOAT;
			if (source.type() == typeid(String))
				return TYPE_STRING;
			if (source.type() == typeid(Array))
				return TYPE_ARRAY;
			if (source.type() == typeid(ByteArray))
				return TYPE_BYTES;
			if (source.type() == typeid(bool))
				return TYPE_BOOLEAN;
			if (source.type() == typeid(Map))
				return TYPE_OBJECT;

			return TYPE_CUSTOM;
		}

		static 
		Encoder 
		getEncoder(char flag)
		{
			static std::map<char, Encoder> encoders;

			if (encoders.empty())
			{
				encoders[TYPE_INT]			= &Qark::encodeTrivial<int>;
				encoders[TYPE_UINT]			= &Qark::encodeTrivial<unsigned int>;
				encoders[TYPE_FLOAT]		= &Qark::encodeTrivial<float>;
				encoders[TYPE_BOOLEAN]		= &Qark::encodeTrivial<bool>;
				encoders[TYPE_STRING]		= &Qark::encodeString;
				encoders[TYPE_OBJECT]		= &Qark::encodeMap;
				encoders[TYPE_ARRAY]		= &Qark::encodeArray;
			}

			return encoders[flag];
		}

		static 
		Decoder 
		getDecoder(char flag)
		{
			static std::map<char, Decoder> decoders;

			if (decoders.empty())
			{
				decoders[TYPE_INT]			= &Qark::decodeTrivial<int>;
				decoders[TYPE_UINT]			= &Qark::decodeTrivial<unsigned int>;
				decoders[TYPE_FLOAT]		= &Qark::decodeTrivial<float>;
				decoders[TYPE_BOOLEAN]	= &Qark::decodeTrivial<bool>;
				decoders[TYPE_STRING]		= &Qark::decodeString;
				decoders[TYPE_OBJECT]		= &Qark::decodeMap;
				decoders[TYPE_ARRAY]		= &Qark::decodeArray;
			}

			return decoders[flag];
		}

		static 
		void 
		encodeRecursive(std::stringstream& target, const Object& source)
		{
			char flag = getType(source);
			std::cout << "encoded flag : " << flag << std::endl << std::flush;
			write(target, flag);

			Encoder f = getEncoder(flag);
			f(target, source);
		}

		static 
		void 
		decodeRecursive(std::stringstream& source, Object& target)
		{
			char flag = 0;
			read(source, flag);

			std::cout << "decoded flag : " << flag << std::endl << std::flush;
			Decoder f = getDecoder(flag);
			f(source, target);
		}

		template <typename T>
		static void
		write(std::stringstream& stream, const T& value)
		{
			stream.write(reinterpret_cast<const char*>(&value), sizeof (T));
		}

		template <typename T>
		static void
		read(std::stringstream& stream, T& value)
		{
			stream.read(reinterpret_cast<char*>(&value), sizeof (T));
		}

		template <typename T>
		static void
		encodeTrivial(std::stringstream& stream, const Object& value)
		{
			std::cout << "   encode trivial " << std::endl << std::flush;

			write(stream, minko::Any::cast<T>(value));
		}

		template <typename T>
		static void
		decodeTrivial(std::stringstream& stream, Object& value)
		{
			std::cout << "   decode trivial " << std::endl << std::flush;

			value = T();
			read(stream, minko::Any::cast<T&>(value));
		}

		static 
		void
		encodeString(std::stringstream& stream, const Object& value)
		{
			std::cout << "   encode string Object " << std::endl << std::flush;

			const String& str = minko::Any::cast<const String&>(value);

			encodeString(stream, str);
		}

		static 
		void 
		encodeString(std::stringstream& stream, const String& value)
		{
			std::cout << "   encode string " << value << std::endl << std::flush;

			unsigned short size = value.size();

			std::cout << "    size " << size << std::endl << std::flush;

			write(stream, size);
			stream.write(value.c_str(), size);
		}


		static 
		void
		decodeString(std::stringstream& stream, Object& value)
		{
			std::cout << "   decode string Object " << std::endl << std::flush;

			value = std::string();
			String& str = minko::Any::cast<String&>(value);

			decodeString(stream, str);
		}

		static 
		void
		decodeString(std::stringstream& stream, String& value)
		{
			std::cout << "   decode string " << std::endl << std::flush;
			unsigned short size = 0;
			read(stream, size);

			std::cout << "    size " << size << std::endl << std::flush;

			char* data = new char[size];

			stream.read(data, size);
			value.assign(data, size);

			std::cout << "      result : " << value << std::endl << std::flush;

			delete[] data;
		}


		static 
		void 
		encodeMap(std::stringstream& stream, const Object& value)
		{
			std::map<std::string, Object> map = minko::Any::cast<std::map<std::string, Object>>(value);

			unsigned short size = map.size();

			write(stream, size);

			for (std::map<std::string, Object>::iterator it = map.begin(); it != map.end(); ++it)
			{
				std::cout <<  "key : " << it->first << std::endl << std::flush;
				encodeString(stream, it->first);
				encodeRecursive(stream, it->second);
			}
		}

		static
		void
		decodeMap(std::stringstream& stream, Object& value)
		{
			value = std::map<String, Any>();

			unsigned short size = 0;
			read(stream, size);

			unsigned short copy = size;

			while (size > 0)
			{
				std::string key;
				Any			mappedValue = 0;

				decodeString(stream, key);

				std::cout << "key : " << key << std::endl << std::flush;

				decodeRecursive(stream, mappedValue);

				std::map<String, Any> map = minko::Any::cast<std::map<String, Any>>(value);

				map[key] = mappedValue;
				size--;
			}
		}

		static
		void
		encodeArray(std::stringstream& stream, const Object& value)
		{
			Array list = minko::Any::cast<Array>(value);

			short size = list.size();

			write(stream, size);

			for (unsigned int i = 0; i < list.size(); ++i)
			{
				std::cout << "    element : " << i << std::endl << std::flush;
				encodeRecursive(stream, list[i]);
			}

			std::cout << "    end list" << std::endl << std::flush;
		}

		static
		void
		decodeArray(std::stringstream& stream, Object& value)
		{
			value = Array();

			short size = 0;
			read(stream, size);

			for (short i = 0; i < size; ++i)
			{
				Array list = minko::Any::cast<Array>(value);
				Any  newElement = 0;
				std::cout << "    element : " << i << std::endl << std::flush;

				list.push_back(newElement);
				decodeRecursive(stream, newElement);
			}

			std::cout << "    end list" << std::endl << std::flush;

		}

	};
}
