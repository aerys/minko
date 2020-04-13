//	DevIL.NET
//	Copyright (c) 2005, Marco Mastropaolo
//	All rights reserved.

//	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
//	following conditions are met:

//		* Redistributions of source code must retain the above copyright notice, this list of conditions and the 
//		following disclaimer.
//		* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the 
//		following disclaimer in the documentation and/or other materials provided with the distribution.
//		* Neither the name of DevIL.NET nor the names of its contributors may be used to endorse or promote products 
//		derived from this software without specific prior written permission.

//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
//	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
//	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
//	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
//	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
//	USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#using <mscorlib.dll>
#using <System.Drawing.dll>
#include <windows.h>
#include "IL/il.h"
#include "IL/ilu.h"

#undef LoadBitmap // brain damaged windows.h macros..

typedef ILboolean (*PFNILUSCALE)(ILuint Width, ILuint Height, ILuint Depth);
typedef ILvoid (*PFNILUIMAGEPARAMETER)(ILenum PName, ILenum Param);



namespace DevIL
{
	class StringAutoMarshal
	{
		char* m_string;

		inline explicit StringAutoMarshal(const StringAutoMarshal& i_Source) : m_string(NULL) {}
		inline StringAutoMarshal& operator= (const StringAutoMarshal& i_Source) {}

	public:
		inline explicit StringAutoMarshal(const System::String __gc* i_strString) : m_string(NULL)
		{
			m_string = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(const_cast<System::String __gc*>(i_strString));
		}


		inline ~StringAutoMarshal()
		{
			System::Runtime::InteropServices::Marshal::FreeHGlobal((void*)m_string);
		}

		inline char* GetNativeString()
		{
			return m_string;
		}

		inline operator char*()
		{
			return m_string;
		}
	};

	public __value enum DevILScaleFilter
	{
		NEAREST = ILU_NEAREST,
		LINEAR = ILU_LINEAR,
		BILINEAR = ILU_BILINEAR,
		BOX = ILU_SCALE_BOX,
		TRIANGLE = ILU_SCALE_TRIANGLE,
		BELL = ILU_SCALE_BELL,
		BSPLINE = ILU_SCALE_BSPLINE,
		LANCZOS3 = ILU_SCALE_LANCZOS3,
		MITCHELL =  ILU_SCALE_MITCHELL
	};

	public __value enum DevILScaleKind
	{
		DO_NOT_SCALE,
		WIDTH_AND_HEIGHT,
		WIDTH_ONLY,
		HEIGHT_ONLY,
		KEEPRATIO_USING_WIDTH,
		KEEPRATIO_USING_HEIGHT
	};


	public __value enum DevILErrorCode
	{
		OK = 0,
		ILU_DLL_NOT_FOUND = 0x0001,
		INVALID_ENUM = IL_INVALID_ENUM,
		OUT_OF_MEMORY = IL_OUT_OF_MEMORY,
		FORMAT_NOT_SUPPORTED = IL_FORMAT_NOT_SUPPORTED,
		INTERNAL_ERROR = IL_INTERNAL_ERROR,
		INVALID_VALUE = IL_INVALID_VALUE,
		ILLEGAL_OPERATION = IL_ILLEGAL_OPERATION,
		ILLEGAL_FILE_VALUE = IL_ILLEGAL_FILE_VALUE,
		INVALID_FILE_HEADER	= IL_INVALID_FILE_HEADER,
		INVALID_PARAM = IL_INVALID_PARAM,
		COULD_NOT_OPEN_FILE	= IL_COULD_NOT_OPEN_FILE,
		INVALID_EXTENSION = IL_INVALID_EXTENSION,
		FILE_ALREADY_EXISTS	= IL_FILE_ALREADY_EXISTS,
		OUT_FORMAT_SAME = IL_OUT_FORMAT_SAME,
		STACK_OVERFLOW = IL_STACK_OVERFLOW,
		STACK_UNDERFLOW = IL_STACK_UNDERFLOW,
		INVALID_CONVERSION = IL_INVALID_CONVERSION,
		BAD_DIMENSIONS = IL_BAD_DIMENSIONS,
		FILE_READ_ERROR = IL_FILE_READ_ERROR,
		FILE_WRITE_ERROR = IL_FILE_WRITE_ERROR,
		LIB_GIF_ERROR = IL_LIB_GIF_ERROR,
		LIB_JPEG_ERROR = IL_LIB_JPEG_ERROR,
		LIB_PNG_ERROR = IL_LIB_PNG_ERROR,
		LIB_TIFF_ERROR = IL_LIB_TIFF_ERROR,
		LIB_MNG_ERROR = IL_LIB_MNG_ERROR,
		UNKNOWN_ERROR = IL_UNKNOWN_ERROR
	};


	public __gc class DevIL
	{
	protected:
		static ILuint s_iImageID = 0;
		static bool s_bInitDone = false;
		static bool s_bIluLoaded = false;
		static DevILErrorCode s_eErrCode = OK;
		static PFNILUSCALE pfnIluScale = NULL;
		static PFNILUIMAGEPARAMETER pfnIluImageParameter = NULL;
		static void LoadILU();
	public:
		static System::Drawing::Bitmap __gc* LoadBitmap(System::String __gc* i_szFileName);
		static System::Drawing::Bitmap __gc* LoadBitmapAndScale(System::String __gc* i_szFileName, 
													int i_iWidth, int i_iHeight, 
													DevILScaleFilter i_eFilter, DevILScaleKind i_eKind);
		static bool SaveBitmap(System::String __gc* i_szFileName, System::Drawing::Bitmap __gc* i_poBitmap);
		static DevILErrorCode GetErrorCode();
	};
}



