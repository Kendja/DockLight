//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on November 20, 2015 
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
#include <stdio.h>
#include <string>
#include <limits.h>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <cstring>
#include "DockPanel.h"
#include "Utilities.h"
#include <gdkmm/screen.h>
#include <gtkmm/alignment.h>
#include <gdkmm/rgba.h>
#include "math.h"


// the static collection
std::vector<DockItem*>*DockPanel::_itemsvector;

DockPanel::DockPanel()
{
    m_preview = new XPreview();

    m_panelLocation = panel_locationType::BOTTOM;
    m_mouseLeftDoubleClickButtonDown =
    m_mouseRightButtonDown =
    m_mouseLeftButtonDown = false;
    m_mouseIn = false;
    m_selectedIndex = -1;
    m_currentMoveIndex = 0;
    m_fpstimer.start();
    frames = 0;
    last_time = 0;

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


    _itemsvector = new std::vector<DockItem*>();

    DockItem *dockItem = new DockItem();
    dockItem->m_image = Gdk::Pixbuf::create_from_file(Utilities::getExecPath("home.ico"),
            DEF_ICONSIZE, DEF_ICONSIZE, true);

    _itemsvector->push_back(dockItem);

    std::string binpath = Utilities::getExecPath();

    g_print("\nPATH=%s\n", binpath.c_str());
    readPinItems(binpath.c_str());
    

    // Set X Window signals
    WnckScreen *wnckscreen;
    wnckscreen = wnck_screen_get_default();

    g_signal_connect(wnckscreen, "window-opened",
            G_CALLBACK(DockPanel::on_window_opened), NULL);
    g_signal_connect(wnckscreen, "window_closed",
            G_CALLBACK(DockPanel::on_window_closed), NULL);
    g_signal_connect(wnckscreen, "active-window-changed",
            G_CALLBACK(DockPanel::on_active_window_changed), NULL);


    m_TimeoutConnection = Glib::signal_timeout().connect(sigc::mem_fun(*this,
            &DockPanel::on_timeoutDraw), 50);
    
    // Context Menus
    MenuItemNewApp.set_label("Open");
    MenuItemNewApp.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_menuNew_event));
    m_Menu_Popup.append(MenuItemNewApp);

    // Add a separator
    m_Menu_Popup.append(m_separatorMenuItem);

    MenuItemPin.set_label("Pin to Dock");
    MenuItemPin.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_Pin_event));
    m_Menu_Popup.append(MenuItemPin);

    MenuItemRemoveFromDock.set_label("Remove from Dock");
    MenuItemRemoveFromDock.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_RemoveFromDock_event));
    m_Menu_Popup.append(MenuItemRemoveFromDock);

    // Add a separator
    m_Menu_Popup.append(m_separatorMenuItem2);

    
    MenuItemCloseAll.set_label("Close all");
    MenuItemCloseAll.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_CloseAll_event));
    m_Menu_Popup.append(MenuItemCloseAll);
    m_Menu_Popup.set_border_width(4);
    m_Menu_Popup.show_all(); // Show the menu
    m_Menu_Popup.accelerate(*this); // Connect the menu to this Widget
    

    Gdk::RGBA color;
    color.set_rgba(1.0, 1.0, 1.0, 1.0);
    m_Menu_Popup.override_background_color(color, Gtk::STATE_FLAG_NORMAL);

    // HomeMenu
    m_QuitMenuItem.set_label("Quit");
    m_QuitMenuItem.signal_activate().connect(sigc::mem_fun(*this, &DockPanel::on_QuitMenu_event));
    m_HomeMenu_Popup.append(m_QuitMenuItem);
    m_HomeMenu_Popup.show_all();
    m_HomeMenu_Popup.accelerate(*this);
}

void DockPanel::setPanelLocation(panel_locationType panelLocation)
{
    this->m_panelLocation = panelLocation;
    m_preview->setPanelLocation(this->m_panelLocation);
}

void DockPanel::on_menuNew_event()
{
    if (m_currentMoveIndex < 1)
        return;

    LaunchApplication(_itemsvector->at(m_currentMoveIndex));
}

