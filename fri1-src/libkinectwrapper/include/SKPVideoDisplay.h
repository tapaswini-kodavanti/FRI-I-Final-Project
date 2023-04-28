#ifndef SKP_VIDEO_DISPLAY_H
#define SKP_VIDEO_DISPLAY_H

#include "SKPRecipient.h"
#include <gtk/gtk.h>

#include <string>


class SKPVideoDisplay : public SKPRecipient {
public:
    //Display rows & columns
    SKPVideoDisplay(std::string imgName, int cols = 1920, int rows = 1080);
    ~SKPVideoDisplay();
    void receiveFrame(SKPacket &skp);
    void buildWidgets(GtkWidget *container);
    static gboolean drawCallback (GtkWidget *widget, cairo_t *cr, gpointer data);
    gboolean doDraw(cairo_t *cr);
protected:
    bool _initialized;
    int _rows, _cols;
    //cv::Mat _colorMat;
    size_t _bytes;
    GtkWidget *_drawingArea;
    //GdkPixbuf *_pixbuf;
    unsigned char *_buf;
    std::string _imgName;
};

#endif