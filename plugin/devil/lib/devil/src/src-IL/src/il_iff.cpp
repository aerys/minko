//-----------------------------------------------------------------------------
//
// ImageLib Sources
// Last modified: 03/01/2009
//
// Filename: src-IL/src/il_iff.cpp
//
// Description: Reads from an .iff file.  Contribution from GAIA:
//				http://gaia.fdi.ucm.es/grupo/projects/javy/devzone.html#DevILNotes.
//
//-----------------------------------------------------------------------------

#include "il_internal.h"

#ifndef IL_NO_IFF

// Chunk type, data and functions:
typedef struct _iff_chunk {
	ILuint	tag;
	ILuint	start;
	ILuint	size;
	ILuint	chunkType;
} iff_chunk;

#define CHUNK_STACK_SIZE (32)
static iff_chunk chunkStack[CHUNK_STACK_SIZE];
static int chunkDepth = -1;

iff_chunk iff_begin_read_chunk();
void iff_end_read_chunk();
char *iff_read_data(int size);
ILboolean iLoadIffInternal(void);


/* Define the IFF tags we are looking for in the file. */
const ILuint IFF_TAG_CIMG = ('C' << 24) | ('I' << 16) | ('M' << 8) | ('G'); 
const ILuint IFF_TAG_FOR4 = ('F' << 24) | ('O' << 16) | ('R' << 8) | ('4'); 
const ILuint IFF_TAG_TBHD = ('T' << 24) | ('B' << 16) | ('H' << 8) | ('D'); 
const ILuint IFF_TAG_TBMP = ('T' << 24) | ('B' << 16) | ('M' << 8) | ('P');
const ILuint IFF_TAG_RGBA = ('R' << 24) | ('G' << 16) | ('B' << 8) | ('A');
const ILuint IFF_TAG_CLPZ = ('C' << 24) | ('L' << 16) | ('P' << 8) | ('Z');
const ILuint IFF_TAG_ESXY = ('E' << 24) | ('S' << 16) | ('X' << 8) | ('Y');
const ILuint IFF_TAG_ZBUF = ('Z' << 24) | ('B' << 16) | ('U' << 8) | ('F');
const ILuint IFF_TAG_BLUR = ('B' << 24) | ('L' << 16) | ('U' << 8) | ('R');
const ILuint IFF_TAG_BLRT = ('B' << 24) | ('L' << 16) | ('R' << 8) | ('T');
const ILuint IFF_TAG_HIST = ('H' << 24) | ('I' << 16) | ('S' << 8) | ('T');

// Flags
#define RGB_FLAG     (1)
#define ALPHA_FLAG   (2)
#define ZBUFFER_FLAG (4)

// Function for decompress the file.
char *iff_decompress_rle(ILuint numBytes, char *compressedData, 
							ILuint compressedDataSize, 
							ILuint *compressedStartIndex);

char *iffReadUncompressedTile(ILushort width, ILushort height, ILbyte depth);
char *iff_decompress_tile_rle(ILushort width, ILushort height, ILushort depth, 
				 char *compressedData, ILuint compressedDataSize);


//! Reads an IFF file
ILboolean ilLoadIff(const ILstring FileName)
{
	ILHANDLE iffFile;
	ILboolean ret = IL_FALSE;

	iffFile = iopenr(FileName);
	if (iffFile == NULL) {
		ilSetError(IL_COULD_NOT_OPEN_FILE);
		return ret;
	}
	ret = ilLoadIffF(iffFile);
	icloser(iffFile);
	return ret;
}


//! Reads an already-opened IFF file
ILboolean ilLoadIffF(ILHANDLE File)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputFile(File);
	FirstPos = itell();
	bRet = iLoadIffInternal();
	iseek(FirstPos, IL_SEEK_SET);

	// Lbm files can have the .iff extension as well, so if Iff-loading failed,
	//  try to load it as a Lbm.
	if (bRet == IL_FALSE)
		return ilLoadIlbmF(File);

	return bRet;
}


//! Reads from a memory "lump" that contains an IFF
ILboolean ilLoadIffL(const void *Lump, ILuint Size)
{
	ILuint		FirstPos;
	ILboolean	bRet;

	iSetInputLump(Lump, Size);
	FirstPos = itell();
	bRet = iLoadIffInternal();
	iseek(FirstPos, IL_SEEK_SET);

	// Lbm files can have the .iff extension as well, so if Iff-loading failed,
	//  try to load it as a Lbm.
	if (bRet == IL_FALSE)
		return ilLoadIlbmL(Lump, Size);

	return IL_TRUE;
}

