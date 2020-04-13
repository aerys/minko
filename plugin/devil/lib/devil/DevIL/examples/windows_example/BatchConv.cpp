#ifdef  _DEBUG
#define IL_DEBUG
#endif//_DEBUG

#include <IL/il.h>
#include <windows.h>
#include <direct.h>
#include <string>
using namespace std;

TCHAR *ImageExtArray[] =
{
	L"jpe", L"jpg", L"jpeg",
	L"bmp",
	L"ico",
	L"pbm", L"pgm", L"pnm", L"ppm",
	L"png",
	L"bw", L"rgb", L"rgba", L"sgi",
	L"tga", L"tif", L"tiff",
	L"pcx",
	NULL
};


void	ParseDirs(const string &_Dir, char **ExtList, char *ConvExt, bool Recurse);
bool	IsDir(WIN32_FIND_DATA *_Data);
char	*GetExtension(const char *FileName);
bool	CheckExtension(char *Arg, char *Ext);


TCHAR	*Ext;
string	NewExt;
int		i, j;


//void BatchConv(TCHAR *Directory, TCHAR *ExtList, TCHAR *ConvExt, bool Recurse)
//{
//	ILuint Id, OrigId;
//	ilGenImages(1, &Id);
//	OrigId = ilGetInteger(IL_CUR_IMAGE);
//	ilBindImage(Id);
//	if (ExtList == NULL)
//		ParseDirs(string(Directory), ImageExtArray, ConvExt, Recurse);
//	else {
//		/*char **List = ConvertExtList(ExtList);
//		ParseDirs(string(Directory), ConvertExtList(ExtList), ConvExt, Recurse);
//		DestroyExtList(List);*/
//	}
//	ilDeleteImages(1, &Id);
//	ilBindImage(OrigId);
//	return;
//}
//
//
//void ParseDirs(const string &_Dir, TCHAR **ExtList, TCHAR *ConvExt, bool Recurse)
//{
//	HANDLE			Search;
//	WIN32_FIND_DATA	FindData;
//
//	_chdir(_Dir.c_str());
//	Search = FindFirstFile("*.*", &FindData);
//
//	do {
//		if (!strcmp(FindData.cFileName, ".") || !strcmp(FindData.cFileName, ".."))
//			continue;
//		if (IsDir(&FindData) && Recurse) {
//			_chdir(FindData.cFileName);
//			string NewDir = _Dir + string("\\");
//			NewDir += FindData.cFileName;
//			ParseDirs(NewDir, ExtList, ConvExt, Recurse);
//			_chdir("..");
//		}
//		Ext = GetExtension(FindData.cFileName);
//		if (Ext == NULL)
//			continue;
//		if (!_stricmp(Ext, ConvExt))  // Already has that extension.
//			continue;
//		for (j = 0; ExtList[j] != NULL; j++) {
//			if (CheckExtension(FindData.cFileName, ExtList[j])) {
//				string NewName;
//				for (i = 0; i < Ext - FindData.cFileName; i++) {
//					NewName += FindData.cFileName[i];
//				}
//				NewName += ConvExt;
//				if (!ilLoadImage(FindData.cFileName))
//					break;
//				ilSaveImage((TCHAR*)NewName.c_str());
//				break;
//			}
//		}
//	} while (FindNextFile(Search, &FindData));
//
//	FindClose(Search);
//	return;
//}
//
//
//// Is the file actually a directory?
//bool IsDir(WIN32_FIND_DATA *_Data)
//{
//	if (_Data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
//		return true;
//	return false;
//}


TCHAR *GetExtension(const TCHAR *FileName)
{
	bool PeriodFound = false;
	TCHAR *Ext = (TCHAR*)FileName;
	long i, Len = (long)wcslen(FileName);

	if (FileName == NULL || !Len)  // if not a good filename/extension, exit early
		return NULL;

	Ext += Len;  // start at the end

	for (i = Len; i >= 0; i--) {
		if (*Ext == '.') {  // try to find a period 
			PeriodFound = true;
			break;
		}
		Ext--;
	}

	if (!PeriodFound)  // if no period, no extension
		return NULL;

	return Ext+1;
}


// Simple function to test if a filename has a given extension, disregarding case
bool CheckExtension(TCHAR *Arg, TCHAR *Ext)
{
	bool	PeriodFound = false;
	TCHAR	*Argu = Arg;  // pointer to arg so we don't destroy arg
	unsigned int i;

	if (Arg == NULL || Ext == NULL || !wcslen(Arg) || !wcslen(Ext))  // if not a good filename/extension, exit early
		return false;

	Argu += wcslen(Arg);  // start at the end


	for (i = (int)wcslen(Arg); i >= 0; i--) {
		if (*Argu == '.') {  // try to find a period 
			PeriodFound = true;
			break;
		}
		Argu--;
	}

	if (!PeriodFound)  // if no period, no extension
		return false;

	if (!_wcsicmp(Argu+1, Ext))  // extension and ext match?
		return true;

	return false;  // if all else fails, return IL_FALSE
}

