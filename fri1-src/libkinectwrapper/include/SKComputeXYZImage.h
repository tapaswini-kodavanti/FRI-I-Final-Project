#ifndef SK_COMPUTE_XYZ_IMAGE_H
#define SK_COMPUTE_XYZ_IMAGE_H

#include "SKPRecipient.h"

class SKComputeXYZImage : public SKPRecipient {
public:
    SKComputeXYZImage();
    ~SKComputeXYZImage();

    void receiveFrame(SKPacket &skp);
    void addRecipient(SKPRecipient *skpr);
    
protected:
    std::vector<SKPRecipient *> _recipients;
};

#endif