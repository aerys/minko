//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Last modified: 02/09/2009
//
// Filename: src-IL/src/il_wdp.c
//
// Description: Reads a Microsoft HD Photo (.wdp or .hdp)
//   Based very much on the Microsoft HD Photo Device Porting Kit 1.0
//   available at 
//   http://www.microsoft.com/downloads/details.aspx?FamilyID=285eeffd-d86c-48c3-ab93-3abd5ee7f1ce&displaylang=en.
//
// Note: The license that the Device Porting Kit is under is not very clear.
//   Commentary on the license can be found at http://en.wikipedia.org/wiki/HD_Photo.
//   Lots of this code is taken from the examples in the DPK and from code
//   within the DPK itself.  For this reason, this file is not under the LGPL
//   license, unlike the rest of DevIL.
//
//-----------------------------------------------------------------------------


#include "il_internal.h"
#ifndef IL_NO_WDP
#include <WMPGlue.h>
#include "il_wdp.h"

#if defined(_WIN32) && defined(IL_USE_PRAGMA_LIBS)
	#if defined(_MSC_VER) || defined(__BORLANDC__)
		#ifndef _DEBUG
			#pragma comment(lib, "wmplib.lib")
		#else
			#pragma comment(lib, "wmplib-d.lib")
		#endif
	#endif
#endif


//! Reads a WDP file
ILboolean ilLoadWdp(ILconst_string FileName)
{
	ILHANDLE	WdpFile;
	ILboolean	bWdp = IL_FALSE;
	
	WdpFile = iopenr(FileName);
	if (WdpFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return bWdp;
	}

	bWdp = ilLoadWdpF(WdpFile);
	icloser(WdpFile);

	return bWdp;
}


//! Reads an already-opened WDP file
ILboolean ilLoadWdpF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;
	
	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadWdpInternal();
	iseek(FirstPos, IL_SEEK_SET);
	
	return bRet;
}


//! Reads from a memory "lump" that contains a WDP
ILboolean ilLoadWdpL(const void *Lump, ILuint Size)
{
	iSetInputLump(Lump, Size);
	return iLoadWdpInternal();
}

//@TODO: Put in ilPKImageEncode_WritePixels_DevIL?
ERR WriteDevILHeader(PKImageEncode* pIE)
{
    struct WMPStream* pS = pIE->pStream;

	if (IsEqualGUID(&GUID_PKPixelFormat24bppRGB, &pIE->guidPixFormat) || IsEqualGUID(&GUID_PKPixelFormat24bppBGR, &pIE->guidPixFormat))
    {
        pIE->cbPixel = 3;
    }
    else if (IsEqualGUID(&GUID_PKPixelFormat32bppBGRA, &pIE->guidPixFormat) 
        || IsEqualGUID(&GUID_PKPixelFormat32bppBGR, &pIE->guidPixFormat)
        || IsEqualGUID(&GUID_PKPixelFormat32bppPBGRA, &pIE->guidPixFormat))
    {
        pIE->cbPixel = 4;
    }
    else if (IsEqualGUID(&GUID_PKPixelFormat8bppGray, &pIE->guidPixFormat))
    {
        pIE->cbPixel = 1;
    }
	else if (IsEqualGUID(&GUID_PKPixelFormat16bppGray, &pIE->guidPixFormat))
    {
        pIE->cbPixel = 2;
    }
	else if (IsEqualGUID(&GUID_PKPixelFormat128bppRGBAFloat, &pIE->guidPixFormat))
    {
        pIE->cbPixel = 16;//4;
    }


	pIE->offPixel = pIE->offStart;
    pIE->fHeaderDone = !IL_FALSE;

    return WMP_errSuccess;
}