ILboolean iLoadIffInternal(void)
{
	iff_chunk chunkInfo;
    
	// -- Header info.
	ILuint width, height;
	ILuint flags, compress;
	ILushort tiles;

	ILenum	format;
	ILubyte bpp;

	ILboolean tileImageDataFound;

	// -- Initialize the top of the chunk stack.
	chunkDepth = -1;

	// -- File should begin with a FOR4 chunk of type CIMG
	chunkInfo = iff_begin_read_chunk();
	if (chunkInfo.chunkType != IFF_TAG_CIMG) {
		ilSetError(IL_ILLEGAL_FILE_VALUE);
		return IL_FALSE;
	}


	/*
	* Read the image header
	* OK, we have a FOR4 of type CIMG, look for the following tags
	*		FVER	
	*		TBHD	bitmap header, definition of size, etc.
	*		AUTH
	*		DATE
	*/
	while (1) {

		chunkInfo = iff_begin_read_chunk();
		
		// -- Right now, the only info we need about the image is in TBHD
		// -- so search this level until we find it.
		if( chunkInfo.tag == IFF_TAG_TBHD ) {
			// -- Header chunk found
			width = GetBigUInt();
			height = GetBigUInt();
			GetBigShort(); // -- Don't support 
			GetBigShort(); // -- Don't support 
			flags = GetBigUInt();
			GetBigShort(); // -- Don't support
			tiles = GetBigUShort();
			compress	= GetBigUInt();
			
			iff_end_read_chunk();
		
			if( compress > 1 ) {
				ilSetError(IL_ILLEGAL_FILE_VALUE);
				return	IL_FALSE;
			}
   				break;
		} else
			iff_end_read_chunk();
	} /* END find TBHD while loop */

	if (!(flags & RGB_FLAG)) {
		ilSetError(IL_ILLEGAL_FILE_VALUE);
		return	IL_FALSE;
	}

	if (flags & ALPHA_FLAG) {
		format = IL_RGBA; bpp = 4;
	} else {
		format = IL_RGB; bpp = 3;
	}

	if (!ilTexImage(width, height, 1, bpp, format, IL_UNSIGNED_BYTE, NULL))
		return IL_FALSE;

	iCurImage->Origin = IL_ORIGIN_UPPER_LEFT;

	tileImageDataFound = IL_FALSE;

	while (!tileImageDataFound) {
		ILuint tileImage;
		ILuint tileZ;
	
		chunkInfo = iff_begin_read_chunk();

		/*
		 * OK, we have a FOR4 of type TBMP, (embedded FOR4)
		 * look for the following tags
		 *		RGBA	color data,	RLE compressed tiles of 32 bbp data
		 *		ZBUF	z-buffer data, 32 bit float values
		 *		CLPZ	depth map specific, clipping planes, 2 float values
		 *		ESXY	depth map specific, eye x-y ratios, 2 float values
		 *		HIST	
		 *		VERS
		 *		FOR4 <size>	BLUR (twice embedded FOR4)
		 */
		if (chunkInfo.chunkType != IFF_TAG_TBMP) {
			iff_end_read_chunk();
			continue;
		}
		tileImageDataFound = IL_TRUE;
		tileImage = 0;	// Si no RGBA, tileImage = tiles...
		if (flags & ZBUFFER_FLAG)
			tileZ = 0;
		else
			tileZ = tiles;

		// Read tiles
		while ( (tileImage < tiles) || (tileZ < tiles)) {
			char	 *tileData;
			ILushort x1, x2, y1, y2, tile_width, tile_height;
			ILuint remainingDataSize;
			ILushort	tile_area;
			ILuint	tileCompressed;

			chunkInfo = iff_begin_read_chunk();
			if ((chunkInfo.tag != IFF_TAG_RGBA) && (chunkInfo.tag != IFF_TAG_ZBUF)) {
				ilSetError(IL_ILLEGAL_FILE_VALUE);
				return IL_FALSE;
			}
			x1 = GetBigUShort();	y1 = GetBigUShort();
			x2 = GetBigUShort();	y2 = GetBigUShort();

			remainingDataSize = chunkInfo.size - 4*sizeof(ILushort);
			tile_width = x2 - x1 + 1;
			tile_height = y2 - y1 + 1;
			tile_area = tile_width * tile_height;

			if ((ILint)remainingDataSize >= (tile_width * tile_height * bpp))
				tileCompressed = 0;
			else
				tileCompressed = 1;

			if (chunkInfo.tag == IFF_TAG_RGBA) {
				if (tileCompressed) {
					char	*data = iff_read_data(remainingDataSize);
					if (data) {
						tileData = iff_decompress_tile_rle(tile_width, tile_height,
															bpp, data, remainingDataSize);
						ifree(data);
					}
				} else {
					tileData = iffReadUncompressedTile(tile_width, tile_height, bpp);
				}

				if (tileData) {
					// Dump RGBA data to our data structure 
					ILushort	i;
					ILuint		base;
					base = bpp*(width * y1 + x1);
					for (i = 0; i < tile_height; i++) {
						memcpy(&iCurImage->Data[base + bpp*i*width],
								&tileData[bpp*i*tile_width],
								tile_width*bpp*sizeof(char));
					}
					ifree(tileData);
					tileData = NULL;
	    
					iff_end_read_chunk();
					tileImage++;
				} else
					return IL_FALSE;
			} else if (chunkInfo.tag == IFF_TAG_ZBUF) {
				tileZ++;
				iff_end_read_chunk();
			}

		}
	}
	//ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);  // Why was this here?
	return ilFixImage();
}

