#pragma once

extern "C" {
#include "minilzo.h"
}

#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

struct MMiniLZO
{
	MMiniLZO();

	static int
	Compress(const lzo_bytep src, lzo_uint  src_len,
				   lzo_bytep dst, lzo_uintp dst_len)
	{
		return lzo1x_1_compress(src, src_len, dst, dst_len, s_CompressWorkMemory);
	}

	static int
	Decompress(const lzo_bytep src, lzo_uint  src_len,
					 lzo_bytep dst, lzo_uintp dst_len)
	{
		return lzo1x_decompress(src, src_len, dst, dst_len, 0);
	}

private:
	static HEAP_ALLOC(s_CompressWorkMemory, LZO1X_1_MEM_COMPRESS);
	static MMiniLZO s_MiniLZO;
};
