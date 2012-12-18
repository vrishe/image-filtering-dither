#ifndef _PALETTEPRIORITYQUANTIZATIONFILTER_H_
#define _PALETTEPRIORITYQUANTIZATIONFILTER_H_

#include "IImageFilter.h"


namespace ipo
{
	class RGBPriorityQuantizationFilter :
		public IImageFilter
	{
	private:
		til::uint64 _color_number;

	public:
		RGBPriorityQuantizationFilter(til::uint64 color_number);
		~RGBPriorityQuantizationFilter();

		bool Apply(til::Image *image) const;
	};
}

#endif

