#ifndef COLOROPS_H
#define COLOROPS_H

#include <eigen3/Eigen/Dense>
#include <OpenImageIO/imagebuf.h>

using namespace OIIO;
using Eigen::Matrix4d;

Matrix4d createGainMatrix(const double&);
Matrix4d createLiftMatrix(const double&);
Matrix4d createHueMatrix(const int&);
void hueRotateMatrix(Matrix4d&, double);
void xRotateMatrix(Matrix4d&, double, double);
void yRotateMatrix(Matrix4d&, double, double);
void zRotateMatrix(Matrix4d&, double, double);
Matrix4d createSaturationMatrix(const double&);
Matrix4d createRGBMatrix(double, double, double);
void applyMatricesToImage(ImageBuf&, const std::vector<Matrix4d>&);
void desaturateImage(ImageBuf&);

#endif // COLOROPS_H
