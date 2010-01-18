#ifndef COLORIMAGE_H_
#define COLORIMAGE_H_
#include "ImageBase.h"
#include "BinaryImage.h"
#include "Threshold.h"

class ColorImage : public ImageBase
{
public:
	ColorImage(ImageType type);
	BinaryImage * thresholdRGB(int redLow, int redHigh, int greenLow, int greenHigh, int blueLow, int blueHigh);
	BinaryImage * thresholdHSL(int hueLow, int hueHigh, int saturationLow, int saturationHigh, int luminenceLow, int luminenceHigh);
	BinaryImage * thresholdHSV(int hueLow, int hueHigh, int saturationLow, int saturationHigh, int valueHigh, int valueLow);
	BinaryImage * thresholdHSI(int hueLow, int hueHigh, int saturationLow, int saturationHigh, int intensityLow, int intensityHigh);
	BinaryImage * thresholdRGB(Threshold &threshold);
	BinaryImage * thresholdHSL(Threshold &threshold);
	BinaryImage * thresholdHSV(Threshold &threshold);
	BinaryImage * thresholdHSI(Threshold &threshold);
	MonoImage * getRedPlane();
	MonoImage * getGreenPlane();
	MonoImage * getBluePlane();
	MonoImage * getHSLHuePlane();
	MonoImage * getHSVHuePlane();
	MonoImage * getHSIHuePlane();
	MonoImage * getHSLSaturationPlane();
	MonoImage * getHSVSaturationPlane();
	MonoImage * getHSISaturationPlane();
	MonoImage * getLuminancePlane();
	MonoImage * getValuePlane();
	MonoImage * getIntensityPlane();
	void replaceRedPlane(MonoImage *plane);
	void replaceGreenPlane(MonoImage *plane);
	void replaceBluePlane(MonoImage *plane);
	void replaceHSLHuePlane(MonoImage *plane);
	void replaceHSVHuePlane(MonoImage *plane);
	void replaceHSIHuePlane(MonoImage *plane);
	void replaceHSLSaturationPlane(MonoImage *plane);
	void replaceHSVSaturationPlane(MonoImage *plane);
	void replaceHSISaturationPlane(MonoImage *plane);
	void replaceLuminancePlane(MonoImage *plane);
	void replaceValuePlane(MonoImage *plane);
	void replaceIntensityPlane(MonoImage *plane);
	void colorEqualize();
	void luminanceEqualize();
	
private:
	BinaryImage * threshold(ColorMode colorMode, int low1, int high1, int low2, int high2, int low3, int high3);
	void equalize(bool allPlanes);
	MonoImage * extractColorPlane(ColorMode mode, int planeNumber);
	MonoImage * extractFirstColorPlane(ColorMode mode);
	MonoImage * extractSecondColorPlane(ColorMode mode);
	MonoImage * extractThirdColorPlane(ColorMode mode);
	void replacePlane(ColorMode mode, MonoImage *plane, int planeNumber);
	void replaceFirstColorPlane(ColorMode mode, MonoImage *plane);
	void replaceSecondColorPlane(ColorMode mode, MonoImage *plane);
	void replaceThirdColorPlane(ColorMode mode, MonoImage *plane);
};
#endif
