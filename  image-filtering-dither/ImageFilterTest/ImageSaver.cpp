#include "ImageSaver.h"

#include <vector>
#include <fstream>

namespace ImageSaver
{
	bool SaveAsBmp(til::Image& image, const _TCHAR *file_name)
	{
		if (image.GetBitDepth() != til::Image::BPP_32B_R8G8B8) return false;

		std::ofstream bmp_out(file_name, std::ios::binary);
		if (!bmp_out.is_open()) return false;

		bitmap_info_header bitmap_info = { 0x00 }; 
		bitmap_info.biSize			= sizeof(bitmap_info_header);
		bitmap_info.biWidth			= image.GetWidth();
		bitmap_info.biHeight		= -static_cast<til::int32>(image.GetHeight());
		bitmap_info.biPlanes		= 1;
		bitmap_info.biBitCount		= 24;
		bitmap_info.biCompression	= 0;	// BI_RGB;
		bitmap_info.biXPelsPerMeter	= 4000;
		bitmap_info.biYPelsPerMeter	= 4000;

		bitmap_file_header bitmap_header	= { 0x00 };
		bitmap_header.bfType				= 0x4D42;   // "BM"
		bitmap_header.bfOffBits				= sizeof(bitmap_file_header) + bitmap_info.biSize;

		std::vector<til::byte> bitmap_data;
		for (std::size_t i = 0, div = image.GetWidth(), max_i = image.GetWidth() * image.GetHeight(); i < max_i; ++i)
		{
			if (i % div == 0)
			{
				std::size_t pad_count = (bitmap_data.size() / 3) % div;
				if (pad_count > 0) 
				{
					bitmap_data.insert(bitmap_data.end(), pad_count, 0x00);
				}
			}

			unsigned long pixel_data = reinterpret_cast<unsigned long*>(image.GetPixels())[i];
			bitmap_data.push_back(RED(pixel_data));
			bitmap_data.push_back(GREEN(pixel_data));
			bitmap_data.push_back(BLUE(pixel_data));
		}

		bitmap_header.bfSize = bitmap_header.bfOffBits + bitmap_data.size();

		bmp_out.write(reinterpret_cast<char*>(&bitmap_header), sizeof(bitmap_file_header));
		bmp_out.write(reinterpret_cast<char*>(&bitmap_info), sizeof(bitmap_info_header));
		bmp_out.write(reinterpret_cast<char*>(bitmap_data.begin()._Ptr), bitmap_header.bfSize - bitmap_header.bfOffBits);

		bmp_out.close();

		return true;
	}
}