/*
 * IFF Chunking Routines.
 *
 */

iff_chunk iff_begin_read_chunk()
{
	chunkDepth++;
	if (chunkDepth >= CHUNK_STACK_SIZE){
		ilSetError(IL_STACK_OVERFLOW);
		return chunkStack[0];
	}
	if (chunkDepth < 0) {
		ilSetError(IL_STACK_UNDERFLOW);
		return chunkStack[0];
	}

	chunkStack[chunkDepth].start = itell();
	chunkStack[chunkDepth].tag = GetBigInt();
	chunkStack[chunkDepth].size = GetBigInt();
    
	if (chunkStack[chunkDepth].tag == IFF_TAG_FOR4) {
		// -- We have a form, so read the form type tag as well. 
		chunkStack[chunkDepth].chunkType = GetBigInt();
	} else {
		chunkStack[chunkDepth].chunkType = 0;
	} 
	
	return chunkStack[chunkDepth];
}

void iff_end_read_chunk()
{
	ILuint	end;
	int		part;
  
	end = chunkStack[chunkDepth].start + chunkStack[chunkDepth].size + 8;
    
	if (chunkStack[chunkDepth].chunkType != 0) {
		end += 4;
	}
	// Add padding 
	part = end % 4;
	if (part != 0) {
	    end += 4 - part;   
	}

	iseek(end, IL_SEEK_SET);

	chunkDepth--;
}

char * iff_read_data(int size)
{
	char *buffer = (char*)ialloc(size * sizeof(char));
	if (buffer == NULL)
		return NULL;
	
	if (iread(buffer, size*sizeof(char), 1) != 1) {
		ifree(buffer);
		return NULL;
	}

	return buffer;
}

/*
	IFF decompress functions
*/

char *iffReadUncompressedTile(ILushort width, ILushort height, ILbyte depth)
{

	char	*data = NULL;
	char	*iniPixel;
	char	*finPixel;
	int		i, j;
	int		tam = width* height * depth * sizeof(char);

	data = (char*)ialloc(tam);
	if (data == NULL)
		return NULL;

	if (iread(data, tam, 1) != 1) {
		ifree(data);
		return NULL;
	}

	iniPixel = data;
	for (i = 0; i < tam / depth; i++) {
		finPixel = iniPixel + depth;
		for (j = 0; j < (depth /2); j++) {
			char aux;
			aux = *iniPixel; 
			*(finPixel--) = *iniPixel;
			*(iniPixel++) = aux;
		}
	}
	return data;
}


char *iff_decompress_tile_rle(ILushort width, ILushort height, ILushort depth, 
							  char *compressedData, ILuint compressedDataSize)
{

	char	*channels[4];
	char	*data;
	int		i, k, row, column;
	ILuint	compressedStart = 0;

	// Decompress only in RGBA.
	if (depth != 4) {
		ilSetError(IL_ILLEGAL_FILE_VALUE);
		return NULL;
	}

	for (i = depth-1; i >= 0; --i) {
		channels[i] = iff_decompress_rle(width * height, compressedData, 
				      compressedDataSize, &compressedStart);
		if (channels[i] == NULL)
			return NULL;
	}

    // Build all the channels from the decompression into an RGBA array.
	data = (char*)ialloc(width * height * depth * sizeof(char));
	if (data == NULL)
		return NULL;

	for (row = 0; row < height; row++)
		for (column = 0; column < width; column++)
			for (k = 0; k < depth; k++)
				data[depth*(row*width + column) + k] =
					channels[k][row*width + column];
	
	ifree(channels[0]); ifree(channels[1]);
	ifree(channels[2]); ifree(channels[3]);

	return data;
}

char *iff_decompress_rle(ILuint numBytes, char *compressedData, 
							ILuint compressedDataSize, 
							ILuint *compressedStartIndex)
{

	char	*data = (char*)ialloc(numBytes * sizeof(char));
	unsigned char	nextChar, count;
	int		i;
	ILuint	byteCount = 0;

	if (data == NULL)
		return NULL;

	memset(data, 0, numBytes*sizeof(char));

	while (byteCount < numBytes) {
		if (*compressedStartIndex >= compressedDataSize)
			break;
		nextChar = compressedData[*compressedStartIndex];
		(*compressedStartIndex)++;
		count = (nextChar & 0x7f) + 1;
		if ((byteCount + count) > numBytes) break;
		if (nextChar & 0x80) {
			// Duplication run
			nextChar = compressedData[*compressedStartIndex];
			(*compressedStartIndex)++;
			// assert ((byteCount + count) <= numBytes);
			for (i = 0; i < count; i++) {
				data[byteCount]= nextChar;
				byteCount++;
			}
		} else {
			// Verbatim run
			for (i = 0; i < count; i++) {
				data[byteCount] = compressedData[*compressedStartIndex];
				(*compressedStartIndex)++;
				byteCount++;
			}
		}
		//assert(byteCount <= numBytes);
	}

	return data;

}

#endif //IL_NO_IFF
