#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdio.h>
#include "nivision.h"
#include "Utility.h"

#define DEFAULT_BORDER_SIZE				3

class ImageBase
{
public:
	Image *image;
	ImageBase(ImageType type);
	~ImageBase();
	void write(const char *fileName);
	int getHeight();
	int getWidth();
};
#endif
