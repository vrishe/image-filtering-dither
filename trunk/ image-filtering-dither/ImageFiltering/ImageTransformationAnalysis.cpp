
#include "ImageTransformationAnalysis.h"
#include "IImageFilter.h"

#include <math.h>

namespace ipo
{
	namespace ImageTransformationAnalysis 
	{
		til::uint64 FindStandardDeviation(til::Image &ref_image, til::Image &res_image) {
			if (ref_image.GetBitDepth() != til::Image::BPP_32B_R8G8B8) return false;
			if (res_image.GetBitDepth() != til::Image::BPP_32B_R8G8B8) return false;

			til::uint32 *ref_image_pixels = reinterpret_cast<til::uint32*>(ref_image.GetPixels());
			til::uint32 *res_image_pixels = reinterpret_cast<til::uint32*>(res_image.GetPixels());

			if (ref_image.GetWidth() != res_image.GetWidth() || ref_image.GetHeight() != res_image.GetHeight()) return false;

			til::uint64 result = 0;
			for (til::uint64 i = 0, max = ref_image.GetWidth() * res_image.GetHeight(); i < max; ++i) {

				til::uint64 error = RED(ref_image_pixels[i])	- RED(res_image_pixels[i]) +
									GREEN(ref_image_pixels[i])	- GREEN(res_image_pixels[i]) +
									BLUE(ref_image_pixels[i])	- BLUE(res_image_pixels[i]);
				error *= error;
				result = max(result, error);
			}

			return result;
		}
	}
}



