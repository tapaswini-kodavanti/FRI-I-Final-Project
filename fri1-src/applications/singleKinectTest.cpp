#include <SKComputeXYZImage.h>
#include "SKConfig.h"
#include "SKPRAprilTag.h"
// #include "SKPFaceDetector.h"
#include "SKPVideoDisplay.h"
#include "SKWrapper.h"
#include "ColorBlob/SMColorBlob.h"
#include <SKDepthViewer.h>

#include <gtk/gtk.h>
#include <iostream>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <stdio.h>
#include <string.h>

using namespace std;

bool keepRunning = true;
GtkWidget *window;

void *skwThread(void *data) {
 
    DoOnce *skw = (DoOnce *)data;
    while(keepRunning) {
        // take pic & save to file
        skw->doOnce(); 
        // move motor

    }
    return NULL;
}

// void *depthThread(void *data){
//     MultiDepthViewer* depthViewer = ((MultiDepthViewer*) data);
//     depthViewer->initOpenGLWindow(); // Must init in same thread as display? GL Context not current error
//     while(true){
//         depthViewer->displayContent();
//     }
// }

gboolean exit_program(GtkWidget *widget, GdkEvent *event, gpointer data) {
    //if menu closed exit program entirely. 
    keepRunning = false;
    //exit(0);
    return TRUE;
}


static void buildUI (GtkApplication *app, gpointer user_data){
    window = gtk_application_window_new(app);
    gtk_window_set_title (GTK_WINDOW (window), "Single Kinect" );
    gtk_window_set_default_size(GTK_WINDOW(window), 1920, 1080 );

    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);

    SKPVideoDisplay* skpVideoDisplay = (SKPVideoDisplay*) user_data;

    skpVideoDisplay->buildWidgets(window);

    g_signal_connect(window, "destroy", G_CALLBACK(exit_program), NULL);
    gtk_widget_show_all (window);
}

void *depthThread(void *data){
    SKDepthViewer* depthViewer = ((SKDepthViewer*) data);
    depthViewer->initOpenGLWindow(); // Must init in same thread as display? GL Context not current error
    while(true){
        depthViewer->displayContent();
    }
}

int main(int argc, char **argv) {
    g_thread_init(NULL);

    SKConfig skc;
    SKWrapper skw(skc);
    // SKPRAprilTag skpra("RGB1080p", "apriltag", "tagcorners", true);
    SMColorBlob skpra("RGB1080p", "colorblob");
    // SKPFaceDetector spfd(skw);
    //SKPVideoDisplay skpVideoDisplay("face_detections");
    // SKPVideoDisplay skpVideoDisplay("apriltag");

    
    // SKPVideoDisplay skpVideoDisplay("DEPTH_REGISTERED_640x576_RGB", 640, 576); // Depth Viewer
    SKPVideoDisplay skpVideoDisplay("colorblob"); // Masked image

    skw.addRecipient(&skpra);
    // spfd.addRecipient(&skpVideoDisplay);

    //skw.addRecipient(&spfd);
    skpra.addRecipient(&skpVideoDisplay);

    SKComputeXYZImage skxyz;
    SKDepthViewer skd("DEPTH_REGISTERED_640x576_RGB");
    skpra.addRecipient(&skxyz);
    skxyz.addRecipient(&skd);


    // MultiDepthViewer depthViewer(skw.getRawCalibration());
    // split.addRecipient(&depthViewer);

    GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (buildUI), &skpVideoDisplay);

    pthread_t threadS;
    pthread_t threadD;
    pthread_create(&threadS, NULL, skwThread, &skw);
    pthread_create(&threadD, NULL, depthThread, &skd);

    // pthread_t thread2;
    // pthread_create(&thread2, NULL, depthThread, &depthViewer);

    int status = g_application_run (G_APPLICATION (app), 0, argv);
    g_object_unref (app);

    pthread_join(threadS, NULL);
    pthread_join(threadD, NULL);

    return 0;
}
