#include "RGBImage.h"

/**
 * Create a new 0x0 image.
 */
RGBImage::RGBImage() : ColorImage(IMAQ_IMAGE_RGB)
{
}

/**
 * Create a new image by loading a file.
 * @param fileName The path of the file to load.
 */
RGBImage::RGBImage(const char *fileName) : ColorImage(IMAQ_IMAGE_RGB)
{
	imaqReadFile(image, fileName, NULL, NULL);
}
