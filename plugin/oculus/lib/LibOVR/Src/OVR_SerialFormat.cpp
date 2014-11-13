/************************************************************************************

Filename    :   OVR_System.cpp
Content     :   General kernel initialization/cleanup, including that
                of the memory allocator.
Created     :   September 19, 2012
Notes       : 

Copyright   :   Copyright 2014 Oculus VR, Inc. All Rights reserved.

Licensed under the Oculus VR Rift SDK License Version 3.1 (the "License"); 
you may not use the Oculus VR Rift SDK except in compliance with the License, 
which is provided at the time of installation or download, or which 
otherwise accompanies this software in either electronic or hard copy form.

You may obtain a copy of the License at

http://www.oculusvr.com/licenses/LICENSE-3.1 

Unless required by applicable law or agreed to in writing, the Oculus VR SDK 
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#include "OVR_SerialFormat.h"

#ifdef SERIAL_FORMAT_UNIT_TEST
#include "Kernel/OVR_Log.h"
#endif

namespace OVR {


//// Serial Format Detection

SerialFormatType DetectBufferFormat(uint8_t firstByte, int sizeInBytes)
{
	switch (firstByte)
	{
	case SerialFormatType_DK2:
		if (sizeInBytes == 12)
		{
			return SerialFormatType_DK2;
		}
		break;
	default:
		break;
	}

	return SerialFormatType_Invalid;
}


//// DK2 Helpers

static bool ValidDK2ProductId(int x)
{
	switch (x)
	{
	case DK2ProductId_DK1:
	case DK2ProductId_DK2:
	case DK2ProductId_Refurb:
		return true;
	default:
		break;
	}

	return false;
}

static bool ValidDK2PartId(int x)
{
	switch (x)
	{
	case DK2PartId_HMD:
	case DK2PartId_PTC:
	case DK2PartId_Carton:
		return true;
	default:
		break;
	}

	return false;
}


//// DK2BinarySerialFormat

bool DK2BinarySerialFormat::FromBuffer(const uint8_t buffer[12], bool allowUnknownTypes)
{
	// Format Type must be 0
	
	int formatType = buffer[0];

	if (formatType != SerialFormatType_DK2)
	{
		return false;
	}

	// Product Id

	int productId = buffer[1] >> 4;

	if (!allowUnknownTypes && !ValidDK2ProductId(productId))
	{
		return false;
	}

	ProductId = (DK2ProductId)productId;

	// Part Id

	int partId = buffer[1] & 15;

	if (!allowUnknownTypes && !ValidDK2PartId(partId))
	{
		return false;
	}

	PartId = (DK2PartId)partId;

	// Minutes Since Epoch (May 1, 2014)

	MinutesSinceEpoch = buffer[4] | ((uint32_t)buffer[3] << 8) | ((uint32_t)buffer[2] << 16);

	// Unit number on that day

	UnitNumber = buffer[6] | ((uint32_t)buffer[5] << 8);

	// Hash of MAC address

	MacHash[0] = buffer[7];
	MacHash[1] = buffer[8];
	MacHash[2] = buffer[9];
	MacHash[3] = buffer[10];
	MacHash[4] = buffer[11];

	return true;
}

void DK2BinarySerialFormat::ToBuffer(uint8_t buffer[12])
{
	// Serialize to buffer
	buffer[0] = SerialFormatType_DK2;
	buffer[1] = (uint8_t)((ProductId << 4) | (PartId));
	buffer[2] = (uint8_t)(MinutesSinceEpoch >> 16);
	buffer[3] = (uint8_t)(MinutesSinceEpoch >> 8);
	buffer[4] = (uint8_t)MinutesSinceEpoch;
	buffer[5] = (uint8_t)(UnitNumber >> 8);
	buffer[6] = (uint8_t)UnitNumber;

	buffer[7] = MacHash[0];
	buffer[8] = MacHash[1];
	buffer[9] = MacHash[2];
	buffer[10] = MacHash[3];
	buffer[11] = MacHash[4];
}

bool DK2BinarySerialFormat::operator==(const DK2BinarySerialFormat& rhs)
{
	if (ProductId != rhs.ProductId)
		return false;
	if (PartId != rhs.PartId)
		return false;
	if (MinutesSinceEpoch != rhs.MinutesSinceEpoch)
		return false;
	if (UnitNumber != rhs.UnitNumber)
		return false;
	for (int ii = 0; ii < 5; ++ii)
	{
		if (MacHash[ii] != rhs.MacHash[ii])
			return false;
	}
	return true;
}


//// DK2PrintedSerialFormat

// Base-32 Crockford decoding rules:
// 0 o O => 0
// 1 i | I L l => 1
// 2, 3, 4, 5, 6, 7, 8, 9 => 2 - 9
// a, b, c, d, e, f, g, h => 10 - 17
// j, k => 18, 19
// m, n => 20, 21
// p, q, r, s, t => 22, 23, 24, 25, 26
// v, w, x, y, z => 27, 28, 29, 30, 31
static const char Base32FromChar[256] = {
	// Null - Unit Separator
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	// (sp)!"#$%&'()*+,-./
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	// 0123456789:;<=>?
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
	// @ - _ (upper case)
	-1, 10, 11, 12, 13, 14, 15, 16, 17,  1, 18, 19,  1, 20, 21,  0,
	22, 23, 24, 25, 26, -1, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1,
	// ` - DEL (lower case)
	-1, 10, 11, 12, 13, 14, 15, 16, 17,  1, 18, 19,  1, 20, 21,  0,
	22, 23, 24, 25, 26, -1, 27, 28, 29, 30, 31, -1,  1, -1, -1, -1,

	// Extended ASCII:
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

// Base-32 Crockford encoding rules:
// 0-9 => 0-9
// 10 - 17 => a, b, c, d, e, f, g, h
// 18, 19 => j, k
// 20, 21 => m, n
// 22, 23, 24, 25, 26 => p, q, r, s, t
// 27, 28, 29, 30, 31 => v, w, x, y, z
static const char* CharFromBase32 = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";

bool DK2PrintedSerialFormat::FromBase32(const char* str, bool allowUnknownTypes)
{
	// Note: Truncated strings get caught by returning negative values from the table like other invalid characters

	// Product Id

	int productId = Base32FromChar[(unsigned char)str[0]];
	if (productId < 0 || (!allowUnknownTypes && !ValidDK2ProductId(productId)))
	{
		return false;
	}

	ProductId = (DK2ProductId)productId;

	// Label Type

	int labelType = Base32FromChar[(unsigned char)str[1]];
	if (labelType < 0 || (!allowUnknownTypes && !ValidDK2PartId(labelType)))
	{
		return false;
	}

	LabelType = (DK2LabelType)labelType;

	uint8_t dataBytes[7];
	for (int ii = 0; ii < 7; ++ii)
	{
		int c = Base32FromChar[(unsigned char)str[2 + ii]];
		if (c < 0) return false;
		dataBytes[ii] = (uint8_t)c;
	}

	// Minutes Since Epoch

	MinutesSinceEpoch = dataBytes[3] | ((uint32_t)dataBytes[2] << 5) | ((uint32_t)dataBytes[1] << 10) | ((uint32_t)dataBytes[0] << 15);

	// Unit Number

	UnitNumber = dataBytes[6] | ((uint32_t)dataBytes[5] << 5) | ((uint32_t)dataBytes[4] << 10);

	// MAC Hash

	for (int ii = 0; ii < 3; ++ii)
	{
		int c = Base32FromChar[(unsigned char)str[9 + ii]];
		if (c < 0)
		{
			return false;
		}

		MacHashLow[ii] = (uint8_t)c;
	}

	// String must be exactly 12 characters
	if (str[12] != '\0')
	{
		return false;
	}

	return true;
}

String DK2PrintedSerialFormat::ToBase32()
{
	String s;

	s += CharFromBase32[ProductId];
	s += CharFromBase32[LabelType];
	s += CharFromBase32[(MinutesSinceEpoch >> 15) & 31];
	s += CharFromBase32[(MinutesSinceEpoch >> 10) & 31];
	s += CharFromBase32[(MinutesSinceEpoch >> 5) & 31];
	s += CharFromBase32[MinutesSinceEpoch & 31];
	s += CharFromBase32[(UnitNumber >> 10) & 31];
	s += CharFromBase32[(UnitNumber >> 5) & 31];
	s += CharFromBase32[UnitNumber & 31];
	s += CharFromBase32[MacHashLow[0] & 31];
	s += CharFromBase32[MacHashLow[1] & 31];
	s += CharFromBase32[MacHashLow[2] & 31];

	return s;
}

bool DK2PrintedSerialFormat::operator==(const DK2PrintedSerialFormat& rhs)
{
	if (ProductId != rhs.ProductId)
		return false;
	if (LabelType != rhs.LabelType)
		return false;
	if (MinutesSinceEpoch != rhs.MinutesSinceEpoch)
		return false;
	if (UnitNumber != rhs.UnitNumber)
		return false;
	for (int ii = 0; ii < 3; ++ii)
	{
		if (MacHashLow[ii] != rhs.MacHashLow[ii])
			return false;
	}
	return true;
}

bool DK2PrintedSerialFormat::operator==(const DK2BinarySerialFormat& rhs)
{
	if (ProductId != rhs.ProductId)
		return false;
	if (LabelType != rhs.PartId)
		return false;
	if (MinutesSinceEpoch != rhs.MinutesSinceEpoch)
		return false;
	if (UnitNumber != rhs.UnitNumber)
		return false;
	for (int ii = 0; ii < 3; ++ii)
	{
		if (MacHashLow[ii] != (rhs.MacHash[ii] & 31))
			return false;
	}
	return true;
}

void DK2PrintedSerialFormat::FromBinary(const DK2BinarySerialFormat& bin)
{
	ProductId = bin.ProductId;
	LabelType = bin.PartId;
	MinutesSinceEpoch = bin.MinutesSinceEpoch;
	UnitNumber = bin.UnitNumber;
	MacHashLow[0] = bin.MacHash[0] & 31;
	MacHashLow[1] = bin.MacHash[1] & 31;
	MacHashLow[2] = bin.MacHash[2] & 31;
}


//// Human Strings

// For generator polynomial 101111b:
static const uint8_t GF32_LOG_TABLE[32] = {
    64, 31, 1, 12, 2, 24, 13, 27, 3, 8, 25, 10, 14, 18, 28, 5,
    4, 17, 9, 7, 26, 23, 11, 30, 15, 21, 19, 20, 29, 22, 6, 16
};
static const uint8_t GF32_ALOG_TABLE[129] = {
    1, 2, 4, 8, 16, 15, 30, 19, 9, 18, 11, 22, 3, 6, 12, 24,
    31, 17, 13, 26, 27, 25, 29, 21, 5, 10, 20, 7, 14, 28, 23, 1,
    2, 4, 8, 16, 15, 30, 19, 9, 18, 11, 22, 3, 6, 12, 24, 31,
    17, 13, 26, 27, 25, 29, 21, 5, 10, 20, 7, 14, 28, 23, 1, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0
};
static const uint8_t GF32_INV_TABLE[32] = {
    0, 1, 23, 26, 28, 19, 13, 16, 14, 21, 30, 25, 17, 6, 8, 20,
    7, 12, 29, 5, 15, 9, 27, 2, 31, 11, 3, 22, 4, 18, 10, 24
};

// Multiplication in GF(32) : x * y = e^(log(x) + log(y))
OVR_FORCE_INLINE uint8_t GF32Multiply(uint8_t x, uint8_t y)
{
    return GF32_ALOG_TABLE[GF32_LOG_TABLE[x] + GF32_LOG_TABLE[y]];
}
OVR_FORCE_INLINE uint8_t GF32Divide(uint8_t x, uint8_t y)
{
    return GF32_ALOG_TABLE[GF32_LOG_TABLE[x] + 31 - GF32_LOG_TABLE[y]];
}

// Normally these inputs are constant so this is just useful for development
static bool ValidateHumanStringBits(int dataBits, int checkBits)
{
    // If no data bits or data bits leave no room for a checksum,
    if (dataBits < 1 || dataBits >= 160)
    {
        OVR_ASSERT(false);
        return false;
    }

    // If no check bits or check bits leave no room for a checksum,
    if (checkBits < 1 || checkBits >= 160)
    {
        OVR_ASSERT(false);
        return false;
    }
    // Note: It's recommended to have at least 5 check bits so that
    // character typos can be caught.

    // The sum of check bits and data bits must be a multiple of 5!
    if ((checkBits + dataBits) % 5 != 0)
    {
        OVR_ASSERT(false);
        return false;
    }

    // The total number of human string digits cannot exceed 32.
    if ((checkBits + dataBits + 4) / 5 > 32)
    {
        OVR_ASSERT(false);
        return false;
    }

    return true;
}

// Generate check buffer
static void GenerateChecks(const uint8_t* data, int dataCount, uint8_t* checks, int checkCount, int msbCount)
{
    // For each check symbol to generate,
    for (int i = 0; i < checkCount; ++i)
    {
        // Matrix multiplication inner loop
        uint8_t x = 0;
        for (int j = 0; j < dataCount; ++j)
        {
            // GF(32): x += d[i,j] * k, k = 1 / (j - i), ie. Cauchy matrix
            x ^= GF32Multiply(data[j], GF32_INV_TABLE[(checkCount + j) ^ i]);
        }
        checks[i] = x;
    }

    // Clear high bits of checks
    if (msbCount > 0)
    {
        checks[checkCount - 1] &= (1 << msbCount) - 1;
    }
}

// Generates a Human-readable labels with built-in checksums
// Returns false on error
bool GenerateHumanString(int dataBits,               // Number of bits to store
                         int checkBits,              // Number of extra bits of checksum data
                         const unsigned char* input, // Bytes stored LSB-first
                         const int dataBytes,        // Size of data buffer in bytes
                         String& resultStr)          // Output string
{
    if (!ValidateHumanStringBits(dataBits, checkBits))
    {
        return false;
    }

    // If the buffer is the wrong size,
    if (dataBytes < (dataBits + 7) / 8)
    {
        OVR_ASSERT(false);
        return false;
    }

    resultStr.Clear();

    // Copy data bits
    int dataCount = (dataBits + 4) / 5;
    uint8_t data[32] = { 0 }; // Initialized to zeros
    for (int i = 0; i < dataBits; ++i)
    {
        // Grab bit
        int bit = (input[i / 8] >> (i % 8)) & 1;

        // Copy to data
        data[i / 5] |= bit << (i % 5);
    }

    // Generate checks
    int checkCount = (checkBits + 4) / 5;
    uint8_t checks[32];
    GenerateChecks(data, dataCount, checks, checkCount, checkBits % 5);

    // Copy full parts of checks after the partial character (if it exists)
    int fullCheckCount = checkBits / 5;
    for (int i = 0; i < fullCheckCount; ++i)
    {
        data[dataCount + i] = checks[i];
    }

    // If there is a partial check character,
    int partialCheckBits = checkBits % 5;
    if (partialCheckBits > 0)
    {
        // Write the high bits of the partial high symbol there
        int word = checks[fullCheckCount] << (5 - partialCheckBits);
        word &= 0x1f;
        data[dataCount - 1] |= word;
    }

    // Write base-32 representation of data
    int outCount = (dataBits + checkBits) / 5;
    for (int i = 0; i < outCount; ++i)
    {
        resultStr += CharFromBase32[data[i]];
    }

    return true;
}

// Fix errors in data found by checks
static bool FixHumanChecksum(uint8_t* data, int dataCount, uint8_t* checks, int checkCount, int msbCount)
{
    uint8_t actualChecks[32];
    GenerateChecks(data, dataCount, actualChecks, checkCount, msbCount);

    // Search for check mismatches
    int checkFails = 0;
    for (int i = 0; i < checkCount; ++i)
    {
        if (actualChecks[i] != checks[i])
        {
            ++checkFails;
        }
    }

    // Calculate full check count
    int fullCheckCount = checkCount;
    if (msbCount > 0)
    {
        fullCheckCount--;
    }

    // If there is a lot of full check data,
    if (fullCheckCount >= 2)
    {
        // Accept one error in the checks
        if (checkFails <= 1)
        {
            return true;
        }

        // Attempt to correct single errors:
        for (int errPosition = 0; errPosition < dataCount; ++errPosition)
        {
            uint8_t oldData = data[errPosition];

            // Use the first check symbol for corrections
            uint8_t x = 0;
            for (int j = 0; j < dataCount; ++j)
            {
                if (j != errPosition)
                {
                    x ^= GF32Multiply(data[j], GF32_INV_TABLE[checkCount + j]);
                }
            }
            x ^= checks[0];

            // Calculate missing symbol
            data[errPosition] = GF32Divide(x, GF32_INV_TABLE[checkCount + errPosition]);

            GenerateChecks(data, dataCount, actualChecks, checkCount, msbCount);

            // Search for check mismatches again:
            int checkFails = 0;
            for (int i = 0; i < checkCount; ++i)
            {
                if (actualChecks[i] != checks[i])
                {
                    // Ignore the partial checksum in case it got corrupted too
                    if (msbCount > 0 &&
                        errPosition == dataCount - 1 &&
                        i == checkCount - 1)
                    {
                        continue;
                    }
                    ++checkFails;
                    break;
                }
            }

            // Accept the correction if all checks agree now
            if (checkFails == 0)
            {
                // Keep the correction if it succeeded
                return true;
            }

            data[errPosition] = oldData;
        }
    }
    else
    {
        // Do not accept any check fails
        if (checkFails <= 0)
        {
            return true;
        }
    }

    return false;
}

// Returns number of bytes written to result, or zero on failure
int ProcessHumanString(int dataBits,               // Number of bits to expect
                       int checkBits,              // Number of extra bits of checksum data
                       String humanStr,            // Human readable input string
                       unsigned char* result,      // Output data buffer stored LSB-first
                       const int resultBytes)      // Size of result buffer in bytes
{
    if (!ValidateHumanStringBits(dataBits, checkBits))
    {
        return 0;
    }

    // If the string is not precisely the right length,
    if (humanStr.GetLengthI() != (dataBits + checkBits + 4) / 5)
    {
        return 0;
    }

    // If the result buffer is not large enough,
    int dataBytes = (dataBits + 7) / 8;
    if (dataBytes > resultBytes)
    {
        OVR_ASSERT(false); // Programming error
        return 0;
    }

    // Convert human string to 5-bit symbols
    uint8_t data[32];
    const char* cHumanStr = humanStr.ToCStr();
    for (int i = 0; i < humanStr.GetLengthI(); ++i)
    {
        char word = Base32FromChar[(unsigned char)cHumanStr[i]];
        if (word < 0)
        {
            // Attempt to replace invalid characters,
            // and rely on error correction to fix it.
            word = 0;
        }
        data[i] = word;
    }

    // Extract checks from data
    int dataCount = (dataBits + 4) / 5;
    int checkCount = (checkBits + 4) / 5;
    uint8_t checks[32];

    // Copy full check characters
    int fullCheckCount = checkBits / 5;
    for (int i = 0; i < fullCheckCount; ++i)
    {
        checks[i] = data[dataCount + i];
    }

    // If there is a partial check character,
    int partialCheckBits = checkBits % 5;
    if (partialCheckBits > 0)
    {
        // Read high bits as the check high partial symbol
        uint8_t word = data[dataCount - 1] >> (5 - partialCheckBits);
        checks[fullCheckCount] = word;

        // Clear high bits of data that contain check bits
        data[dataCount - 1] ^= word << (5 - partialCheckBits);
    }

    // Fuzzy accept input
    if (!FixHumanChecksum(data, (dataBits + 4) / 5, checks, checkCount, checkBits % 5))
    {
        return 0;
    }

    // Copy data bits to output buffer
    memset(result, 0, resultBytes);
    for (int i = 0; i < dataBits; ++i)
    {
        // Grab data bit
        int bit = (data[i / 5] >> (i % 5)) & 1;

        // Write it into result
        result[i / 8] |= bit << (i % 8);
    }

    return dataBytes;
}


//// Unit Tests

#ifdef SERIAL_FORMAT_UNIT_TEST

int DecodeBase32(char ch)
{
    if (ch >= '2' && ch <= '9')
        return 2 + ch - '2';
    if (ch >= 'a' && ch <= 'h')
        return 10 + ch - 'a';
    if (ch >= 'A' && ch <= 'H')
        return 10 + ch - 'A';
    if (ch >= 'j' && ch <= 'k')
        return 18 + ch - 'j';
    if (ch >= 'J' && ch <= 'K')
        return 18 + ch - 'J';
    if (ch >= 'm' && ch <= 'n')
        return 20 + ch - 'm';
    if (ch >= 'M' && ch <= 'N')
        return 20 + ch - 'M';
    if (ch >= 'p' && ch <= 't')
        return 22 + ch - 'p';
    if (ch >= 'P' && ch <= 'T')
        return 22 + ch - 'P';
    if (ch >= 'v' && ch <= 'z')
        return 27 + ch - 'v';
    if (ch >= 'V' && ch <= 'Z')
        return 27 + ch - 'V';

    switch (ch)
    {
    case '0':
    case 'o':
    case 'O':
        return 0;
    case '1':
    case 'i':
    case '|':
    case 'I':
    case 'L':
    case 'l':
        return 1;
    }

    return -1;
}

void TestSerialFormatStuff()
{
    for (int ii = 0; ii < 256; ++ii)
    {
        OVR_ASSERT(Base32FromChar[ii] == (char)DecodeBase32((char)ii));
    }

    DK2BinarySerialFormat sa;
    sa.ProductId = DK2ProductId_DK2;
    sa.PartId = DK2PartId_HMD;
    sa.MinutesSinceEpoch = 65000;
    sa.UnitNumber = 2;
    sa.MacHash[0] = 0xa1;
    sa.MacHash[1] = 0xb2;
    sa.MacHash[2] = 0xc3;
    sa.MacHash[3] = 0xd4;
    sa.MacHash[4] = 0xe5;

    uint8_t buffer[12];
    sa.ToBuffer(buffer);

    DK2BinarySerialFormat sb;
    bool success = sb.FromBuffer(buffer);
    OVR_ASSERT(success);
    OVR_UNUSED(success);

    OVR_ASSERT(sa == sb);

    DK2PrintedSerialFormat psn;
    psn.FromBinary(sb);

    OVR_ASSERT(psn == sa);

    String s = psn.ToBase32();

    DK2PrintedSerialFormat psn2;
    psn2.FromBase32(s.ToCStr());

    OVR_ASSERT(psn == psn2);

    unsigned char data[32];
    unsigned char result[32];

    int trials = 0;
    int trial_failures = 0;

    for (int seed = 0; seed < 20; ++seed)
    {
        srand(seed);

        for (int dataBits = 1; dataBits < 160; ++dataBits)
        {
            for (int checkBits = 1; checkBits < 160 - dataBits; ++checkBits)
            {
                String human;

                int fillCheckBits = (checkBits + dataBits) % 5;
                if (fillCheckBits > 0)
                {
                    fillCheckBits = 5 - fillCheckBits + checkBits;
                }
                else
                {
                    fillCheckBits = checkBits;
                }

                for (int i = 0; i < sizeof(data); ++i)
                {
                    data[i] = (unsigned char)rand();
                    if (seed == 1)
                    {
                        data[i] = 0;
                    }
                    else if (seed == 0)
                    {
                        data[i] = 0xff;
                    }
                }

                if (dataBits % 8 != 0)
                {
                    data[dataBits / 8] &= (1 << (dataBits % 8)) - 1;
                }

                if (!GenerateHumanString(dataBits, fillCheckBits, data, sizeof(data), human))
                {
                    OVR_ASSERT(false);
                }

                int bytes = ProcessHumanString(dataBits, fillCheckBits, human, result, sizeof(result));

                if (bytes <= 0)
                {
                    OVR_ASSERT(false);
                }

                OVR_ASSERT(bytes == (dataBits + 7) / 8);

                for (int i = 0; i < bytes; ++i)
                {
                    OVR_ASSERT(data[i] == result[i]);
                }

                // Produce maximal failures and verify that we catch them:

                if (fillCheckBits >= 5)
                {
                    char* cstr = (char*)human.ToCStr();

                    for (int i = 0; i < fillCheckBits / 5; ++i)
                    {
                        cstr[i] ^= 1;
                    }

                    trials++;
                    bytes = ProcessHumanString(dataBits, fillCheckBits, cstr, result, sizeof(result));

                    // Undo errors
                    for (int i = 0; i < fillCheckBits / 5; ++i)
                    {
                        cstr[i] ^= 1;
                    }

                    if (bytes > 0)
                    {
                        for (int i = 0; i < bytes; ++i)
                        {
                            if (data[i] != result[i])
                            {
                                trial_failures++;
                                break;
                            }
                        }
                    }

                    // If enough check bits,
                    if (fillCheckBits >= 10)
                    {
                        // Introduce one error at all positions and verify it is corrected:
                        char* cstr = (char*)human.ToCStr();

                        for (int j = 0; j < human.GetLengthI(); ++j)
                        {
                            if (dataBits % 5 != 0 && j == (dataBits / 5))
                            {
                                // Note: Corrupting a shared character
                                // effectively breaks two symbols at once and recovery is not possible..
                                continue;
                            }

                            cstr[j] ^= 1;

                            bytes = ProcessHumanString(dataBits, fillCheckBits, cstr, result, sizeof(result));

                            OVR_ASSERT(bytes > 0);

                            for (int i = 0; i < bytes; ++i)
                            {
                                OVR_ASSERT(data[i] == result[i]);
                            }

                            cstr[j] ^= 1;
                        }
                    }
                }
            }
        }
    }

    // Allow some number of false positives in exchange for error correction
    OVR_ASSERT(trial_failures <= 0.008 * trials);

    LogText("[SerialFormatUnitTest] Success: False accept rate = %d / %d\n", trial_failures, trials);
}

#endif // SERIAL_FORMAT_UNIT_TEST


} // OVR
