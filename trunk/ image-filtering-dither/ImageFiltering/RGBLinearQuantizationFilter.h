#ifndef _PALETTELINEARQUANTIZATIONFILTER_H_
#define _PALETTELINEARQUANTIZATIONFILTER_H_

#include "IImageFilter.h"

namespace ipo
{
	class RGBLinearQuantizationFilter :
		public IImageFilter
	{
	public:
		typedef unsigned long rgb_quants;

	private:
		rgb_quants _palette_quants;

	public:

		RGBLinearQuantizationFilter(rgb_quants palette_quants);
		~RGBLinearQuantizationFilter();

		bool Apply(til::Image &image) const;
	};
}

#define RGB_QUANTS(r, g, b) ((ipo::RGBLinearQuantizationFilter::rgb_quants)(((til::byte)r) | ((til::byte)g) << 8 | ((til::byte)b) << 16))

#endif

