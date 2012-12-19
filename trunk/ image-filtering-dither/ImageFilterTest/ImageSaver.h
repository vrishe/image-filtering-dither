#ifndef _IMAGESAVER_H_
#define _IMAGESAVER_H_

#include <TinyImageLoader.h>
#include <tchar.h>

namespace ImageSaver
{
	typedef long fxpt2dot30;

	/* ICM Color Definitions */
	// The following two structures are used for defining RGB's in terms of CIEXYZ.

	typedef struct tag_ciexyz
	{
		fxpt2dot30 ciexyzX;
		fxpt2dot30 ciexyzY;
		fxpt2dot30 ciexyzZ;
	} ciexyz;

	typedef struct tag_ciexyz_triple
	{
		ciexyz  ciexyzRed;
		ciexyz  ciexyzGreen;
		ciexyz  ciexyzBlue;
	} ciexyz_triple;

	#pragma pack(2)
	struct tag_bitmap_file_header {
		unsigned short  bfType;
		unsigned long   bfSize;
		unsigned short  bfReserved1;
		unsigned short  bfReserved2;
		unsigned long   bfOffBits;
	};
	typedef tag_bitmap_file_header bitmap_file_header;

	typedef struct tag_bitmap_info_header {
	  unsigned long		biSize;
	  long				biWidth;
	  long				biHeight;
	  unsigned short	biPlanes;
	  unsigned short	biBitCount;
	  unsigned long		biCompression;
	  unsigned long		biSizeImage;
	  long				biXPelsPerMeter;
	  long				biYPelsPerMeter;
	  unsigned long		biClrUsed;
	  unsigned long		biClrImportant;
	} bitmap_info_header;

	bool SaveAsBmp(til::Image& image, const _TCHAR *file_name);
}

#endif