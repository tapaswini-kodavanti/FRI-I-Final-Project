#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <opencv2/opencv.hpp>
#include "SKPRecipient.h"
#include <string>
#include <vector>

class Transformation {
protected:
    k4a::capture _capture;
public:
    void applyTransformation();
}

#endif