ERR ilPKImageEncode_WritePixels_DevIL(PKImageEncode* pIE, U32 cLine, U8* pbPixel, U32 cbStride)
{
    ERR err = WMP_errSuccess;

    struct WMPStream* pS = pIE->pStream;
    size_t cbLineM = 0, cbLineS = 0;
    I32 i = 0;
    static U8 pPadding[4] = {0};

    // header
    if (!pIE->fHeaderDone)
    {
        // WriteBMPHeader() also inits this object
        Call(WriteDevILHeader(pIE));
    }

    // body
    // calculate line size in memory and in stream
    cbLineM = pIE->cbPixel * pIE->uWidth;
    cbLineS = (cbLineM + 3) / 4 * 4;

    //FailIf(pRect->X < 0 || pID->uWidth <= pRect->X, WMP_errInvalidParameter);
    //FailIf(pRect->Y < 0 || pID->uHeight <= pRect->Y, WMP_errInvalidParameter);
    //FailIf(pRect->Width < 0 || pID->uWidth < pRect->X + pRect->Width, WMP_errInvalidParameter);
    //FailIf(pRect->Height < 0 || pID->uHeight < pRect->Y + pRect->Height, WMP_errInvalidParameter);
    FailIf(cbStride < cbLineM, WMP_errInvalidParameter);

    for (i = cLine - 1; 0 <= i; --i)
    {
        size_t offM = cbStride * i;
        size_t offS = cbLineS * (pIE->uHeight - (pIE->idxCurrentLine + i + 1));

        Call(pS->SetPos(pS, pIE->offPixel + offS));
        Call(pS->Write(pS, pbPixel + offM, cbLineM));
    }
    Call(pS->Write(pS, pPadding, (cbLineS - cbLineM)));
    pIE->idxCurrentLine += cLine;

Cleanup:
    return err;
}


ERR PKImageEncode_Create_DevIL(
    PKImageEncode** ppIE)
{
    ERR err = WMP_errSuccess;
    PKImageEncode* pIE = NULL;

    Call(PKImageEncode_Create(ppIE));

    pIE = *ppIE;
    pIE->WritePixels = ilPKImageEncode_WritePixels_DevIL;

Cleanup:
    return err;
}


ERR iWmpDecAppCreateEncoderFromExt(
    PKCodecFactory* pCFactory,
    const char* szExt,
    PKImageEncode** ppIE)
{
    ERR err = WMP_errSuccess;
    const PKIID* pIID = NULL;

    // get encod PKIID
    Call(GetImageEncodeIID(szExt, &pIID));

    // Create encoder
    //Call(PKCodecFactory_CreateCodec(pIID, ppIE));

    Call(PKImageEncode_Create_DevIL(ppIE));

Cleanup:
    return err;
}


ERR iCloseWS_File(struct WMPStream** ppWS)
{
    ERR err = WMP_errSuccess;
    /*struct WMPStream* pWS = *ppWS;

    fclose(pWS->state.file.pFile);
    Call(WMPFree((void**)ppWS));

Cleanup:*/
    return err;
}

Bool iEOSWS_File(struct WMPStream* pWS)
{
    //return feof(pWS->state.file.pFile);
	return ieof();
}

ERR iReadWS_File(struct WMPStream* pWS, void* pv, size_t cb)
{
	// For some reason, the WDP images load just fine, but it tries to read too much,
	//  so IL_FILE_READ_ERROR is set.  So we get rid of the error.
	if (iread(pv, 1, (ILuint)cb) != cb)
		ilGetError();
    return WMP_errSuccess;
}

ERR iWriteWS_File(struct WMPStream* pWS, const void* pv, size_t cb)
{
    ERR err = WMP_errSuccess;

    if (0 != cb) {
		FailIf(1 != iwrite(pv, (ILuint)cb, 1), WMP_errFileIO);
    }

Cleanup:
    return err;
}

ERR iSetPosWS_File(struct WMPStream* pWS, size_t offPos)
{
    ERR err = WMP_errSuccess;

    //FailIf(0 != fseek(pWS->state.file.pFile, (long)offPos, SEEK_SET), WMP_errFileIO);
	FailIf(0 != iseek((ILuint)offPos, IL_SEEK_SET), WMP_errFileIO);

Cleanup:
    return err;
}

