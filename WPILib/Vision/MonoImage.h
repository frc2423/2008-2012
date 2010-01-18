#ifndef MONOIMAGE_H_
#define MONOIMAGE_H_

#include "ImageBase.h"

#include <vector>

using namespace std;

class MonoImage : public ImageBase
{
public:
	MonoImage();
	vector<EllipseMatch> * DetectEllipses(EllipseDescriptor *ellipseDescriptor,
					CurveOptions *curveOptions,
					ShapeDetectionOptions *shapeDetectionOptions,
					ROI *roi);
	vector<EllipseMatch> * DetectEllipses(EllipseDescriptor *ellipseDescriptor);
};
#endif
