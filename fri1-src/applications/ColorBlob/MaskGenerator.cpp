#include "MaskGenerator.h"
// #include <python3.6/Python.h>
#include <python3.6/Python.h>


#include <opencv2/opencv.hpp>
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/ndarrayobject.h>
#include <string>
#include <vector>
#include <iostream>

// export PYTHONPATH=`pwd` in build and in colorblob directory
cv::Mat MaskGenerator::getModelMask(cv::Mat cbMat, std::string model) {

    Py_Initialize();

    // Retrieve the correspondnig module and function for the masking technique
    char* c = const_cast<char*>(model.c_str());
    PyObject* python_module = PyImport_ImportModule(c);
    if (python_module == nullptr) {
        PyErr_Print();
         std::cout << "null";
    }
    PyObject* python_masking_func = PyObject_GetAttrString(python_module, "get_mask"); 

    // Convert the cv::Mat to an nparray
    dimensions[0] = cbMat.rows;
    dimensions[1] = cbMat.cols;
    dimensions[2] = cbMat.channels();

    cv::Mat input_mat;
    cv::cvtColor(cbMat, input_mat, cv::COLOR_BGR2RGB);
    PyObject* python_np_arr = PyArray_SimpleNewFromData(3, dimensions, NPY_UINT8, input_mat.data);

    // Call the masking function with the converted image data (result should be np array)
    PyObject* python_result = PyObject_CallObject(python_masking_func, python_np_arr);

    // Convert the numpy array to a cv::Mat
    cv::Mat cbMat_processed = npToMat(python_result);

    return cbMat;
}

cv::Mat MaskGenerator::npToMat(PyObject *self){
    PyArrayObject *image;
    cv::Mat blank = cv::Mat(1, 10, CV_32F, {1});
    return blank;
    if (!PyArg_ParseTuple(self, "o!", &PyArray_Type, &image)) {
        return blank;
    }

    int rows = dimensions[0]; //PyLong_AsLong(PyTuple_GetItem(size ,0));
    int cols = dimensions[1]; //PyLong_AsLong(PyTuple_GetItem(size ,1));
    int nchannels = dimensions[2];//PyLong_AsLong(PyTuple_GetItem(size ,2));
    char my_arr[rows * nchannels * cols];

    for(size_t length = 0; length<(rows * nchannels * cols); length++){
        my_arr[length] = (*(char *)PyArray_GETPTR1(image, length));
    }

    cv::Mat my_img = cv::Mat(cv::Size(cols, rows), CV_8UC3, &my_arr);
    return my_img;
}

