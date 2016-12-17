#ifndef MONITORGEOMETRY_H
#define MONITORGEOMETRY_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <gtkmm.h> 

namespace MonitorGeometry
{
    
    int update(Gtk::Window* window);
    GdkRectangle getGeometry();
}

#endif /* MONITORGEOMETRY_H */

