#include "SKConfig.h"
#include "SKPRAprilTag.h"
#include "SKPVideoDisplay.h"
#include "SKWrapper.h"
#include "SMColorBlob.h"


#include <gtk/gtk.h>
#include <iostream>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <stdio.h>
#include <string.h>



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


using namespace std;

int main(int argc, char **argv) {
    g_thread_init(NULL);

    SKConfig skc;
    SKWrapper skw(skc);
    SMColorBlob skpra("RGB1080p", "colorblob");
    //get color image
    //call neural net to segment image
    //get segmented color image
    //overlay
    SKPVideoDisplay skpVideoDisplay("colorblob");

    GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (buildUI), &skpVideoDisplay);

    pthread_t thread;
    pthread_create(&thread, NULL, skwThread, &skw);

    int status = g_application_run (G_APPLICATION (app), 0, argv);
    g_object_unref (app);

    pthread_join(thread, NULL);

    return 0;
}