cv::Mat MaskGenerator::getColorMask(cv::Mat cbMat, std::string color) {
    std::vector<cv::Mat> _chans(3);
    cv::split(cbMat, _chans);
    
    
    cv::Mat initial = color == "blue" ? _chans[2] : color == "green" ? _chans[1] : _chans[0];
    cv::Mat subtract = color == "blue" ? _chans[1] : color == "green" ? _chans[0] : _chans[2];
    int thresh = color == "green" ? 60 : 55;

    cv::subtract(initial, subtract, subtraction);
    cv::threshold(subtraction, threshold, thresh, 205, cv::THRESH_BINARY | cv::THRESH_OTSU);

    std::vector<cv::Mat> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(threshold, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    int largest_i = 0;
    for (int i = 0; i < contours.size(); i++) {

        if (cv::contourArea(contours[i]) > cv::contourArea(contours[largest_i])) {
            largest_i = i;
        }
    }
    cv::Mat mask = cv::Mat::zeros(_chans[0].rows, _chans[0].cols, CV_8UC1);
    cv::drawContours(mask, contours, largest_i, cv::Scalar(205), cv::LineTypes::FILLED, 8, hierarchy);

    return mask;
}

cv::Mat MaskGenerator::getRGBMask(cv::Mat cbMat) {
    cv::Mat blue_mask = getColorMask(cbMat, "blue");
    cv::Mat green_mask = getColorMask(cbMat, "green");
    cv::Mat red_mask = getColorMask(cbMat, "red");
    
    cv::bitwise_or(blue_mask, green_mask, final_mask);
    cv::bitwise_or(red_mask, final_mask, final_mask);
    return final_mask;
}

// void MaskGenerator::findBlue() {
//     cv::subtract(_chans[0], _chans[2], blue_subtraction);
//     cv::threshold(blue_subtraction, blue_threshold, 55, 205, cv::THRESH_BINARY);

//     std::vector<cv::Mat> contours;
//     std::vector<cv::Vec4i> hierarchy;
//     cv::findContours(blue_threshold, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

//     int largest_i = 0;
//     for (int i = 0; i < contours.size(); i++) {
//         if (cv::contourArea(contours[i]) > cv::contourArea(contours[largest_i])) {
//             largest_i = i;
//         }
//     }

//     blue_mask = cv::Mat::zeros(_chans[0].rows, _chans[0].cols, CV_8UC1);
//     cv::drawContours(blue_mask, contours, largest_i, cv::Scalar(205), cv::LineTypes::FILLED, 8, hierarchy);

//     cv::Mat temp;
//     _frame.cv::Mat::copyTo(temp, blue_mask);
//     blue_cup = temp;
//     skp.allocateCVMat(skp.getCVMat(blue_cup).rows, skp.getCVMat(blue_cup).cols, CV_8UC3, "blue_cup");
//     skp.copyCVMat("blue_cup", _outImage);
// }

// void MaskGenerator::findGreen() {
//     cv::subtract(_chans[1], _chans[0], green_subtraction);
//     cv::threshold(green_subtraction, green_threshold, 60, 205, cv::THRESH_BINARY);
    
//     std::vector<cv::Mat> contours;
//     std::vector<cv::Vec4i> hierarchy;
//     cv::findContours(green_threshold, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

//     int largest_i = 0;
//     for (int i = 0; i < contours.size(); i++) {
//         if (cv::contourArea(contours[i]) > cv::contourArea(contours[largest_i])) {
//             largest_i = i;
//         }
//     }

//     green_mask = cv::Mat::zeros(_chans[1].rows, _chans[1].cols, CV_8UC1);
//     cv::drawContours(green_mask, contours, largest_i, cv::Scalar(205), cv::LineTypes::FILLED, 8, hierarchy);
//     cv::Mat temp;
//     _frame.cv::Mat::copyTo(temp, green_mask);
//     green_cup = temp;
//     skp.allocateCVMat(skp.getCVMat(green_cup).rows, skp.getCVMat(green_cup).cols, CV_8UC3, "green_cup");
//     skp.copyCVMat("green_cup", _outImage);
// }

// void MaskGenerator::findRed() {
//     cv::subtract(_chans[2], _chans[1], red_subtraction);
//     cv::threshold(red_subtraction, red_threshold, 55, 205, cv::THRESH_BINARY);
    
//     std::vector<cv::Mat> contours;
//     std::vector<cv::Vec4i> hierarchy;
//     cv::findContours(red_threshold, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

//     int largest_i = 0;
//     for (int i = 0; i < contours.size(); i++) {
//         if (cv::contourArea(contours[i]) > cv::contourArea(contours[largest_i])) {
//             largest_i = i;
//         }
//     }
//     red_mask = cv::Mat::zeros(_chans[2].rows, _chans[2].cols, CV_8UC1);
//     cv::drawContours(red_mask, contours, largest_i, cv::Scalar(205), cv::LineTypes::FILLED, 8, hierarchy);
//     cv::Mat temp;
//     _frame.cv::Mat::copyTo(temp, red_mask);
//     red_cup = temp;
//     skp.allocateCVMat(skp.getCVMat(red_cup).rows, skp.getCVMat(red_cup).cols, CV_8UC3, "red_cup");
//     skp.copyCVMat("red_cup", _outImage);
// }