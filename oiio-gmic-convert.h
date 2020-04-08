#ifndef OIIOGMICCONVERT
#define OIIOGMICCONVERT

#include <OpenImageIO/imagebuf.h>
#include <gmic.h>

using namespace OIIO;

bool imageBufToGmic(std::shared_ptr<ImageBuf>&, gmic_image<float>&);
bool gmicToImageBuf(gmic_image<float>&, std::shared_ptr<ImageBuf>&);

#endif // OIIOGMICCONVERT_H
