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

#include "DockPanel.h"

#include "Utilities.h"
#include "MonitorGeometry.h"
#include "IconLoader.h"
#include "DockPosition.h"

// static members
std::vector<DockItem*> DockPanel::m_dockitems;
int DockPanel::m_currentMoveIndex;
bool DockPanel::m_previewWindowActive;

DockPanel::DockPanel() :
m_frames(0),
m_last_time(0),
m_titleElapsedSeconds(0),
m_mouseIn(FALSE),
m_mouseRightClick(FALSE),
m_mouseLeftButtonDown(FALSE),
m_mouseRightButtonDown(FALSE),
m_cellwidth(DEF_CELLWIDTH),
m_previousCellwidth(DEF_CELLWIDTH),
m_iconsize(DEF_ICONSIZE),
m_cellheight(DEF_CELLHIGHT),
m_applicationpath(Utilities::getExecPath()),
m_applicationDatapath(m_applicationpath+"/data")
{

    m_currentMoveIndex = -1;


    // Set masks for mouse events
    add_events(Gdk::BUTTON_PRESS_MASK |
            Gdk::BUTTON_RELEASE_MASK |
            Gdk::BUTTON_PRESS_MASK |
            Gdk::SCROLL_MASK |
            Gdk::SMOOTH_SCROLL_MASK |
            Gdk::POINTER_MOTION_HINT_MASK |
            Gdk::FOCUS_CHANGE_MASK |
            Gdk::ENTER_NOTIFY_MASK |
            Gdk::LEAVE_NOTIFY_MASK |
            Gdk::POINTER_MOTION_MASK);




}

/**
 * preInit load the attached icons and initializes variables, 
 * and create the popup menus
 * @param Gtk::Window*  window
 * @return return 0 is success or -1 is an error found
 */
int DockPanel::preInit(Gtk::Window* window)
{
    m_AppWindow = window;

    std::string filename = Utilities::getExecPath(DEF_ICONFILE);
    gboolean isexists = g_file_test(filename.c_str(), G_FILE_TEST_EXISTS);
    if (!isexists) {
        g_critical("init: home.ico could not be found!\n");
        return -1;
    }

    // assumes that the home.ico exists.
    DockItem* dockItem = new DockItem();
    dockItem->m_image = Gdk::Pixbuf::create_from_file(Utilities::getExecPath(DEF_ICONFILE).c_str(),
            DEF_ICONSIZE, DEF_ICONSIZE, true);

    dockItem->m_appname = "Home";
    dockItem->m_realgroupname = "Home";
    m_dockitems.push_back(dockItem);

    m_previewWindowActive = false;
    m_preview.setOwner(*this);

    loadAttachedItems();


    // Menus
    // Home Menu items
    m_CloseAllWindowsMenuItem.set_label("Close all Windows");
    m_CloseAllWindowsMenuItem.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_CloseAllWindows_event));
    m_HomeMenu_Popup.append(m_CloseAllWindowsMenuItem);

    m_HomeMenu_Popup.append(m_separatorMenuItem1);

    m_QuitMenuItem.set_label("Quit");
    m_QuitMenuItem.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_QuitMenu_event));
    m_HomeMenu_Popup.append(m_QuitMenuItem);

    m_HomeMenu_Popup.show_all();
    m_HomeMenu_Popup.accelerate(*this);


    // Item Menu
    m_MenuItemNewApp.set_label("Open new");
    m_MenuItemNewApp.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_menuNew_event));
    m_Menu_Popup.append(m_MenuItemNewApp);

    m_Menu_Popup.append(m_separatorMenuItem2);

    m_MenuItemAttach.set_label("Attach to Dock");
    m_MenuItemAttach.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_AttachToDock_event));
    m_Menu_Popup.append(m_MenuItemAttach);

    m_MenuItemDetach.set_label("Detach from Dock");
    m_MenuItemDetach.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_DetachFromDock_event));
    m_Menu_Popup.append(m_MenuItemDetach);

    m_Menu_Popup.append(m_separatorMenuItem3);

    m_MenuItemMinimizedAll.set_label("Minimize all");
    m_MenuItemMinimizedAll.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_MinimieAll_event));
    m_Menu_Popup.append(m_MenuItemMinimizedAll);


    m_MenuItemCloseAll.set_label("Close all");
    m_MenuItemCloseAll.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_CloseAll_event));
    m_Menu_Popup.append(m_MenuItemCloseAll);


    m_Menu_Popup.show_all();
    m_Menu_Popup.accelerate(*this);

    return 0;
}