void DockPanel::on_Pin_event()
{
    if (m_currentMoveIndex < 1)
        return;

    DockItem *dockitem = _itemsvector->at(m_currentMoveIndex);

    if (dockitem->m_isFixed == 1)
        return; // already pinge

    dockitem->m_isFixed = 1;
    dockitem->m_isDirty = true;

    char filename[PATH_MAX];
    sprintf(filename, "%s//%s_%s_%s_png",
            Utilities::getExecPath().c_str(),
            dockitem->m_realgroupname.c_str(),
            dockitem->m_appname.c_str(),
            dockitem->m_instancename.c_str()
            );
    dockitem->m_image->save(filename, "png");

    g_print("DockItem is now Pinged selected for index : %d\n", m_currentMoveIndex);

}

void DockPanel::on_QuitMenu_event()
{
    //FIXME: segmentation fault...
    Gtk::Main::quit();

}

void DockPanel::on_RemoveFromDock_event()
{
    if (m_currentMoveIndex < 0)
        return;

    DockItem *dockitem = _itemsvector->at(m_currentMoveIndex);

    if (dockitem->m_isFixed != 1)
        return;

    if (dockitem->m_items->size() > 0)
        return;

    char filename[PATH_MAX];
    sprintf(filename, "%s/%s_%s_%s_png",
            Utilities::getExecPath().c_str(),
            dockitem->m_realgroupname.c_str(),
            dockitem->m_appname.c_str(),
            dockitem->m_instancename.c_str()
            );

    if (remove(filename) != 0) {
        g_print("on_UnPin_event. ERROR remove file. ");
    }

    _itemsvector->erase(_itemsvector->begin() + m_currentMoveIndex);
}

void DockPanel::on_CloseAll_event()
{
    if (m_currentMoveIndex < 0)
        return;

    DockItem *dockitem = _itemsvector->at(m_currentMoveIndex);
    for (auto item : *dockitem->m_items) {

        WnckWindow *window = item->m_window;
        if (window == NULL)
            continue;

        int ct = gtk_get_current_event_time();
        wnck_window_close(window, (guint32) ct);
    }
}

void DockPanel::readPinItems(const char* path)
{
    DIR* dirFile = opendir(path);
    struct dirent* hFile;
    errno = 0;
    while ((hFile = readdir(dirFile)) != NULL) {
        if (!strcmp(hFile->d_name, ".")) continue;
        if (!strcmp(hFile->d_name, "..")) continue;
        if ((hFile->d_name[0] == '.')) continue; // in linux hidden files all start with '.'

        if (std::strstr(hFile->d_name, "_png")) {

            std::string filename = hFile->d_name;
            std::string imageFilePath = std::string(path) + std::string("/") + filename;
            g_print("found an .png file: %s\n", imageFilePath.c_str());

            std::vector<std::string> tokens = Utilities::split(filename, '_');

            DockItem* item = new DockItem();
            item->m_groupname = tokens.at(0);
            item->m_realgroupname = tokens.at(0);

            item->m_appname = tokens.at(1);
            item->m_instancename = tokens.at(2);
            item->m_window = NULL;
            item->m_xid = 0;
            item->m_image = item->m_image->create_from_file(imageFilePath);
            // item->m_image = item->m_image->scale_simple(DEF_ICONSIZE, DEF_ICONSIZE, Gdk::INTERP_BILINEAR);

            item->m_isFixed = 1;
            item->m_isDirty = true;

            item->m_items->clear(); //TODO: need to free space

            if (strcmp(item->m_groupname.c_str(), "Wine") == 0) {
                item->m_groupname = item->m_instancename;
            }
            _itemsvector->push_back(item);
        }
    }
    closedir(dirFile);
}

DockPanel::~DockPanel()
{
    // TODO: free pointers
    //Will probably happen anyway, in the destructor.
    m_TimeoutConnection.disconnect();
}

/* bool DockPanel::on_enter_notify_event(GdkEventCrossing* crossing_event)
 * 
 * handles on_leave_notify_event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 */