void DevIL::DevIL::LoadILU()
{
	HMODULE hM = LoadLibrary("ILU.dll");

	if (hM)
	{
		pfnIluScale = (PFNILUSCALE)GetProcAddress(hM, "iluScale");
		pfnIluImageParameter = (PFNILUIMAGEPARAMETER)GetProcAddress(hM, "iluImageParameter");
		s_bIluLoaded = (pfnIluScale != NULL) && (pfnIluImageParameter != NULL);
	}
}



System::Drawing::Bitmap __gc* DevIL::DevIL::LoadBitmap(System::String __gc* i_szFileName)
{
	return LoadBitmapAndScale(i_szFileName, 0, 0, NEAREST, DO_NOT_SCALE);
}

DevIL::DevILErrorCode DevIL::DevIL::GetErrorCode()
{
	if (s_eErrCode == OK) 
	{
		return DevILErrorCode(ilGetError());
	}
	else
	{
		DevILErrorCode eErr = s_eErrCode;
		s_eErrCode = OK;
		return eErr;
	}
}

System::Drawing::Bitmap __gc* DevIL::DevIL::LoadBitmapAndScale(System::String __gc* i_szFileName, 
															   int i_iWidth, int i_iHeight, 
															   DevILScaleFilter i_eFilter,
															   DevILScaleKind i_eKind)
{
	if (!s_bInitDone)
	{
		ilInit();
		s_bInitDone = true;
	}
	ilGenImages(1, &s_iImageID);
	ilBindImage(s_iImageID);

	if (0==ilLoadImage(StringAutoMarshal(i_szFileName)))
	{
		return NULL;
	}

	int iW = ilGetInteger(IL_IMAGE_WIDTH);
	int iH = ilGetInteger(IL_IMAGE_HEIGHT);
	bool bResize = true;

	switch(i_eKind)
	{
		case WIDTH_AND_HEIGHT:
			iW = i_iWidth;
			iH = i_iHeight;
			break;
		case WIDTH_ONLY:
			iW = i_iWidth;
			break;
		case HEIGHT_ONLY:
			iH = i_iHeight;
			break;
		case KEEPRATIO_USING_WIDTH:
			iH = int(double(iH) * double(i_iWidth) / double(iW));
			iW = i_iWidth;
			break;
		case KEEPRATIO_USING_HEIGHT:
			iW = int(double(iW) * double(i_iHeight) / double(iH));
			iH = i_iHeight;
			break;
		case DO_NOT_SCALE:
			bResize = false;
			break;
		default:
			break;
	}

	System::Drawing::Bitmap __gc* pBmp = __gc new System::Drawing::Bitmap(iW, iH, System::Drawing::Imaging::PixelFormat::Format32bppArgb);				

	System::Drawing::Rectangle rect; rect.X = 0; rect.Y = 0; rect.Width = iW; rect.Height = iH;

	System::Drawing::Imaging::BitmapData __gc* pBd = pBmp->LockBits(rect, 
		System::Drawing::Imaging::ImageLockMode::WriteOnly,
		System::Drawing::Imaging::PixelFormat::Format32bppArgb);

	void* pScan0 = (void*)(pBd->Scan0);

	// this is dangerous bc we ignore stride.. but it works and is FAST! ;)

	if (bResize)
	{
		if (!s_bIluLoaded)
		{
			LoadILU();
		}
		
		if (s_bIluLoaded) 
		{
			pfnIluImageParameter(ILU_FILTER, (int)(i_eFilter));
			pfnIluScale(iW, iH, 1);
		}
		else
		{
			s_eErrCode = ILU_DLL_NOT_FOUND;
			return NULL;
		}
	}
	
	ilConvertImage(IL_BGRA, IL_UNSIGNED_BYTE); // support for non 32bit images..
	ilCopyPixels(0, 0, 0, iW, iH, 1, IL_BGRA, IL_UNSIGNED_BYTE, pScan0);
	
	ilDeleteImages(1, &s_iImageID);

	pBmp->UnlockBits(pBd);

	return pBmp;
}

bool DevIL::DevIL::SaveBitmap(System::String __gc* i_szFileName, System::Drawing::Bitmap __gc* i_poBitmap)
{
	if (!s_bInitDone)
	{
		ilInit();
		s_bInitDone = true;
	}
	ilGenImages(1, &s_iImageID);
	ilBindImage(s_iImageID);

	int iW = i_poBitmap->get_Width();
	int iH = i_poBitmap->get_Height();

	System::Drawing::Rectangle rect; rect.X = 0; rect.Y = 0; rect.Width = iW; rect.Height = iH;

	i_poBitmap->RotateFlip(System::Drawing::RotateFlipType::RotateNoneFlipY);

	System::Drawing::Imaging::BitmapData __gc* pBd = i_poBitmap->LockBits(rect, 
		System::Drawing::Imaging::ImageLockMode::ReadOnly,
		System::Drawing::Imaging::PixelFormat::Format32bppArgb);

	void* pScan0 = (void*)(pBd->Scan0);

	bool bSuccess = 0!= ilTexImage(iW, iH, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, pScan0);

	if (!bSuccess)
	{
		return false;
	}

	bool bRes = ilSaveImage(StringAutoMarshal(i_szFileName))!=0;

	ilDeleteImages(1, &s_iImageID);

	i_poBitmap->UnlockBits(pBd);
	i_poBitmap->RotateFlip(System::Drawing::RotateFlipType::RotateNoneFlipY);

	return bRes;
}

