
#include "UniformDistributionDither.h"

#include <exception>

namespace ipo
{
	UniformDistributionDither::UniformDistributionDither(til::Image &image, float distibution_mask[], til::byte movement_pattern) : 
		_move_pattern(movement_pattern) {
		_ref_image = &image;
		/*for (int i = 0; i < 8; ++i)
			_distrib_mask[i] = distibution_mask[i];*/
		CopyMemory(_distrib_mask, distibution_mask, 8 * sizeof(float));
	}

	UniformDistributionDither::~UniformDistributionDither(void)	{}

	til::uint32 UniformDistributionDither::colorWithError(const til::uint32 *image_pixels, til::uint64 pos, int red_err, int green_err, int blue_err) const {
		til::byte red = RED(image_pixels[pos]) + static_cast<til::byte>(red_err * _distrib_mask[0]);
		if (red_err > 0 && red < RED(image_pixels[pos]))
			red = 0xff;
		else if (red_err < 0 && red > RED(image_pixels[pos]))
			red = 0x00;

		til::byte green = GREEN(image_pixels[pos]) + static_cast<til::byte>(green_err * _distrib_mask[0]);
		if (green_err > 0 && green < GREEN(image_pixels[pos]))
			green = 0xff;
		else if (green_err < 0 && green > GREEN(image_pixels[pos]))
			green = 0x00;

		til::byte blue = BLUE(image_pixels[pos]) + static_cast<til::byte>(blue_err * _distrib_mask[0]);
		if (blue_err > 0 && blue < BLUE(image_pixels[pos]))
			blue = 0xff;
		else if (blue_err < 0 && blue > BLUE(image_pixels[pos]))
			blue = 0x00;

		return RGB(red, green, blue);
	}

	bool UniformDistributionDither::Apply(til::Image &image) const {
		if (image.GetBitDepth() != til::Image::BPP_32B_R8G8B8) return false;

		til::uint32 *image_pixels = reinterpret_cast<til::uint32*>(image.GetPixels());
		til::uint32 *ref_image_pixels = reinterpret_cast<til::uint32*>(_ref_image->GetPixels());

		if (_ref_image->GetWidth() != image.GetWidth() || _ref_image->GetHeight() != image.GetHeight()) return false;

		til::uint64 imageWidth, imageHeight;
		if ( !static_cast<til::byte>(_move_pattern << 7) ) {
			imageWidth	= image.GetWidth();
			imageHeight = image.GetHeight();
		}
		else {
			imageWidth	= image.GetHeight();
			imageHeight = image.GetWidth();
		}

		for (til::uint64 i = 0; i < imageHeight; ++i) {
			til::uint64 offset = 0;
			int changer = 1;
			if ( i % 2 == 1 && static_cast<til::byte>(_move_pattern << 6)) {
				offset = imageWidth - 1;
				changer = -1;
			}
			for (til::uint64 j = 0; j < imageWidth; ++j) {
				til::uint64 pos = i*imageWidth + (j*changer + offset);

				int red_err		= RED(ref_image_pixels[pos])	- RED(image_pixels[pos]),
					green_err	= GREEN(ref_image_pixels[pos])	- GREEN(image_pixels[pos]),
					blue_err	= BLUE(ref_image_pixels[pos])	- BLUE(image_pixels[pos]);

				if (red_err == 0 && green_err == 0 && blue_err == 0) continue;

				til::uint64 neighborhood[8] = {
					pos - imageWidth - 1,
					pos - imageWidth,
					pos - imageWidth + 1,
					pos - 1,
					pos + 1,
					pos + imageWidth + 1,
					pos + imageWidth,
					pos + imageWidth - 1
				};
				
				if (neighborhood[0] >= imageWidth * (i-1) && neighborhood[0] < pos)
					image_pixels[neighborhood[0]] = colorWithError(image_pixels, neighborhood[0], red_err, green_err, blue_err);
				if (neighborhood[1] <  pos)
					image_pixels[neighborhood[1]] = colorWithError(image_pixels, neighborhood[1], red_err, green_err, blue_err);
				if (neighborhood[2] <  imageWidth * i && neighborhood[0] < pos)
					image_pixels[neighborhood[2]] = colorWithError(image_pixels, neighborhood[2], red_err, green_err, blue_err);
				if (neighborhood[3] >= imageWidth * i && neighborhood[0] < pos)
					image_pixels[neighborhood[3]] = colorWithError(image_pixels, neighborhood[3], red_err, green_err, blue_err);

				if (neighborhood[4] <  imageWidth * (i+1) && neighborhood[4] > pos)
					image_pixels[neighborhood[4]] = colorWithError(image_pixels, neighborhood[4], red_err, green_err, blue_err);
				if (neighborhood[5] <  imageWidth * (i+2) && neighborhood[5] < imageWidth*imageHeight && neighborhood[5] > pos)
					image_pixels[neighborhood[5]] = colorWithError(image_pixels, neighborhood[5], red_err, green_err, blue_err);
				if (neighborhood[6] <= imageWidth*imageHeight && neighborhood[6] > pos)
					image_pixels[neighborhood[6]] = colorWithError(image_pixels, neighborhood[6], red_err, green_err, blue_err);
				if (neighborhood[7] >= imageWidth * i && neighborhood[7] < imageWidth*imageHeight && neighborhood[7] > pos)
					image_pixels[neighborhood[7]] = colorWithError(image_pixels, neighborhood[7], red_err, green_err, blue_err);
			}
		}

		return true;
	}
}