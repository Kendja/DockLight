//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on January  2017
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//****************************************************************
#include "MonitorGeometry.h"
#include "Defines.h"
#include "AppWindow.h"
#include "Configuration.h"

// use deprecate methods for compatibility with older gtk-3 versions. 
#define GTKDEPRECATED 1

namespace MonitorGeometry
{

    int ScreenHeight;
    int ScreenWidth;
    int AppWindowHeight;
    int StrutHeight;

    Gtk::Window* m_window = nullptr;



    GdkRectangle geometry;

    int getStrutHeight()
    {
        return StrutHeight;
    }

    int getAppWindowTopPosition()
    {
        return ScreenHeight - StrutHeight;
    }

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

        m_window = window;

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

        // set the strut
        return updateStrut(window, DEF_PANELHIGHT);
    }

    int ApplyStrut(int height)
    {

        if (m_window == nullptr) {
            g_critical(" updateStrut::update: window is null.");
            return -1;
        }
        GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(m_window->gobj()));
        GdkWindow *gdk_window = gtk_widget_get_window(toplevel);
        if (gdk_window == NULL) {
            g_critical(" updateStrut::update: gdk_window is null.");
            return -1;
        }

        // Position only  BOTTOM for now. 
        // A future implementation for TOP,LEFT,RIGHT may be a possibility...
        long insets[12] = {0};
        panel_locationType location = panel_locationType::BOTTOM;
        switch (location) {
            case panel_locationType::BOTTOM:
                
                if( Configuration::getAutohide() == false)
                    m_window->move(geometry.x, Gdk::screen_height() - height);

                insets[strutsPosition::BottomStart] = geometry.x;
                insets[strutsPosition::BottomEnd] = geometry.x + geometry.width - 1;
                insets[strutsPosition::Bottom] = (height + (Gdk::screen_height() -(geometry.y + geometry.height)));

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

        return 0;
    }

    int updateStrut()
    {
        if (m_window == nullptr)
            return -1;

        return updateStrut(m_window, StrutHeight);

    }

    int updateStrut(int height)
    {
        if (m_window == nullptr)
            return -1;

        return updateStrut(m_window, height);
    }

    int updateStrut(Gtk::Window* window, int height)
    {

        StrutHeight = height;

        if (Configuration::getAutohide()) {

            if (!DockPosition::isVisible()) {
                window->move(geometry.x, Gdk::screen_height() - height);
                DockPosition::setVisibleState(true);
                
            } else {
                window->move(geometry.x, Gdk::screen_height() - DEF_PANELHIGHTHIDE);
                DockPosition::setVisibleState(false);
            }
            
            return 0;
        }

        return ApplyStrut(height);
    }

    int RemoveStrut()
    {
        return ApplyStrut(0);
    }


}