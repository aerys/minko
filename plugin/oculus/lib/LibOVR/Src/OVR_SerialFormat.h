/************************************************************************************

PublicHeader:   n/a
Filename    :   OVR_SerialFormat.h
Content     :   Serial Number format tools
Created     :   June 12, 2014

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

#ifndef OVR_SerialFormat_h
#define OVR_SerialFormat_h

#include "Kernel/OVR_Types.h"
#include "Kernel/OVR_String.h"

namespace OVR {


//-----------------------------------------------------------------------------
// SerialFormatType enumeration

enum SerialFormatType
{
	SerialFormatType_Invalid = -1, // Invalid format
	SerialFormatType_DK2 = 0,	   // Format used for DK2
};

// Returns the expected serial format based on the first byte of the buffer
SerialFormatType DetectBufferFormat(uint8_t firstByte, int sizeInBytes);


//-----------------------------------------------------------------------------
// DK2 Serial Format

enum DK2ProductId
{
	DK2ProductId_DK1    = 1, // DK1
	DK2ProductId_DK2    = 2, // Product Id used for initial DK2 launch
	DK2ProductId_Refurb = 3, // Refurbished DK2
};

enum DK2PartId
{
	DK2PartId_HMD    = 0, // HMD
	DK2PartId_PTC    = 1, // PTC(camera)
	DK2PartId_Carton = 2, // Carton: An HMD + PTC combo (should not be stamped on a component) AKA Overpack
};

typedef DK2PartId DK2LabelType; // Printed Serial Number version


// DK2 tool for reading/writing the binary serial format
class DK2BinarySerialFormat
{
public:
	static const SerialFormatType FormatType = SerialFormatType_DK2; // first byte

	DK2ProductId ProductId;         // [4 bits] 2 = DK2
	DK2PartId    PartId;            // [4 bits] 0 means HMD, 1 means PTC(camera)
	int          MinutesSinceEpoch; // [3 bytes] Number of minutes that have elapsed since the epoch: May 1st, 2014
	// [0] = high byte, [1] = middle byte, [2] = low byte
	int          UnitNumber;        // [2 bytes] Value that increments each time a new serial number is created.  Resets to zero each day
	// [0] = high byte, [1] = low byte
	uint8_t      MacHash[5];        // [5 bytes] 5 most significant bytes of MD5 hash from first ethernet adapter mac address

	bool operator==(const DK2BinarySerialFormat& rhs);

public:
	// Returns false if the input is invalid in some way
	bool FromBuffer(const uint8_t buffer[12], bool allowUnknownTypes = false);

	// Fills the provided buffer with 12 bytes
	void ToBuffer(uint8_t buffer[12]);
};


// DK2 tool for reading/writing the printed serial format
class DK2PrintedSerialFormat
{
public:
	DK2ProductId ProductId;         // [1 char] 2 = DK2, 3 = Reconditioned bundle
	DK2LabelType LabelType;         // [1 char] 0 means HMD, 1 means PTC(camera), 2 means Overpack(bundle)
	int          MinutesSinceEpoch; // [4 char] Number of minutes that have elapsed since the epoch: May 1st, 2014
	int          UnitNumber;        // [3 char] Value that increments each time a new serial number is created.  Resets to zero each day
	uint8_t      MacHashLow[3];     // [3 char] 3 least significant bytes of mac hash

	bool operator==(const DK2PrintedSerialFormat& rhs);
	bool operator==(const DK2BinarySerialFormat& rhs);

public:
	// Convert from binary to printed
	void FromBinary(const DK2BinarySerialFormat& bin);

	// Returns false if the input is invalid in some way
	// Convert from a 12 character printed serial number
	bool FromBase32(const char* str, bool allowUnknownTypes = false);

	// Returns a long human-readable base32 string (20 characters), NOT a printed serial number
	String ToBase32();
};


// Generates a Human-readable labels with built-in checksums
// Returns false on error
bool GenerateHumanString(int dataBits,               // Number of bits to store
                         int checkBits,              // Number of extra bits of checksum data
                         const unsigned char* input, // Bytes stored LSB-first
                         int dataBytes,              // Size of data buffer in bytes
                         String& resultStr);         // Output string

// Returns number of bytes written to result, or zero on failure
int ProcessHumanString(int dataBits,               // Number of bits to expect
                       int checkBits,              // Number of extra bits of checksum data
                       String humanStr,            // Human readable input string
                       unsigned char* result,      // Output data buffer stored LSB-first
                       int resultBytes);           // Size of result buffer in bytes


//#define SERIAL_FORMAT_UNIT_TEST
#ifdef SERIAL_FORMAT_UNIT_TEST
void TestSerialFormatStuff();
#endif


} // OVR

#endif // OVR_SerialFormat_h
