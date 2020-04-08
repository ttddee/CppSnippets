#include "colorops.h"
#include <OpenImageIO/imagebufalgo_util.h>

using Eigen::Matrix4f;

Matrix4d createGainMatrix(const double& g)
{
    Matrix4d gainMatrix(4, 4);
    gainMatrix.setIdentity();
    gainMatrix(0, 0) = g;
    gainMatrix(1, 1) = g;
    gainMatrix(2, 2) = g;
    return gainMatrix;
}

Matrix4d createLiftMatrix(const double& l)
{
    Matrix4d liftMatrix(4, 4);
    liftMatrix.setIdentity();
    liftMatrix(3, 0) = l;
    liftMatrix(3, 1) = l;
    liftMatrix(3, 2) = l;
    return liftMatrix;
}

Matrix4d createHueMatrix(const int& r)
{
    Matrix4d hueMatrix(4, 4);
    hueMatrix.setIdentity();
    hueRotateMatrix(hueMatrix, r);
    return hueMatrix;
}

void hueRotateMatrix(Matrix4d& input, double rot)
{
    // Does not preserve luminance!
    // See http://www.graficaobscura.com/matrix/index.html

    //Rotate grey into positive Z
    double mag = sqrt(2.0);
    const double xrs = 1.0 / mag;
    const double xrc = xrs;
    xRotateMatrix(input, xrs, xrc);
    mag = sqrt(3.0);
    const double yrs = -1.0 / mag;
    const double yrc = sqrt(2.0) / mag;
    yRotateMatrix(input, yrs, yrc);
    // Rotate hue
    const double zrs = sin(rot * M_PI / 180.0);
    const double zrc = cos(rot * M_PI / 180.0);
    zRotateMatrix(input, zrs, zrc);
    // Rotate grey vector back
    yRotateMatrix(input, -yrs, yrc);
    xRotateMatrix(input, -xrs, xrc);
}

void xRotateMatrix(Matrix4d& input, double rs, double rc)
{
    Matrix4d rot(4, 4);
    rot.setIdentity();
    rot(1, 1) = rc;
    rot(1, 2) = rs;
    rot(2, 1) = -rs;
    rot(2, 2) = rc;
    input = input * rot;
}

void yRotateMatrix(Matrix4d& input, double rs, double rc)
{
    Matrix4d rot(4, 4);
    rot.setIdentity();
    rot(0, 0) = rc;
    rot(0, 2) = -rs;
    rot(2, 0) = rs;
    rot(2, 2) = rc;
    input = input * rot;
}

void zRotateMatrix(Matrix4d& input, double rs, double rc)
{
    Matrix4d rot(4, 4);
    rot.setIdentity();
    rot(0, 0) = rc;
    rot(0, 1) = rs;
    rot(1, 0) = -rs;
    rot(1, 1) = rc;
    input = input * rot;
}

Matrix4d createSaturationMatrix(const double& s)
{
    Matrix4d satMatrix(4, 4);
    satMatrix.setIdentity();

    const double rwgt = 0.3086;
    const double gwgt = 0.6094;
    const double bwgt = 0.0820;

    double _a = (1.0 - s) * rwgt + s;
    double _b = (1.0 - s) * rwgt;
    double _c = (1.0 - s) * rwgt;
    double _d = (1.0 - s) * gwgt;
    double _e = (1.0 - s) * gwgt + s;
    double _f = (1.0 - s) * gwgt;
    double _g = (1.0 - s) * bwgt;
    double _h = (1.0 - s) * bwgt;
    double _i = (1.0 - s) * bwgt + s;

    satMatrix << _a, _b, _c, 0,
                 _d, _e, _f, 0,
                 _g, _h, _i, 0,
                 0,  0,  0,  1;
    return satMatrix;
}

Matrix4d createRGBMatrix(double r, double g, double b)
{
    Matrix4d rgbMatrix(4, 4);
    rgbMatrix.setIdentity();
    rgbMatrix << r, 0, 0, 0,
                 0, g, 0, 0,
                 0, 0, b, 0,
                 0, 0, 0, 1;
    return rgbMatrix;
}

void applyMatricesToImage(ImageBuf& image, const std::vector<Matrix4d>& matrices)
{
    Matrix4f all(4, 4);
    all.setIdentity();
    for (const Matrix4d& m : matrices)
        all *= m.cast<float>();

    float c[4][4] = {
        {all(0,0), all(0,1), all(0,2), all(0,3)}, // Mult R
        {all(1,0), all(1,1), all(1,2), all(1,3)}, // Mult G
        {all(2,0), all(2,1), all(2,2), all(2,3)}, // Mult B
        {all(3,0), all(3,1), all(3,2), all(3,3)}  // Add RGB
    }; // Raw array is faster than eigen matrix

    ImageBufAlgo::parallel_image(image.roi_full(), 0, [&](ROI roi)
    {
        ImageBuf::Iterator<float> d(image, roi);
        for (; !d.done(); ++d)
        {
            d[0] = d[0] * c[0][0] + d[1] * c[1][0] + d[2] * c[2][0] + c[3][0];
            d[1] = d[0] * c[0][1] + d[1] * c[1][1] + d[2] * c[2][1] + c[3][1];
            d[2] = d[0] * c[0][2] + d[1] * c[1][2] + d[2] * c[2][2] + c[3][2];
        }
    });
}

void desaturateImage(ImageBuf& input)
{
    float weights[3] = { 0.3086f, 0.6094f, 0.0820f };
    input = ImageBufAlgo::channel_sum(input, weights);
    // Copy R channel to G and B
    int channelorder[] = {0, 0, 0};
    input = ImageBufAlgo::channels(input, 3, channelorder);
}
