
#include "RGBLinearQuantizationFilter.h"

#include <vector>

namespace ipo
{
	RGBLinearQuantizationFilter::RGBLinearQuantizationFilter(rgb_quants palette_quants)
		: _palette_quants(palette_quants)
	{
	}


	RGBLinearQuantizationFilter::~RGBLinearQuantizationFilter(void)
	{
	}

	bool RGBLinearQuantizationFilter::Apply(til::Image *image) const
	{
		if (image == NULL) return false;
		if (image->GetBitDepth() != til::Image::BPP_32B_R8G8B8) return false;

		til::uint32 *image_pixels = reinterpret_cast<til::uint32*>(image->GetPixels());

		std::vector<til::byte> palette[3] = { 
			std::vector<til::byte>(max(((til::byte)(_palette_quants)),       1), 0x00),
			std::vector<til::byte>(max(((til::byte)(_palette_quants >>  8)), 1), 0x00),
			std::vector<til::byte>(max(((til::byte)(_palette_quants >> 16)), 1), 0x00)
		};

		til::byte index[] = { 
			static_cast<til::byte>(0x100 / palette[0].size()) + (palette[0].size() % 2 * static_cast<til::byte>(palette[0].size() > 1)), 
			static_cast<til::byte>(0x100 / palette[1].size()) + (palette[1].size() % 2 * static_cast<til::byte>(palette[1].size() > 1)), 
			static_cast<til::byte>(0x100 / palette[2].size()) + (palette[2].size() % 2 * static_cast<til::byte>(palette[2].size() > 1)) 
		};

		for(std::vector<til::byte>::size_type i = 0, max = palette[0].size(); i < max; ++i) palette[0][i] = static_cast<til::byte>(i * index[0]);
		for(std::vector<til::byte>::size_type i = 0, max = palette[1].size(); i < max; ++i) palette[1][i] = static_cast<til::byte>(i * index[1]);
		for(std::vector<til::byte>::size_type i = 0, max = palette[2].size(); i < max; ++i) palette[2][i] = static_cast<til::byte>(i * index[2]);

		for (til::uint64 i = 0, max = image->GetWidth() * image->GetHeight(); i < max; ++i)
		{
			image_pixels[i] = RGB(
					palette[0][index[0] == 0x00 ? 0 : RED(image_pixels[i])   / index[0]],
					palette[1][index[1] == 0x00 ? 0 : GREEN(image_pixels[i]) / index[1]],
					palette[2][index[2] == 0x00 ? 0 : BLUE(image_pixels[i])  / index[2]]
				);
		}

		return true;
	}
}