/**
 * PostInit sets the signals handlers
 * @return 
 */
void DockPanel::postInit()
{
    m_fpstimer.start();
    // m_titleTimer.start();

    m_TimeoutConnection = Glib::signal_timeout().connect(sigc::mem_fun(*this,
            &DockPanel::on_timeoutDraw), 1000 / 60);

    WnckScreen *wnckscreen = wnck_screen_get(0); //_default();

    g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
            G_CALLBACK(DockPanel::on_window_opened), NULL);

    g_signal_connect(wnckscreen, "window_closed",
            G_CALLBACK(DockPanel::on_window_closed), NULL);

    g_signal_connect(wnckscreen, "active_window_changed",
            G_CALLBACK(DockPanel::on_active_window_changed_callback), NULL);

}

DockPanel::~DockPanel()
{

    m_titlewindow.close();

    for (auto item : m_dockitems)
        delete(item);

    m_dockitems.clear();

    if (m_TimeoutConnection)
        m_TimeoutConnection.disconnect(); //Will probably happen anyway, in the destrctor.
}

/**
 * get the item index from mouse coordinates.
 * @param x
 * @param y
 * @return 
 */
int DockPanel::getIndex(int x, int y)
{
    //    if (!m_mouseIn)
    //        return -1;

    int center = MonitorGeometry::getGeometry().width / 2;
    Gdk::Point mouse(x, y);
    int result = -1;
    int idx = 0;
    int col = center - (m_dockitems.size() * m_cellwidth) / 2;



    for (auto item : m_dockitems) {
        if (item->m_image == NULLPB)
            continue;

        if (mouse.get_x() >= col && mouse.get_x() <= col + m_cellwidth) {
            return idx;
        }

        col += m_cellwidth;
        idx++;
    }


    return result;
}

/**
 * handles on_leave_notify_event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 * @param crossing_event
 * @return 
 */
bool DockPanel::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    m_mouseIn = true;
    return true;
}

/**
 * handles on_leave_notify_event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 * @param crossing_event
 * @return 
 */
bool DockPanel::on_leave_notify_event(GdkEventCrossing* crossing_event)
{

    m_mouseIn = false;
    m_titlewindow.hide();
    // if(!m_preview.m_mouseIn )
    //   m_preview.hide();


    if (m_mouseRightClick) {
        m_mouseRightClick = false;
        return true;
    }

    if (!m_previewWindowActive) {
        WnckScreen *wnckscreen = wnck_screen_get(0);
        WnckWindow *window = wnck_screen_get_active_window(wnckscreen);
        if (window == NULL) {
            m_currentMoveIndex = -1;
            return true;
        }


        DockPanel::setItemImdexFromActiveWindow(window);
    }
    return true;
}

void DockPanel::previewWindowClosed()
{
    WnckScreen *wnckscreen = wnck_screen_get(0);
    WnckWindow *window = wnck_screen_get_active_window(wnckscreen);
    if (window == NULL) {
        m_currentMoveIndex = -1;
        return;
    }


    DockPanel::setItemImdexFromActiveWindow(window);
}

/**
 * handles Mouse notion: process mouse button event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further.
 * @param event
 * @return 
 */
bool DockPanel::on_motion_notify_event(GdkEventMotion*event)
{
    m_currentMoveIndex = getIndex(event->x, event->y);
    return false;
}

/** 
 * bool DockPanel::on_button_press_event(GdkEventButton *event)
 * 
 * handles Mouse button press : process mouse button event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 */
bool DockPanel::on_button_press_event(GdkEventButton *event)
{
    if ((event->type == GDK_BUTTON_PRESS)) {

        m_currentMoveIndex = getIndex(event->x, event->y);

        m_mouseRightClick = false;

        // Check if the event is a left button click.
        if (event->button == 1 && !m_mouseLeftButtonDown) {
            m_mouseLeftButtonDown = true;
            // The event has been handled.
            return true;
        }
        // Check if the event is a right button click.
        if (event->button == 3 && !m_mouseRightButtonDown) {
            m_mouseRightButtonDown = true;
            // The event has been handled.
            return true;
        }
    }
    return false;
}

