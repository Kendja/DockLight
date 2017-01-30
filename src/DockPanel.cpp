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
#include "WindowControl.h"


#include  <glibmm/i18n.h>
#include <gtkmm/window.h>
#include <gtkmm/messagedialog.h>
#include <gdkmm/cursor.h>

#include <limits.h>



// static members
std::vector<DockItem*> DockPanel::m_dockitems;
int DockPanel::m_currentMoveIndex;
bool DockPanel::m_previewWindowActive;
bool DockPanel::m_dragdropsStarts;

DockPanel::DockPanel() :
m_titleElapsedSeconds(0),
m_mouseIn(FALSE),
m_mouseRightClick(FALSE),
m_mouseLeftButtonDown(FALSE),
m_mouseRightButtonDown(FALSE),
m_cellwidth(DEF_CELLWIDTH),
m_iconsize(DEF_ICONSIZE),
m_previousCellwidth(DEF_CELLWIDTH),
m_cellheight(DEF_CELLHIGHT),
m_popupMenuOn(false),
m_launcherWindow(nullptr),
m_mouseclickEventTime(0),
m_dragdropTimerSet(false),
m_dragdropMouseDown(false),
m_dragdropItemIndex(-1),
m_applicationpath(Utilities::getExecPath()),
m_applicationDatapath(m_applicationpath + "/" + DEF_DATADIRNAME),
m_applicationAttachmentsPath(m_applicationpath + "/" + DEF_ATTACHMENTDIR),
m_homeiconFilePath(Utilities::getExecPath(DEF_ICONNAME))
{

    DockPanel::m_dragdropsStarts = false;
    DockPanel::m_currentMoveIndex = -1;

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
int DockPanel::preInit(Gtk::Window* window, bool autohide)
{
    // assumes that the "docklight.home.ico" exists.
    const char* filename = m_homeiconFilePath.c_str();
    DockItem* dockItem = new DockItem();
    try {
        dockItem->m_image = Gdk::Pixbuf::create_from_file(filename,
                DEF_ICONSIZE, DEF_ICONSIZE, true);
    } catch (Glib::FileError fex) {
        g_critical("preInit: file %s could not be found!\n", filename);
        return -1;

    } catch (Gdk::PixbufError bex) {
        g_critical("preInit: file %s PixbufError!\n", filename);
        return -1;
    }


    dockItem->m_appname = _("Desktop");
    dockItem->m_realgroupname = _("Desktop");
    m_dockitems.push_back(dockItem);

    m_previewWindowActive = false;
    m_preview.setOwner(*this);


    if (loadAttachments() != 0)
        return -1;


    // Menus
    // Home Menu items
    m_AutohideMenuItem.set_label(_("Autohide"));
    m_AutohideMenuItem.set_active(autohide);
    m_AutohideMenuItem.signal_toggled().
            connect(sigc::mem_fun(*this, &DockPanel::on_AutohideToggled_event));

    m_HomeUnMinimizeAllWindowsMenuItem.set_label(_("Show all"));
    m_HomeUnMinimizeAllWindowsMenuItem.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_HomeUnMinimizeAllWindows_event));

    m_HomeMinimizeAllWindowsExceptActiveMenuItem.set_label(_("Minimize all except active"));
    m_HomeMinimizeAllWindowsExceptActiveMenuItem.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_HomeMinimizeAllWindowsExceptActive_event));


    m_HomeMinimizeAllWindowsMenuItem.set_label(_("Minimize all"));
    m_HomeMinimizeAllWindowsMenuItem.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_HomeMinimizeAllWindows_event));

    m_HomeCloseAllWindowsExceptActiveMenuItem.set_label(_("Close all except active"));
    m_HomeCloseAllWindowsExceptActiveMenuItem.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_HomeCloseAllWindowsExceptActive_event));


    m_HomeCloseAllWindowsMenuItem.set_label(_("Close all"));
    m_HomeCloseAllWindowsMenuItem.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_HomeCloseAllWindows_event));

    m_HomeMenu_Popup.append(m_AutohideMenuItem);
    m_HomeMenu_Popup.append(m_separatorMenuItem0);
    m_HomeMenu_Popup.append(m_HomeUnMinimizeAllWindowsMenuItem);
    m_HomeMenu_Popup.append(m_separatorMenuItem5);
    m_HomeMenu_Popup.append(m_HomeMinimizeAllWindowsExceptActiveMenuItem);
    m_HomeMenu_Popup.append(m_HomeMinimizeAllWindowsMenuItem);

    m_HomeMenu_Popup.append(m_separatorMenuItem4);
    m_HomeMenu_Popup.append(m_HomeCloseAllWindowsExceptActiveMenuItem);
    m_HomeMenu_Popup.append(m_HomeCloseAllWindowsMenuItem);
    m_HomeMenu_Popup.append(m_separatorMenuItem1);

    m_HomeMenu_Popup.signal_deactivate().
            connect(sigc::mem_fun(*this, &DockPanel::on_MenuDeactivated_event));

    // m_HomeMenu_Popup.signal_enter_notify_event().
    //        connect(sigc::mem_fun(*this, &DockPanel::on_MenuEnterNotify_event());

    m_HelpMenuItem.set_label(_("Help"));
    m_HelpMenuItem.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_HelpMenu_event));


    m_AboutMenuItem.set_label(_("About"));
    m_AboutMenuItem.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_AboutMenu_event));

    m_QuitMenuItem.set_label(_("Quit"));
    m_QuitMenuItem.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_QuitMenu_event));


    m_HomeMenu_Popup.append(m_HelpMenuItem);
    m_HomeMenu_Popup.append(m_AboutMenuItem);
    m_HomeMenu_Popup.append(m_separatorMenuItem8);
    m_HomeMenu_Popup.append(m_QuitMenuItem);
    m_HomeMenu_Popup.show_all();
    m_HomeMenu_Popup.accelerate(*this);


    // Item Menu
    m_MenuItemNewApp.set_label(_("Open new"));
    m_MenuItemNewApp.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_menuNew_event));
    m_Menu_Popup.append(m_MenuItemNewApp);

    m_Menu_Popup.append(m_separatorMenuItem2);

    m_MenuItemAttach.set_label(_("Attach to Dock"));
    m_MenuItemAttach.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_AttachToDock_event));
    m_Menu_Popup.append(m_MenuItemAttach);

    m_MenuItemDetach.set_label(_("Detach from Dock"));
    m_MenuItemDetach.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_DetachFromDock_event));
    m_Menu_Popup.append(m_MenuItemDetach);

    m_Menu_Popup.append(m_separatorMenuItem3);

    m_MenuItemUnMinimizedAll.set_label(_("Show all"));
    m_MenuItemUnMinimizedAll.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_UnMinimieAll_event));

    m_MenuItemMinimizedAllExceptActive.set_label(_("Minimize all except active"));
    m_MenuItemMinimizedAllExceptActive.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_MinimieAllExceptActive_event));

    m_MenuItemMinimizedAll.set_label(_("Minimize all"));
    m_MenuItemMinimizedAll.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_MinimieAll_event));

    m_Menu_Popup.append(m_MenuItemUnMinimizedAll);

    m_Menu_Popup.append(m_separatorMenuItem7);
    m_Menu_Popup.append(m_MenuItemMinimizedAllExceptActive);
    m_Menu_Popup.append(m_MenuItemMinimizedAll);

    m_MenuItemCloseAllExceptActive.set_label(_("Close all except active"));
    m_MenuItemCloseAllExceptActive.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_CloseAllExceptActive_event));

    m_MenuItemCloseAll.set_label(_("Close all"));
    m_MenuItemCloseAll.signal_activate().
            connect(sigc::mem_fun(*this, &DockPanel::on_CloseAll_event));


    m_Menu_Popup.append(m_separatorMenuItem6);
    m_Menu_Popup.append(m_MenuItemCloseAllExceptActive);
    m_Menu_Popup.append(m_MenuItemCloseAll);

    m_Menu_Popup.signal_deactivate().
            connect(sigc::mem_fun(*this, &DockPanel::on_MenuDeactivated_event));

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

    m_TimeoutConnection = Glib::signal_timeout().connect(sigc::mem_fun(*this,
            &DockPanel::on_timeoutDraw), DEF_FRAMERATE);

    // Gets the default WnckScreen on the default display.
    WnckScreen *wnckscreen = wnck_screen_get_default();

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
    m_popupMenuOn = false;
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
    m_infowindow.hide();
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

