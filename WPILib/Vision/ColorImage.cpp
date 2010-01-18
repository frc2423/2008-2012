#include "ColorImage.h"

ColorImage::ColorImage(ImageType type) : ImageBase(type)
{
}

/**
 * Perform a threshold operation on a ColorImage.
 * Perform a threshold operation on a ColorImage using the ColorMode supplied
 * as a parameter.
 * @param colorMode The type of colorspace this operation should be performed in
 * @returns a pointer to a binary image
 */
BinaryImage * ColorImage::threshold(ColorMode colorMode,
		int low1, int high1,
		int low2, int high2,
		int low3, int high3)
{
	BinaryImage *result = new BinaryImage();
	Range range1 = {low1, high1},
		range2 = {low2, high2},
		range3 = {low3, high3};
	
	int success = imaqColorThreshold(result->image, image, 1, colorMode, &range1, &range2, &range3);
	wpi_imaqAssert(success, "ImaqThreshold error");
	return result;
}

/**
 * Perform a threshold in RGB space.
 * @param redLow Red low value
 * @param redHigh Red high value
 * @param greenLow Green low value
 * @param greenHigh Green high value
 * @param blueLow Blue low value
 * @param blueHigh Blue high value
 * @returns A pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage * ColorImage::thresholdRGB(int redLow, int redHigh, int greenLow, int greenHigh, int blueLow, int blueHigh)
{
	return threshold(IMAQ_RGB, redLow, redHigh, greenLow, greenHigh, blueLow, blueHigh);
}

/**
 * Perform a threshold in RGB space.
 * @param threshold a reference to the Threshold object to use.
 * @returns A pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage * ColorImage::thresholdRGB(Threshold &t)
{
	return threshold(IMAQ_RGB, t.plane1Low, t.plane1High,
								t.plane2Low, t.plane2High,
								t.plane3Low, t.plane3High);
}

/**
 * Perform a threshold in HSL space.
 * @param hueLow Low value for hue
 * @param hueHigh High value for hue
 * @param saturationLow Low value for saturation
 * @param saturationHigh High value for saturation
 * @param luminenceLow Low value for luminence
 * @param luminenceHigh High value for luminence
 * @returns a pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage * ColorImage::thresholdHSL(int hueLow, int hueHigh, int saturationLow, int saturationHigh, int luminenceLow, int luminenceHigh)
{
	return threshold(IMAQ_HSL, hueLow, hueHigh, saturationLow, saturationHigh, luminenceLow, luminenceHigh);
}

/**
 * Perform a threshold in HSL space.
 * @param threshold a reference to the Threshold object to use.
 * @returns A pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage * ColorImage::thresholdHSL(Threshold &t)
{
	return threshold(IMAQ_HSL, t.plane1Low, t.plane1High,
								t.plane2Low, t.plane2High,
								t.plane3Low, t.plane3High);
}

/**
 * Perform a threshold in HSV space.
 * @param hueLow Low value for hue
 * @param hueHigh High value for hue
 * @param saturationLow Low value for saturation
 * @param saturationHigh High value for saturation
 * @param valueLow Low value
 * @param valueHigh High value
 * @returns a pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage * ColorImage::thresholdHSV(int hueLow, int hueHigh, int saturationLow, int saturationHigh, int valueLow, int valueHigh)
{
	return threshold(IMAQ_HSV, hueLow, hueHigh, saturationLow, saturationHigh, valueLow, valueHigh);
}

/**
 * Perform a threshold in HSV space.
 * @param threshold a reference to the Threshold object to use.
 * @returns A pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage * ColorImage::thresholdHSV(Threshold &t)
{
	return threshold(IMAQ_HSV, t.plane1Low, t.plane1High,
								t.plane2Low, t.plane2High,
								t.plane3Low, t.plane3High);
}

/**
 * Perform a threshold in HSI space.
 * @param hueLow Low value for hue
 * @param hueHigh High value for hue
 * @param saturationLow Low value for saturation
 * @param saturationHigh High value for saturation
 * @param valueLow Low intensity
 * @param valueHigh High intensity
 * @returns a pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage * ColorImage::thresholdHSI(int hueLow, int hueHigh, int saturationLow, int saturationHigh, int intensityLow, int intensityHigh)
{
	return threshold(IMAQ_HSI, hueLow, hueHigh, saturationLow, saturationHigh, intensityLow, intensityHigh);
}

/**
 * Perform a threshold in HSI space.
 * @param threshold a reference to the Threshold object to use.
 * @returns A pointer to a BinaryImage that represents the result of the threshold operation.
 */
