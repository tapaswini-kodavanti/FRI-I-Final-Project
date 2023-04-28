//TRANSFORMS FOR COLOR REGISTERED DEPTH IMAGE + OpenCV

k4a::image colorDepthImage = k4a::image::create(K4A_IMAGE_FORMAT_COLOR_BGRA32, depthImage.get_width_pixels(), depthImage.get_height_pixels(),
    depthImage.get_width_pixels() * 4 * sizeof(int8_t));
_wrapper->transformation.color_image_to_depth_camera(depthImage, getBGRAColorKinect(), &colorDepthImage);

cv::Mat cv_bgra = cv::Mat(colorDepthImage.get_height_pixels(), colorDepthImage.get_width_pixels(), CV_8UC4);
memcpy(cv_bgra.data, colorDepthImage.get_buffer(), cv_bgra.rows * cv_bgra.cols * cv_bgra.elemSize());

cv::cvtColor(cv_bgra, rgbDepth, cv::COLOR_BGRA2RGB);
rgbDepthReady = true;



k4a::image bgraColor;
bool bgraColorReady = false;

k4a::image xyzPointCloud;
bool xyzPointCloudReady = false;

cv::Mat rgbColor;
bool rgbColorReady = false;

cv::Mat rgbDepth;
bool rgbDepthReady = false;

cv::Mat rgbColorPreview;
bool rgbColorPreviewReady = false;