//bool DockPanel::on_MenuEnterNotify_event(GdkEventCrossing* crossing_event)
//{
//
//}

void DockPanel::on_MenuDeactivated_event()
{
    m_popupMenuOn = false;
}

bool DockPanel::ispopupMenuActive()
{

    return m_popupMenuOn || m_previewWindowActive;

}

void DockPanel::dropDockItem(GdkEventButton *event)
{
    int relativeMouseX = DockPosition::getDockItemRelativeMouseXPos(
            (int) m_dockitems.size(), m_currentMoveIndex,
            m_cellwidth, (int) event->x);

    int dropIndex = m_currentMoveIndex;
    if (relativeMouseX < (DEF_CELLWIDTH / 2)) {

        dropIndex = m_currentMoveIndex > m_dragdropItemIndex ?
                m_currentMoveIndex - 1 : m_currentMoveIndex;

    }
    if (relativeMouseX > (DEF_CELLWIDTH / 2)) {


        dropIndex = m_currentMoveIndex > m_dragdropItemIndex ?
                m_currentMoveIndex : m_currentMoveIndex + 1;


    }

    std::vector<DockItem*> tmp(m_dockitems.begin() +
            m_dragdropItemIndex, m_dockitems.begin() + m_dragdropItemIndex + 1);

    m_dockitems.erase(m_dockitems.begin() +
            m_dragdropItemIndex, m_dockitems.begin() + m_dragdropItemIndex + 1);

    m_dockitems.insert(m_dockitems.begin() + dropIndex, tmp.begin(), tmp.end());

    g_print("Drop from %d to %d \n", m_dragdropItemIndex, dropIndex);
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


        // Set Drag and drop variables and Starts the timer
        if (event->button == 1 && !m_dragdropTimerSet && m_currentMoveIndex > 0) {

            // remember the item we want to drag
            m_dragdropItemIndex = m_currentMoveIndex;
            int relativex = DockPosition::getDockItemRelativeMouseXPos(
                    (int) m_dockitems.size(), m_dragdropItemIndex,
                    m_cellwidth, (int) event->x);

            // sets the relative item mouse coordinates         
            m_dragdropMousePoint.set_x(relativex);
            m_dragdropMousePoint.set_y((int) event->y);

            // Variables are set 
            m_dragdropTimerSet = true;
            // Start the timer
            m_dragdropMouseDown = true;
            m_dragdropTimer.start();
            g_print("Drag timer starts\n");
        }


        // remember the time to check it later 
        m_mouseclickEventTime = gtk_get_current_event_time();

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
    // Check if a item was drop

    if (m_dragdropMouseDown) {
        m_dragdropsStarts = false;
        m_dragdropTimerSet = false;
        m_dragdropMouseDown = false;

        m_dragDropWindow.Hide();
        m_dragdropTimer.stop();
        g_print("Reset Darg And Drop \n");
        if (m_dragdropItemIndex != m_currentMoveIndex && m_currentMoveIndex > 0) {

            g_print("Drag %d Drop %d\n", m_dragdropItemIndex, m_currentMoveIndex);
            dropDockItem(event);

            //            int idx = 0;
            //            for(DockItem* item: m_dockitems){
            //                
            //                if( item->m_isAttached) {
            //                  g_print("%d %s \n",idx++,item->m_instancename.c_str());
            //                }
            //                
            //            }
        }

    }


    // Taking Too Long To Release the mouse.
    // That is not a valid Click.
    if ((gtk_get_current_event_time() - m_mouseclickEventTime) > 200) {
        m_mouseLeftButtonDown = false;
        m_mouseRightButtonDown = false;
        return true;
    }

    if (!m_mouseIn)
        return true;




    if (m_mouseLeftButtonDown) {

        //  m_selectedIndex = m_currentMoveIndex;
        SelectWindow(m_currentMoveIndex, event);
        m_mouseLeftButtonDown = false;

        // Returning TRUE means we handled the event, so the signal
        // emission should be stopped (don’t call any further callbacks
        // that may be connected). Return FALSE to continue invoking callbacks.
        return TRUE;

    }

    if (m_mouseRightButtonDown) {

        m_preview.hideMe();
        m_mouseRightClick = true;
        m_mouseRightButtonDown = false;


        // Items 
        if (m_currentMoveIndex > 0) {
            DockItem *dockitem = m_dockitems.at(m_currentMoveIndex);
            int isExitstWindows =
                    WindowControl::isExitstWindowsByDockItem(dockitem);
            bool isExitstActiveWindow =
                    WindowControl::isExitsActivetWindowByDockItem(dockitem);
            bool maximizedexistst =
                    WindowControl::isExistsUnMaximizedWindowsByDockItem(dockitem);
            //bool minizedexitst =
            //        WindowControl::isExistsMinimizedWindowsByDockItem(dockitem);


            m_MenuItemMinimizedAll.set_sensitive(isExitstWindows > 0 && maximizedexistst);

            m_MenuItemMinimizedAllExceptActive.set_sensitive(isExitstWindows > 1
                    && maximizedexistst && isExitstActiveWindow);
            m_MenuItemUnMinimizedAll.set_sensitive(isExitstWindows);


            m_MenuItemCloseAllExceptActive.set_sensitive(isExitstWindows > 1 && isExitstActiveWindow);
            m_MenuItemCloseAll.set_sensitive(isExitstWindows);

            m_MenuItemAttach.set_sensitive(dockitem->m_isAttached == false);
            m_MenuItemDetach.set_sensitive(dockitem->m_isAttached &&
                    dockitem->m_items.size() == 0);


            if (!m_Menu_Popup.get_attach_widget())
                m_Menu_Popup.attach_to_widget(*this);


            //m_HomeMenu_Popup.resize_children();
            m_Menu_Popup.set_halign(Gtk::Align::ALIGN_CENTER);
            m_Menu_Popup.popup(sigc::mem_fun(*this,
                    &DockPanel::on_popup_menu_position), 1, event->time);

            m_popupMenuOn = true;
            return true;
        }
        //Home
        if (m_currentMoveIndex == 0) {

            int wincount = WindowControl::windowscount();
            //int minimizedexitst = WindowControl::isExistsMinimizedWindows();
            int unminimized = WindowControl::unMinimizedWindowsCount();

            m_HomeCloseAllWindowsMenuItem.set_sensitive(wincount > 0);
            m_HomeCloseAllWindowsExceptActiveMenuItem.set_sensitive(wincount > 1);
            m_HomeMinimizeAllWindowsMenuItem.set_sensitive(unminimized > 0);
            m_HomeUnMinimizeAllWindowsMenuItem.set_sensitive(wincount > 0);

            m_HomeMinimizeAllWindowsExceptActiveMenuItem.set_sensitive(unminimized > 1);

            if (!m_HomeMenu_Popup.get_attach_widget())
                m_HomeMenu_Popup.attach_to_widget(*this);

            //m_HomeMenu_Popup.resize_children();
            m_HomeMenu_Popup.set_halign(Gtk::Align::ALIGN_CENTER);
            m_HomeMenu_Popup.popup(sigc::mem_fun(*this,
                    &DockPanel::on_popup_homemenu_position), 1, event->time);

            m_popupMenuOn = true;

            return true;
        }

        // Returning TRUE means we handled the event, so the signal
        // emission should be stopped (don’t call any further callbacks
        // that may be connected). Return FALSE to continue invoking callbacks.
        return TRUE;
    }

    // Returning TRUE means we handled the event, so the signal
    // emission should be stopped (don’t call any further callbacks
    // that may be connected). Return FALSE to continue invoking callbacks.
    return TRUE;
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

    x = MonitorGeometry::getGeometry().x + col + (m_cellwidth / 2) +
            (m_cellwidth * m_currentMoveIndex);
    y = DockPosition::getHomeMenuTopPosition();
}