/** 
 * bool DockPanel::on_button_release_event(GdkEventButton *event)
 * 
 * handles Mouse button released : process mouse button event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 */
bool DockPanel::on_button_release_event(GdkEventButton *event)
{

    if (!m_mouseIn)
        return false;

    if (m_mouseLeftButtonDown) {

        //  m_selectedIndex = m_currentMoveIndex;
        SelectWindow(m_currentMoveIndex, event);
        m_mouseLeftButtonDown = false;

        // The event has been handled.
        return true;
    }

    if (m_mouseRightButtonDown) {
      
        m_preview.hideMe();
        m_mouseRightClick = true;
        m_mouseRightButtonDown = false;


        if (m_currentMoveIndex > 0) {
            DockItem *dockitem = m_dockitems.at(m_currentMoveIndex);
            bool maximizedexistst = isExitstMaximizedWindows();
            m_MenuItemMinimizedAll.set_sensitive(dockitem->m_items.size() > 0 && maximizedexistst);
            m_MenuItemCloseAll.set_sensitive(dockitem->m_items.size() > 0);
            m_MenuItemAttach.set_sensitive(dockitem->m_isAttached == false);
            m_MenuItemDetach.set_sensitive(dockitem->m_isAttached &&
                    dockitem->m_items.size() == 0);

            if (!m_Menu_Popup.get_attach_widget())
                m_Menu_Popup.attach_to_widget(*this);


            m_HomeMenu_Popup.resize_children();
            m_Menu_Popup.set_halign(Gtk::Align::ALIGN_CENTER);
            m_Menu_Popup.popup(sigc::mem_fun(*this,
                    &DockPanel::on_popup_menu_position), 1, event->time);

            return true;
        }
        if (m_currentMoveIndex == 0) {
            if (!m_HomeMenu_Popup.get_attach_widget())
                m_HomeMenu_Popup.attach_to_widget(*this);

            m_HomeMenu_Popup.resize_children();
            m_HomeMenu_Popup.set_halign(Gtk::Align::ALIGN_CENTER);
            m_HomeMenu_Popup.popup(sigc::mem_fun(*this,
                    &DockPanel::on_popup_homemenu_position), 1, event->time);

            return true;
        }

        // Propagate the event further.
        return false;
    }

    return false;
}

/**
 * Handle the home menu popup position.
 * @param x
 * @param y
 * @param push_in
 */
void DockPanel::on_popup_homemenu_position(int& x, int& y, bool& push_in)
{
    int center = (MonitorGeometry::getGeometry().width / 2) -
            (m_HomeMenu_Popup.get_allocated_width() / 2);

    int col = center - (m_dockitems.size() * m_cellwidth) / 2;

    x = MonitorGeometry::getGeometry().x + col + (m_cellwidth / 2) + (m_cellwidth * m_currentMoveIndex);
    y = MonitorGeometry::getAppWindowTopPosition();

    // This is a fix for a BUG! in  Gtk::Menu.
    // The position don't work on resolution smaller or equal then 768 height.
    if (MonitorGeometry::getGeometry().height <= 768) {
        y -= HOME_POPUPMENU_Y_768_REPOSITION; // Modify this value depend of the menu children count
    }
}

void DockPanel::on_popup_menu_position(int& x, int& y, bool& push_in)
{
    int center = (MonitorGeometry::getGeometry().width / 2) -
            (m_Menu_Popup.get_allocated_width() / 2);

    int col = center - (m_dockitems.size() * m_cellwidth) / 2;

    x = MonitorGeometry::getGeometry().x + col + (m_cellwidth / 2) + (m_cellwidth * m_currentMoveIndex);
    y = MonitorGeometry::getAppWindowTopPosition();

    // This is a fix for a BUG! in  Gtk::Menu.
    // The position don't work on resolution smaller or equal then 768 height.
    if (MonitorGeometry::getGeometry().height <= 768) {
        y -= ITEM_POPUPMENU_Y_768_REPOSITION; // Modify this value depend of the menu children count.
    }
}

void DockPanel::on_QuitMenu_event()
{
    m_AppWindow->close();
}

void DockPanel::on_menuNew_event()
{

    int index = m_currentMoveIndex;
    if (index < 1)
        return;

    Launcher::Launch(m_dockitems.at(index));

}

