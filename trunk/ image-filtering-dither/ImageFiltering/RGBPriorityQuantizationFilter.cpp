
#include "RGBPriorityQuantizationFilter.h"

#include <iterator>
#include <vector>
#include <map>
#include <algorithm>

namespace ipo
{
	#define __foreach(it, i, c) for(it i = c.begin(), _end = c.end(); i != _end; ++i)

	typedef std::pair <til::uint32, size_t> rgb_palette_element;
	typedef std::multimap <til::uint32, size_t> rgb_palette;

	RGBPriorityQuantizationFilter::RGBPriorityQuantizationFilter(til::uint64 color_number) : _color_number(color_number) {}

	RGBPriorityQuantizationFilter::~RGBPriorityQuantizationFilter(void)	{}

	bool UDgreater_second (rgb_palette_element elem1, rgb_palette_element elem2) {
		return elem1.second > elem2.second;
	}

	til::uint32 find_nearest_color (std::vector <rgb_palette_element> _array, til::uint32 _value) {
		til::uint32 result = 0;
		til::uint32 difference = -1;

		__foreach(std::vector<rgb_palette_element>::iterator, element, _array) {
			til::uint32 cur_dif = 
				abs((RED	(_value) - RED	(element->first))) +
				abs((GREEN	(_value) - GREEN(element->first))) +
				abs((BLUE	(_value) - BLUE	(element->first)));
			if (cur_dif < difference) {
				difference = cur_dif;
				result = element->first;
			}
		}

		return result;
	}

	//bool UDless_first (rgb_palette_element elem1, rgb_palette_element elem2) {
	//	return elem1.first < elem2.first;
	//}

	//til::uint32 find_nearest_color (std::vector <rgb_palette_element> _array, til::uint32 _value) {
	//	size_t	imin = 0,
	//			imax = _array.size() - 1,
	//			imid;

	//	while (imin < imax) {
	//		imid = (imin + imax) / 2;
	//		if (imid >= imax) break;
 //
	//		// reduce the search
	//		if (_array[imid].first < _value)
	//			imin = imid + 1;
	//		else
	//			imax = imid;
	//	} // At exit of while:
	//	//   if _array is empty, then imax < imin
	//	//   otherwise imax == imin
 //
	//	if (imax == imin)
	//		return _array[imin].first;
	//	else
	//		return -1;
	//}

	bool RGBPriorityQuantizationFilter::Apply(til::Image &image) const {
		if (image.GetBitDepth() != til::Image::BPP_32B_R8G8B8) return false;

		til::uint32 *image_pixels = reinterpret_cast<til::uint32*>(image.GetPixels());

		rgb_palette palette;

		til::uint64 pixelCount = image.GetWidth() * image.GetHeight();
		for (til::uint64 i = 0; i < pixelCount; ++i) {
			rgb_palette::iterator element = palette.find(image_pixels[i]);
			if (element != palette.end()) {
				element->second++;
			}
			else {
				palette.insert(rgb_palette_element(image_pixels[i], 1));
			}
		}
		
		til::uint64 palette_size = palette.size();
		std::vector <rgb_palette_element> palette_final(static_cast<std::size_t>(palette_size));
		rgb_palette::iterator iterator = palette.begin();
		for (til::uint64 i = 0; i < palette_size; ++i) {
			palette_final[static_cast<std::size_t>(i)] = *iterator;
			iterator++;
		}
		std::sort(palette_final.begin(), palette_final.end(), UDgreater_second);
		palette_final.resize(static_cast<std::size_t>(_color_number));
		
		for (til::uint64 i = 0; i < pixelCount; ++i)
		{
			image_pixels[i] = find_nearest_color(palette_final, image_pixels[i]);
		}

		return true;
	}
}
