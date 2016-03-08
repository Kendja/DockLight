//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on February 3, 2016, 5:48 PM
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
#ifndef XPREVIEW_H
#define	XPREVIEW_H

#include <gtkmm.h>
<<<<<<< HEAD
#include <gtk/gtk.h> // user c version for samples
//#include <X11/Intrinsic.h>
=======
#include <gtk/gtk.h> 
#include <X11/Intrinsic.h>
>>>>>>> current
#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <gdk/gdkx.h>
#include "DockItem.h"
#include <math.h>
#include <pango/pangocairo.h> 
#include "Utilities.h"

class XPreview : public Gtk::Window
{
public:
    XPreview(); 
    
    void hideMe();
    bool m_mouseIn;
    bool m_active;
    int m_currentIndex;
    Gtk::Menu m_Menu_Popup;
    Gtk::MenuItem m_MenuCloseWindow;
    void on_menuCloseWindow_event();
    void setXid(DockItem* item);
    int getIndex(int x, int y);
    virtual ~XPreview();
    void setPanelLocation(panel_locationType panelLocation);
protected:

    panel_locationType m_panelLocation;
    bool on_scroll_event(GdkEventScroll *event);
    bool on_button_press_event(GdkEventButton *event);
    bool on_motion_notify_event(GdkEventMotion*event);

    //Override default signal handler:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    bool on_drawX(const Cairo::RefPtr<Cairo::Context>& cr);

    bool on_enter_notify_event(GdkEventCrossing* crossing_event);
    bool on_leave_notify_event(GdkEventCrossing* crossing_event);
    bool on_timeoutDraw();

    Glib::RefPtr<Gdk::Pixbuf> PixbufConvert(GdkPixbuf* icon);
   
    void drawText(const Cairo::RefPtr<Cairo::Context>& cr,
            int x, int y, const std::string text);
    
    void draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr,
           int x, int y, int width, int height);
    
private:

    DockItem* m_item;
};

#endif	/* XPREVIEW_H */