void DockPanel::on_AttachToDock_event()
{
    if (m_currentMoveIndex < 1)
        return;

    DockItem *dockitem = m_dockitems.at(m_currentMoveIndex);

    if (dockitem->m_isAttached)
        return; // already attached

    dockitem->m_isAttached = true;
    dockitem->m_isDirty = true;

    char filename[PATH_MAX];
    std::string s = dockitem->m_realgroupname;
    std::replace(s.begin(), s.end(), ' ', '_'); // replace all ' ' to '_'
    sprintf(filename, "%s/%s.png", m_applicationDatapath.c_str(), s.c_str());

    dockitem->m_image->save(filename, "png");

    g_print("DockItem is now Attached for index : %d\n", m_currentMoveIndex);

}

void DockPanel::on_DetachFromDock_event()
{
    if (m_currentMoveIndex < 0)
        return;

    DockItem *dockitem = m_dockitems.at(m_currentMoveIndex);

    if (!dockitem->m_isAttached)
        return;

    // can't detach if still open
    if (dockitem->m_items.size() > 0)
        return;

    char filename[PATH_MAX];

    std::string s = dockitem->m_realgroupname;
    std::replace(s.begin(), s.end(), ' ', '_'); // replace all ' ' to '_'
    sprintf(filename, "%s//%s.png", m_applicationDatapath.c_str(), s.c_str());

    g_print(" Remove path %s n", m_applicationDatapath.c_str());

    if (remove(filename) != 0) {
        g_print("\non_UnPin_event. ERROR remove file. ");
        return;
    }

    int idx = m_currentMoveIndex;
    WnckWindow *window = dockitem->m_window;
    setItemImdexFromActiveWindow(window);

    delete(dockitem);
    m_dockitems.erase(m_dockitems.begin() + idx);



}

void DockPanel::on_CloseAll_event()
{
    if (m_currentMoveIndex < 0)
        return;

    DockItem *dockitem = m_dockitems.at(m_currentMoveIndex);
    for (auto item : dockitem->m_items) {
        WnckWindow *window = item->m_window;
        if (window == NULL)
            continue;

        int ct = gtk_get_current_event_time();
        wnck_window_close(window, (guint32) ct);
    }
}

void DockPanel::on_MinimieAll_event()
{
    if (m_currentMoveIndex < 0)
        return;

    DockItem *dockitem = m_dockitems.at(m_currentMoveIndex);
    for (auto item : dockitem->m_items) {
        WnckWindow *window = item->m_window;
        if (window == NULL)
            continue;

        if (wnck_window_is_minimized(window) == false)
            wnck_window_minimize(window);
    }
}

void DockPanel::on_CloseAllWindows_event()
{
    WnckScreen *screen;
    GList *window_l;

    screen = wnck_screen_get_default();
    wnck_screen_force_update(screen);

    for (window_l = wnck_screen_get_windows(screen);
            window_l != NULL; window_l = window_l->next) {

        WnckWindow *window = WNCK_WINDOW(window_l->data);
        if (window == NULL)
            continue;

        WnckWindowType wt = wnck_window_get_window_type(window);

        if (wt == WNCK_WINDOW_DESKTOP ||
                wt == WNCK_WINDOW_DOCK ||
                wt == WNCK_WINDOW_TOOLBAR ||
                wt == WNCK_WINDOW_MENU) {
            continue;
        }

        int ct = gtk_get_current_event_time();
        wnck_window_close(window, (guint32) ct);
    }

}

/* bool DockPanel::on_scroll_event(GdkEventScroll * e)
 * 
 * handles the mouse scroll . 
 * 
 */
bool DockPanel::on_scroll_event(GdkEventScroll * e)
{
    int index = m_currentMoveIndex;
    if (index == -1 || index == 0)
        return true;

    DockItem *item = m_dockitems.at(index)->GetNext();
    if (item == nullptr)
        return true;

    WnckWindow *itemWindow = item->m_window;
    int ct = gtk_get_current_event_time();

    if ((int) e->delta_y == (int) 1) {
        wnck_window_activate(itemWindow, (guint32) ct);
    } else if ((int) e->delta_y == (int) - 1) {
        wnck_window_activate(itemWindow, (guint32) ct);
    }

    // Event has been handled
    return true;
}


int m_oldcellsize = DEF_CELLWIDTH;

