#pragma once
#include "Types.h"

#pragma pack(push,1)
struct BitMapHeader {

	u16 fileType;
	u32 fileSize;
	u16 reserved1;
	u16 reserved2;
	u32 bitmapOffset;
	u32 size;
	s32 width;
	s32 height;
	u16 planes;
	u16 bitsPerPixel;
	u32 compression;
	u32 sizeOfBitMap;
	s32 horzResolution;
	s32 vertResolution;
	u32 colorsUsed;
	u32 colorsImportant;
};
#pragma pack(pop)