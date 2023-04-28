#ifndef SK_CONFIG_H
#define SK_CONFIG_H

#include <k4a/k4a.hpp>

class SKConfig {
public:
    SKConfig();
    
    k4a_device_configuration_t _config;
    size_t _rowsC, _colsC;
};

#endif