void DockPanel::on_popup_menu_position(int& x, int& y, bool& push_in)
{
    int center = (MonitorGeometry::getGeometry().width / 2) -
            (m_Menu_Popup.get_allocated_width() / 2);

    int col = center - (m_dockitems.size() * m_cellwidth) / 2;

    x = MonitorGeometry::getGeometry().x + col + (m_cellwidth / 2) +
            (m_cellwidth * m_currentMoveIndex);
    y = DockPosition::getItemMenuTopPosition();
}

void DockPanel::on_HelpMenu_event()
{

}

void DockPanel::on_AboutMenu_event()
{
    m_about.show(m_AppWindow);
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

    DockItem* item = m_dockitems.at(index);

    if (!Launcher::Launch(item)) {
        createLauncher(item);
    }

}

int DockPanel::getAttachedOrderIndex()
{

    int idx = 0;
    int count = 0;

    for (DockItem* item : m_dockitems) {
        if (!item->m_isAttached)
            continue;
        
        idx = item->m_attachedIndex;
        
        if( count >= m_currentMoveIndex){
            g_print("HERE SWAP\n");
            break;
        }
        
        count++;
    }
    
    return idx + 1;
 
}

void DockPanel::on_AttachToDock_event()
{
    if (m_currentMoveIndex < 1)
        return;

    
    int a = getAttachedOrderIndex();
            
    return;
    
    
    DockItem * dockitem = m_dockitems.at(m_currentMoveIndex);

    if (dockitem->m_isAttached)
        return; // already attached
    

    char filename[NAME_MAX];
    std::string s = dockitem->m_realgroupname;
    std::replace(s.begin(), s.end(), ' ', '_'); // replace all ' ' to '_'
    sprintf(filename, "%s/%2d_%s.png", m_applicationAttachmentsPath.c_str(),
            getAttachedOrderIndex(),
            s.c_str());

    dockitem->m_isAttached = true;
    dockitem->m_isDirty = true;
    dockitem->m_image->save(filename, "png");
    
    
    g_print("DockItem is now Attached for index : %d\n", m_currentMoveIndex);

}

