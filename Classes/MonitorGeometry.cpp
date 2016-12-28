#include "MonitorGeometry.h"
#include "Defines.h"
#include "AppWindow.h"

//#define GTKDEPRECATED 1

namespace MonitorGeometry
{

    int ScreenHeight;
    int ScreenWidth;
    int AppWindowHeight;
    GdkRectangle geometry;

    /**
     * Returns the Monitor geometry
     * @return  GdkRectangle geometry
     */
    GdkRectangle getGeometry()
    {
        return geometry;
    }

    /**
     * returns the Gdk::screen_height()
     * @return Gdk::screen_height()
     */
    int getScreenHeight()
    {
        return ScreenHeight;
    }

    /**
     * returns the Gdk::screen_width()
     * @return Gdk::screen_width()
     */
    int getScreenWidth()
    {
        return ScreenWidth;
    }

    /**
     * return the resize height from the App window
     * @return int Height
     */
    int getAppWindowHeight()
    {
        return AppWindowHeight;
    }

    /**
     * update the window position and store the monitor geometry
     * Reserve screen (STRUT) space and dock the window
     * @param window
     * @return 0 = success or -1 error
     */
    int update(Gtk::Window* window)
    {
        int result = 0;

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

#ifdef GTKDEPRECATED
            Gdk::Rectangle monitor_geo;
            auto screen = Gdk::Screen::get_default();
            GdkScreen *defaultscreen = gdk_screen_get_default();
#pragma GCC diagnostic ignored "-Wdeprecated-declarations" 
            gint dm = gdk_screen_get_primary_monitor(defaultscreen);
            screen->get_monitor_geometry(dm, monitor_geo);

            geometry.x = monitor_geo.get_x();
            geometry.y = monitor_geo.get_y();
            geometry.width = monitor_geo.get_width();
            geometry.height = monitor_geo.get_height();
#else
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
#endif
        ScreenHeight = Gdk::screen_height();
        ScreenWidth = Gdk::screen_width();

        // Resize the DOCK Window with 100 as height value. it must be 100.
        AppWindowHeight = 100; /*magic number*/
        window->resize(Gdk::screen_width(), AppWindowHeight);

        // Debug
        g_print("geometry: %d/%d\n", geometry.width, geometry.height);
        g_print("screen: %d/%d\n", Gdk::screen_width(), Gdk::screen_height());
        g_print("window height: %d\n", AppWindowHeight);


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