#include "SKPacket.h"
#include "SKConfig.h"
#include <opencv2/imgproc.hpp>

using namespace std;

SKPacket::SKPacket(SKWrapper *wrapper) : _wrapper(wrapper) {
}

SKPacket::~SKPacket() { 
}

SKWrapper *SKPacket::getSKWrapper(){
    return _wrapper;
}

k4a::image SKPacket::getDepthImage(){
    return _depthImage;
}

k4a::image SKPacket::getColorImage(){
    return _colorImage;
}

k4a::capture SKPacket::getCapture() {
    return _capture;
}

void SKPacket::setCapture(k4a::capture cap) {
    _capture = cap;
    _colorImage = _capture.get_color_image();

    uint8_t *buffer = _colorImage.get_buffer();

    //image.get_format() == K4A_IMAGE_FORMAT_COLOR_BGRA32
    allocateCVMat(_colorImage.get_height_pixels(), _colorImage.get_width_pixels(), CV_8UC4, "BGRA1080p");
    allocateCVMat(_colorImage.get_height_pixels(), _colorImage.get_width_pixels(), CV_8UC3, "RGB1080p");
    memcpy(_namedMats["BGRA1080p"].data, buffer, _colorImage.get_width_pixels() * _colorImage.get_height_pixels() * 4);
    cv::cvtColor(_namedMats["BGRA1080p"], _namedMats["RGB1080p"], cv::COLOR_BGRA2RGB);

    _depthImage = _capture.get_depth_image();
    uint8_t *depth_buffer = _depthImage.get_buffer();

    //image.get_format() == K4A_IMAGE_FORMAT_COLOR_BGRA32
    allocateCVMat(_depthImage.get_height_pixels(), _depthImage.get_width_pixels(), CV_16U, "DEPTH16");
    memcpy(_namedMats["DEPTH16"].data, depth_buffer, _depthImage.get_width_pixels() * _depthImage.get_height_pixels() * 2);

    // xyzPointCloud =
    //     k4a::image::create(
    //         K4A_IMAGE_FORMAT_CUSTOM,
    //         depth_image.get_width_pixels(), depth_image.get_height_pixels(),
    //         depth_image.get_width_pixels() * 3 * sizeof(int16_t));
    //_wrapper->transformation.depth_image_to_point_cloud(depth_image, K4A_CALIBRATION_TYPE_DEPTH, &xyzPointCloud);

} 

cv::Mat &SKPacket::getCVMat(std::string name) {
    return _namedMats[name];
}

void SKPacket::allocateCVMat(int rows, int cols, int format, std::string name) {
    cv::Mat mat = cv::Mat(rows, cols, format);
    _namedMats[name] = mat;
}

void SKPacket::copyCVMat(std::string fromName, std::string toName) {
    //cv::Mat &fromMat = _namedMats[fromName];
    //cv::Mat mat(fromMat.rows, fromMat.cols, fromMat.type());
    //fromMat.copyTo(mat);
    //_namedMats[toName] = mat;

    cv::Mat &fromMat = _namedMats[fromName];
    fromMat.copyTo(_namedMats[toName]);

}

void SKPacket::setCVMat(cv::Mat mat, std::string name) {
    _namedMats[name] = mat;
}

Eigen::MatrixXd &SKPacket::getEigenMat(std::string name) {
    return _namedEigenMats[name];
}


k4a::image &SKPacket::getK4AImage(std::string name) {
    return _namedImages[name];
}