BinaryImage * ColorImage::thresholdHSI(Threshold &t)
{
	return threshold(IMAQ_HSI, t.plane1Low, t.plane1High,
								t.plane2Low, t.plane2High,
								t.plane3Low, t.plane3High);
}

/**
 * Extract a color plane from the image
 * @param mode The ColorMode to use for the plane extraction
 * @param planeNumber Which plane is to be extracted
 * @returns A pointer to a MonoImage that represents the extracted plane.
 */
MonoImage * ColorImage::extractColorPlane(ColorMode mode, int planeNumber) {
	MonoImage *result = new MonoImage();
	wpi_assert(image != NULL);
	int success = imaqExtractColorPlanes(image, 
										 mode, 
										 (planeNumber == 1) ? result->image : NULL, 
										 (planeNumber == 2) ? result->image : NULL, 
										 (planeNumber == 3) ? result->image : NULL);
	wpi_imaqAssert(success, "Imaq ExtractColorPlanes failed");
	return result;
}

/*
 * Extract the first color plane for an image.
 * @param mode The color mode in which to operate
 * @returns a pointer to a MonoImage that is the extracted plane.
 */
MonoImage * ColorImage::extractFirstColorPlane(ColorMode mode)
{
	return extractColorPlane(mode, 1);
}

/*
 * Extract the second color plane for an image.
 * @param mode The color mode in which to operate
 * @returns a pointer to a MonoImage that is the extracted plane.
 */
MonoImage * ColorImage::extractSecondColorPlane(ColorMode mode)
{
	return extractColorPlane(mode, 2);
}

/*
 * Extract the third color plane for an image.
 * @param mode The color mode in which to operate
 * @returns a pointer to a MonoImage that is the extracted plane.
 */
MonoImage * ColorImage::extractThirdColorPlane(ColorMode mode)
{
	return extractColorPlane(mode, 3);
}

/*
 * Extract the red plane from an RGB image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage * ColorImage::getRedPlane()
{
	return extractFirstColorPlane(IMAQ_RGB);
}

/*
 * Extract the green plane from an RGB image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage * ColorImage::getGreenPlane()
{
    return extractSecondColorPlane(IMAQ_RGB);
}

/*
 * Extract the blue plane from an RGB image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage * ColorImage::getBluePlane()
{
    return extractThirdColorPlane(IMAQ_RGB);
}

/*
 * Extract the Hue plane from an HSL image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage * ColorImage::getHSLHuePlane()
{
	return extractFirstColorPlane(IMAQ_HSL);
}

/*
 * Extract the Hue plane from an HSV image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage * ColorImage::getHSVHuePlane()
{
	return extractFirstColorPlane(IMAQ_HSV);
}

/*
 * Extract the Hue plane from an HSI image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage * ColorImage::getHSIHuePlane()
{
	return extractFirstColorPlane(IMAQ_HSI);
}

/*
 * Extract the Luminance plane from an HSL image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage * ColorImage::getLuminancePlane()
{
	return extractThirdColorPlane(IMAQ_HSL);
}

/*
 * Extract the Value plane from an HSV image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage * ColorImage::getValuePlane()
{
	return extractThirdColorPlane(IMAQ_HSV);
}

/*
 * Extract the Intensity plane from an HSI image.
 * @returns a pointer to a MonoImage that is the extraced plane.
 */
MonoImage * ColorImage::getIntensityPlane()
{
	return extractThirdColorPlane(IMAQ_HSI);
}

/**
 * Replace a plane in the ColorImage with a MonoImage
 * Replaces a single plane in the image with a MonoImage
 * @param mode The ColorMode in which to operate
 * @param plane The pointer to the replacement plane as a MonoImage
 * @param planeNumber The plane number (1, 2, 3) to replace
 */
