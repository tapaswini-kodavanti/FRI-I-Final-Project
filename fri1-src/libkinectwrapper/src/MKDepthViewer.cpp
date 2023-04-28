#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <k4a/k4a.hpp>
#include <iostream>
#include <assert.h>
#include "include/MKPRecipient.h"
#include <map>
#include "include/MultiKinectPacket.h"
#include "include/DepthViewer.h"
#include <GL/glut.h>
#include "include/BodyTrackConstants.h"
#include "include/MKPPool.h"

using namespace std;

Eigen::MatrixXd rotationRT(Eigen::AngleAxisd rot) {
    Eigen::MatrixXd mat = Eigen::MatrixXd::Identity(4, 4);
    mat.block(0, 0, 3, 3) = rot.toRotationMatrix();
    return mat;
}
Eigen::MatrixXd translationRT(Eigen::VectorXd trans) {
    Eigen::MatrixXd mat = Eigen::MatrixXd::Identity(4, 4);
    mat.block(0, 3, trans.rows(), 1) = trans;
    for (int i = 0; i < 4; i++) {
        // In-place; note that (3, 3) is updated last
        mat(i, 3) /= mat(3, 3);
    }
    return mat;
}

DepthViewer::DepthViewer(GLFWwindow *win){
    window = win;
}

void DepthViewer::prepareFrame(KinectPacket &kp, int index){
    xyz_image_back = kp.getXYZDepth();
    mapped_color_image_back = kp.getRGBDepth();
    body_frame_back = kp.getBodyFrame();

    cameraNum = index + 1;

    if(!drawReady){
        transform = k4a::transformation(kp.getKinectWrapper()->getCalibration());    
    }
}

// Calling code should hold draw_lock
void DepthViewer::swapBuffers() {
    std::swap(xyz_image_back, xyz_image_front);
    std::swap(mapped_color_image_back, mapped_color_image_front);
    std::swap(body_frame_back, body_frame_front);
}

void glColorFromNumber(int num) {
    switch (num % 10) {
        case 0: glColor3d(1,0,0); break;
        case 1: glColor3d(0,.7,0); break;
        case 2: glColor3d(0,0,1); break;
        case 3: glColor3d(1,1,0); break;
        case 4: glColor3d(0,1,1); break;
        case 5: glColor3d(1,0,1); break;
        case 6: glColor3d(1,0.5,0); break;
        case 7: glColor3d(0.5,0,1); break;
        case 8: glColor3d(.7,1,0); break;
        case 9: glColor3d(0,.5,1); break;
    }
}

// Calling code should hold draw_lock
void DepthViewer::drawPoints() {
    k4a::image my_xyz_image = xyz_image_front;
    cv::Mat my_color_image = mapped_color_image_front;

    size_t size = my_xyz_image.get_size();
    void *buffer = my_xyz_image.get_buffer();
    void *colorBufferFinal = my_color_image.data;

    glPointSize(4.0f); // May be too large, easier to see output
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_SHORT, 0, buffer);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_UNSIGNED_BYTE, 0, colorBufferFinal);
    glDrawArrays(GL_POINTS, 0, size / 6);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

// // Calling code should hold draw_lock
// void DepthViewer::drawSkeletons() {
//     glPointSize(10);
//     for (size_t b = 0; b < body_frame_front.get_num_bodies(); b++) {
//         k4abt_body_t body = body_frame_front.get_body(b);
//         glColorFromNumber(globalBodyIds[body.id]);
//         glLineWidth(2.5);
//         glBegin(GL_LINES);
//         for (size_t j = 0; j < K4ABT_JOINT_COUNT; j++) {
//             k4abt_joint_t joint = body.skeleton.joints[j];
//             k4abt_joint_t parent = body.skeleton.joints[JOINT_PARENTS[j]];
//             glVertex3d(joint.position.xyz.x, joint.position.xyz.y, joint.position.xyz.z);
//             glVertex3d(parent.position.xyz.x, parent.position.xyz.y, parent.position.xyz.z);
//         }
//         glEnd();
//         glBegin(GL_POINTS);
//         for (size_t j = 0; j < K4ABT_JOINT_COUNT; j++) {
//             glVertex3d(body.skeleton.joints[j].position.xyz.x, body.skeleton.joints[j].position.xyz.y, body.skeleton.joints[j].position.xyz.z);
//         }
//         glEnd();
//     }
// }
// // Calling code should hold draw_lock
// void MultiDepthViewer::drawSolvedBodies() {
//     for (size_t i = 0; i < solvedBodies.size(); i++) {
//         k4abt_body_t body = solvedBodies[i];
//         glColor3d(1, 1, 0); // Could use glColorFromNumber, but I'd like to differentiate
//         glBegin(GL_POINTS);
//         for (size_t j = 0; j < K4ABT_JOINT_COUNT; j++) {
//             glVertex3d(body.skeleton.joints[j].position.xyz.x, body.skeleton.joints[j].position.xyz.y, body.skeleton.joints[j].position.xyz.z);
//         }
//         glEnd();
//         glLineWidth(2.5);
//         glBegin(GL_LINES);
//         for (size_t j = 0; j < K4ABT_JOINT_COUNT; j++) {
//             k4abt_joint_t joint = body.skeleton.joints[j];
//             k4abt_joint_t parent = body.skeleton.joints[JOINT_PARENTS[j]];
//             glVertex3d(joint.position.xyz.x, joint.position.xyz.y, joint.position.xyz.z);
//             glVertex3d(parent.position.xyz.x, parent.position.xyz.y, parent.position.xyz.z);
//         }
//         glEnd();
//     }
// }

