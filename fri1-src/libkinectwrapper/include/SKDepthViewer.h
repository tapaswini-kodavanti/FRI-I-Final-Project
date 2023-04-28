#ifndef SK_DEPTH_VIEWER_H
#define SK_DEPTH_VIEWER_H

#include <SKPRecipient.h>

#include <k4a/k4a.hpp>
#include <opencv2/opencv.hpp>

#include <Eigen/Eigen>

#include <GLFW/glfw3.h>

#include <string>

class SKDepthViewer : public SKPRecipient {
public:
    SKDepthViewer(std::string colorImageName);
    ~SKDepthViewer();

    void receiveFrame(SKPacket &skp);

    void initOpenGLWindow();
    void displayContent();

protected:
    void drawPoints();
    void swapBuffers();

    std::string _colorImageName;
    bool _drawReady;

    GLFWwindow *_window;
    int _windowWidth, _windowHeight;
    k4a::image _xyzFront, _xyzBack;
    cv::Mat _mappedColorFront, _mappedColorBack;
    double _lastMouseX, _lastMouseY;

    Eigen::MatrixXd _camera;
};

#endif