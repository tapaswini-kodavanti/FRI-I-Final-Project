#define GL_GLEXT_PROTOTYPES
#include <SKDepthViewer.h>
#include <SKPacket.h>

#include <GL/gl.h>

#include <iostream>

using namespace std;

static void errorCallback(int error, const char* description){
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

SKDepthViewer::SKDepthViewer(std::string colorImageName) :
    _colorImageName(colorImageName), _drawReady(false), _windowWidth(1920), _windowHeight(1080) {
    // initOpenGLWindow();

    _camera = Eigen::MatrixXd::Identity(4, 4);

    _camera = rotationRT(Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitZ())) * _camera;
    _camera = rotationRT(Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitY())) * _camera;
    _camera = translationRT(Eigen::Vector3d(0, 0, -1900)) * _camera;
}

SKDepthViewer::~SKDepthViewer() {}

void SKDepthViewer::receiveFrame(SKPacket &skp) {
    _xyzBack = skp.getK4AImage("K4AXYZ");
    _mappedColorBack = skp.getCVMat(_colorImageName);
    swapBuffers();
    _drawReady = true;
}

void SKDepthViewer::initOpenGLWindow() {   
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit())
        abort();

    _window = glfwCreateWindow(_windowWidth, _windowHeight, "Camera View", NULL, NULL);

    if (!_window) {
        glfwTerminate();
        abort();
    }

    glfwMakeContextCurrent(_window);
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
}

void SKDepthViewer::drawPoints() {
    size_t size = _xyzFront.get_size();
    void *buffer = _xyzFront.get_buffer();
    void *colorBufferFinal = _mappedColorFront.data;

    glPointSize(4.0f); // May be too large, easier to see output
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_SHORT, 0, buffer);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_UNSIGNED_BYTE, 0, colorBufferFinal);
    glDrawArrays(GL_POINTS, 0, size / 6);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void SKDepthViewer::displayContent() {
    if(_drawReady) {
        glfwMakeContextCurrent(_window);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        double currentMouseX, currentMouseY;
        glfwGetCursorPos(_window, &currentMouseX, &currentMouseY);
        double mouseChangeX = currentMouseX - _lastMouseX, mouseChangeY = currentMouseY - _lastMouseY;
        _lastMouseX = currentMouseX;
        _lastMouseY = currentMouseY;

        if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_1)) {
            // Rotation
            _camera = rotationRT(Eigen::AngleAxisd(mouseChangeX * -0.1 * M_PI / 180, Eigen::Vector3d::UnitY())) * _camera;
            _camera = rotationRT(Eigen::AngleAxisd(mouseChangeY * -0.1 * M_PI / 180, Eigen::Vector3d::UnitX())) * _camera;
        }
        if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_2)) {
            // Translation
            _camera = translationRT(Eigen::Vector3d(mouseChangeX, -mouseChangeY, 0)) * _camera;
        }

        glMultMatrixd(_camera.data());

        glViewport(0, 0, _windowWidth, _windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);

        glPushMatrix();
        // Eigen::MatrixXd mat = rigidTransformations.at(i);
        // mat = mat.inverse();
        // glMultMatrixd(mat.data());
        // frameRecipients.at(i)->
        drawPoints();
        glPopMatrix();

        glClear(GL_DEPTH_BUFFER_BIT);

        glFlush();
        glfwSwapBuffers(_window);
        glfwPollEvents();   
    }
}

void SKDepthViewer::swapBuffers() {
    std::swap(_xyzBack, _xyzFront);
    std::swap(_mappedColorBack, _mappedColorFront);
}