void DockPanel::on_DetachFromDock_event()
{


    if (m_currentMoveIndex < 0)
        return;

    DockItem * dockitem = m_dockitems.at(m_currentMoveIndex);

    if (!dockitem->m_isAttached)
        return;

    // can't detach if still open
    if (dockitem->m_items.size() > 0)
        return;

    char filename[PATH_MAX];

    std::string s = dockitem->m_realgroupname;
    std::replace(s.begin(), s.end(), ' ', '_'); // replace all ' ' to '_'
    sprintf(filename, "%s//%s.png", m_applicationAttachmentsPath.c_str(), s.c_str());

    //g_print(" Remove path %s n", m_applicationAttachmentsPath.c_str());

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

void DockPanel::on_CloseAllExceptActive_event()
{
    if (m_currentMoveIndex < 0)
        return;

    DockItem * dockitem = m_dockitems.at(m_currentMoveIndex);
    WindowControl::closeAllExceptActiveByDockItem(dockitem);
}

void DockPanel::on_CloseAll_event()
{
    if (m_currentMoveIndex < 0)
        return;

    DockItem * dockitem = m_dockitems.at(m_currentMoveIndex);
    WindowControl::closeAllByDockItem(dockitem);
}

void DockPanel::on_UnMinimieAll_event()
{
    if (m_currentMoveIndex < 0)
        return;

    DockItem * dockitem = m_dockitems.at(m_currentMoveIndex);
    WindowControl::unMinimizeAllByDockItem(dockitem);
}

void DockPanel::on_MinimieAllExceptActive_event()
{
    if (m_currentMoveIndex < 0)
        return;

    DockItem * dockitem = m_dockitems.at(m_currentMoveIndex);
    WindowControl::minimizeAllExceptActiveByDockItem(dockitem);
}

void DockPanel::on_MinimieAll_event()
{
    if (m_currentMoveIndex < 0)
        return;

    DockItem * dockitem = m_dockitems.at(m_currentMoveIndex);
    WindowControl::minimizeAllByDockItem(dockitem);

}

void DockPanel::on_AutohideToggled_event()
{
    //discover the new state.
    bool autohide = m_AutohideMenuItem.get_active();
    DockPosition::setAutoHide(autohide);
    if (autohide)
        MonitorGeometry::RemoveStrut();

    MonitorGeometry::updateStrut();

}

void DockPanel::on_HomeMinimizeAllWindows_event()
{
    WindowControl::HomeMinimizeAll();
}

void DockPanel::on_HomeUnMinimizeAllWindows_event()
{
    WindowControl::HomeUnMinimizeAll();
}

void DockPanel::on_HomeMinimizeAllWindowsExceptActive_event()
{
    WindowControl::HomeMinimizeAllExceptActive();
}

void DockPanel::on_HomeCloseAllWindowsExceptActive_event()
{
    WindowControl::HomeCloseAllExceptActive();
}

void DockPanel::on_HomeCloseAllWindows_event()
{
    WindowControl::HomeCloseAllWindows();

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

    DockItem * item = m_dockitems.at(index)->GetNext();
    if (item == nullptr)
        return true;

    WnckWindow * itemWindow = item->m_window;
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

    if (!m_previewWindowActive && !m_dragdropsStarts && m_dragdropItemIndex > 0 &&
            m_dragdropTimerSet && m_dragdropTimer.elapsed() > 0.5) {
        m_dragdropsStarts = true;
        m_dragdropTimer.stop();
        DockItem* item = m_dockitems.at(m_dragdropItemIndex);

        m_dragDropWindow.Show(item->m_image, m_cellwidth, m_dragdropMousePoint);

    }


    Gtk::Widget::queue_draw();


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

    if (m_previewWindowActive || DockPanel::m_dragdropsStarts)
        return;

    WnckWindow * window = wnck_screen_get_active_window(screen);
    if (window == NULL) {
        m_currentMoveIndex = -1;

        return;
    }

    DockPanel::setItemImdexFromActiveWindow(window);
}

void DockPanel::setItemImdexFromActiveWindow(WnckWindow *window)
{
    if (DockPanel::m_dragdropsStarts)
        return;

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

    int cw;
    int iw;
    DockPosition::getDockItemGeometry(m_dockitems.size() + 1, cw, iw);

    if (iw < DEF_MINCONSIZE) {
        g_warning("there are to many dock Items. Please close some windows and try again.");
        //return;
    }


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

    std::string titlename =
            Launcher::getTitleNameFromDesktopFile(instancename, realgroupname);

    if (realgroupname == "Wine")
        realgroupname = instancename;


    if (instancename == DOCKLIGHT_INSTANCENAME)
        return;

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

    //g_print("m_iconsize %d\n", m_iconsize);

    // compute the cell height if need to resized
    if ((resizeNeeded && m_previousCellwidth != m_cellwidth)) {

        int iconrestsapce = DEF_CELLHIGHT - (DEF_ICONSIZE + DEF_ICONTOPCELLMARGIN);
        int substractpixels = (DEF_CELLHIGHT - (m_iconsize + DEF_ICONTOPCELLMARGIN)) - iconrestsapce;

        m_cellheight = DEF_CELLHIGHT - substractpixels;
        m_previousCellwidth = m_cellwidth;

        MonitorGeometry::updateStrut(m_AppWindow, DEF_PANELHIGHT - substractpixels);

        //g_print("%d %d %d\n", iconrestsapce, substractpixels, (int) m_titleTimer.elapsed());
    }


    int center = MonitorGeometry::getGeometry().width / 2;
    int col = center - (m_dockitems.size() * m_cellwidth) / 2;
    int pos_x = col = center - ((m_dockitems.size() * m_cellwidth) / 2);

    // Timer control for the title Window
    if (m_mouseIn && m_currentMoveIndex == -1) {
        m_titlewindow.hide();
        m_infowindow.hide();

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
            m_infowindow.hide();
            if (m_previewWindowActive)
                m_preview.hideMe();

        }

        if (m_titleItemOldindex == m_currentMoveIndex) {
            if (m_titleElapsedSeconds > 0.3 && m_titleShow == false && !m_previewWindowActive) {

                DockItem* item = m_dockitems.at(m_currentMoveIndex);
                std::string title = item->getTitle();

                if (item->m_items.size() > 1) {
                    char buff[NAME_MAX];
                    sprintf(buff, "%s (%d)", title.c_str(), (int) item->m_items.size());
                    title = buff;
                }

                m_titlewindow.setText(title);
                int centerpos = DockPosition::getDockItemCenterPos(
                        m_dockitems.size(),
                        m_currentMoveIndex,
                        m_titlewindow.get_width()
                        );

                m_titlewindow.move(centerpos, MonitorGeometry::getAppWindowTopPosition() - 30);
                m_titleShow = true;

            }

            m_titleElapsedSeconds = m_titleTimer.elapsed();
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
        double radius = 2.0;
        int margin = 4;

        if (m_iconsize <= 25) {
            radius = 1.5;
            margin = 3;
        }
        cr->set_source_rgb(1.0, 1.0, 1.0);
        if (item->m_items.size() == 1) {
            cr->arc(col + (m_cellwidth / 2), m_cellheight, radius, 0, 2 * M_PI);
        } else if (item->m_items.size() > 1) {
            cr->arc((col + (m_cellwidth / 2)) - margin, m_cellheight, radius, 0, 2 * M_PI);
            cr->arc((col + (m_cellwidth / 2)) + margin, m_cellheight, radius, 0, 2 * M_PI);
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

    DockItem* dockitem = m_dockitems.at(index);
    int itemscount = dockitem->m_items.size();

    if (itemscount == 0 && dockitem->m_isAttached) {
        if (!Launcher::Launch(dockitem)) {
            createLauncher(dockitem);
        }

        return;
    }

    if (m_dockitems.at(index)->m_window == NULL)
        return;

    int previewWidth = DEF_PREVIEW_WIDTH;
    int previewHeight = DEF_PREVIEW_HEIGHT;

    DockPosition::getPreviewItemGeometry(itemscount, previewWidth, previewHeight);

    if (previewHeight < DEF_PREVIEW_MINHEIGHT) {

        // TODO: add text to a string resource
        char message[PATH_MAX];
        sprintf(message, _("(%s) %d windows.\nthere are to many windows open for a Preview.\nClose some windows and try again."),
                dockitem->getTitle().c_str(), itemscount);

        m_infowindow.setText(message);
        int centerpos = DockPosition::getDockItemCenterPos(
                (int) m_dockitems.size(),
                m_currentMoveIndex,
                m_infowindow.get_width()
                );

        m_infowindow.move(centerpos,
                MonitorGeometry::getAppWindowTopPosition() - 90);

        return;
    }

    m_previewWindowActive = true;
    m_preview.Activate(dockitem, (int) m_dockitems.size(), index);
}

int DockPanel::loadAttachments()
{

    DIR* dirFile = opendir(m_applicationAttachmentsPath.c_str());
    if (dirFile == 0) {
        if (ENOENT == errno)
            g_critical("Error loading attachments: Directory does not exist.\n");

        return -1;
    }

    struct dirent* hFile;
    errno = 0;
    while ((hFile = readdir(dirFile)) != NULL) {
        if (!strcmp(hFile->d_name, ".")) continue;
        if (!strcmp(hFile->d_name, "..")) continue;
        if ((hFile->d_name[0] == '.')) continue; // in linux hidden files all start with '.'

        if (strstr(hFile->d_name, ".png")) {

            std::string filename = hFile->d_name;
            std::string imageFilePath = std::string(m_applicationAttachmentsPath) +
                    std::string("/") + filename;

            std::vector<std::string> tokens = Utilities::split(filename, '_');
            std::size_t found = filename.find_last_of(".");
            if (found < 1)
                continue;

            std::string appname = filename.substr(0, found);
            std::replace(appname.begin(), appname.end(), '_', ' ');

            std::string index = appname.substr(0, 2);
            appname = appname.substr(3, appname.length() - 3);

            int idx;
            try {
                idx = std::stoi(index.c_str());

            } catch (std::invalid_argument fex) {
                g_critical("loadAttachments: std::invalid_argument\n");
                return -1;

            } catch (std::out_of_range) {
                g_critical("loadAttachments: std::out_of_range\n");
                return -1;
            }

            std::string titlename =
                    Launcher::getTitleNameFromDesktopFile(appname);

            DockItem * item = new DockItem();
            item->m_appname = appname;
            item->m_attachedIndex = idx;
            item->m_instancename = Utilities::stringToLower(appname.c_str());
            item->m_realgroupname = appname;
            item->m_titlename = titlename;
            item->m_window = NULL;
            item->m_xid = 0;
            item->m_image = NULLPB;


            try {

                item->m_image = Gdk::Pixbuf::create_from_file(imageFilePath,
                        DEF_ICONSIZE, DEF_ICONSIZE, true);

            } catch (Glib::FileError fex) {
                g_critical("Attachment file not found %s\n", appname.c_str());
                return -1;

            } catch (Gdk::PixbufError bex) {
                g_critical("Attachment file PixbufError %s\n", appname.c_str());
                return -1;
            }


            item->m_isAttached = true;
            item->m_isDirty = true;


            m_dockitems.push_back(item);
        }
        //sort
        // Sort by index
        int size = (int) m_dockitems.size();
        int i, m, j;

        for (i = 0; i < size - 1; i = i + 1) {
            m = i;
            for (j = i + 1; j < size; j = j + 1) {

                int a = m_dockitems.at(j)->m_attachedIndex;
                int b = m_dockitems.at(m)->m_attachedIndex;

                if (a < b) {
                    m = j;
                }
            }
            std::swap(m_dockitems.at(i), m_dockitems.at(m));
        }
    }
    closedir(dirFile);
    return 0;
}

void DockPanel::createLauncher(DockItem* item)
{
    if (m_launcherWindow == nullptr) {
        m_launcherWindow = new LauncherWindow(); // TODO: free space in dtor
        m_launcherWindow->init(*this, item);
        m_launcherWindow->show_all();
    }

}

