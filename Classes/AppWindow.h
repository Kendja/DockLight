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
#ifndef APPWINDOW_H
#define	APPWINDOW_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1



#include <libwnck/libwnck.h>
#include <gtkmm.h> 

#include "DockPanel.h"
#include "Defines.h"

class AppWindow : public Gtk::Window
{
public:
    AppWindow();
    int Init(panel_locationType location);
    virtual ~AppWindow();
private:
    DockPanel m_dockpanel;
    
    static void configureCallback(GtkWindow* parentWindow, GdkEvent* event, gpointer data);
    static void window_opened_callback(WnckScreen    *screen,WnckWindow    *window, gpointer data);
    static void window_geometry_changed_callback (WnckWindow *window, gpointer user_data);
    
};

#endif	/* APPWINDOW_H */

