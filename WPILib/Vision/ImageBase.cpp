/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include "ImageBase.h"
#include "VisionAPI.h"

/**
 * Create a new instance of an ImageBase.
 * Imagebase is the base of all the other image classes. The constructor
 * creates any type of image and stores the pointer to it in the class.
 * @param type The type of image to create
 */
ImageBase::ImageBase(ImageType type)
{
	image = imaqCreateImage(type, DEFAULT_BORDER_SIZE);
}

/**
 * Frees memory associated with an ImageBase.
 * Destructor frees the imaq image allocated with the class.
 */
ImageBase::~ImageBase()
{
	if(image)
		imaqDispose(image);
}

/**
 * Writes an image to a file with the given filename.
 * Write the image to a file in the flash on the cRIO.
 * @param fileName The name of the file to write
 */
void ImageBase::write(const char *fileName)
{
	Priv_SetWriteFileAllowed(1);
	int success = imaqWriteFile(image, fileName, NULL);
	wpi_imaqAssert(success, "Imaq Image writeFile error");
}

/**
 * Gets the height of an image.
 * @returns height of the image in pixels.
 */
int ImageBase::getHeight()
{
	int height;
	imaqGetImageSize(image, NULL, &height);
	return height;
}

/**
 * Gets the width of an image.
 * @returns the width of the image in pixels.
 */
int ImageBase::getWidth()
{
	int width;
	imaqGetImageSize(image, &width, NULL);
	return width;
}
