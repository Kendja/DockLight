#ifndef MONITORGEOMETRY_H
#define MONITORGEOMETRY_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <gtkmm.h>
/*
#include <gtkmm/window.h>
#include <gdkmm/screen.h>
#include <gdkmm/monitor.h>
#include <gdkmm/general.h>
*/

namespace MonitorGeometry
{    
    int update(Gtk::Window* window);
    GdkRectangle getGeometry();
    int getScreenHeight();
    int getScreenWidth();
    int getAppWindowHeight();
    
   
}

#endif /* MONITORGEOMETRY_H */

