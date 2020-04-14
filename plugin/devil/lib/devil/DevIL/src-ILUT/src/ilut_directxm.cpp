//-----------------------------------------------------------------------------
//
// ImageLib Utility Toolkit Sources
// Copyright (C) 2000-2017 by Denton Woods
// Last modified: 12/25/2001
//
// Filename: src-ILUT/src/ilut_directxm.cpp
//
// Description: DirectX functions for textures
//
//-----------------------------------------------------------------------------

#include "ilut_internal.h"
#ifdef ILUT_USE_DIRECTXM

#include <d3dm.h>
#include <d3dmtypes.h>

#pragma comment(linker, "/nodefaultlib:oldnames.lib")
#pragma comment(lib, "d3dm.lib")


ILimage*	MakeD3DmCompliant(IDirect3DMobileDevice *Device, D3DMFORMAT *DestFormat);
ILenum		GetD3DmCompat(ILenum Format);
D3DMFORMAT	GetD3DFormat(ILenum Format);
ILboolean	iD3DmCreateMipmaps(IDirect3DMobileTexture *Texture, ILimage *Image);
IDirect3DMobileTexture*  ilutD3DmTexture(IDirect3DMobileDevice *Device);

ILboolean	FormatsDXmChecked = IL_FALSE;
ILboolean	FormatsDXmsupported[6] = 
{ 
	IL_FALSE, 
	IL_FALSE, 
	IL_FALSE, 
	IL_FALSE, 
	IL_FALSE, 
	IL_FALSE 
};

D3DMFORMAT	FormatsDXm[6] =
{ 
	D3DMFMT_R8G8B8, 
	D3DMFMT_A8R8G8B8, 
	//D3DMFMT_L8, 
	D3DMFMT_DXT1, 
	D3DMFMT_DXT3, 
	D3DMFMT_DXT5 
};


ILboolean ilutD3DmInit()
{
	return IL_TRUE;
}


ILvoid CheckFormatsDXm(IDirect3DMobileDevice *Device)
{
	D3DMDISPLAYMODE	DispMode;
	HRESULT			hr;
	IDirect3DMobile	*TestD3Dm;
	ILuint			i;

	IDirect3DMobileDevice_GetDirect3D(Device, (IDirect3DMobile**)&TestD3Dm);
	IDirect3DMobileDevice_GetDisplayMode(Device, &DispMode);

	for (i = 0; i < 6; i++) {
		hr = IDirect3DMobile_CheckDeviceFormat(TestD3Dm, D3DMADAPTER_DEFAULT,
			D3DMDEVTYPE_DEFAULT, DispMode.Format, 0, D3DMRTYPE_TEXTURE, FormatsDXm[i]);
		FormatsDXmsupported[i] = SUCCEEDED(hr);
	}

	IDirect3DMobileDevice_Release(TestD3Dm);
	FormatsDXmChecked = IL_TRUE;

	return;
}


ILboolean ILAPIENTRY ilutD3DmTexFromFile(IDirect3DMobileDevice *Device, TCHAR *FileName, IDirect3DMobileTexture **Texture)
{
	iBindImageTemp();
	if (!ilLoadImage(FileName))
		return IL_FALSE;

	*Texture = ilutD3DmTexture(Device);

	return IL_TRUE;
}


#ifndef _WIN32_WCE
ILboolean ILAPIENTRY ilutD3DmVolTexFromFile(IDirect3DMobileDevice *Device, TCHAR *FileName, IDirect3DVolumeTexture8 **Texture)
{
	iBindImageTemp();
	if (!ilLoadImage(FileName))
		return IL_FALSE;

	*Texture = ilutD3DmVolumeTexture(Device);

	return IL_TRUE;
}
#endif//_WIN32_WCE


ILboolean ILAPIENTRY ilutD3DmTexFromFileInMemory(IDirect3DMobileDevice *Device, ILvoid *Lump, ILuint Size, IDirect3DMobileTexture **Texture)
{
	iBindImageTemp();
	if (!ilLoadL(IL_TYPE_UNKNOWN, Lump, Size))
		return IL_FALSE;

	*Texture = ilutD3DmTexture(Device);

	return IL_TRUE;
}


