#include "stdafx.h"
#include "tga.h"

void TransformBit(int pixel_byte, WORD **p_data, int w, int h)
{
	WORD rg, b;
	const WORD blue = 0x001F;
	const WORD red_green = 0x7FE0;
	
	if (pixel_byte == 2 /*&& g_gl_g_bit == 6*/)
		//if (pixel_byte == 2)
	{
		for (int i = 0; i < w*h; i++)
		{
			b = (WORD)((*(*p_data + i)) & blue);
			rg = (WORD)(((*(*p_data + i)) & red_green) << 1);
			*(*p_data + i) = (WORD)(rg + b);
		}
	}
}


//-----------------------------------------------------------------------------
//- CopyLine
//- for TGA reverse
//
//  `buf_length는 dest, src 동일해야 한다.
//-----------------------------------------------------------------------------
void CopyLine(BYTE *dest, BYTE *src, int buf_length, int pixel_byte, bool reverse)
{
	if (buf_length < 2) return;
	
	if (reverse == true)
	{
        for (int i = 0; i < buf_length; i++)
			memcpy(dest+i*buf_length*pixel_byte, src+(buf_length-i-1)*buf_length*pixel_byte, pixel_byte);
	}
	else
	{
		for (int i = 0; i < buf_length; i++)
			memcpy(dest, src, buf_length*pixel_byte);
	}
}

bool Tga::Load(const char * sz_filename)
{
	if (sz_filename == NULL)
		return false;
	
	FILE        *fp;
	int         rcount;
	BYTE        *p_originalbuf = NULL; // 맨처음
	BYTE        *p_decompbuf = NULL; // 압축 풀림 
	BYTE        *p_temp = NULL;
	int         pixel_byte;
	int         total_byte;
	bool        process_reverse1 = false;
	bool        process_reverse2 = false;
	S_TGAHEAD	head;
	
	fp = fopen(sz_filename, "rb");
	if (fp == NULL)
		return false;
	
	// read TGA head
	rcount = fread(&head, sizeof(S_TGAHEAD), 1, fp);
	if (!(rcount > 0))
	{
		fclose(fp);
		return false;
	}
	
//	fseek(fp, sizeof(S_TGAHEAD), SEEK_SET); // goto data
	
	// is this file TGA?
	if (head.idsize != 0 || 
		head.colormaptype != 0 ||
		head.colormapbits != 0 ||
		head.xstart != 0 || 
		head.ystart != 0 ||
		head.bpp == 8) 
	{
		fclose(fp);
		return false;
	}
	
	pixel_byte = head.bpp / 8; // get pixel byte
	total_byte = head.width * pixel_byte * head.height;
	
	// alloc mem
	p_originalbuf = new BYTE[total_byte];
	p_decompbuf = new BYTE[total_byte];
	p_temp = new BYTE[total_byte];
	
	BYTE *p_data;
	p_data = new BYTE[total_byte];
	
	rcount = fread(p_originalbuf, total_byte, 1, fp); // 압출 풀 데이터를 읽는다.
	if (!(rcount > 0) &&  head.imagetype == 2)
	{
		delete []p_data;
		delete []p_temp;
		delete []p_decompbuf;
		delete []p_originalbuf;

		fclose(fp);
		return false;
	}
	
	//
	// 2, 10번만 지원!
	//
	switch (head.imagetype)
	{
		//case 1: // 압축되지 않은 palette 중심 이미지
        // return false;
	case 2: // 압축되지 않은 RGB 이미지
		delete []p_decompbuf;
		p_decompbuf = p_originalbuf;
		p_originalbuf = NULL;
		break;
		//case 3: // 압축되지 않은 Monochrome 이미지
        // return false;
		//case 9: // RLE(run length encoding)된 palette 중심 이미지
        // return false;
	case 10: // RLE된 RGB 이미지
		{
			char data[4];
			int i, len, read = 0;
			BOOL rle;
			BYTE *dest = p_decompbuf, *src = p_originalbuf;

			while(read < total_byte)
			{
				len = (*src & 0x7F) +1;
				rle = *src & 0x80;
				src++;

				if(rle)
				{
					memcpy(data, src, pixel_byte);
					src += pixel_byte;
					for (int i = 0; i < len; i++)
					{
						memcpy(dest, data, pixel_byte);
						dest += pixel_byte;
						read += pixel_byte;
					}
				}
				else
				{
					memcpy(dest, src, pixel_byte*len);
					src += pixel_byte*len;
					dest += pixel_byte*len;
					read += pixel_byte*len;
				}
			}
		}
		break;
		//case 11: // RLE된 Monochrome 이미지
        // return false;
	default:
		delete []p_data;
		delete []p_temp;
		delete []p_decompbuf;
		delete []p_originalbuf;

		return false;
	}
	
	// check the descriptor
	int h;
	if ((head.descriptor & 0x20))
	{ // 거꾸로 저장되었으므로 뒤집자!//말쟈-_-;
		for (h = 0; h < head.height; h++)
			CopyLine(p_temp+head.width*pixel_byte*((head.height-1)-h),
			p_decompbuf+head.width*pixel_byte*h,
			head.width,
			pixel_byte,
			false);
		
		delete []p_decompbuf;
		p_decompbuf = p_temp;
		p_temp = NULL;
		process_reverse1 = true;
	}
	if (head.descriptor & 0x10)
	{ // 좌우가 바뀌었으므로 뒤집자!
		for (h = 0; h < head.height; h++)
			CopyLine(p_data+head.width*pixel_byte*h,
			p_decompbuf+head.width*pixel_byte*h,
			head.width,
			pixel_byte,
			true);
		
		process_reverse2 = true;
	}
	
	if (process_reverse2 == false)
	{
		delete []p_data;
		p_data = p_decompbuf;
		p_decompbuf = NULL;
	}
	
	// 압축이 풀린 이미지 상태에서 변환을 한다.
//	TransformBit(pixel_byte, (WORD **)(&p_data), head.width, head.height);
	
	delete []p_originalbuf;
	delete []p_decompbuf;
	delete []p_temp;

	//Create(head.width, head.height, head.bpp, NULL, (char *)p_data, true);
	delete []p_data;
	
	fclose(fp);
	
	return true;
}