bool DockPanel::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    m_mouseIn = true;
    return true;
}

/* DockPanel::on_leave_notify_event(GdkEventCrossing* crossing_event)
 * 
 * handles on_leave_notify_event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 */
bool DockPanel::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    m_mouseIn = false;
    return true;
}

/* bool DockPanel::on_motion_notify_event(GdkEventMotion*event)
 * 
 * handles on_motion_notify_event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 */
bool DockPanel::on_motion_notify_event(GdkEventMotion*event)
{
    m_currentMoveIndex = getIndex(event->x, event->y);

    if (m_preview->m_active && m_selectedIndex != m_currentMoveIndex) {
        m_selectedIndex = -1;
        m_preview->m_mouseIn = true;
        m_preview->hideMe();
    }

    return true;
}

/* bool DockPanel::on_button_release_event(GdkEventButton *event)
 * 
 * handles Mouse button released : process mouse button event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 */
bool DockPanel::on_button_release_event(GdkEventButton *event)
{
    m_LastEventButton = event;

    if (m_mouseLeftButtonDown) {
        m_selectedIndex = m_currentMoveIndex;
        SelectWindow(m_currentMoveIndex, event);
        m_mouseLeftButtonDown = false;
        
        // The event has been handled.
        return true;
    }

    if (m_mouseRightButtonDown) {
        m_preview->m_mouseIn = true;
        m_preview->hideMe();

        m_mouseRightButtonDown = false;

        if (m_currentMoveIndex > 0) {
            DockItem *dockitem = _itemsvector->at(m_currentMoveIndex);

            MenuItemCloseAll.set_sensitive(dockitem->m_items->size() > 0);
            MenuItemPin.set_sensitive(dockitem->m_isFixed != 1);
            MenuItemRemoveFromDock.set_sensitive(dockitem->m_isFixed == 1 &&
                    dockitem->m_items->size() == 0);

            if (!m_Menu_Popup.get_attach_widget())
                m_Menu_Popup.attach_to_widget(*this);

            m_Menu_Popup.set_halign(Gtk::Align::ALIGN_CENTER);
            m_Menu_Popup.popup(sigc::mem_fun(*this,
                    &DockPanel::on_popup_menu_position), 1, event->time /* gtk_get_current_event_time()*/);

            return true;
        }

        if (m_currentMoveIndex == 0) {
            if (!m_HomeMenu_Popup.get_attach_widget())
                m_HomeMenu_Popup.attach_to_widget(*this);

            m_HomeMenu_Popup.set_halign(Gtk::Align::ALIGN_CENTER);
            m_HomeMenu_Popup.popup(sigc::mem_fun(*this,
                    &DockPanel::on_popup_homemenu_position), 1, event->time /* gtk_get_current_event_time()*/);
            return true;
        }
    }
    // Propagate the event further.
    return false;
}

void DockPanel::on_popup_homemenu_position(int& x, int& y, bool& push_in)
{
    int center = (this->monitor_geo.get_width() / 2) -
            (m_HomeMenu_Popup.get_allocated_width() / 2);

    int col = center - (this->getCountItems() * DEF_CELLSIZE) / 2;
    x = col + (DEF_CELLSIZE / 2);

}

void DockPanel::on_popup_menu_position(int& x, int& y, bool& push_in)
{
    int center = (this->monitor_geo.get_width() / 2) -
            (m_Menu_Popup.get_allocated_width() / 2);

    int col = center - (this->getCountItems() * DEF_CELLSIZE) / 2;
    x = col + (DEF_CELLSIZE / 2) + (DEF_CELLSIZE * m_currentMoveIndex);

}

/* bool DockPanel::on_button_press_event(GdkEventButton *event)
 * 
 * handles Mouse button press : process mouse button event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 */
