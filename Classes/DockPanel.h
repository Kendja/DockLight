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
#ifndef DOCKPANEL_H
#define DOCKPANEL_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include <libwnck/libwnck.h>
#include <glibmm/timer.h>
#include <gtkmm.h> 

#include <cmath>
#include <limits>
#include <iostream>


#include "DockItem.h"
#include "Launcher.h"
#include "TitleWindow.h"
#include "Preview.h"
#include "DockPosition.h"


// This is a fix for a BUG! in  Gtk::Menu.
// The position don't work on resolution smaller or equal then 768 height.
#define HOME_POPUPMENU_Y_768_REPOSITION  30    // Modify this value depend of the menu children count
#define ITEM_POPUPMENU_Y_768_REPOSITION  100   // Modify this value depend of the menu children count

using namespace std;

class DockPanel : public Gtk::DrawingArea
{
public:

    DockPanel();
    int init(Gtk::Window* window);
    virtual ~DockPanel();
    std::string getApplicationPath();
    static bool m_previewWindowActive;
    void previewWindowClosed();
    
    int getCurrentIndex() { return m_currentMoveIndex; };
private:
    Gtk::Window* m_AppWindow;
    TitleWindow m_titlewindow;
    Preview m_preview;

    static std::vector<DockItem*> m_dockitems;
    static int m_currentMoveIndex;
    static void setItemImdexFromActiveWindow(WnckWindow *window);
    
    
    std::string m_applicationpath;
    sigc::connection m_TimeoutConnection;

    double m_last_time;
    gint m_frames;
    float m_curFPS;
    gint m_fps;
    Glib::Timer m_fpstimer;

    // Timer for showing the title window
    Glib::Timer m_titleTimer;
    gdouble m_titleElapsedSeconds;
    int m_titleItemOldindex = 0;
    bool m_titleShow = false;


    void loadAttachedItems();
    void SelectWindow(int index, GdkEventButton * event);
    bool isExitstMaximizedWindows();


    void on_QuitMenu_event();
    void on_menuNew_event();
    void on_DetachFromDock_event();
    void on_AttachToDock_event();
    void on_CloseAll_event();
    void on_MinimieAll_event();
    void on_CloseAllWindows_event();


    // Mome menu 
    Gtk::Menu m_HomeMenu_Popup;
    Gtk::MenuItem m_QuitMenuItem;

    // Item menu
    Gtk::Menu m_Menu_Popup;
    Gtk::MenuItem m_MenuItemNewApp;
    Gtk::MenuItem m_MenuItemAttach;
    Gtk::MenuItem m_MenuItemDetach;
    Gtk::MenuItem m_MenuItemCloseAll;
    Gtk::MenuItem m_MenuItemMinimizedAll;
    Gtk::MenuItem m_CloseAllWindowsMenuItem;

    Gtk::SeparatorMenuItem m_separatorMenuItem1;
    Gtk::SeparatorMenuItem m_separatorMenuItem2;
    Gtk::SeparatorMenuItem m_separatorMenuItem3;

    gboolean m_mouseLeftButtonDown;
    gboolean m_mouseRightButtonDown;
    gboolean m_mouseIn;
    gboolean m_mouseRightClick;

    //callback slot
    void on_popup_homemenu_position(int& x, int& y, bool& push_in);
    void on_popup_menu_position(int& x, int& y, bool& push_in);

    static void Update(WnckWindow* window, Window_action actiontype);
    static void on_window_opened(WnckScreen *screen, WnckWindow *window, gpointer data);
    static void on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data);
    static void on_active_window_changed_callback(WnckScreen *screen,
            WnckWindow *previously_active_window, gpointer user_data);

    int getIndex(int x, int y);

protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    bool on_timeoutDraw();

    // Mouse handlers
    // http://www.horde3d.org/wiki/index.php5?title=Tutorial_-_Setup_Horde_with_Gtkmm  
    virtual bool on_button_press_event(GdkEventButton *event);
    virtual bool on_button_release_event(GdkEventButton *event);
    virtual bool on_motion_notify_event(GdkEventMotion*event);
    virtual bool on_enter_notify_event(GdkEventCrossing* crossing_event);
    virtual bool on_leave_notify_event(GdkEventCrossing* crossing_event);
    virtual bool on_scroll_event(GdkEventScroll* e);





};



#endif /* DOCKPANEL_H */

