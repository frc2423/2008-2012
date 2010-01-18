#include "HSLImage.h"

/**
 * Create a new 0x0 image.
 */
HSLImage::HSLImage() : ColorImage(IMAQ_IMAGE_HSL) {}

/**
 * Create a new image by loading a file.
 * @param fileName The path of the file to load.
 */
HSLImage::HSLImage(const char *fileName) : ColorImage(IMAQ_IMAGE_HSL)
{
	int success = imaqReadFile(image, fileName, NULL, NULL);
	wpi_imaqAssert(success, "Imaq ReadFile error");
}
