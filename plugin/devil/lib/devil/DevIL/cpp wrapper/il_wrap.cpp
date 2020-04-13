#define _IL_WRAP_BUILD_LIB
#include <il/il_wrap.h>

//#define TEST_WRAPPER  // Comment-out to get rid of main()

#ifdef TEST_WRAPPER
#include <iostream.h>

int main(int argc, char **argv)
{
	if (argc < 2) {
		cout << "Please specify a filename." << endl;
		return 1;
	}

	ilImage Image(argv[1]);

	cout << Image.Width() << ' ' << Image.Height() << ' ' << (ILuint)Image.Bpp() << endl;

	ilEnable(IL_FILE_OVERWRITE);

	Image.Save("test.tga");

	return 0;
}
#endif


//
//
// ILIMAGE
//
//

ilImage::ilImage()
{
	this->Id = 0;
	//this->iStartUp();
	this->iGenBind();
	return;
}


ilImage::ilImage(char *FileName)
{
	this->Id = 0;
	//this->iStartUp();
	this->iGenBind();
	ilLoadImage(FileName);
	return;
}


ilImage::ilImage(const ilImage &Image)
{
	this->Id = 0;
	//this->iStartUp();
	this->iGenBind();
	*this = Image;
	return;
}


ilImage::~ilImage()
{
	if (this->Id)
		ilDeleteImages(1, &this->Id);
	this->Id = 0;
	return;
}


ILboolean ilImage::Load(char *FileName)
{
	this->iGenBind();
	return ilLoadImage(FileName);
}


ILboolean ilImage::Load(char *FileName, ILenum Type)
{
	this->iGenBind();
	return ilLoad(Type, FileName);
}


ILboolean ilImage::Save(char *FileName)
{
	this->iGenBind();
	return ilSaveImage(FileName);
}


ILboolean ilImage::Save(char *FileName, ILenum Type)
{
	this->iGenBind();
	return ilSave(Type, FileName);
}



//
// ImageLib functions
//

ILboolean ilImage::ActiveImage(ILuint Number)
{
	if (this->Id) {
		this->Bind();
		return ilActiveImage(Number);
	}
	return IL_FALSE;
}


ILboolean ilImage::ActiveLayer(ILuint Number)
{
	if (this->Id) {
		this->Bind();
		return ilActiveLayer(Number);
	}
	return IL_FALSE;
}


ILboolean ilImage::ActiveMipmap(ILuint Number)
{
	if (this->Id) {
		this->Bind();
		return ilActiveMipmap(Number);
	}
	return IL_FALSE;
}


ILboolean ilImage::Clear()
{
	if (this->Id) {
		this->Bind();
		return ilClearImage();
	}
	return IL_FALSE;
}


ILvoid ilImage::ClearColour(ILclampf Red, ILclampf Green, ILclampf Blue, ILclampf Alpha)
{
	ilClearColour(Red, Green, Blue, Alpha);
	return;
}


ILboolean ilImage::Convert(ILenum NewFormat)
{
	if (this->Id) {
		this->Bind();
		return ilConvertImage(NewFormat, IL_UNSIGNED_BYTE);
	}
	return IL_FALSE;
}


ILboolean ilImage::Copy(ILuint Src)
{
	if (this->Id) {
		this->Bind();
		return ilCopyImage(Src);
	}
	return IL_FALSE;
}


ILboolean ilImage::Default()
{
	if (this->Id) {
		this->Bind();
		return ilDefaultImage();
	}
	return IL_FALSE;
}


ILboolean ilImage::Flip()
{
	if (this->Id) {
		this->Bind();
		return iluFlipImage();
	}
	return IL_FALSE;
}


ILboolean ilImage::SwapColours()
{
	if (this->Id) {
		this->Bind();
		return iluSwapColours();
	}
	return IL_FALSE;
}


ILboolean ilImage::Resize(ILuint Width, ILuint Height, ILuint Depth)
{
	if (this->Id) {
		this->Bind();
		return iluScale(Width, Height, Depth);
	}
	return IL_FALSE;
}


