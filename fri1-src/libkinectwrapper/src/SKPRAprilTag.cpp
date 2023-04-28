#include "SKPRAprilTag.h"
#include "SKPacket.h"
#include "SKConfig.h"
#include <apriltag/apriltag.h>
#include <apriltag/tagStandard41h12.h>
#include <apriltag/tag36h11.h>

using namespace cv;
using namespace std;


SKPRAprilTag::SKPRAprilTag(
    std::string image, std::string outImage, std::string resultMat, bool render) :
    _image(image),  _resultMat(resultMat), _outImage(outImage), _render(render) {
    td = apriltag_detector_create();
    tf = tag36h11_create();
    apriltag_detector_add_family(td, tf);
}

void SKPRAprilTag::receiveFrame(SKPacket &skp) {
    skp.allocateCVMat(skp.getCVMat(_outImage).rows, skp.getCVMat(_outImage).cols, CV_8UC3, _outImage);
    skp.copyCVMat(_image, _outImage);
    skp.getEigenMat(_resultMat) = Eigen::MatrixXd(0,0);

    Mat gray;
    cvtColor(skp.getCVMat(_image), gray, COLOR_BGR2GRAY);
    image_u8_t im = {.width = gray.cols,
                     .height = gray.rows,
                     .stride = gray.cols,
                     .buf = gray.data};

    zarray_t* tags = apriltag_detector_detect(td, &im);

    // TODO assumes only one apriltag in frame - how do we want to handle more?
    for (int i = 0; i < zarray_size(tags); i++){
        apriltag_detection_t *det;
        zarray_get(tags, i, &det);

        Eigen::MatrixXd detectedCorners(2,4);

        for (int i = 0; i < 4; i++){
            double x = det->p[i][0];
            double y = det->p[i][1];
            detectedCorners(0, i) = x;
            detectedCorners(1, i) = y;
            cv::Point detection(x, y);
            cv::circle(skp.getCVMat(_outImage), detection, 5, Scalar(255, 255, 255), 5);
        }


        skp.getEigenMat(_resultMat) = detectedCorners;
    }

    for(size_t i = 0; i < _recipients.size(); i++) {
        _recipients[i]->receiveFrame(skp);
    }
}

void SKPRAprilTag::addRecipient(SKPRecipient *skpr) {
    _recipients.push_back(skpr);
}
