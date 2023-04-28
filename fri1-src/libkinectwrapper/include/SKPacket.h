#ifndef SK_PACKET_H
#define SK_PACKET_H

#include <Eigen/Eigen>
#include <k4a/k4a.h>

#include <opencv2/opencv.hpp>

#include <map>
#include <string>

#include <cstdlib>
#include "SKWrapper.h"

class SKConfig;
class SKWrapper;

class SKPacket {
public:
    SKPacket(SKWrapper *wrapper);
    ~SKPacket();

    SKWrapper *getSKWrapper();

    k4a::image getDepthImage();
    k4a::image getColorImage();

    k4a::capture getCapture();
    void setCapture(k4a::capture capture);

    cv::Mat &getCVMat(std::string name);
    void allocateCVMat(int rows, int cols, int format, std::string name);
    void setCVMat(cv::Mat mat, std::string name);
    void copyCVMat(std::string fromName, std::string toName);

    Eigen::MatrixXd &getEigenMat(std::string name);

    k4a::image &getK4AImage(std::string name);

private:
    SKWrapper *_wrapper;
    std::map<std::string, cv::Mat> _namedMats;
    std::map<std::string, Eigen::MatrixXd> _namedEigenMats;
    std::map<std::string, k4a::image> _namedImages;

    k4a::capture _capture;
    k4a::image _depthImage, _colorImage;
};

#endif