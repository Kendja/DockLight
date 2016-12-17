//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on November 20, 2016
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
#include "MonitorGeometry.h"

/*
 * http://web.mit.edu/ghudson/dev/nokrb/third/libwnck/libwnck/test-wnck.c
 * constructor 
 * set the window transparence
 */
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
    }

}

/**
 * initialize the window
 */
int AppWindow::Init(panel_locationType location)
{
    this->set_title(APPNAME);

    this->set_focus_on_map(false);
    this->set_skip_taskbar_hint(true);
    this->set_skip_pager_hint(true);
    this->set_accept_focus(false);
    this->set_focus_visible(false);
    this->set_realized(false);
    this->set_decorated(false);
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DOCK);

    // Add the dock panel
    this->add(m_dockpanel);

    // it must be shown before changing properties. 
    // also need to call show to get a valid gdk_window
    this->show_all();

    GdkScreen *screen = gdk_screen_get_default();
    WnckScreen *wnckscreen = wnck_screen_get(0);
    
    // needed for "configure-event"
    this->add_events(Gdk::STRUCTURE_MASK);

    g_signal_connect(G_OBJECT(wnckscreen), "window_opened",
            G_CALLBACK(window_opened_callback), NULL);

    g_signal_connect(G_OBJECT(wnckscreen), "application_opened",
            G_CALLBACK(application_opened_callback), NULL);

    // The monitors-changed signal is emitted when the number, 
    // size or position of the monitors attached to the screen change.
    g_signal_connect(screen, "monitors-changed",
            G_CALLBACK(monitor_size_changed_callback), (gpointer) this);

    // read the monitor geometry and repositioning the window
    if (MonitorGeometry::update(this) != 0) {
        return -1;
    }

    return m_dockpanel.init(this);
}

void AppWindow::application_opened_callback(WnckScreen *screen, WnckApplication *app)
{
//    g_print("Application opened: ");
//    const char* appname = wnck_application_get_name(app);
//    if (appname != NULL) {
//        g_print("%s\n", appname);
//    }

    

    //queue_refill_model ();
}

void AppWindow::window_opened_callback(WnckScreen *screen, WnckWindow *window, gpointer data)
{
}

void AppWindow::monitor_size_changed_callback(GdkScreen *screen, gpointer gtkwindow)
{
    g_info("Monitor settings changed\n");
    // Adapt the monitor geometry and repositioning the window
    MonitorGeometry::update((Gtk::Window*)gtkwindow);

}

void AppWindow::window_geometry_changed_callback(WnckWindow *window, gpointer user_data)
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

