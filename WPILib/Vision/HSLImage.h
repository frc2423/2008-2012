#include "ColorImage.h"

class HSLImage : public ColorImage {
public:
    /**
     * Create a new 0x0 image.
     */
    HSLImage();

    /**
     * Create a new image by loading a file.
     * @param fileName The path of the file to load.
     */
    HSLImage(const char *fileName);

};