bool DockPanel::on_button_press_event(GdkEventButton *event)
{
    m_currentMoveIndex = getIndex(event->x, event->y);
    if ((event->type == GDK_BUTTON_PRESS)) {
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
    // Propagate the event further.
    return false;
}

// obsolete
void DockPanel::ShowPreview(int index, GdkEventButton *event)
{
    if (index < 1)
        return;

}


/* void DockPanel::LaunchApplication(const DockItem * item)
 * gtk-launch launches an application using the given name. The application 
 * is started with proper startup notification on a default display, unless specified otherwise.
 * gtk-launch takes at least one argument, the name of the application to launch. 
 * The name should match application desktop file name, as residing in /usr/share/application, 
 * with or without the '.desktop' suffix.
 * If called with more than one argument, the rest of them besides the application name 
 * are considered URI locations and are passed as arguments to the launched application.
 */
void DockPanel::LaunchApplication(const DockItem * item)
{
    char command[128];
    int result = 0;

    std::string instancename = item->m_instancename;
    std::string groupname = item->m_groupname;
    std::string appname = item->m_appname;

    const std::string extensions[] = {".py", ".exe"};
    instancename = Utilities::removeExtension(instancename, extensions);
    groupname = Utilities::removeExtension(groupname, extensions);
    appname = Utilities::removeExtension(appname, extensions);

    // wine handling
    if (strcmp(item->m_realgroupname.c_str(), "Wine") == 0) {
        sprintf(command, "%s &", instancename.c_str());
        result = std::system(command);
        if (result == 0)
            return;
    }

    sprintf(command, "gtk-launch %s ", instancename.c_str());
    result = std::system(command);
    if (result == 0)
        return;

    // lower case 
    std::string lowerNameInstanceName = Utilities::stringToLower(instancename.c_str());
    sprintf(command, "gtk-launch %s", lowerNameInstanceName.c_str());
    result = std::system(command);
    if (result == 0)
        return;

    sprintf(command, "gtk-launch %s", groupname.c_str());
    result = std::system(command);
    if (result == 0)
        return;

    std::string lowerGrpName = Utilities::stringToLower(groupname.c_str());
    sprintf(command, "gtk-launch %s", lowerGrpName.c_str());
    result = std::system(command);
    if (result == 0)
        return;


    sprintf(command, "gtk-launch %s", appname.c_str());
    result = std::system(command);
    if (result == 0)
        return;

    std::string lowerAppName = Utilities::stringToLower(appname.c_str());
    sprintf(command, "gtk-launch %s", lowerAppName.c_str());
    result = std::system(command);
    if (result == 0)
        return;

    // without gtk-launch
    sprintf(command, "%s &", lowerNameInstanceName.c_str());
    result = std::system(command);
    if (result == 0)
        return;

    sprintf(command, "%s &", lowerGrpName.c_str());
    result = std::system(command);
    if (result == 0)
        return;


    sprintf(command, "%s &", lowerAppName.c_str());
    result = std::system(command);
    if (result == 0)
        return;

}

void DockPanel::SelectWindow(int index, GdkEventButton * event)
{
    if (index < 1)
        return;

    DockItem *dockitem = _itemsvector->at(index);
    int itemscount = dockitem->m_items->size();

    if (itemscount == 0 || (itemscount == 0 && dockitem->m_isFixed == 1)) {
        LaunchApplication(dockitem);
        return;
    }

    if (_itemsvector->at(index)->m_window == NULL)
        return;

    int width = (DEF_PREVIEW_WIDTH * dockitem->m_items->size() - 1) + 30;
    m_preview->resize(width, DEF_PREVIEW_HEIGHT);

    if (m_panelLocation == panel_locationType::TOP) {
        m_preview->move(event->x - width / 2, DEF_PANELBCKHIGHT + 4);
    } else {
        m_preview->move(event->x - width / 2, Gdk::screen_height() - (DEF_PREVIEW_HEIGHT + DEF_PANELBCKHIGHT + 4));
    }

    m_preview->setXid(dockitem);
    m_preview->present();
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

    DockItem *item = _itemsvector->at(index)->GetNext();
    if (item == NULL)
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

/*
 * 
 * force to redraw the entire content.
 * 
 */
bool DockPanel::on_timeoutDraw()
{
    /*
    auto win = this->get_window();
    if (win) {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                get_allocation().get_height());

        win->invalidate_rect(r, false);
    }
     */
    Gtk::Widget::queue_draw();
    return true;
}

/* int DockPanel::getIndex(int x, int y)
 * 
 * get the item index from mouse coordinates.
 * 
 */
int DockPanel::getIndex(int x, int y)
{
    if (!m_mouseIn)
        return -1;

    int center = this->monitor_geo.get_width() / 2;
    Gdk::Point mouse(x, y);
    int result = -1;
    int idx = 0;
    int col = center - (this->getCountItems() * DEF_CELLSIZE) / 2;

    for (auto item : *_itemsvector) {
        if (item->m_image == (Glib::RefPtr<Gdk::Pixbuf>)NULL)
            continue;

        if (mouse.get_x() >= col && mouse.get_x() <= col + DEF_CELLSIZE) {
            return idx;
        }

        col += DEF_CELLSIZE;
        idx++;
    }

    return result;

}

/* int DockPanel::getCountItems()
 * 
 * count all items where m_image are not null
 * 
 */
int DockPanel::getCountItems()
{
    int count = 0;
    for (auto item : *_itemsvector) {
        if (item->m_image == (Glib::RefPtr<Gdk::Pixbuf>)NULL)
            continue;

        count++;
    }

    return count;
}

/* void DockPanel::Update(WnckWindow* window, bool mode)
 * 
 * Update the items and handle the X Window events window_close and window_open.
 * 
 */
void DockPanel::Update(WnckWindow* window, bool mode)
{
//  WNCK_WINDOW_NORMAL,       /* document/app window */
//  WNCK_WINDOW_DESKTOP,      /* desktop background */
//  WNCK_WINDOW_DOCK,         /* panel */
//  WNCK_WINDOW_DIALOG,       /* dialog */
//  WNCK_WINDOW_TOOLBAR,      /* tearoff toolbar */
//  WNCK_WINDOW_MENU,         /* tearoff menu */
//  WNCK_WINDOW_UTILITY,      /* palette/toolbox window */
//  WNCK_WINDOW_SPLASHSCREEN  /* splash screen */
  
    WnckWindowType wt = wnck_window_get_window_type(window);

    if (wt == WNCK_WINDOW_DESKTOP || 
        wt == WNCK_WINDOW_DOCK || 
        wt == WNCK_WINDOW_TOOLBAR ||
        wt == WNCK_WINDOW_MENU ||
        wt == WNCK_WINDOW_UTILITY ||
        wt == WNCK_WINDOW_SPLASHSCREEN) {
        return;
    }

    const char* groupname = wnck_window_get_class_group_name(window);
    const char* realgroupname = wnck_window_get_class_group_name(window);
    const char* appname = wnck_window_get_name(window);
    const char* instancename = wnck_window_get_class_instance_name(window);

    if (groupname == NULL || realgroupname == NULL || 
            appname == NULL || instancename == NULL) {
        g_print("Update wnck returns NULL\n");
        return;
    }

    // handle window_open event
    if (mode == 1) {
        // Handles special case with wine
        if (std::strncmp(groupname, "Wine", 4) == 0) {
            groupname = instancename;
        }

        auto appIcon = DockPanel::GetWindowIcon(window);
        appIcon = appIcon->scale_simple(DEF_ICONSIZE, 
                DEF_ICONSIZE, Gdk::INTERP_BILINEAR);


        // handle DockItems groups
        for (auto item : *_itemsvector) {
            if (std::strcmp(groupname, item->m_groupname.c_str()) == 0) {
                DockItem* newItem = new DockItem();
                newItem->m_groupname = groupname;
                newItem->m_realgroupname = realgroupname;
                newItem->m_appname = appname;
                newItem->m_instancename = instancename;
                newItem->m_isDirty = false;
                newItem->m_window = window;
                newItem->m_xid = wnck_window_get_xid(window);
                newItem->m_image = appIcon;

                if (item->m_isFixed && item->m_isDirty) {
                    item->m_window = window;
                    item->m_xid = wnck_window_get_xid(window);
                    item->m_isDirty = false;
                    item->m_items->clear(); //TODO: need to free space
                }

                item->m_items->push_back(newItem);
                return;
            }

        }

        // Create a new DockItem
        DockItem* item = new DockItem();

        item->m_groupname = groupname;
        item->m_realgroupname = realgroupname;
        item->m_appname = appname;
        item->m_instancename = instancename;
        item->m_window = window;
        item->m_xid = wnck_window_get_xid(window);
        item->m_image = appIcon;

        item->m_items->push_back(item);
        _itemsvector->push_back(item);

    } else // remove item
    {
        // find the item to remove;
        int xid = wnck_window_get_xid(window);
        int i = 1;
        bool found = false;
        for (; i < _itemsvector->size(); i++) {
            for (auto ci : *_itemsvector->at(i)->m_items) {
                if (ci->m_xid == xid) {
                    found = true;
                    break;
                }
            }
            if (found)
                break;
        }

        if (xid > 0 && found) {

            auto item = _itemsvector->at(i);
            if (item->m_items->size() == 1) {
                // is dirty!
                if (item->m_isFixed == 1) {
                    item->m_isDirty = true;

                    item->m_items->erase(item->m_items->begin());
                    return;
                }

                item->m_window = NULL;
                item->m_xid = 0;
                item->m_image = (Glib::RefPtr<Gdk::Pixbuf>)NULL;

                // remove this item
                _itemsvector->erase(_itemsvector->begin() + i);
                return;

            } else {
                // search for the xid and remove the item
                int idx = 0;
                for (auto c : *item->m_items) {
                    if (c->m_xid == xid) {
                        item->m_items->erase(item->m_items->begin() + idx);
                        return;
                    }

                    idx++;
                }

            }

        }
    }

}

/* void DockPanel::get_FramesPerSecond()
 * 
 * calculates Frames Per Second
 * 
 */
void DockPanel::get_FramesPerSecond()
{
    ++frames;
    last_time += m_fpstimer.elapsed();

    if (last_time >= 0) {
        _curFPS = (float) (frames / last_time);
        frames = 0;
        last_time = 0;
        m_fpstimer.reset();
    }

    //  g_print("FPS: %d\n", (int)_curFPS);
}

/*
 * 
 * draw the cairo content
 * 
 */
bool DockPanel::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    Glib::RefPtr<Gdk::Pixbuf> icon;
    
    //FIXME: get the coordinates from the current monitor
    int center = this->monitor_geo.get_width() / 2;
    int col = center - (this->getCountItems() * DEF_CELLSIZE) / 2;

#ifdef DEBUG
    cr->set_source_rgb(1.0, 1.0, 1.0);
    cr->move_to(center, 0);
    cr->line_to(center, DEF_PANELHIGHT);
    cr->set_line_width(1);
    cr->stroke();

    cr->set_source_rgb(1.0, 1.0, 1.0);
    for (auto item : *_itemsvector) {
        if (item->m_image == (Glib::RefPtr<Gdk::Pixbuf>)NULL)
            continue;

        cr->rectangle(col, 0, DEF_CELLSIZE, DEF_PANELHIGHT);
        cr->set_line_width(1);
        cr->stroke();



        col += DEF_CELLSIZE;
    }
#endif

    int itemsCount = this->getCountItems();
    double width = (itemsCount * DEF_CELLSIZE) ;//+ DEF_CELLSIZE;
    double height = DEF_PANELBCKHIGHT;
    double radius = 2.0;
    int x = col = center - ((itemsCount * DEF_CELLSIZE) / 2) ;//- DEF_CELLSIZE / 2;
    int y = DEF_PANELBCKTOP;

    
    cr->set_source_rgba(0.0, 0.0, 0.8, 0.2); // partially translucent
    //cr->rectangle(x, y, width, height);
    Utilities::RoundedRectangle(cr, x, y, width, height, radius);
    cr->fill_preserve();
    cr->close_path();
    cr->stroke();
    cr->restore();
   
    if ((m_currentMoveIndex != -1 && m_mouseIn) ||
            (m_currentMoveIndex == m_selectedIndex &&
            m_preview->m_active && m_currentMoveIndex != -1)) {

        // rectangle background selector
        int pos_x = col + (DEF_CELLSIZE / 2) + (DEF_CELLSIZE * m_currentMoveIndex) - DEF_CELLSIZE / 2;
        int pos_y = DEF_PANELBCKTOP;
        int pos_width = DEF_CELLSIZE - 1;
        int pos_height = DEF_PANELBCKHIGHT; //- 12;

        cr->set_line_width(2);
        cr->set_source_rgba(1.0, 1.0, 1.0, 0.4); // partially translucent
        //cr->rectangle(pos_x, pos_y, pos_width, pos_height);
        Utilities::RoundedRectangle(cr, pos_x, pos_y, pos_width, pos_height, 2.0);
        cr->fill();

        // rectangle frame selector
        cr->set_source_rgba(1.0, 1.0, 1.0, 0.8); // partially translucent
        //cr->rectangle(pos_x, pos_y, pos_width, pos_height);
        Utilities::RoundedRectangle(cr, pos_x, pos_y, pos_width, pos_height, 2.0);
        cr->stroke();

    }


    cr->set_source_rgb(1.0, 1.0, 1.0);
    col = center - (itemsCount * DEF_CELLSIZE) / 2;
    int idx = 0;
    for (auto item : *_itemsvector) {
        if (item->m_image == (Glib::RefPtr<Gdk::Pixbuf>)NULL)
            continue;

        if (item->m_items->size() == 1) {
            cr->arc(col + (DEF_CELLSIZE / 2), DEF_PANELBCKHIGHT, 2, 0, 2 * M_PI);
            cr->fill();
        } else if (item->m_items->size() > 1) {
            cr->arc((col + (DEF_CELLSIZE / 2)) - 4, DEF_PANELBCKHIGHT, 2, 0, 2 * M_PI);
            cr->arc((col + (DEF_CELLSIZE / 2)) + 4, DEF_PANELBCKHIGHT, 2, 0, 2 * M_PI);
            cr->fill();
        }

        col += DEF_CELLSIZE;
        idx++;
    }

    col = center - (itemsCount * DEF_CELLSIZE) / 2;
    int idxc = 0;
    for (auto item : *_itemsvector) {
        if (item->m_image == (Glib::RefPtr<Gdk::Pixbuf>)NULL)
            continue;

        icon = item->m_image;

        if (m_currentMoveIndex == idxc && m_mouseIn) {
            // icon = PixbufConvert(dimm_icon(item->m_image->gobj()));
            //    icon = icon->scale_simple(64,64,Gdk::INTERP_BILINEAR);
            //cr->scale(200,200);
            //http://www.lucidarme.me/?p=4828
        }


        Gdk::Cairo::set_source_pixbuf(cr, icon, col + 5, DEF_OFFSE_TOP);
        cr->paint();



        int pos_x = col + (DEF_CELLSIZE / 2) - (DEF_CELLSIZE/2 );
        int pos_y = DEF_PANELBCKTOP;
        int pos_width = DEF_CELLSIZE - 1;
        int pos_height = DEF_PANELBCKHIGHT; //- 12;

        cr->set_source_rgba(1.0, 1.0, 1.0, 0.8); // partially translucent
       cr->set_line_width(0.7);
        Utilities::RoundedRectangle(cr, pos_x, pos_y, pos_width, pos_height, 2.0);
        cr->stroke();

     
        col += DEF_CELLSIZE;
        idxc++;
    }

    get_FramesPerSecond();

}

/* Glib::RefPtr<Gdk::Pixbuf> DockPanel::GetWindowIcon(WnckWindow * window)
 * 
 * Get the window icons as Gdk::Pixbuf.
 * 
 */
Glib::RefPtr<Gdk::Pixbuf> DockPanel::GetWindowIcon(WnckWindow * window)
{
    Glib::RefPtr<Gdk::Pixbuf> result;
    GdkPixbuf *icon = NULL;
    GError *error = NULL;
   
    std::string iconname = wnck_window_get_class_instance_name(window);

    if (iconname.empty())
        iconname = wnck_window_get_icon_name(window);

    if (iconname.empty())
        return Gdk::Pixbuf::create_from_file(Utilities::getExecPath("home.ico"), //TODO:: load an empty icon
            DEF_CELLSIZE, DEF_CELLSIZE, true);

    const std::string extensions[] = {".py", ".exe"};
    iconname = Utilities::removeExtension(iconname, extensions);
    std::string lowerName = Utilities::stringToLower(iconname.c_str());

    GtkIconTheme *icon_theme = gtk_icon_theme_get_default();
    if (gtk_icon_theme_has_icon(icon_theme, lowerName.c_str())) {

        icon = gtk_icon_theme_load_icon(icon_theme, lowerName.c_str(),
                64,
                GTK_ICON_LOOKUP_GENERIC_FALLBACK, &error);
    } else {

        // give the icon with lower resolution.
        icon = wnck_window_get_icon(window);
    }

    if (icon == NULL)
        return Gdk::Pixbuf::create_from_file(Utilities::getExecPath("home.ico"), //TODO:: load an empty icon
            DEF_CELLSIZE, DEF_CELLSIZE, true);

    result = PixbufConvert(icon);

    g_object_unref(icon);
    return result;

}

Glib::RefPtr<Gdk::Pixbuf> DockPanel::PixbufConvert(GdkPixbuf * icon)
{
    Glib::RefPtr<Gdk::Pixbuf> result;

    int width = gdk_pixbuf_get_width(icon);
    int height = gdk_pixbuf_get_height(icon);
    int stride = gdk_pixbuf_get_rowstride(icon);
    gboolean hasalpha = gdk_pixbuf_get_has_alpha(icon);
    int bitsaple = gdk_pixbuf_get_bits_per_sample(icon);

    guint8 *pdata;
    guchar *pixels = gdk_pixbuf_get_pixels(icon);

    pdata = static_cast<guint8*> (pixels);

    result = Gdk::Pixbuf::create_from_data(pdata,
            Gdk::COLORSPACE_RGB, hasalpha, bitsaple, width, height, stride);

    return result;
}

/* borrowed from libwnck selector.c */
GdkPixbuf * DockPanel::dimm_icon(GdkPixbuf * pixbuf)
{
    int x, y, pixel_stride, row_stride;
    guchar *row, *pixels;
    int w, h;
    GdkPixbuf *dimmed;

    w = gdk_pixbuf_get_width(pixbuf);
    h = gdk_pixbuf_get_height(pixbuf);

    if (gdk_pixbuf_get_has_alpha(pixbuf))
        dimmed = gdk_pixbuf_copy(pixbuf);
    else
        dimmed = gdk_pixbuf_add_alpha(pixbuf, FALSE, 0, 0, 0);

    pixel_stride = 4;

    row = gdk_pixbuf_get_pixels(dimmed);
    row_stride = gdk_pixbuf_get_rowstride(dimmed);

    for (y = 0; y < h; y++) {
        pixels = row;
        for (x = 0; x < w; x++) {
            pixels[3] /= 2;
            pixels += pixel_stride;
        }
        row += row_stride;
    }

    return dimmed;
}

/*
 * 
 * Focus the Window. 
 * 
 */
void DockPanel::FocusWindow(WnckWindow * window)
{
    WnckScreen *screen = wnck_screen_get_default();
    WnckWorkspace *active_workpace;
    WnckWorkspace *screen_workpace;

    active_workpace = wnck_window_get_workspace(window);
    screen_workpace = wnck_screen_get_workspace(screen, 0);

    int ct = gtk_get_current_event_time();

    if (active_workpace != NULL && screen_workpace != active_workpace)
        wnck_window_activate(window, (guint32) ct);


    if (wnck_window_is_minimized(window))
        wnck_window_unminimize(window, (guint32) ct);

    wnck_window_activate_transient(window, (guint32) ct);

}