ILboolean ilImage::TexImage(ILuint Width, ILuint Height, ILuint Depth, ILubyte Bpp, ILenum Format, ILenum Type, ILvoid *Data)
{
	if (this->Id) {
		this->Bind();
		return ilTexImage(Width, Height, Depth, Bpp, Format, Type, Data);
	}
	return IL_FALSE;
}



//
// Image handling
//

ILvoid ilImage::Bind() const
{
	if (this->Id)
		ilBindImage(this->Id);
	return;
}


// Note:  Behaviour may be changed!
ILvoid ilImage::Bind(ILuint Image)
{
	if (this->Id == Image)
		return;
	this->Delete();  // Should we delete it?
	this->Id = Image;
	ilBindImage(this->Id);
	return;
}


ILvoid ilImage::Delete()
{
	if (this->Id == 0)
		return;
	ilDeleteImages(1, &this->Id);
	this->Id = 0;
	return;
}




//
// Image characteristics
//

ILuint ilImage::Width()
{
	if (this->Id) {
		this->Bind();
		return ilGetInteger(IL_IMAGE_WIDTH);
	}
	return 0;
}


ILuint ilImage::Height()
{
	if (this->Id) {
		this->Bind();
		return ilGetInteger(IL_IMAGE_HEIGHT);
	}
	return 0;
}


ILuint ilImage::Depth()
{
	if (this->Id) {
		this->Bind();
		return ilGetInteger(IL_IMAGE_DEPTH);
	}
	return 0;
}


ILubyte ilImage::Bpp()
{
	if (this->Id) {
		this->Bind();
		return ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);
	}
	return 0;
}


ILubyte ilImage::Bitpp()
{
	if (this->Id) {
		this->Bind();
		return ilGetInteger(IL_IMAGE_BITS_PER_PIXEL);
	}
	return 0;
}


ILenum ilImage::Format()
{
	if (this->Id) {
		this->Bind();
		return ilGetInteger(IL_IMAGE_FORMAT);
	}
	return 0;
}

ILenum ilImage::PaletteType()
{
	if (this->Id) {
		this->Bind();
		return ilGetInteger(IL_PALETTE_TYPE);
	}
	return 0;
}

ILenum ilImage::PaletteAlphaIndex()
{
	if (this->Id) {
		this->Bind();
		return ilGetInteger(IL_PNG_ALPHA_INDEX);
	}
	return 0;
}


ILenum ilImage::Type()
{
	if (this->Id) {
		this->Bind();
		return ilGetInteger(IL_IMAGE_TYPE);
	}
	return 0;
}


ILenum ilImage::NumImages()
{
	if (this->Id) {
		this->Bind();
		return ilGetInteger(IL_NUM_IMAGES);
	}
	return 0;
}


ILenum ilImage::NumMipmaps()
{
	if (this->Id) {
		this->Bind();
		return ilGetInteger(IL_NUM_MIPMAPS);
	}
	return 0;
}


ILuint ilImage::GetId() const
{
	return this->Id;
}

ILenum ilImage::GetOrigin(void)
{
    ILinfo Info;

	if (this->Id) {
		this->Bind();
        iluGetImageInfo(&Info);
		return Info.Origin;
	}
	return 0;
}

ILubyte* ilImage::GetData()
{
	if (this->Id) {
		this->Bind();
		return ilGetData();
	}
	return 0;
}


ILubyte* ilImage::GetPalette()
{
	if (this->Id) {
		this->Bind();
		return ilGetPalette();
	}
	return 0;
}



//
// Private members
//

ILvoid ilImage::iStartUp()
{
	ilInit();
	iluInit();
	ilutInit();
	return;
}


ILvoid ilImage::iGenBind()
{
	if (this->Id == 0) {
		ilGenImages(1, &this->Id);
	}
	ilBindImage(this->Id);
	return;
}



//
// Operators
//

ilImage& ilImage::operator = (ILuint Image)
{
	if (this->Id == 0)
		this->Id = Image;
	else {
		this->Bind();
		ilCopyImage(Image);
	}

	return *this;
}