/**
 * Timeout handler to regenerate the frame. 
 * force to redraw the entire content.
 */
bool DockPanel::on_timeoutDraw()
{

    //    DockPosition::setItemsCount(m_dockitems.size());
    //    m_cellwidth = DockPosition::getCellSize();
    //    m_iconsize = DockPosition::getIconSize();
    //
    //   // g_print("Recal %d %d\n",m_cellwidth,(int)m_titleTimer.elapsed());
    //    
    //    if (m_cellwidth != m_oldcellsize) {
    //        //g_print("Recal %d \n",(int)m_titleTimer.elapsed());
    //        m_oldcellsize = m_cellwidth;
    //        for (DockItem* item : m_dockitems) {
    //
    //            Glib::RefPtr<Gdk::Pixbuf> appIcon = item->m_image;
    //            item->m_image = appIcon->scale_simple(m_iconsize,
    //                    m_iconsize, Gdk::INTERP_BILINEAR);
    //
    //        }
    //
    //    }

    Gtk::Widget::queue_draw();

    ++m_frames;
    m_last_time += m_fpstimer.elapsed();

    if (m_last_time >= 0) {
        m_curFPS = (float) (m_frames / m_last_time);
        m_frames = 0;
        m_last_time = 0;
        m_fpstimer.reset();
    }

    //g_print("FPS: %d\n", (int) m_curFPS);

    return true;
}

/**
 * Emitted when a new Wnck.Window is opened on screen.
 * @param screen
 * @param window
 * @param data
 */
void DockPanel::on_window_opened(WnckScreen *screen, WnckWindow* window, gpointer data)
{
    Update(window, Window_action::OPEN);
}

/**
 * Emitted when a Wnck.Window is closed on screen.
 * @param screen
 * @param window
 * @param data
 */
void DockPanel::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data)
{
    Update(window, Window_action::CLOSE);
}

/**
 * Emitted when the active window on screen has changed.
 * @param screen
 * @param previously_active_window
 * @param user_data
 */
void DockPanel::on_active_window_changed_callback(WnckScreen *screen,
        WnckWindow *previously_active_window, gpointer user_data)
{
    if (m_previewWindowActive)
        return;

    WnckWindow *window = wnck_screen_get_active_window(screen);
    if (window == NULL) {
        m_currentMoveIndex = -1;
        return;
    }

    DockPanel::setItemImdexFromActiveWindow(window);
}

void DockPanel::setItemImdexFromActiveWindow(WnckWindow *window)
{
    int xid = wnck_window_get_xid(window);

    int idx = 0;
    bool found = false;
    for (auto item : m_dockitems) {
        for (auto chiditem : item->m_items) {
            if (xid == chiditem->m_xid) {
                m_currentMoveIndex = idx;
                found = true;
                break;
            }
        }
        if (found)
            break;
        idx++;
    }

    if (!found)
        m_currentMoveIndex = -1;
}

/**
 * void DockPanel::Update(WnckWindow* window, bool mode)
 * Update the items and handle the X Window events window_close and window_open.
 * @param window
 * @param actiontype
 */