ERR iGetPosWS_File(struct WMPStream* pWS, size_t* poffPos)
{
    ERR err = WMP_errSuccess;
    long lOff = 0;

    //FailIf(-1 == (lOff = ftell(pWS->state.file.pFile)), WMP_errFileIO);
	lOff = itell();
    *poffPos = (size_t)lOff;

Cleanup:
    return err;
}

ERR ilCreateWS_File(struct WMPStream** ppWS, const char* szFilename, const char* szMode)
{
    ERR err = WMP_errSuccess;
    struct WMPStream* pWS = NULL;

	*ppWS = icalloc(1, sizeof(**ppWS));
	if (*ppWS == NULL)
		return WMP_errOutOfMemory;
    pWS = *ppWS;

    pWS->Close = iCloseWS_File;
    pWS->EOS = iEOSWS_File;

    pWS->Read = iReadWS_File;
    pWS->Write = iWriteWS_File;
    //pWS->GetLine = GetLineWS_File;

    pWS->SetPos = iSetPosWS_File;
    pWS->GetPos = iGetPosWS_File;

    //pWS->state.file.pFile = fopen(szFilename, szMode);
	pWS->state.file.pFile = NULL;
    //FailIf(NULL == pWS->state.file.pFile, WMP_errFileIO);

Cleanup:    
    return err;
}


ERR ilPKCodecFactory_CreateDecoderFromFile(PKImageDecode** ppDecoder)
{
    ERR err = WMP_errSuccess;

    char *pExt = ".wdp";  // We are loading a WDP file, so we have to tell the library that with this extension.
    PKIID* pIID = NULL;

    struct WMPStream* pStream = NULL;
    PKImageDecode* pDecoder = NULL;

    // get decode PKIID
    Call(GetImageDecodeIID(pExt, &pIID));

    // create stream
    Call(ilCreateWS_File(&pStream, NULL, "rb"));

    // Create decoder
    Call(PKCodecFactory_CreateCodec(pIID, ppDecoder));
    pDecoder = *ppDecoder;

    // attach stream to decoder
    Call(pDecoder->Initialize(pDecoder, pStream));
    pDecoder->fStreamOwner = !0;

Cleanup:
    return err;
}


ERR ilPKCreateFactory(PKFactory** ppFactory, U32 uVersion)
{
    ERR err = WMP_errSuccess;
    PKFactory* pFactory = NULL;

    Call(PKAlloc(ppFactory, sizeof(**ppFactory)));
    pFactory = *ppFactory;

    pFactory->CreateStream = PKCreateFactory_CreateStream;

    pFactory->CreateStreamFromFilename = ilCreateWS_File;
    pFactory->CreateStreamFromMemory = CreateWS_Memory;
    
    pFactory->Release = PKCreateFactory_Release;

Cleanup:
    return err;
}