//ILboolean ILAPIENTRY ilutD3DmVolTexFromFileInMemory(IDirect3DMobileDevice *Device, ILvoid *Lump, ILuint Size, IDirect3DVolumeTexture8 **Texture)
//{
//	iBindImageTemp();
//	if (!ilLoadL(IL_TYPE_UNKNOWN, Lump, Size))
//		return IL_FALSE;
//
//	*Texture = ilutD3DmVolumeTexture(Device);
//
//	return IL_TRUE;
//}


ILboolean ILAPIENTRY ilutD3DmTexFromResource(IDirect3DMobileDevice *Device, HMODULE SrcModule, TCHAR *SrcResource, IDirect3DMobileTexture **Texture)
{
	HRSRC	Resource;
	ILubyte	*Data;

	iBindImageTemp();

	Resource = (HRSRC)LoadResource(SrcModule, FindResource(SrcModule, SrcResource, RT_BITMAP));
	Data = (ILubyte*)LockResource(Resource);
	if (!ilLoadL(IL_TYPE_UNKNOWN, Data, SizeofResource(SrcModule, FindResource(SrcModule, SrcResource, RT_BITMAP))))
		return IL_FALSE;

	*Texture = ilutD3DmTexture(Device);

	return IL_TRUE;
}


//ILboolean ILAPIENTRY ilutD3DmVolTexFromResource(IDirect3DMobileDevice *Device, HMODULE SrcModule, TCHAR *SrcResource, IDirect3DVolumeTexture8 **Texture)
//{
//	HRSRC	Resource;
//	ILubyte	*Data;
//
//	iBindImageTemp();
//
//	Resource = (HRSRC)LoadResource(SrcModule, FindResource(SrcModule, SrcResource, RT_BITMAP));
//	Data = (ILubyte*)LockResource(Resource);
//	if (!ilLoadL(IL_TYPE_UNKNOWN, Data, SizeofResource(SrcModule, FindResource(SrcModule, SrcResource, RT_BITMAP))))
//		return IL_FALSE;
//
//	*Texture = ilutD3DmVolumeTexture(Device);
//
//	return IL_TRUE;
//}


ILboolean ILAPIENTRY ilutD3DmTexFromFileHandle(IDirect3DMobileDevice *Device, ILHANDLE File, IDirect3DMobileTexture **Texture)
{
	iBindImageTemp();
	if (!ilLoadF(IL_TYPE_UNKNOWN, File))
		return IL_FALSE;

	*Texture = ilutD3DmTexture(Device);

	return IL_TRUE;
}


//ILboolean ILAPIENTRY ilutD3DmVolTexFromFileHandle(IDirect3DMobileDevice *Device, ILHANDLE File, IDirect3DVolumeTexture8 **Texture)
//{
//	iBindImageTemp();
//	if (!ilLoadF(IL_TYPE_UNKNOWN, File))
//		return IL_FALSE;
//
//	*Texture = ilutD3DmVolumeTexture(Device);
//
//	return IL_TRUE;
//}


D3DMFORMAT D3DGetDXTCNumDXm(ILenum DXTCFormat)
{
	switch (DXTCFormat)
	{
		case IL_DXT1: return D3DMFMT_DXT1;
		case IL_DXT3: return D3DMFMT_DXT3;
		case IL_DXT5: return D3DMFMT_DXT5;
	}
	return 0;
}


