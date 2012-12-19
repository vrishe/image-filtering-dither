#ifndef _UNIFORMDISTRIBUTIONDITHER_H_
#define _UNIFORMDISTRIBUTIONDITHER_H_

#include "IImageFilter.h"

namespace ipo
{
	class UniformDistributionDither :
		public IImageFilter
	{
	private:
		til::Image *_ref_image;
		float _distrib_mask[8];
		til::byte _move_pattern;

		til::uint32 colorWithError(const til::uint32 *image_pixels, til::uint64 pos, int red_err, int green_err, int blue_err) const;
	public:
		UniformDistributionDither(til::Image &image, float distibution_mask[], til::byte movement_pattern);
		~UniformDistributionDither();

		bool Apply(til::Image &image) const;
	};
}

#define DITH_MOVE_HORIZONTAL 0x00
#define DITH_MOVE_VERTICAL 0x01
#define DITH_STARTPOINT_CHANGE 0x02
#define DITH_STARTPOINT_REMAIN 0x00

#endif