ILboolean iLoadWdpInternal(/*ILconst_string FileName*/)
{
	ERR err = WMP_errSuccess;
	PKFactory* pFactory = NULL;
	PKCodecFactory* pCodecFactory = NULL;
	PKImageDecode* pDecoder = NULL;
    PKPixelInfo PI;
	PKPixelFormatGUID guidPixFormat;
	PKFormatConverter* pConverter = NULL;
    U32 cFrame = 0, i = 0;
	PKRect Rect;
    struct WMPStream* pEncodeStream = NULL;
    PKImageEncode* pEncoder = NULL;

	//Call(PKCreateFactory(&pFactory, PK_SDK_VERSION));
	//Call(PKCreateCodecFactory(&pCodecFactory, WMP_SDK_VERSION));
	//Call(pCodecFactory->CreateDecoderFromFile(FileName, &pDecoder));
	Call(ilPKCreateFactory(&pFactory, PK_SDK_VERSION));
	Call(PKCreateCodecFactory(&pCodecFactory, WMP_SDK_VERSION));
	Call(ilPKCodecFactory_CreateDecoderFromFile(&pDecoder));

	//guidPixFormat = GUID_PKPixelFormat24bppRGB;
	guidPixFormat = GUID_PKPixelFormat32bppBGRA;
	//guidPixFormat = GUID_PKPixelFormat8bppGray;
	//guidPixFormat = GUID_PKPixelFormat16bppGray;

    // Color transcoding
    if (IsEqualGUID(&guidPixFormat, &GUID_PKPixelFormat8bppGray) || IsEqualGUID(&guidPixFormat, &GUID_PKPixelFormat16bppGray)){ // ** => Y transcoding
        pDecoder->guidPixFormat = guidPixFormat;
        pDecoder->WMP.wmiI.cfColorFormat = Y_ONLY;
    }
	else if(IsEqualGUID(&guidPixFormat, &GUID_PKPixelFormat24bppRGB) && pDecoder->WMP.wmiI.cfColorFormat == CMYK){ // CMYK = > RGB
		pDecoder->WMP.wmiI.cfColorFormat = CF_RGB;
		pDecoder->guidPixFormat = guidPixFormat;
		pDecoder->WMP.wmiI.bRGB = 1; //RGB
	}

	PI.pGUIDPixFmt = &guidPixFormat;
    PixelFormatLookup(&PI, LOOKUP_FORWARD);

    pDecoder->WMP.wmiSCP.bfBitstreamFormat = 0;
    pDecoder->WMP.wmiSCP.uAlphaMode = 0;
    pDecoder->WMP.wmiSCP.sbSubband = SB_ALL;
    pDecoder->WMP.bIgnoreOverlap = FALSE;

    pDecoder->WMP.wmiI.cfColorFormat = PI.cfColorFormat;

    pDecoder->WMP.wmiI.bdBitDepth = PI.bdBitDepth;
    pDecoder->WMP.wmiI.cBitsPerUnit = PI.cbitUnit;

	//==== Validate thumbnail decode parameters =====
    pDecoder->WMP.wmiI.cThumbnailWidth = pDecoder->WMP.wmiI.cWidth;
    pDecoder->WMP.wmiI.cThumbnailHeight = pDecoder->WMP.wmiI.cHeight;
    pDecoder->WMP.wmiI.bSkipFlexbits = FALSE;

	pCodecFactory->CreateFormatConverter(&pConverter);
	pConverter->Initialize(pConverter, pDecoder, NULL, guidPixFormat);

	// Right now, we are just assuming one frame.
	// @TODO: Deal with multiple frames.
    //pDecoder->GetFrameCount(pDecoder, &cFrame);
	//pDecoder->SelectFrame(pDecoder, 1);

	if (!ilTexImage(pDecoder->uWidth, pDecoder->uHeight, 1, 4, IL_BGRA, IL_UNSIGNED_BYTE, NULL))
		goto Cleanup;
	//ilTexImage(pDecoder->uWidth, pDecoder->uHeight, 1, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, Data);

	pFactory->CreateStreamFromMemory(&pEncodeStream, iCurImage->Data, iCurImage->SizeOfData);
    iWmpDecAppCreateEncoderFromExt(pCodecFactory, ".wdp", &pEncoder);
	pEncoder->Initialize(pEncoder, pEncodeStream, ".wdp", 0);

    pEncoder->pStream->GetPos(pEncoder->pStream, &pEncoder->offStart);

	// Set the region that we want to be the whole image.
	Rect.X = 0; Rect.Y = 0; Rect.Height = pDecoder->uHeight; Rect.Width = pDecoder->uWidth;
	pEncoder->SetPixelFormat(pEncoder, guidPixFormat);
    pEncoder->SetSize(pEncoder, Rect.Width, Rect.Height);
	pEncoder->WriteSource = PKImageEncode_Transcode;
    pEncoder->WriteSource(pEncoder, pConverter, &Rect);


Cleanup:
	// Release everything all at the end.
	PKImageDecode_Release(&pDecoder);
	if (pEncoder)
		PKImageEncode_Release(&pEncoder);
	PKCreateCodecFactory_Release(&pCodecFactory);
	PKCreateFactory_Release(&pFactory);
	PKFormatConverter_Release(&pConverter);

	if (err != WMP_errSuccess)
		return IL_FALSE;
	return IL_TRUE;
}

#endif//IL_NO_WDP
