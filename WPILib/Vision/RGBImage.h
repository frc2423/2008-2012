#include "ColorImage.h"

/**
 * A color image represented in RGB color space at 3 bytes per pixel.
 * @author dtjones
 */
class RGBImage : public ColorImage {
public:

    /**
     * Create a new 0x0 image.
     */
    RGBImage();
    
    /**
     * Create a new image by loading a file.
     * @param fileName The path of the file to load.
     */
    RGBImage(const char *fileName);
};