IDirect3DMobileTexture* ILAPIENTRY ilutD3DmTexture(IDirect3DMobileDevice *Device)
{
	IDirect3DMobileTexture *Texture;
	D3DMLOCKED_RECT Rect;
	D3DMFORMAT Format;
	ILimage	*Image;
	ILenum	DXTCFormat;
	ILuint	Size;
	ILubyte	*Buffer;

	Image = ilutCurImage = ilGetCurImage();
	if (ilutCurImage == NULL) {
		ilSetError(ILUT_ILLEGAL_OPERATION);
		return NULL;
	}

	if (!FormatsDXmChecked)
		CheckFormatsDXm(Device);

	if (ilutGetBoolean(ILUT_D3D_USE_DXTC) && FormatsDXmsupported[3] && FormatsDXmsupported[4] && FormatsDXmsupported[5]) {
		if (ilutCurImage->DxtcData != NULL && ilutCurImage->DxtcSize != 0) {
			Format = D3DGetDXTCNumDXm(ilutCurImage->DxtcFormat);

			if (FAILED(IDirect3DMobileDevice_CreateTexture(Device, ilutCurImage->Width,
				ilutCurImage->Height, ilutGetInteger(ILUT_D3D_MIPLEVELS), 0, Format,
				ilutGetInteger(ILUT_D3D_POOL), &Texture)))
					return NULL;
			if (FAILED(IDirect3DMobileTexture_LockRect(Texture, 0, &Rect, NULL, 0)))
				return NULL;
			memcpy(Rect.pBits, ilutCurImage->DxtcData, ilutCurImage->DxtcSize);
			goto success;
		}

		if (ilutGetBoolean(ILUT_D3D_GEN_DXTC)) {
			DXTCFormat = ilutGetInteger(ILUT_DXTC_FORMAT);

			Size = ilGetDXTCData(NULL, 0, DXTCFormat);
			if (Size != 0) {
				Buffer = (ILubyte*)ialloc(Size);
				if (Buffer == NULL)
					return NULL;
				Size = ilGetDXTCData(Buffer, Size, DXTCFormat);
				if (Size == 0) {
					ifree(Buffer);
					return NULL;
				}

				Format = D3DGetDXTCNumDXm(DXTCFormat);
				if (FAILED(IDirect3DMobileDevice_CreateTexture(Device, ilutCurImage->Width,
					ilutCurImage->Height, ilutGetInteger(ILUT_D3D_MIPLEVELS), 0, Format,
					ilutGetInteger(ILUT_D3D_POOL), &Texture))) {
						ifree(Buffer);
						return NULL;
				}
				if (FAILED(IDirect3DMobileTexture_LockRect(Texture, 0, &Rect, NULL, 0))) {
					ifree(Buffer);
					return NULL;
				}
				memcpy(Rect.pBits, Buffer, Size);
				ifree(Buffer);
				goto success;
			}
		}
	}

	Image = MakeD3DmCompliant(Device, &Format);
	if (Image == NULL) {
		if (Image != ilutCurImage)
			ilCloseImage(Image);
		return NULL;
	}
	if (FAILED(IDirect3DMobileDevice_CreateTexture(Device, Image->Width, Image->Height,
		ilutGetInteger(ILUT_D3D_MIPLEVELS), 0, Format, ilutGetInteger(ILUT_D3D_POOL), &Texture))) {
		if (Image != ilutCurImage)
			ilCloseImage(Image);
		return NULL;
	}
	if (FAILED(IDirect3DMobileTexture_LockRect(Texture, 0, &Rect, NULL, 0)))
		return NULL;
	memcpy(Rect.pBits, Image->Data, Image->SizeOfPlane);

success:
	IDirect3DMobileTexture_UnlockRect(Texture, 0);
	// Just let D3DX filter for us.
	//D3DXFilterTexture(Texture, NULL, D3DX_DEFAULT, D3DX_FILTER_BOX);
	iD3DmCreateMipmaps(Texture, Image);

	if (Image != ilutCurImage)
		ilCloseImage(Image);

	return Texture;
}


//IDirect3DVolumeTexture8* ILAPIENTRY ilutD3DmVolumeTexture(IDirect3DMobileDevice *Device)
//{
//	IDirect3DVolumeTexture8	*Texture;
//	D3DLOCKED_BOX	Box;
//	D3DMFORMAT		Format;
//	ILimage			*Image;
//
//	ilutCurImage = ilGetCurImage();
//	if (ilutCurImage == NULL) {
//		ilSetError(ILUT_ILLEGAL_OPERATION);
//		return NULL;
//	}
//
//	if (!FormatsDXmChecked)
//		CheckFormatsDXm(Device);
//
//	Image = MakeD3DmCompliant(Device, &Format);
//	if (Image == NULL)
//		return NULL;
//	if (FAILED(IDirect3DMobileDevice_CreateVolumeTexture(Device, Image->Width, Image->Height,
//		Image->Depth, 1, 0, Format, ilutGetInteger(ILUT_D3D_POOL), &Texture)))
//		return NULL;	
//	if (FAILED(IDirect3DVolumeTexture8_LockBox(Texture, 0, &Box, NULL, 0)))
//		return NULL;
//
//	memcpy(Box.pBits, Image->Data, Image->SizeOfData);
//	if (!IDirect3DVolumeTexture8_UnlockBox(Texture, 0))
//		return IL_FALSE;
//
//	// We don't want to have mipmaps for such a large image.
//
//	if (Image != ilutCurImage)
//		ilCloseImage(Image);
//
//	return Texture;
//}