ilImage& ilImage::operator = (const ilImage &Image)
{
	if (Id == 0)
		Id = Image.GetId();
	else {
		Bind();
		ilCopyImage(Image.GetId());
	}

	return *this;
}



//
//
// ILFILTERS
//
//

ILboolean ilFilters::Alienify(ilImage &Image)
{
	Image.Bind();
	return iluAlienify();
}


ILboolean ilFilters::BlurAvg(ilImage &Image, ILuint Iter)
{
	Image.Bind();
	return iluBlurAvg(Iter);
}



ILboolean ilFilters::BlurGaussian(ilImage &Image, ILuint Iter)
{
	Image.Bind();
	return iluBlurGaussian(Iter);
}


ILboolean ilFilters::Contrast(ilImage &Image, ILfloat Contrast)
{
	Image.Bind();
	return iluContrast(Contrast);
}


ILboolean ilFilters::EdgeDetectE(ilImage &Image)
{
	Image.Bind();
	return iluEdgeDetectP();
}


ILboolean ilFilters::EdgeDetectP(ilImage &Image)
{
	Image.Bind();
	return iluEdgeDetectP();
}


ILboolean ilFilters::EdgeDetectS(ilImage &Image)
{
	Image.Bind();
	return iluEdgeDetectS();
}


ILboolean ilFilters::Emboss(ilImage &Image)
{
	Image.Bind();
	return iluEmboss();
}


ILboolean ilFilters::Gamma(ilImage &Image, ILfloat Gamma)
{
	Image.Bind();
	return iluGammaCorrect(Gamma);
}


ILboolean ilFilters::Negative(ilImage &Image)
{
	Image.Bind();
	return iluNegative();
}


ILboolean ilFilters::Noisify(ilImage &Image, ILubyte Factor)
{
	Image.Bind();
	return iluNoisify(Factor);
}


ILboolean ilFilters::Pixelize(ilImage &Image, ILuint PixSize)
{
	Image.Bind();
	return iluPixelize(PixSize);
}


ILboolean ilFilters::Saturate(ilImage &Image, ILfloat Saturation)
{
	Image.Bind();
	return iluSaturate1f(Saturation);
}


ILboolean ilFilters::Saturate(ilImage &Image, ILfloat r, ILfloat g, ILfloat b, ILfloat Saturation)
{
	Image.Bind();
	return iluSaturate4f(r, g, b, Saturation);
}


ILboolean ilFilters::ScaleColours(ilImage &Image, ILfloat r, ILfloat g, ILfloat b)
{
	Image.Bind();
	return iluScaleColours(r, g, b);
}


ILboolean ilFilters::Sharpen(ilImage &Image, ILfloat Factor, ILuint Iter)
{
	Image.Bind();
	return iluSharpen(Factor, Iter);
}



//
//
// ILOPENGL
//
//

#ifdef ILUT_USE_OPENGL
ILvoid ilOgl::Init()
{
	ilutRenderer(ILUT_OPENGL);
	return;
}


GLuint ilOgl::BindTex(ilImage &Image)
{
	Image.Bind();
	return ilutGLBindTexImage();
}


ILboolean ilOgl::Upload(ilImage &Image, ILuint Level)
{
	Image.Bind();
	return ilutGLTexImage(Level);
}


GLuint ilOgl::Mipmap(ilImage &Image)
{
	Image.Bind();
	return ilutGLBuildMipmaps();
}


ILboolean ilOgl::Screen()
{
	return ilutGLScreen();
}


ILboolean ilOgl::Screenie()
{
	return ilutGLScreenie();
}
#endif//ILUT_USE_OPENGL



//
//
// ILALLEGRO
//
//

#ifdef ILUT_USE_ALLEGRO
ILvoid ilAlleg::Init()
{
	ilutRenderer(IL_ALLEGRO);
	return;
}


BITMAP *ilAlleg::Convert(ilImage &Image, PALETTE Pal)
{
	Image.Bind();
	return ilutConvertToAlleg(Pal);
}
#endif//ILUT_USE_ALLEGRO



//
//
// ILWIN32
//
//

