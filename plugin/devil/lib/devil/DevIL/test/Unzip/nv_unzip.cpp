/*********************************************************************NVMH1****
File:
nv_unzip.cpp

Copyright (C) 1999, 2000 NVIDIA Corporation
This file is provided without support, instruction, or implied warranty of any
kind.  NVIDIA makes no guarantee of its fitness for a particular purpose and is
not liable under any circumstances for any damages or loss whatsoever arising
from the use or inability to use this file or items derived from it.

Comments:


******************************************************************************/

#ifndef __nv_util_h__
#include <nv_util.h>
#endif // __nv_util_h__

#ifndef _unz_H
#include <unzip.h>
#endif // _unz_H

namespace unzip
{
    unsigned char * open(const char * filename, const char * inzipfile, unsigned int * size)
    {
	    char filename_inzip[256];
        int err=UNZ_OK;
        FILE *fout=NULL;
        unsigned char * buf;
        unzFile uf=NULL;
    
        uf = unzOpen(filename);

        if (unzLocateFile(uf,inzipfile,0)!=UNZ_OK) //CASESENSITIVITY
        {
            printf("file %s not found in the zipfile\n",filename);
            return NULL;
        }

        unz_file_info file_info;
	    uLong ratio=0;
	    err = unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);

	    if (err!=UNZ_OK)
	    {
		    printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
		    return NULL;
	    }

  
  	    err = unzOpenCurrentFile(uf);
	    if (err!=UNZ_OK)
	    {
		    printf("error %d with zipfile in unzOpenCurrentFile\n",err);
            return NULL;
	    }

        *size = file_info.uncompressed_size;
        buf = new unsigned char[file_info.uncompressed_size];

        unsigned int count = 0;
        err = 1;
        while (err > 0)
        {
            err = unzReadCurrentFile(uf,&buf[count],65535);
	        if (err<0)	
	        {
		        printf("error %d with zipfile in unzReadCurrentFile\n",err);
                break;
	        }
            else
                count += err;
        }
        assert(count == file_info.uncompressed_size);
        if (err==UNZ_OK)
        {
		    err = unzCloseCurrentFile (uf);
		    if (err!=UNZ_OK)
		    {
			    printf("error %d with zipfile in unzCloseCurrentFile\n",err);
		    }
        }
        else
        {
            *size = 0;
            delete [] buf;
            buf = NULL;
            unzCloseCurrentFile(uf); /* don't lose the error */
        }

        return buf;
    }

} // namespace unzip