ILimage *MakeD3DmCompliant(IDirect3DMobileDevice *Device, D3DMFORMAT *DestFormat)
{
	ILimage	*Converted, *Scaled, *CurImage;

	*DestFormat = D3DMFMT_A8R8G8B8;

	// Images must be in BGRA format.
	if (ilutCurImage->Format != IL_BGRA) {
		Converted = iConvertImage(ilutCurImage, IL_BGRA, IL_UNSIGNED_BYTE);
		if (Converted == NULL)
			return NULL;
	}
	else {
		Converted = ilutCurImage;
	}

	// Images must have their origin in the upper left.
	if (Converted->Origin != IL_ORIGIN_UPPER_LEFT) {
		CurImage = ilutCurImage;
		ilSetCurImage(Converted);
		iluFlipImage();
		ilSetCurImage(CurImage);
	}

	// Images must have powers-of-2 dimensions.
	if (ilNextPower2(ilutCurImage->Width) != ilutCurImage->Width ||
		ilNextPower2(ilutCurImage->Height) != ilutCurImage->Height ||
		ilNextPower2(ilutCurImage->Depth) != ilutCurImage->Depth) {
			Scaled = iluScale_(Converted, ilNextPower2(ilutCurImage->Width),
						ilNextPower2(ilutCurImage->Height), ilNextPower2(ilutCurImage->Depth));
			if (Converted != ilutCurImage) {
				ilCloseImage(Converted);
			}
			if (Scaled == NULL) {
				return NULL;
			}
			Converted = Scaled;
	}

	return Converted;
}


ILboolean iD3DmCreateMipmaps(IDirect3DMobileTexture *Texture, ILimage *Image)
{
	D3DMLOCKED_RECT		Rect;
	D3DMSURFACE_DESC	Desc;
	ILuint				NumMips, Width, Height, i;
	ILimage				*CurImage, *MipImage, *Temp;

	NumMips = IDirect3DMobileTexture_GetLevelCount(Texture);
	Width = Image->Width;
	Height = Image->Height;

	CurImage = ilGetCurImage();
	MipImage = ilCopyImage_(CurImage);
	ilSetCurImage(MipImage);
	if (!iluBuildMipmaps()) {
		ilCloseImage(MipImage);
		ilSetCurImage(CurImage);
		return IL_FALSE;
	}
	ilSetCurImage(CurImage);
	Temp = MipImage->Mipmaps;

	// Counts the base texture as 1.
	for (i = 1; i < NumMips && Temp != NULL; i++) {
		if (FAILED(IDirect3DMobileTexture_LockRect(Texture, i, &Rect, NULL, 0)))
			return IL_FALSE;

		Width = IL_MAX(1, Width / 2);
		Height = IL_MAX(1, Height / 2);

		IDirect3DMobileTexture_GetLevelDesc(Texture, i, &Desc);
		if (Desc.Width != Width || Desc.Height != Height) {
			IDirect3DMobileTexture_UnlockRect(Texture, i);
			return IL_FALSE;
		}

		memcpy(Rect.pBits, Temp->Data, Temp->SizeOfData);

		IDirect3DMobileTexture_UnlockRect(Texture, i);
		Temp = Temp->Next;
	}

	ilCloseImage(MipImage);

	return IL_TRUE;
}


//
// SaveSurfaceToFile.cpp
//
// Copyright (c) 2001 David Galeano
//
// Permission to use, copy, modify and distribute this software
// is hereby granted, provided that both the copyright notice and 
// this permission notice appear in all copies of the software, 
// derivative works or modified versions.
//
// THE AUTHOR ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
// CONDITION AND DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES 
// WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
//

