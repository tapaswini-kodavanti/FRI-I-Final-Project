#include <SKComputeXYZImage.h>
#include <SKPacket.h>

// #include <k4a/k4a.h>

SKComputeXYZImage::SKComputeXYZImage() {}
SKComputeXYZImage::~SKComputeXYZImage() {}

void SKComputeXYZImage::receiveFrame(SKPacket &skp) {
    k4a::image depth_image = skp.getDepthImage();
    skp.getK4AImage("K4AXYZ") = k4a::image::create(
        K4A_IMAGE_FORMAT_CUSTOM,
        depth_image.get_width_pixels(), depth_image.get_height_pixels(),
        depth_image.get_width_pixels() * 3 * sizeof(int16_t));

    skp.getSKWrapper()->transformation.depth_image_to_point_cloud(depth_image, K4A_CALIBRATION_TYPE_DEPTH, &skp.getK4AImage("K4AXYZ"));

    for(size_t i = 0; i < _recipients.size(); i++) {
        _recipients[i]->receiveFrame(skp);
    }
}

void SKComputeXYZImage::addRecipient(SKPRecipient *skpr) {
    _recipients.push_back(skpr);
}