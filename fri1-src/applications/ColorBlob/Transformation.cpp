#include "SMColorBlob.h"
#include "MaskGenerator.h"
#include "SKPacket.h"

void Transformation::applyTransformation() {
    _colorImage = _capture.get_color_image();

    uint8_t *buffer = _colorImage.get_buffer();

    //image.get_format() == K4A_IMAGE_FORMAT_COLOR_BGRA32

    _depthImage = _capture.get_depth_image();
    xyzPointCloud =
        k4a::image::create(
            K4A_IMAGE_FORMAT_CUSTOM,
            depth_image.get_width_pixels(), depth_image.get_height_pixels(),
            depth_image.get_width_pixels() * 3 * sizeof(int16_t));
    _wrapper->transformation.depth_image_to_point_cloud(depth_image, K4A_CALIBRATION_TYPE_DEPTH, &xyzPointCloud);

}