#include <il/il.h>





typedef struct WADHEAD

{

	GLbyte Type[4];  // either 'IWAD' or 'PWAD'

	GLuint NumLumps;

	GLuint DirOffset;

} WADHEAD;



typedef struct DIRENT

{

	GLuint Offset;

	GLuint Length;

	GLbyte Name[8];

} DIRENT;





FILE *Wad;

WADHEAD WadHead;

GLuint OrigPos;



FILE *TestOpenWad(char *WadName)

{

	Wad = fopen(WadName, "rb");

	fread(&WadHead, sizeof(WADHEAD), 1, Wad);

	fseek(Wad, WadHead.DirOffset, SEEK_SET);

	OrigPos = WadHead.DirOffset;

	return Wad;

}





GLboolean TestLocateLump(char *LumpName)

{

	DIRENT  DirEnt;

	GLboolean NotFound = true;

	GLuint DirsRead = 0;



	

	fseek(Wad, OrigPos, SEEK_SET);



	while (NotFound && DirsRead < WadHead.NumLumps) {

		fread(&DirEnt, sizeof(DIRENT), 1, Wad);

		if (!strnicmp(DirEnt.Name, LumpName, 8)) {

			fseek(Wad, DirEnt.Offset, SEEK_SET);

			return true;

		}

		DirsRead++;

	}



	return false;

}





GLvoid TestCloseWad()

{

	if (Wad)

		fclose(Wad);

	Wad = NULL;

}