#ifdef ILUT_USE_WIN32
ILvoid ilWin32::Init()
{
	ilutRenderer(ILUT_WIN32);
	return;
}


HBITMAP ilWin32::Convert(ilImage &Image)
{
	Image.Bind();
	return ilutConvertToHBitmap(GetDC(NULL));
}


ILboolean ilWin32::GetClipboard(ilImage &Image)
{
	Image.Bind();
	return ilutGetWinClipboard();
}


ILvoid ilWin32::GetInfo(ilImage &Image, BITMAPINFO *Info)
{
	Image.Bind();
	ilutGetBmpInfo(Info);
	return;
}


ILubyte* ilWin32::GetPadData(ilImage &Image)
{
	Image.Bind();
	return ilutGetPaddedData();
}


HPALETTE ilWin32::GetPal(ilImage &Image)
{
	Image.Bind();
	return ilutGetHPal();
}


ILboolean ilWin32::GetResource(ilImage &Image, HINSTANCE hInst, ILint ID, char *ResourceType)
{
	Image.Bind();
	return ilutLoadResource(hInst, ID, ResourceType, IL_TYPE_UNKNOWN);
}


ILboolean ilWin32::GetResource(ilImage &Image, HINSTANCE hInst, ILint ID, char *ResourceType, ILenum Type)
{
	Image.Bind();
	return ilutLoadResource(hInst, ID, ResourceType, Type);
}


ILboolean ilWin32::SetClipboard(ilImage &Image)
{
	Image.Bind();
	return ilutSetWinClipboard();
}
#endif//ILUT_USE_WIN32



//
//
// ILVALIDATE
//
//

ILboolean ilValidate::Valid(ILenum Type, char *FileName)
{
	return ilIsValid(Type, FileName);
}


ILboolean ilValidate::Valid(ILenum Type, FILE *File)
{
	return ilIsValidF(Type, File);
}


ILboolean ilValidate::Valid(ILenum Type, ILvoid *Lump, ILuint Size)
{
	return ilIsValidL(Type, Lump, Size);
}



//
//
// ILSTATE
//
//

ILboolean ilState::Disable(ILenum State)
{
	return ilDisable(State);
}


ILboolean ilState::Enable(ILenum State)
{
	return ilEnable(State);
}


ILvoid ilState::Get(ILenum Mode, ILboolean &Param)
{
	ilGetBooleanv(Mode, &Param);
	return;
}


ILvoid ilState::Get(ILenum Mode, ILint &Param)
{
	ilGetIntegerv(Mode, &Param);
	return;
}


ILboolean ilState::GetBool(ILenum Mode)
{
	return ilGetBoolean(Mode);
}


ILint ilState::GetInt(ILenum Mode)
{
	return ilGetInteger(Mode);
}


const char *ilState::GetString(ILenum StringName)
{
	return ilGetString(StringName);
}


ILboolean ilState::IsDisabled(ILenum Mode)
{
	return ilIsDisabled(Mode);
}


ILboolean ilState::IsEnabled(ILenum Mode)
{
	return ilIsEnabled(Mode);
}


ILboolean ilState::Origin(ILenum Mode)
{
	return ilOriginFunc(Mode);
}


ILvoid ilState::Pop()
{
	ilPopAttrib();
	return;
}


ILvoid ilState::Push(ILuint Bits = IL_ALL_ATTRIB_BITS)
{
	ilPushAttrib(Bits);
	return;
}





//
//
// ILERROR
//
//

ILvoid ilError::Check(ILvoid (*Callback)(const char*))
{
	static ILenum Error;

	while ((Error = ilGetError()) != IL_NO_ERROR) {
		Callback(iluErrorString(Error));
	}

	return;
}


ILvoid ilError::Check(ILvoid (*Callback)(ILenum))
{
	static ILenum Error;

	while ((Error = ilGetError()) != IL_NO_ERROR) {
		Callback(Error);
	}

	return;
}


ILenum ilError::Get()
{
	return ilGetError();
}


const char *ilError::String()
{
	return iluErrorString(ilGetError());
}


const char *ilError::String(ILenum Error)
{
	return iluErrorString(Error);
}
