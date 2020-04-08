#include "conversionops.h"

bool imageBufToGmic(std::shared_ptr<ImageBuf>& buf, gmic_image<float>& img)
{
    // Get amount of elements
    unsigned int elements = buf->xend() * buf->yend() * 3;
    unsigned int third = elements / 3;

    // Create heap array to copy into
    float *input = new float[elements];
    float *pinput = &input[0];

    // Copy ImageBuf data into array
    bool ok = buf->get_pixels(buf->roi_full(), TypeDesc::FLOAT, pinput);
    if (!ok)
    {
        std::cout << "There was a problem reading the ImageBuf." << std::endl;
        return 0;
    }

    // Copy from array into img while reordering from RGBRGB to RRGGBB
    for (unsigned int j = 0; j < third; j++)
    {
        img._data[j] = input[j * 3] * 255;
        img._data[j + third] = input[j * 3 + 1] * 255;
        img._data[j + third * 2] = input[j * 3 + 2] * 255;
    }

    delete [] input;

    return 1;
}

bool gmicToImageBuf(gmic_image<float>& img, std::shared_ptr<ImageBuf>& buf)
{
    // Get amount of elements
    unsigned int elements = buf->xend() * buf->yend() * 3;
    unsigned int third = elements / 3;

    // Create heap output array and set pointer to start of img data
    float *output = new float[elements];
    float *pinput = img._data;
    float *poutput = &output[0];

    // Copy into output array while reordering from RRGGBB to RGBRGB
    for (unsigned int k = 0; k < 3; k++)
        for (unsigned int j = 0; j < third; j++)
            output[j * 3 + k] = *(pinput++) / 255;

    // Use output array as raw data for ImageBuf
    bool ok = buf->set_pixels(buf->spec().roi(), TypeDesc::FLOAT, poutput);
    if (!ok)
    {
        std::cout << "Problem copying pixels to ImageBuf." << std::endl;
        std::cout << buf->geterror() << std::endl;
        return 0;
    }

    delete [] output;

    return 1;
}
