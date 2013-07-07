//
//  Created by Warren Seine on Oct 1, 2011.
//  Copyright (c) 2011 Aerys. All rights reserved.
//

#pragma once

#include "minko/Common.hpp"
#include "minko/Any.hpp"
//#include "miniz.c"


namespace minko
{
	class Qark
	{
	public:
		static const int                  MAGIC                   = 0x3121322b;

		static const unsigned char        FLAG_NONE               = 0;
		static const unsigned char        FLAG_GZIP               = 1;
		static const unsigned char        FLAG_DEFLATE            = 2;

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

		typedef std::vector<char>			ByteArray;
		typedef minko::Any					Object;
		typedef std::string					String;
		typedef std::vector<Object>			Array;
		typedef std::map<String, Object>	Map;
		typedef unsigned char				uint8;

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

			write(stream, FLAG_NONE);
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
			unsigned char compression_flag = 0;

			read(stream, compression_flag);
			int compression_flagint = compression_flag;

			if (compression_flag == FLAG_GZIP)
			{
				std::cout << "zlib method detected " << std::endl << std::flush;
				/*uint step = 0;
				int	cmpStatus;
				
				unsigned long srcLen			= (unsigned long)(source.size() - 5);
				unsigned long compressLen		= compressBound(srcLen);
				unsigned long unCompressLength	= srcLen;

				uint8 *pCmp, *pUncomp;

				pCmp = &(*(source.begin() + 5));

				uint total_succeeded = 0;

				cmpStatus = uncompress(pUncomp, &unCompressLength, pCmp, srcLen);*/

			}
			else if (compression_flag == FLAG_DEFLATE)
			{
				std::cout << "inflate method detected " << std::endl << std::flush;
			}
			else
			{
				std::cout << "no compression method detected " << std::endl << std::flush;
			}

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
				encoders[TYPE_BYTES]		= &Qark::encodeBytes;
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
				decoders[TYPE_BOOLEAN]		= &Qark::decodeTrivial<bool>;
				decoders[TYPE_STRING]		= &Qark::decodeString;
				decoders[TYPE_OBJECT]		= &Qark::decodeMap;
				decoders[TYPE_ARRAY]		= &Qark::decodeArray;
				decoders[TYPE_BYTES]		= &Qark::decodeBytes;
			}

			return decoders[flag];
		}

		static 
		void 
		encodeRecursive(std::stringstream& target, const Object& source)
		{
			char flag = getType(source);
			
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
			write(stream, minko::Any::cast<T>(value));
		}

		template <typename T>
		static void
		decodeTrivial(std::stringstream& stream, Object& value)
		{
			value = T();
			read(stream, minko::Any::cast<T&>(value));
		}

		static 
		void
		encodeString(std::stringstream& stream, const Object& value)
		{
			const String& str = minko::Any::cast<const String&>(value);

			encodeString(stream, str);
		}

		static 
		void 
		encodeString(std::stringstream& stream, const String& value)
		{
			unsigned short size = value.size();

			write(stream, size);
			stream.write(value.c_str(), size);
		}


		static 
		void
		decodeString(std::stringstream& stream, Object& value)
		{

			value = std::string();
			String& str = minko::Any::cast<String&>(value);

			decodeString(stream, str);

			value = std::string(str);
		}

		static 
		void
		decodeString(std::stringstream& stream, String& value)
		{
			unsigned short size = 0;
			read(stream, size);

			char* data = new char[size];

			stream.read(data, size);
			value.assign(data, size);

			delete[] data;
		}


		static 
		void 
		encodeMap(std::stringstream& stream, const Object& value)
		{
			std::map<std::string, Object> map	= minko::Any::cast<std::map<std::string, Object>>(value);
			unsigned short				  size	= map.size();

			write(stream, size);

			for (std::map<std::string, Object>::iterator it = map.begin(); it != map.end(); ++it)
			{
				encodeString(stream, it->first);
				encodeRecursive(stream, it->second);
			}
		}

		static
		void
		decodeMap(std::stringstream& stream, Object& value)
		{
			std::map<String, Any>	map;
			unsigned short			size = 0;

			read(stream, size);

			unsigned short copy = size;

			while (size > 0)
			{
				std::string key;
				Any			mappedValue = 0;

				decodeString(stream, key);
				decodeRecursive(stream, mappedValue);

				map[key] = mappedValue;
				size--;
			}
			value = map;
		}

		static
		void
		encodeArray(std::stringstream& stream, const Object& value)
		{
			Array list = minko::Any::cast<Array>(value);

			short size = list.size();

			write(stream, size);

			for (unsigned int i = 0; i < list.size(); ++i)
				encodeRecursive(stream, list[i]);
		}

		static
		void
		decodeArray(std::stringstream& stream, Object& value)
		{
			Array			list;
			unsigned short	size = 0;

			read(stream, size);

			for (short i = 0; i < size; ++i)
			{
				Any  newElement = 0;
				decodeRecursive(stream, newElement);
				list.push_back(newElement);
			}

			value = list;
		}

		static
		void
		encodeBytes(std::stringstream& stream, const Object& value)
		{
			ByteArray bytes = minko::Any::cast<ByteArray>(value);

			write(stream, true);
			write(stream, bytes.size());

			for (unsigned int i = 0; i < bytes.size(); ++i)
			{
				char byte = bytes[i];

				write(stream, byte);
			}
		}

		static
		void
		decodeBytes(std::stringstream& stream, Object& value)
		{
			bool endian = false;
			int  length = 0;

			read(stream, endian);
			read(stream, length);

			ByteArray	bytes;

			while (length > 0)
			{
				char		newByte = 0;

				read(stream, newByte);
				bytes.push_back(newByte);
				length--;
			}

			value = bytes;
		}

	};
}
