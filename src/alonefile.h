#ifndef ALONEFILE_H
#define ALONEFILE_H

#include <iostream>
#include <cstdint>

#include "pak.h"
#include "unpack.h"
#include "bmp.h"

enum compress_status
{
    ok=0,
    nodosbox,
    nozip,
    notimplode,
    unknown
};

class AloneFile
{
public:
    AloneFile();
    ~AloneFile();
    bool read(FILE* pakfile, const char *filename, unsigned int index);
    void print();

    bool exportAsBMP(u32 offset, u32 width, u8* palette);
    bool exportUncompressed(const char *outfilename);
    bool exportCompressed(const char *outfilename);
    compress_status compress_dosbox_pkzip();

    static u8 palette[];

//protected:
    pakInfoStruct mInfo;
    u32 dummyFiller;//fix unsafe alignment

    u32 mFileOffset;
    u32 mAdditionalDescriptorSize;
    char mNameBuffer[256];

    const char* mPAKFilename;
    long mIndex; //long to align

    char* mComprData;
    char* mDecomprData;

    u8 mTailingBytes[0x10];//what is it? Seems not mandatory...
};

#endif // ALONEFILE_H