MultiDepthViewer::MultiDepthViewer(const vector<Eigen::MatrixXd>& transforms) : rigidTransformations(transforms)
{
    int numCameras = rigidTransformations.size();
    for (int i = 0; i < numCameras; i++) {
        DepthViewer* viewer = new DepthViewer(window);
        frameRecipients.push_back(viewer);
    }

    camera = Eigen::MatrixXd::Identity(4, 4);

    camera = rotationRT(Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitZ())) * camera;
    camera = rotationRT(Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitY())) * camera;
    camera = translationRT(Eigen::Vector3d(0, 0, -1900)) * camera;
}

std::vector<DepthViewer*> MultiDepthViewer::getFrameRecipients(){
    return frameRecipients;
}

void MultiDepthViewer::receiveFrame(MultiKinectPacket &mkp) {
    PROFILE_START;
    AsyncMKPRecipient::receiveFrame(mkp);
    PROFILE_END("MultiDepthViewer.receiveFrame");
}

void MultiDepthViewer::handleCamera(KinectPacket &kp, size_t index){
    frameRecipients.at(index)->prepareFrame(kp, index);
}

void MultiDepthViewer::afterAll(MultiKinectPacket &mkp) {
    draw_lock.lock();
    solvedBodies = mkp.solvedBodies;
    for (size_t i = 0; i < mkp.getKinectWrappers().size(); i++) {
        if (mkp._localToGlobalBodyIds.count(i)) {
            frameRecipients.at(i)->globalBodyIds = mkp._localToGlobalBodyIds.at(i);
        }
        frameRecipients.at(i)->swapBuffers();
        frameRecipients.at(i)->drawReady = true;
    }
    draw_lock.unlock();
}


void MultiDepthViewer::displayContent(){
    for(int i = 0; i < frameRecipients.size(); i++) {
        if (!frameRecipients.at(i)->drawReady){
            return;
        }
    }
    glfwMakeContextCurrent(window);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    double currentMouseX, currentMouseY;
    glfwGetCursorPos(window, &currentMouseX, &currentMouseY);
    double mouseChangeX = currentMouseX - lastMouseX, mouseChangeY = currentMouseY - lastMouseY;
    lastMouseX = currentMouseX;
    lastMouseY = currentMouseY;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
        // Rotation
        camera = rotationRT(Eigen::AngleAxisd(mouseChangeX * -0.1 * M_PI / 180, Eigen::Vector3d::UnitY())) * camera;
        camera = rotationRT(Eigen::AngleAxisd(mouseChangeY * -0.1 * M_PI / 180, Eigen::Vector3d::UnitX())) * camera;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
        // Translation
        camera = translationRT(Eigen::Vector3d(mouseChangeX, -mouseChangeY, 0)) * camera;
    }

    glMultMatrixd(camera.data());

    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    draw_lock.lock();
    for(int i = 0; i < frameRecipients.size(); i++) {
        glPushMatrix();
        Eigen::MatrixXd mat = rigidTransformations.at(i);
        mat = mat.inverse();
        glMultMatrixd(mat.data());
        frameRecipients.at(i)->drawPoints();
        glPopMatrix();
    }
    glClear(GL_DEPTH_BUFFER_BIT);
    for(int i = 0; i < frameRecipients.size(); i++) {
        glPushMatrix();
        Eigen::MatrixXd mat = rigidTransformations.at(i);
        mat = mat.inverse();
        glMultMatrixd(mat.data());
        frameRecipients.at(i)->drawSkeletons();
        glPopMatrix();
    }
    // drawSolvedBodies();
    draw_lock.unlock();
    glFlush();
    glfwSwapBuffers(window);
    glfwPollEvents();   
}

static void error_callback(int error, const char* description){
    fprintf(stderr, "Error: %s\n", description);
}

// https://www.khronos.org/opengl/wiki/OpenGL_Error#Catching_errors_.28the_easy_way.29
static void GLAPIENTRY onGLMessage( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam ){
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            fprintf(stderr, "GL message (severity HIGH): %s\n", message);
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            fprintf(stderr, "GL message (severity MEDIUM): %s\n", message);
            break;
        case GL_DEBUG_SEVERITY_LOW:
            fprintf(stderr, "GL message (severity LOW): %s\n", message);
            break;
        default:
            fprintf(stderr, "GL message (severity 0x%x): %s\n", severity, message);
    }
}

int MultiDepthViewer::initOpenGLWindow() {   
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(windowWidth, windowHeight, "Camera View", NULL, NULL);
    // glViewport(0, 0,  640, 480);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);
    glDebugMessageCallback(onGLMessage, NULL);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // gluPerspective(60,1,1,10);

    const GLdouble pi = 3.1415926535897932384626433832795;
    GLdouble fW, fH;

    //fH = tan( (fovY / 2) / 180 * pi ) * zNear;
    fH = 0.57;
    fW = fH * 1;

    glFrustum( -fW, fW, -fH, fH, 1, 10000);
    
    return 0;
}

MultiDepthViewer::~MultiDepthViewer(){
    glfwTerminate();
}