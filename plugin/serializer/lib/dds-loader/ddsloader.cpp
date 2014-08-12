#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

#include "ddsloader.h"

struct DdsLoadInfo {
  bool compressed;
  bool swap;
  bool palette;
  unsigned int divSize;
  unsigned int blockBytes;
  unsigned int format;
};

DdsLoadInfo loadInfoDXT1 = {
  true, false, false, 4, 8, DDS_LOADER_FORMAT_DXT1
};
DdsLoadInfo loadInfoDXT3 = {
  true, false, false, 4, 16, DDS_LOADER_FORMAT_DXT3
};
DdsLoadInfo loadInfoDXT5 = {
  true, false, false, 4, 16, DDS_LOADER_FORMAT_DXT5
};

bool
loadDDS(const std::vector<unsigned char>& src,
        std::vector<unsigned char>& dst,
        unsigned int& format,
        unsigned int& width,
        unsigned int& height)
{
    size_t s = 0;
    unsigned int x = 0;
    unsigned int y = 0;
    unsigned int mipMapCount = 0;

    auto headerSize = sizeof(DDS_header);

    auto hdr = *reinterpret_cast<const DDS_header*>(src.data());

    assert(hdr.dwMagic == DDS_MAGIC);
    assert(hdr.dwSize == 124);

    if (hdr.dwMagic != DDS_MAGIC || hdr.dwSize != 124 ||
        !(hdr.dwFlags & DDSD_PIXELFORMAT) || !(hdr.dwFlags & DDSD_CAPS))
    {
        return false;
    }

    width = hdr.dwWidth;
    height = hdr.dwHeight;
    assert(!(width & (width - 1)));
    assert(!(height & (height - 1)));

    DdsLoadInfo * li;

    if (PF_IS_DXT1(hdr.sPixelFormat)) {
        li = &loadInfoDXT1;
    }
    else if (PF_IS_DXT3(hdr.sPixelFormat)) {
        li = &loadInfoDXT3;
    }
    else if (PF_IS_DXT5(hdr.sPixelFormat)) {
        li = &loadInfoDXT5;
    }
    else {
        return false;
    }

    format = li->format;

    //fixme: handle cube textures

    x = width;
    y = height;
    
    mipMapCount = (hdr.dwFlags & DDSD_MIPMAPCOUNT) ? hdr.dwMipMapCount : 1;

    size_t size = std::max(li->divSize, x) / li->divSize * std::max(li->divSize, y) / li->divSize * li->blockBytes;
    assert(size == hdr.dwPitchOrLinearSize);
    assert(hdr.dwFlags & DDSD_LINEARSIZE);

    dst.resize(size);

    auto offset = headerSize;

    dst.assign(src.begin() + offset, src.begin() + offset + size);

    return true;
}
