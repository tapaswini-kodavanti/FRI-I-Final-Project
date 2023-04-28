#ifndef MASK_GENERATOR_H
#define MASK_GENERATOR_H

#include <python3.6/Python.h>
#include <opencv2/opencv.hpp>
#include <numpy/ndarrayobject.h>
#include <string>
#include <vector>

class MaskGenerator {
protected:
    cv::Mat subtraction;
    cv::Mat threshold;
    cv::Mat final_mask;
    npy_intp dimensions[3];
public:
    cv::Mat getModelMask(cv::Mat cbMat, std::string model);
    cv::Mat getColorMask(cv::Mat cbMat, std::string color);
    cv::Mat getRGBMask(cv::Mat cbMat);
    cv::Mat npToMat(PyObject *self);

};

#endif