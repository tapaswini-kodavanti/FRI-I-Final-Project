#ifndef DEPTH_VIEWER_H
#define DEPTH_VIEWER_H
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <k4a/k4a.hpp>
// #include <k4abt.hpp>
#include <iostream>
#include <assert.h>
#include <Eigen/Dense>
#include "AsyncMKPRecipient.h"
#include <map>
#include <mutex>
#include "MultiKinectPacket.h"

class DepthViewer{
private:
    k4a::calibration calib;
    k4a::transformation transform = NULL;
    GLFWwindow *window;    
    int cameraNum;
public: 
    bool drawReady = false;
    // "back" values are for the frame being prepared from the Kinect threads.
    // "front" values are for the frame being drawn; these should only be used while holding MultiDepthViewer.draw_lock.
    k4a::image xyz_image_front, xyz_image_back;
    cv::Mat mapped_color_image_front, mapped_color_image_back;
    // k4abt::frame body_frame_front, body_frame_back;
    std::map<uint32_t, uint32_t> globalBodyIds;
    void prepareFrame(KinectPacket &mkp, int index);
    void takeCalibration(k4a::calibration calib);
    void drawPoints();
    // void drawSkeletons();
    void display();
    // void swapBuffers();
    DepthViewer(GLFWwindow *window);

};

class MultiDepthViewer: public AsyncMKPRecipient{

private:
    std::vector<DepthViewer*> frameRecipients;
    std::vector<Eigen::MatrixXd> rigidTransformations;
    // std::vector<k4abt_body_t> solvedBodies; // Should only be used while holding draw_lock
    std::mutex draw_lock;
    int windowWidth = 1920;
    int windowHeight = 1080;
    Eigen::MatrixXd camera;
    GLFWwindow *window;
    double lastMouseX, lastMouseY;
    // void drawSolvedBodies();
public:
    MultiDepthViewer(const std::vector<Eigen::MatrixXd>& transforms);
    ~MultiDepthViewer();
    std::vector<DepthViewer*> getFrameRecipients();
    void receiveFrame(MultiKinectPacket &mkp);
    void handleCamera(KinectPacket &kp, size_t index);
    void afterAll(MultiKinectPacket &mkp);
    int initOpenGLWindow();
    int runner();
    void displayContent();
};

#endif