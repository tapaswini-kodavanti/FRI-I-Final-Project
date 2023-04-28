#ifndef SK_WRAPPER_LIVE_H
#define SK_WRAPPER_LIVE_H

#include "DoOnce.h"

#include <k4a/k4a.hpp>
#include <vector>
#include <string>

class SKConfig;
class SKPRecipient;

//https://github.com/utexas-bwi/single_kinect

class SKWrapper : public DoOnce {
public:
    SKWrapper(SKConfig &skconfig, uint8_t deviceIndex = 0);
    ~SKWrapper();

    void addRecipient(SKPRecipient *_skpr);

    void doOnce();
    SKConfig &getSKConfig();

    //The sync master must be started last
    void start();
    void capture(k4a::capture *cap);
    void setupSync(int32_t color_exposure_usec,int32_t powerline_freq);
    k4a_device_configuration_t  getConfig();
    k4a::calibration            getCalibration();
    std::vector<uint8_t>        getRawCalibration();
   
    k4a::transformation transformation;
protected:
    std::vector<SKPRecipient *> _recipients;
    SKConfig &_skconfig;
    k4a::device _device;
    k4a_device_configuration_t _config;
    k4a::calibration _sensor_calibration;

    //  JWH Will eventually need these, which can be obtained from k4a::calibration (and need not be pulled out, since we have the calibration object)
    //  k4a_calibration_extrinsics_t
    //  k4a_calibration_intrinsics_t
    //  Azure Kinect devices are calibrated with Brown Conrady which is compatible with OpenCV.
    //  From: https://microsoft.github.io/Azure-Kinect-Sensor-SDK/master/unionk4a__calibration__intrinsic__parameters__t.html
};

#endif
