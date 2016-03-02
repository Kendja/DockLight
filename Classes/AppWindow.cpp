//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on November 20, 2015, 12:17 PM 
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
#include "AppWindow.h"
#include <gdkmm/screen.h>
#include "Utilities.h"
#include <gtk-3.0/gtk/gtkwindow.h>
//#include <gtk-2.0/gdk/gdkpixbuf.h>

/****************************************************************
 * 
 * constructor 
 * set the window transparence
 ****************************************************************/
AppWindow::AppWindow()
{
    add_events(Gdk::BUTTON_PRESS_MASK);
    set_app_paintable(true);

    GdkScreen *screen;
    GdkVisual *visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
        //gtk_widget_set_visual(win, visual);
    }
}

/****************************************************************
 * 
 * reserve screen space and dock the window
 *  on location
 ****************************************************************/
int AppWindow::Init(panel_locationType location)
{
    int height = DEF_PANELHIGHT;
    this->m_dockpanel.setPanelLocation(location);

    // Retrieves the Gdk::Rectangle representing the size and position of the 
    // individual monitor within the entire screen area. 
    // Monitor numbers start at 0. To obtain the number of monitors of screen, 
    // use get_n_monitors()
    auto screen = Gdk::Screen::get_default();
    screen->get_monitor_geometry(0, this->m_dockpanel.monitor_geo);

    this->set_default_size(this->m_dockpanel.monitor_geo.get_width(), height);

    this->set_decorated(false);
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);

    // Add the dock panel
    this->add(this->m_dockpanel);
    
    // it must be shown before changing properties. 
    // also need to call show to get a valid gdk_window
    this->show_all();

    GtkWidget *toplevel = gtk_widget_get_toplevel(GTK_WIDGET(this->gobj()));
    GdkWindow *gdk_window = gtk_widget_get_window(toplevel);

    this->set_icon_from_file(Utilities::getExecPath("home.ico"));

    // reserve space (a "strut") for the bar so it does not become obscured
    // when other windows are maximized.
    long insets[12] = {0};

    switch (location) {
        case panel_locationType::TOP:
            this->move(0, 0);

            insets[strutsPosition::Top] = height;
            insets[strutsPosition::TopStart] = 0;
            insets[strutsPosition::TopEnd] = this->m_dockpanel.monitor_geo.get_width() - 1;
            break;

        case panel_locationType::BOTTOM:

            this->move(0, Gdk::screen_height() - DEF_PANELHIGHT);

            insets[strutsPosition::Bottom] = (DEF_PANELHIGHT + (Gdk::screen_height() - (this->m_dockpanel.monitor_geo.get_y() + this->m_dockpanel.monitor_geo.get_height())));
            insets[strutsPosition::BottomStart] = this->m_dockpanel.monitor_geo.get_x();
            insets[strutsPosition::BottomEnd] = (this->m_dockpanel.monitor_geo.get_x() + this->m_dockpanel.monitor_geo.get_width() - 1);
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


//GdkWindow* gdkWindow =  gtk_widget_get_window(GTK_WIDGET(this));
    /*
    //wnck.screen_get_default().get_windows()
     * 
     * 
    GList *window_l;
    //The windows you get are the windows that were created within your process
    for (window_l = gtk_window_list_toplevels() ; window_l != NULL; window_l = window_l->next) 
    {
        GtkWindow* gtkwindow = GTK_WINDOW(window_l->data);
         //WnckWindow *window = WNCK_WINDOW(window_l->data);
        
        g_print("%s\n",gtk_window_get_title(gtkwindow));
                
    }
     */

  //  GList *window_l;
 //   for (window_l = wnck_screen_get_windows(screen); window_l != NULL; window_l = window_l->next) {
  //      WnckWindow *window = WNCK_WINDOW(window_l->data);

/*
    // WnckWindow *wckindow

      //  GdkDrawable *src;
        GdkPixbuf *image;
        XWindowAttributes wa;
        Window win;
       // win = GDK_WINDOW_XID (wckindow);
       
        int x, y, width, xheight, depth;
        gdk_window_get_geometry( gdk_window, &x, &y, &width, &xheight );
        image = gdk_pixbuf_get_from_drawable(NULL,
                                        gdk_window,
                                        NULL, 0, 0, 0, 0, width, xheight);
*/
       // gdk_window_lookup()
        
       // auto src = gdk_window_lookup( gdk_x11_get_default_root_xwindow() );
        
//       image = gdk_pixbuf_get_from_drawable( NULL, 
////                                             win, 0, NULL,
////                                             0, 0, 0, 0,
////                                             12, 12);
         
        
      //  win = GDK_WINDOW_XID (widget->window);

        //  g_print("%s%s\n", wnck_window_get_name(window),
        //         window == active_window ? " (active)" : "");
   // }





    return 0;
}

AppWindow::~AppWindow()
{
}

