#include "SKConfig.h"
#include <iostream>

using namespace std;

SKConfig::SKConfig() :
    _config(K4A_DEVICE_CONFIG_INIT_DISABLE_ALL) {
    _config.camera_fps = K4A_FRAMES_PER_SECOND_30;
    _config.color_resolution = K4A_COLOR_RESOLUTION_1080P;
    _config.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
    //_config.depth_mode = K4A_DEPTH_MODE_OFF;
    _config.synchronized_images_only = true;
    _config.color_format = K4A_IMAGE_FORMAT_COLOR_BGRA32;
    switch(_config.color_resolution) {
        case K4A_COLOR_RESOLUTION_1080P:    // Changeable based on preferred
            _rowsC = 1080;                  // resolution
            _colsC = 1920;
            break;
        default:
            abort();
    }


}