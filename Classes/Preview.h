//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on December 22, 2015
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
#ifndef PREVIEW_H
#define PREVIEW_H

#include <gtkmm.h>
#include "DockItem.h"

class DockPanel;

class Preview : public Gtk::Window
{
public:
    Preview();
    virtual ~Preview();
    void hideMe();
    void setOwner(DockPanel& dockpanel)
    {
        m_dockpanelReference = &dockpanel;
    };
    void setDockItem(DockItem* item);

protected:
    //Override default signal handler:
    // Signal handlers:
    // Note that on_draw is actually overriding a virtual function
    // from the Gtk::Window class. I set it as virtual here in case
    // someone wants to override it again in a derived class.
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    bool on_enter_notify_event(GdkEventCrossing* crossing_event);
    bool on_leave_notify_event(GdkEventCrossing* crossing_event);
    bool on_timeoutDraw();

    bool on_motion_notify_event(GdkEventMotion*event);
    bool on_scroll_event(GdkEventScroll *event);
    bool on_button_press_event(GdkEventButton *event);
    
   
   
private:
    
    static void on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data);

     
    bool m_mouseIn;
    static bool m_isActive;
    int m_currentIndex;
    std::string m_title;
    int m_initialItemMax;
    DockPanel* m_dockpanelReference;
    
    //DockItem* m_dockItemReference;
    static std::vector<DockItem*> m_previewtems;
    
    Glib::Timer m_timer;
    Pango::FontDescription font;

    int getIndex(int x, int y);
};

#endif /* PREVIEW_H */