ILAPI ILboolean ILAPIENTRY ilutD3DmLoadSurface(IDirect3DMobileDevice *Device, IDirect3DMobileSurface *Surface)
{
	HRESULT					hr;
	D3DMSURFACE_DESC		d3dsd;
	LPDIRECT3DMOBILESURFACE	SurfaceCopy;
	D3DMLOCKED_RECT			d3dLR;
	ILboolean				bHasAlpha;
	ILubyte					*Image, *ImageAux, *Data;
	ILuint					y, x;
	ILushort				dwColor;

	IDirect3DMobileSurface_GetDesc(Surface, &d3dsd);

	bHasAlpha = (d3dsd.Format == D3DMFMT_A8R8G8B8 || d3dsd.Format == D3DMFMT_A1R5G5B5);

	if (bHasAlpha) {
		if (!ilTexImage(d3dsd.Width, d3dsd.Height, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL)) {
			return IL_FALSE;
		}
	}
	else {
		if (!ilTexImage(d3dsd.Width, d3dsd.Height, 1, 3, IL_BGR, IL_UNSIGNED_BYTE, NULL)) {
			return IL_FALSE;
		}
	}

	hr = IDirect3DMobileDevice_CreateImageSurface(Device, d3dsd.Width, d3dsd.Height, d3dsd.Format, &SurfaceCopy);
	if (FAILED(hr)) {
		ilSetError(ILUT_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	hr = IDirect3DMobileDevice_CopyRects(Device, Surface, NULL, 0, SurfaceCopy, NULL);
	if (FAILED(hr)) {
		ilSetError(ILUT_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	hr = IDirect3DMobileSurface_LockRect(SurfaceCopy, &d3dLR, NULL, D3DMLOCK_NO_DIRTY_UPDATE | D3DMLOCK_READONLY);
	if (FAILED(hr)) {
		IDirect3DMobileSurface_Release(SurfaceCopy);
		ilSetError(ILUT_ILLEGAL_OPERATION);
		return IL_FALSE;
	}

	Image = (ILubyte*)d3dLR.pBits;
	Data = ilutCurImage->Data;

	for (y = 0; y < d3dsd.Height; y++) {
		if (d3dsd.Format == D3DMFMT_X8R8G8B8) {
			ImageAux = Image;
			for (x = 0; x < d3dsd.Width; x++) {
				Data[0] = ImageAux[0];
				Data[1] = ImageAux[1];
				Data[2] = ImageAux[2];

				Data += 3;
				ImageAux += 4;
			}
		}
		else if (d3dsd.Format == D3DMFMT_A8R8G8B8) {
			memcpy(Data, Image, d3dsd.Width * 4);
		}
		else if (d3dsd.Format == D3DMFMT_R5G6B5) {
			ImageAux = Image;
			for (x = 0; x < d3dsd.Width; x++) {
				dwColor = *((ILushort*)ImageAux);

				Data[0] = (ILubyte)((dwColor&0x001f)<<3);
				Data[1] = (ILubyte)(((dwColor&0x7e0)>>5)<<2);
				Data[2] = (ILubyte)(((dwColor&0xf800)>>11)<<3);

				Data += 3;
				ImageAux += 2;
			}
		}
		else if (d3dsd.Format == D3DMFMT_X1R5G5B5) {
			ImageAux = Image;
			for (x = 0; x < d3dsd.Width; x++) {
				dwColor = *((ILushort*)ImageAux);

				Data[0] = (ILubyte)((dwColor&0x001f)<<3);
				Data[1] = (ILubyte)(((dwColor&0x3e0)>>5)<<3);
				Data[2] = (ILubyte)(((dwColor&0x7c00)>>10)<<3);

				Data += 3;
				ImageAux += 2;
			}
		}
		else if (d3dsd.Format == D3DMFMT_A1R5G5B5) {
			ImageAux = Image;
			for (x = 0; x < d3dsd.Width; x++) {
				dwColor = *((ILushort*)ImageAux);

				Data[0] = (ILubyte)((dwColor&0x001f)<<3);
				Data[1] = (ILubyte)(((dwColor&0x3e0)>>5)<<3);
				Data[2] = (ILubyte)(((dwColor&0x7c00)>>10)<<3);
				Data[3] = (ILubyte)(((dwColor&0x8000)>>15)*255);

				Data += 4;
				ImageAux += 2;
			}
		}

		Image += d3dLR.Pitch;
	}

	IDirect3DMobileSurface_UnlockRect(SurfaceCopy);
	IDirect3DMobileSurface_Release(SurfaceCopy);

	return IL_TRUE;
}

#endif//ILUT_USE_DIRECTXM
