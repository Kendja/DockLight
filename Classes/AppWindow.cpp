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
/*
 * http://web.mit.edu/ghudson/dev/nokrb/third/libwnck/libwnck/test-wnck.c
 * constructor 
 * set the window transparence
 */
AppWindow::AppWindow()
{
    add_events(Gdk::BUTTON_PRESS_MASK);
    set_app_paintable(true);

    WnckScreen *wnckscreen;
    GdkScreen *screen;
    GdkVisual *visual;

    wnckscreen = wnck_screen_get(0);
    
    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
        //gtk_widget_set_visual(win, visual);
    }
    
    // needed for "configure-event"
    this->add_events( Gdk::STRUCTURE_MASK );
    
  //  g_signal_connect (G_OBJECT (wnckscreen), "window_opened",
  //                  G_CALLBACK (window_opened_callback),NULL);
    
    
}
/*
 * 
 * reserve screen space and dock the window
 * on location
 */
int AppWindow::Init(panel_locationType location)
{
    int height = DEF_PANELHIGHT;
    this->m_dockpanel.setPanelLocation(location);
    
   // g_signal_connect (G_OBJECT (this), "geometry_changed",
   //                  G_CALLBACK (window_geometry_changed_callback),NULL);
     
    // Retrieves the Gdk::Rectangle representing the size and position of the 
    // individual monitor within the entire screen area. 
    // Monitor numbers start at 0. To obtain the number of monitors of screen, 
    // use get_n_monitors()
    auto screen = Gdk::Screen::get_default();
    GdkScreen *defaultscreen = gdk_screen_get_default ();
    
    gint dm = gdk_screen_get_primary_monitor (defaultscreen);
    screen->get_monitor_geometry(dm, this->m_dockpanel.monitor_geo);
    this->set_default_size(this->m_dockpanel.monitor_geo.get_width(), height);

    this->set_title("DockLight");
    this->set_focus_on_map(false);
    this->set_skip_taskbar_hint(true);
    this->set_skip_pager_hint(true);
    this->set_gravity(Gdk::GRAVITY_SOUTH);
    this->set_accept_focus(false);
    this->set_focus_visible(false);
    this->set_realized(false);
    this->set_decorated(false);
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);
    //g_signal_connect(G_OBJECT(this), "configure-event", G_CALLBACK(configureCallback), NULL);
    
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
            
            //this->move(this->m_dockpanel.monitor_geo.get_x(), Gdk::screen_height() - DEF_PANELHIGHT);
            this->move(this->m_dockpanel.monitor_geo.get_x(), this->m_dockpanel.monitor_geo.get_height() - DEF_PANELHIGHT);
            
            
            insets[strutsPosition::Bottom] = (DEF_PANELHIGHT + (Gdk::screen_height() - 
                    (this->m_dockpanel.monitor_geo.get_y() + this->m_dockpanel.monitor_geo.get_height())));
            insets[strutsPosition::BottomStart] = this->m_dockpanel.monitor_geo.get_x();
            
            insets[strutsPosition::BottomEnd] = (this->m_dockpanel.monitor_geo.get_x() + 
                    this->m_dockpanel.monitor_geo.get_width() - 1);
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


void AppWindow::window_opened_callback(WnckScreen    *screen,WnckWindow    *window, gpointer       data)
{
         g_print ("Window '%s' opened (pid = %d session_id = %s)\n",
           wnck_window_get_name (window),
           wnck_window_get_pid (window),
           wnck_window_get_session_id (window) ?
           wnck_window_get_session_id (window) : "none");
         
          g_signal_connect (G_OBJECT (window), "geometry_changed",
                    G_CALLBACK (window_geometry_changed_callback),
                    NULL);
          
           
}


void AppWindow::window_geometry_changed_callback (WnckWindow *window, gpointer    user_data)
{
    g_print("\nwindow_geometry_changed_callback\n");
    
}

void AppWindow::configureCallback(GtkWindow* parentWindow, GdkEvent* event, gpointer data)
{
    // do something...
}


AppWindow::~AppWindow()
{
}

