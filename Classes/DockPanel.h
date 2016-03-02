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
#ifndef DOCKPANEL_H
#define	DOCKPANEL_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <glibmm/timer.h>
#include <gtkmm.h> 	
#include "Defines.h"
#include "DockItem.h"
#include <cmath>

#include "XPreview.h"
#include <gdk/gdkx.h>

class DockPanel : public Gtk::DrawingArea
{
public:
    DockPanel();
    virtual ~DockPanel();


    XPreview* m_preview;
    
    void SelectWindow(int index,GdkEventButton *event);
    void ShowPreview(int index, GdkEventButton *event);


    bool on_scroll_event(GdkEventScroll *e);
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    bool on_timeoutDraw();
    int getCountItems();
    int getIndex(int x, int y);
    void FocusWindow(WnckWindow *window);
    void LaunchApplication(const DockItem *dockitem);
    
    static std::string removeExtension(std::string text, const char* extension);
    static std::string removeExtension(std::string text, const std::string extensions[]);
    
    
    
    static void Update(WnckWindow* window, bool mode);
    static Glib::RefPtr<Gdk::Pixbuf> GetWindowIcon(WnckWindow* window);
    static Glib::RefPtr<Gdk::Pixbuf> PixbufConvert(GdkPixbuf* buf);
    static GdkPixbuf* dimm_icon(GdkPixbuf *pixbuf);
    static std::string stringToLower(const char* strp);

    void listFiles(const char* path);
    std::string getexepath();
    std::string getFilePath(const std::string filename);
    
    Gdk::Rectangle monitor_geo;
    

    bool m_mouseLeftButtonDown;
    bool m_mouseRightButtonDown;
    bool m_mouseLeftDoubleClickButtonDown;
    
    void setPanelLocation(panel_locationType m_panelLocation);
    

    bool m_mouseIn;
    //int m_currentIndex;
    int m_currentMoveIndex;
    int m_selectedIndex;

    static std::vector<DockItem*>*_itemsvector;
  // www.lugod.org/presentations/gtkmm/menus.html
    // Popup menu and submenus
    Gtk::Menu m_Menu_Popup;
    
    Gtk::MenuItem MenuItemNewApp;
    Gtk::MenuItem MenuItemPin;
    Gtk::MenuItem MenuItemCloseAll;
   // Gtk::MenuItem MenuItemUnPin;
    Gtk::MenuItem MenuItemRemoveFromDock;
    
    Gtk::SeparatorMenuItem m_separatorMenuItem;
    Gtk::SeparatorMenuItem m_separatorMenuItem2;
    
    Gtk::Menu m_QuitMenu_Popup;
    Gtk::MenuItem m_QuitMenuItem;
    

    // Signal handlers (run when a popup item is clicked)
    void on_QuitMenu_event();
    void on_menuNew_event();
    void on_Pin_event();
    void on_CloseAll_event();
   // void on_UnPin_event();
    void on_RemoveFromDock_event();
    
protected:
    
    panel_locationType m_panelLocation;
    

    virtual bool on_button_press_event(GdkEventButton *event);
    virtual bool on_button_release_event(GdkEventButton *event);
    virtual bool on_motion_notify_event(GdkEventMotion*event);
    //http://www.horde3d.org/wiki/index.php5?title=Tutorial_-_Setup_Horde_with_Gtkmm  
   
    virtual bool on_enter_notify_event(GdkEventCrossing* crossing_event);
    virtual bool on_leave_notify_event(GdkEventCrossing* crossing_event);
    
private:
    // http://www.horde3d.org/wiki/index.php5?title=Tutorial_-_Setup_Horde_with_Gtkmm
    double last_time;
    int frames;
    float _curFPS;
    int fps;
    Glib::Timer m_fpstimer;

    void get_FramesPerSecond();

    //https://git.gnome.org/browse/gtkmm/tree/demos/gtk-demo/example_pixbufs.cc?h=gtkmm-2-24#n127
    sigc::connection m_TimeoutConnection;
    
    static void on_window_opened(WnckScreen *screen, WnckWindow *window,
            gpointer data)
    {
        /* Note: when this event is emitted while screen is initialized, there is no
         * active window yet. */
#ifdef DEBUG
        g_print("---------------->%s\n", wnck_window_get_name(window));
#endif

        Update(window, 1);

    }

    static void on_window_closed(WnckScreen *screen, WnckWindow *window,
            gpointer data)
    {
#ifdef DEBUG
        g_print("window closed %s\n", wnck_window_get_name(window));
#endif
        Update(window, 0);
    }

    static void on_active_window_changed(WnckScreen *screen,
            WnckWindow *previously_active_window,
            gpointer data)
    {
        WnckWindow *active_window;

        active_window = wnck_screen_get_active_window(screen);
#ifdef DEBUG
        if (active_window)
            g_print("active: %s\n", wnck_window_get_name(active_window));
        else
            g_print("no active window\n");
#endif
    }
};





#endif	/* DOCKPANEL_H */