void ColorImage::replacePlane(ColorMode mode, MonoImage *plane, int planeNumber) {
	int success = imaqReplaceColorPlanes(image, 
										 (const Image*) image, 
									     mode, 
									     (planeNumber == 1) ? plane->image : NULL, 
									     (planeNumber == 2) ? plane->image : NULL, 
									     (planeNumber == 3) ? plane->image : NULL);
	wpi_imaqAssert(success, "Imaq ReplaceColorPlanes failed");
}

/**
 * Replace the first color plane with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceFirstColorPlane(ColorMode mode, MonoImage *plane)
{
	replacePlane(mode, plane, 1);
}

/**
 * Replace the second color plane with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceSecondColorPlane(ColorMode mode, MonoImage *plane)
{
	replacePlane(mode, plane, 2);
}

/**
 * Replace the third color plane with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceThirdColorPlane(ColorMode mode, MonoImage *plane)
{ 
	replacePlane(mode, plane, 3);
}

/**
 * Replace the red color plane with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceRedPlane(MonoImage *plane)
{
	replaceFirstColorPlane(IMAQ_RGB, plane);
}

/**
 * Replace the green color plane with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceGreenPlane(MonoImage *plane)
{
	replaceSecondColorPlane(IMAQ_RGB, plane);
}

/**
 * Replace the blue color plane with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceBluePlane(MonoImage *plane)
{
	replaceThirdColorPlane(IMAQ_RGB, plane);
}


/**
 * Replace the Hue color plane in a HSL image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceHSLHuePlane(MonoImage *plane)
{
	return replaceFirstColorPlane(IMAQ_HSL, plane);
}

/**
 * Replace the Hue color plane in a HSV image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceHSVHuePlane(MonoImage *plane)
{
	return replaceFirstColorPlane(IMAQ_HSV, plane);
}

/**
 * Replace the first Hue plane in a HSI image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceHSIHuePlane(MonoImage *plane)
{
	return replaceFirstColorPlane(IMAQ_HSI, plane);
}

/**
 * Replace the Saturation color plane in an HSL image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceHSLSaturationPlane(MonoImage *plane)
{
	return replaceSecondColorPlane(IMAQ_HSL, plane);
}

/**
 * Replace the Saturation color plane in a HSV image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceHSVSaturationPlane(MonoImage *plane)
{
	return replaceSecondColorPlane(IMAQ_HSV, plane);
}

/**
 * Replace the Saturation color plane in a HSI image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceHSISaturationPlane(MonoImage *plane)
{
	return replaceSecondColorPlane(IMAQ_HSI, plane);
}

/**
 * Replace the Luminance color plane in an HSL image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceLuminancePlane(MonoImage *plane)
{
	return replaceThirdColorPlane(IMAQ_HSL, plane);
}

/**
 * Replace the Value color plane in an HSV with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceValuePlane(MonoImage *plane)
{
	return replaceThirdColorPlane(IMAQ_HSV, plane);
}

/**
 * Replace the Intensity color plane in a HSI image with a MonoImage.
 * @param mode The color mode in which to operate.
 * @param plane A pointer to a MonoImage that will replace the specified color plane.
 */
void ColorImage::replaceIntensityPlane(MonoImage *plane)
{
	return replaceThirdColorPlane(IMAQ_HSI, plane);
}

//TODO: frcColorEqualize(Image* dest, const Image* source, int colorEqualization) needs to be modified
//The colorEqualization parameter is discarded and is set to TRUE in the call to imaqColorEqualize.
void ColorImage::equalize(bool allPlanes) {
	// Note that this call uses NI-defined TRUE and FALSE
	int success = imaqColorEqualize(image, (const Image*) image, (allPlanes) ? TRUE : FALSE);
	wpi_imaqAssert(success, "Imaq ColorEqualize error");
}

void ColorImage::colorEqualize()
{
	equalize(true);
}

void ColorImage::luminanceEqualize()
{
	equalize(false);
}
