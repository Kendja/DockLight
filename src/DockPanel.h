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
#include "LauncherWindow.h"
#include "About.h"
#include "DragDropWindow.h"
#include "Preferences.h"
#include "SessionWindow.h"

class DockPanel : public Gtk::DrawingArea
{
public:

    DockPanel();
    int preInit(Gtk::Window* window);
    void postInit();

    virtual ~DockPanel();
    std::string getApplicationPath();
    static bool m_previewWindowActive;
    void previewWindowClosed();

    int getCurrentIndex()
    {
        return m_currentMoveIndex;
    };

    LauncherWindow* m_launcherWindow;
    Preferences* m_preferences;
    SessionWindow* m_sessionWindow;

    bool ispopupMenuActive();
private:
    Gtk::Window* m_AppWindow = nullptr;
    TitleWindow m_titlewindow;
    TitleWindow m_infowindow;
    DragDropWindow m_dragDropWindow;

    About m_about;
    Preview m_preview;
    bool m_popupMenuOn;
    static std::vector<DockItem*> m_dockitems;
    static int m_currentMoveIndex;
    static void setItemImdexFromActiveWindow();
    static void updateSessioWindow(WnckWindow *window);
    void CreateSessionDockItemGrp();
    
    void attachToSessiongrp();
    int getNextSessionGrpNumber(); 

    std::string m_applicationpath;
    std::string m_applicationDatapath;
    std::string m_applicationAttachmentsPath;
    std::string m_homeiconFilePath;
    std::string m_SessionGrpIconFilePath;
      

    sigc::connection m_TimeoutConnection;

    // Timer for showing the title window
    Glib::Timer m_titleTimer;
    gdouble m_titleElapsedSeconds;
    int m_titleItemOldindex = 0;
    bool m_titleShow = false;

    guint32 m_mouseclickEventTime;

    // Drag and Drop 
    static bool m_dragdropsStarts;
    Glib::Timer m_dragdropTimer;
    bool m_dragdropTimerSet;
    bool m_dragdropMouseDown;
    int m_dragdropItemIndex;
    Gdk::Point m_dragdropMousePoint;

    void dropDockItem(GdkEventButton *event);
    
    void saveAttachments(int aIdx, int bIdx);
    int loadAttachments();
    void SelectWindow(int index, GdkEventButton * event);
    void createLauncher(DockItem* item);
    void createPreferences();
    void createSessionWindow();

    void on_QuitMenu_event();
    void on_AboutMenu_event();
    void on_HelpMenu_event();

    void on_menuNew_event();
    void on_DetachFromDock_event();
    void on_AttachToDock_event();
    void on_CloseAll_event();
    void on_CloseAllExceptActive_event();


    void on_UnMinimieAll_event();
    void on_MinimieAll_event();
    void on_MinimieAllExceptActive_event();

    void on_HomeCloseAllWindows_event();
    void on_HomeCloseAllWindowsExceptActive_event();
    void on_HomeMinimizeAllWindows_event();
    void on_HomeUnMinimizeAllWindows_event();
    void on_HomeMinimizeAllWindowsExceptActive_event();
    void on_HomeAddSessionGrp_event();
    void on_HomePreferences_event();  
    void on_AutohideToggled_event();
    void on_MenuDeactivated_event();
    //bool on_MenuEnterNotify_event (GdkEventCrossing* crossing_event);


    // Mome menu 
    Gtk::Menu m_HomeMenu_Popup;
    
    Gtk::MenuItem m_homeSessionGrp;
    Gtk::MenuItem m_HelpMenuItem;
    Gtk::MenuItem m_AboutMenuItem;
    Gtk::MenuItem m_QuitMenuItem;
    Gtk::CheckMenuItem m_AutohideMenuItem;
    Gtk::MenuItem m_preferencesMenuItem;
    

    // Item menu
    Gtk::Menu m_Menu_Popup;
    Gtk::MenuItem m_MenuItemNewApp;
    Gtk::MenuItem m_MenuItemAttach;
    Gtk::MenuItem m_MenuItemDetach;
    Gtk::MenuItem m_MenuItemCloseAll;
    Gtk::MenuItem m_MenuItemCloseAllExceptActive;


    Gtk::MenuItem m_MenuItemUnMinimizedAll;
    Gtk::MenuItem m_MenuItemMinimizedAllExceptActive;
    Gtk::MenuItem m_MenuItemMinimizedAll;


    Gtk::MenuItem m_HomeCloseAllWindowsMenuItem;
    Gtk::MenuItem m_HomeCloseAllWindowsExceptActiveMenuItem;

    Gtk::MenuItem m_HomeMinimizeAllWindowsMenuItem;
    Gtk::MenuItem m_HomeUnMinimizeAllWindowsMenuItem;
    Gtk::MenuItem m_HomeMinimizeAllWindowsExceptActiveMenuItem;

    Gtk::SeparatorMenuItem m_separatorMenuItem0;
    Gtk::SeparatorMenuItem m_separatorMenuItem1;
    Gtk::SeparatorMenuItem m_separatorMenuItem2;
    Gtk::SeparatorMenuItem m_separatorMenuItem3;
    Gtk::SeparatorMenuItem m_separatorMenuItem4;
    Gtk::SeparatorMenuItem m_separatorMenuItem5;
    Gtk::SeparatorMenuItem m_separatorMenuItem6;
    Gtk::SeparatorMenuItem m_separatorMenuItem7;
    Gtk::SeparatorMenuItem m_separatorMenuItem8;
    Gtk::SeparatorMenuItem m_separatorMenuItem9;

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

    int m_cellheight;
    int m_cellwidth;
    int m_previousCellwidth;
    int m_iconsize;


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