void DockPanel::Update(WnckWindow* window, Window_action actiontype)
{

    WnckWindowType wt = wnck_window_get_window_type(window);

    if (wt == WNCK_WINDOW_DESKTOP ||
            wt == WNCK_WINDOW_DOCK ||
            wt == WNCK_WINDOW_MENU ||
            wt == WNCK_WINDOW_SPLASHSCREEN) {
        return;
    }

    const std::string extensions[] = {".py", ".exe", ".sh"};
    const char* _appname = wnck_window_get_name(window);
    if (_appname == NULL) {
        g_print("Update: No Application name....\n");
        return;
    }
    std::string appname(_appname);

    const char* _instancename = wnck_window_get_class_instance_name(window);
    if (_instancename == NULL) {
        _instancename = _appname;
    }

    std::string instancename(_instancename);
    instancename = Utilities::removeExtension(instancename, extensions);

    const char* _realgroupname = wnck_window_get_class_group_name(window);
    if (_realgroupname == NULL) {
        _realgroupname = _appname;
    }

    std::string realgroupname(_realgroupname);
    realgroupname = Utilities::removeExtension(realgroupname, extensions);

    std::string titlename = Utilities::stringToLower(realgroupname.c_str());
    titlename = (Launcher::getTitleNameFromDesktopFile(titlename));

    if (realgroupname == "Wine")
        realgroupname = instancename;


    //DEBUG
    g_print("appname: %s, %s, %s title:%s\n", appname.c_str(),
            instancename.c_str(),
            realgroupname.c_str(),
            titlename.c_str());

    if (actiontype == Window_action::OPEN) {

        Glib::RefPtr<Gdk::Pixbuf> appIcon = NULLPB;

        appIcon = IconLoader::GetWindowIcon(window);
        appIcon = appIcon->scale_simple(DEF_ICONSIZE,
                DEF_ICONSIZE, Gdk::INTERP_BILINEAR);

        // handle DockItems groups
        for (auto item : m_dockitems) {
            if (realgroupname == item->m_realgroupname) {
                DockItem* childItem = new DockItem();
                childItem->m_realgroupname = realgroupname;
                childItem->m_appname = appname;
                childItem->m_titlename = titlename;
                childItem->m_instancename = instancename;
                childItem->m_isDirty = false;
                childItem->m_window = window;
                childItem->m_xid = wnck_window_get_xid(window);
                childItem->m_image = NULLPB;

                if (item->m_isAttached && item->m_isDirty) {
                    item->m_window = window;
                    item->m_xid = childItem->m_xid;
                    item->m_isDirty = false;

                    for (auto itemtorelease : item->m_items)
                        delete(itemtorelease);

                    item->m_items.clear();
                }

                item->m_items.push_back(childItem);
                return;
            }
        }

        // Create a new Item
        DockItem* dockItem = new DockItem();
        dockItem->m_appname = appname;
        dockItem->m_titlename = titlename;
        dockItem->m_realgroupname = realgroupname;
        dockItem->m_instancename = instancename;
        dockItem->m_window = window;
        dockItem->m_xid = wnck_window_get_xid(window);
        dockItem->m_image = appIcon;

        // Create a child items
        DockItem* childItem = new DockItem();
        childItem->m_appname = dockItem->m_appname;
        childItem->m_titlename = titlename;
        childItem->m_realgroupname = dockItem->m_realgroupname;
        childItem->m_instancename = dockItem->m_instancename;
        childItem->m_window = dockItem->m_window;
        childItem->m_xid = dockItem->m_xid;
        childItem->m_image = NULLPB;

        dockItem->m_items.push_back(childItem);
        m_dockitems.push_back(std::move(dockItem));

    } else {
        // find the item to remove;
        int xid = wnck_window_get_xid(window);
        int i = 1;
        bool found = false;
        for (; i < (int) m_dockitems.size(); i++) {
            for (auto ci : m_dockitems.at(i)->m_items) {
                if (ci->m_xid == xid) {
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }
        if (xid > 0 && found) {
            auto item = m_dockitems.at(i);
            if (item->m_items.size() == 1) {
                // if is attached delete the child item.
                if (item->m_isAttached) {
                    item->m_isDirty = true;

                    delete( item->m_items.at(0));
                    item->m_items.erase(item->m_items.begin());
                    return;
                }

                // remove this item
                delete( item->m_items.at(0));
                m_dockitems.erase(m_dockitems.begin() + i);

                // if is not attached then it is at the end on the list.
                // we need to reset the index.
                m_currentMoveIndex = -1;
                return;
            } else {
                // search for the xid and remove the item
                int idx = 0;
                for (auto c : item->m_items) {
                    if (c->m_xid == xid) {
                        delete(c);
                        item->m_items.erase(item->m_items.begin() + idx);
                        return;
                    }
                    idx++;
                }
            }
        }
    }
}

/**
 * Renderer
 * @param Cairo::Context
 * @return true/false 
 */
bool DockPanel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    Glib::RefPtr<Gdk::Pixbuf> icon = NULLPB;

    DockPosition::getDockItemGeometry(m_dockitems.size(), m_cellwidth, m_iconsize);
    bool resizeNeeded = m_cellwidth != DEF_CELLWIDTH;

    if (m_cellwidth == DEF_CELLWIDTH)
        m_cellheight = DEF_CELLHIGHT;

    // compute the cell height if need to resized
    if ((resizeNeeded && m_previousCellwidth != m_cellwidth)) {

        int iconrestsapce = DEF_CELLHIGHT - (DEF_ICONSIZE + DEF_ICONTOPCELLMARGIN);
        int substractpixels = (DEF_CELLHIGHT - (m_iconsize + DEF_ICONTOPCELLMARGIN)) - iconrestsapce;

        m_cellheight = DEF_CELLHIGHT - substractpixels;
        m_previousCellwidth = m_cellwidth;

        MonitorGeometry::updateStrut(m_AppWindow, DEF_PANELHIGHT - substractpixels);

        g_print("%d %d %d\n", iconrestsapce, substractpixels, (int) m_titleTimer.elapsed());
    }


    int center = MonitorGeometry::getGeometry().width / 2;
    int col = center - (m_dockitems.size() * m_cellwidth) / 2;
    int pos_x = col = center - ((m_dockitems.size() * m_cellwidth) / 2);

    // Timer control for the title Window
    if (m_mouseIn && m_currentMoveIndex == -1) {
        m_titlewindow.hide();

        if (m_previewWindowActive)
            m_preview.hideMe();
    }
    if (m_mouseIn && m_currentMoveIndex != -1) {
        if (m_titleItemOldindex != m_currentMoveIndex) {
            m_titleItemOldindex = m_currentMoveIndex;
            m_titleElapsedSeconds = 0;
            m_titleTimer.start();
            m_titleShow = false;

            m_titlewindow.hide();
            if (m_previewWindowActive)
                m_preview.hideMe();

        }

        if (m_titleItemOldindex == m_currentMoveIndex) {
            if (m_titleElapsedSeconds > 0.5 && m_titleShow == false && !m_previewWindowActive) {

                DockItem* item = m_dockitems.at(m_currentMoveIndex);
                m_titlewindow.setText(item->getTitle());

                int centerpos = DockPosition::getCenterPosByCurrentDockItemIndex(
                        m_dockitems.size(),
                        m_currentMoveIndex,
                        m_titlewindow.getCurrentWidth()
                        );

                m_titlewindow.move(centerpos, MonitorGeometry::getAppWindowTopPosition() - 30);

                //g_print("SHOW%d\n", (int) m_currentMoveIndex);
                m_titleShow = true;

            }

            m_titleElapsedSeconds = m_titleTimer.elapsed();
            //g_print("%d\n", (int) m_titleElapsedSeconds);
        }
    }

    // uncomment for a Panel Background color;
    //     cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);
    //     cr->paint();

    cr->set_source_rgba(0.0, 0.0, 0.8, 0.4); // partially translucent
    Utilities::RoundedRectangle(cr, pos_x,
            DEF_CELLTOPMARGIN, (m_dockitems.size() * m_cellwidth),
            m_cellheight, 2.0);
    cr->fill();

    // Selector
    if (m_currentMoveIndex != -1/* && m_mouseIn*/) {

        pos_x = col + (m_cellwidth / 2) + (m_cellwidth * m_currentMoveIndex) - m_cellwidth / 2;
        cr->set_line_width(2);
        cr->set_source_rgba(1.0, 1.0, 1.0, 0.4); // partially translucent
        Utilities::RoundedRectangle(cr, pos_x, DEF_CELLTOPMARGIN, m_cellwidth - 1, m_cellheight, 2.0);
        cr->fill();

        // rectangle border
        cr->set_source_rgba(1.0, 1.0, 1.0, 0.8); // partially translucent     
        Utilities::RoundedRectangle(cr, pos_x, DEF_CELLTOPMARGIN,
                m_cellwidth - 1, m_cellheight, 2.0);
        cr->stroke();
    }




    for (auto item : m_dockitems) {
        if (item->m_image == NULLPB)
            continue;

        // Draw circles
        cr->set_source_rgb(1.0, 1.0, 1.0);
        if (item->m_items.size() == 1) {
            cr->arc(col + (m_cellwidth / 2), m_cellheight, 2, 0, 2 * M_PI);
        } else if (item->m_items.size() > 1) {
            cr->arc((col + (m_cellwidth / 2)) - 4, m_cellheight, 2, 0, 2 * M_PI);
            cr->arc((col + (m_cellwidth / 2)) + 4, m_cellheight, 2, 0, 2 * M_PI);
        }
        cr->fill();

        // Draw icons
        cr->save();
        if (resizeNeeded) {
            // scale_simple is very fast. 
            // However this will not happen often.
            icon = item->m_image->scale_simple(
                    m_iconsize, m_iconsize, Gdk::INTERP_BILINEAR);

        } else {
            icon = item->m_image;
        }
        Gdk::Cairo::set_source_pixbuf(cr, icon, col + 5, DEF_ICONTOPMARGIN);
        cr->paint();
        cr->restore();


        // Draw Rectangles
        cr->set_source_rgba(1.0, 1.0, 1.0, 0.8); // partially translucent
        cr->set_line_width(0.7);
        Utilities::RoundedRectangle(cr, col, DEF_CELLTOPMARGIN, m_cellwidth - 1, m_cellheight, 2.0);
        cr->stroke();

        col += m_cellwidth;
    }

    return true;
}

std::string DockPanel::getApplicationPath()
{
    return m_applicationpath;
}

void DockPanel::SelectWindow(int index, GdkEventButton * event)
{

    if (index < 1)
        return;

    DockItem *dockitem = m_dockitems.at(index);
    int itemscount = dockitem->m_items.size();

    if (itemscount == 0 && dockitem->m_isAttached) {
        Launcher::Launch(dockitem);
        return;
    }

    if (m_dockitems.at(index)->m_window == NULL)
        return;


    int previewWidth = 0;
    int previewHeight = 0;
    int previewWindowWidth = 0;

    
    m_preview.init(dockitem, previewWidth, previewHeight, previewWindowWidth);

    // calculate the preview position. 
    int centerpos = DockPosition::getCenterPosByCurrentDockItemIndex(
            m_dockitems.size(), index, previewWindowWidth);

    int maxwidth = centerpos + previewWindowWidth; //(dockitem->m_items.size() * (DEF_PREVIEW_WIDTH+4) );
    maxwidth -= MonitorGeometry::getGeometry().x;

    if (maxwidth >= MonitorGeometry::getGeometry().width)
        centerpos -= (maxwidth - MonitorGeometry::getGeometry().width) ;

    // Debug
   // g_print("previewWindowWidth %d max %d %d\n", previewWindowWidth, maxwidth,MonitorGeometry::getGeometry().width);
    m_previewWindowActive = true;
    m_preview.show_now();
    m_preview.move(centerpos, MonitorGeometry::getAppWindowTopPosition() - previewHeight );

}

bool DockPanel::isExitstMaximizedWindows()
{
    if (m_currentMoveIndex < 0)
        return false;

    bool result = false;
    DockItem *dockitem = m_dockitems.at(m_currentMoveIndex);
    for (auto item : dockitem->m_items) {

        WnckWindow *window = item->m_window;
        if (window == NULL)
            continue;

        if (wnck_window_is_minimized(window) == FALSE) {
            result = true;
            break;
        }

    }

    return result;
}

void DockPanel::loadAttachedItems()
{
     
    DIR* dirFile = opendir(m_applicationDatapath.c_str());
    struct dirent* hFile;
    errno = 0;
    while ((hFile = readdir(dirFile)) != NULL) {
        if (!strcmp(hFile->d_name, ".")) continue;
        if (!strcmp(hFile->d_name, "..")) continue;
        if ((hFile->d_name[0] == '.')) continue; // in linux hidden files all start with '.'

        if (strstr(hFile->d_name, ".png")) {

            std::string filename = hFile->d_name;
            std::string imageFilePath = std::string(m_applicationDatapath) +
                    std::string("/") + filename;

            g_print("found an .png file: %s\n", imageFilePath.c_str());

            std::vector<std::string> tokens = Utilities::split(filename, '_');
            std::size_t found = filename.find_last_of(".");
            if (found < 1)
                continue;

            std::string appname = filename.substr(0, found);
            std::replace(appname.begin(), appname.end(), '_', ' ');


            std::string titlename = Utilities::stringToLower(appname.c_str());
            titlename = (Launcher::getTitleNameFromDesktopFile(titlename));


            DockItem* item = new DockItem();
            item->m_appname = appname;
            item->m_instancename = Utilities::stringToLower(appname.c_str());
            item->m_realgroupname = appname;
            item->m_titlename = titlename;
            item->m_window = NULL;
            item->m_xid = 0;
            item->m_image = item->m_image->create_from_file(imageFilePath);
            item->m_isAttached = true;
            item->m_isDirty = true;


            m_dockitems.push_back(item);
        }
    }
    closedir(dirFile);
}