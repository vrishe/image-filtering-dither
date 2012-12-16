#ifndef _IIMAGEFILTER_H_
#define _IIMAGEFILTER_H_

#include <TinyImageLoader.h>

#define RED(c)     ((til::byte)(((til::uint32)c)))
#define GREEN(c)   ((til::byte)(((til::uint32)c) >>  8))
#define BLUE(c)    ((til::byte)(((til::uint32)c) >> 16))

namespace ipo
{
	class IImageFilter
	{
	public:
		inline virtual ~IImageFilter() = 0 { };

		virtual bool Apply(til::Image *image) const = 0;
	};

}

#endif

