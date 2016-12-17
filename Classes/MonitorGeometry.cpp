#include "MonitorGeometry.h"
#include "Defines.h"

namespace MonitorGeometry
{
    GdkRectangle geometry;

    GdkRectangle getGeometry()
    {
        return geometry;
    }

    /**
     * reserve screen space and dock the window
     * on location
     */
    int update(Gtk::Window* window)
    {
        int result = 0;
        int height = DEF_PANELHIGHT;

        if (window == NULL) {
            g_critical(" MonitorGeometry::update: window is null.");
            return -1;
        }
        GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(window->gobj()));
        GdkWindow *gdk_window = gtk_widget_get_window(toplevel);
        if (gdk_window == NULL) {
            g_critical(" MonitorGeometry::update: gdk_window is null.");
            return -1;
        }

        // Retrieves the Gdk::Rectangle representing the size and position of the 
        // individual monitor within the entire screen area. 
        GdkDisplay *defaultdisplay = gdk_display_get_default();
        if (defaultdisplay == NULL) {
            g_critical(" AppWindow::Init: there is no default display.");
            return -1;
        }
        GdkMonitor *monitor = gdk_display_get_primary_monitor(defaultdisplay);
        if (monitor == NULL) {
            g_critical(" AppWindow::Init: there is no primary monitor configured by the user..");
            return -1;
        }
        
        // GdkRectangle to be filled with the monitor geometry
        gdk_monitor_get_geometry(monitor, &geometry);
        // set the new size
        //window->set_gravity(Gdk::Gravity::GRAVITY_SOUTH_WEST);
        window->resize(geometry.width, height);

        // Position only for BOTTOM for now. 
        // A future implementation for TOP,LEFT,RIGHT may be a possibility.
        long insets[12] = {0};
        panel_locationType location = panel_locationType::BOTTOM;
        switch (location) {

            case panel_locationType::BOTTOM:
              
                window->move(geometry.x, Gdk::screen_height() - DEF_PANELHIGHT);   
              
                insets[strutsPosition::Bottom] = (DEF_PANELHIGHT + (Gdk::screen_height() -
                        (geometry.y + geometry.height)));
                insets[strutsPosition::BottomStart] = geometry.x;
                insets[strutsPosition::BottomEnd] = geometry.x + geometry.width - 1;
                break;

            default:
                break;
        }
 
        // we set _NET_WM_STRUT, the older mechanism as well as _NET_WM_STRUT_PARTIAL
        gdk_property_change(gdk_window,
                gdk_atom_intern("_NET_WM_STRUT_PARTIAL", FALSE),
                gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                (unsigned char *) &insets, 12);


        gdk_property_change(gdk_window,
                gdk_atom_intern("_NET_WM_STRUT", FALSE),
                gdk_atom_intern("CARDINAL", FALSE), 32, GDK_PROP_MODE_REPLACE,
                (unsigned char *) &insets, 4);

        return result;
    }
}