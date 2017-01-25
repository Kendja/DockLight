//*****************************************************************
//
//  Copyright (C) 2016 Juan R. Gonzalez
//  Created on December 22, 2016
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
#include "Preview.h"
#include "Defines.h"
#include "Utilities.h"
#include "DockPanel.h"
#include "IconLoader.h"
#include "MonitorGeometry.h"
#include "WindowControl.h"

#include <gdk/gdkx.h>

// Static members
std::vector<DockItem*> Preview::m_previewtems;
bool Preview::m_isActive;
std::string Preview::m_instancename;
int Preview::m_previewWidth;
int Preview::m_previewHeight;
Gtk::Window* Preview::m_thisInstance;
int Preview::m_dockItemIndex;
int Preview::m_dockItemsCount;
bool Preview::m_canLeave;
int Preview::m_currentIndex;
bool Preview::m_mouseIn;

/**
 * The “DockLight” preview allows more control over the opened windows you can 
 * navigate to a specific window much easier. Select the window or close the desired 
 * application, the real time preview make that very easy. 
 */
Preview::Preview()
: Gtk::Window(Gtk::WindowType::WINDOW_POPUP),
m_initialItemMax(0)
{
    m_mouseIn = false;
    m_canLeave = true;
    m_currentIndex = -1;
    m_thisInstance = (Gtk::Window*)this;
    m_dockItemIndex = -1;
    m_dockItemsCount = 0;
    m_previewWidth = DEF_PREVIEW_WIDTH;
    m_previewHeight = DEF_PREVIEW_HEIGHT;
    m_isActive = false;

    // Set up the top-level window.
    set_title("DockPreview");
    set_decorated(false);
    set_app_paintable(true);

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

    font.set_family("System");
    font.set_weight(Pango::WEIGHT_NORMAL);

    Glib::signal_timeout().
            connect(sigc::mem_fun(*this, &Preview::on_timeoutDraw), DEF_FRAMERATE);

    WnckScreen *wnckscreen = wnck_screen_get_default();

    g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
            G_CALLBACK(Preview::on_window_opened), NULL);

    g_signal_connect(wnckscreen, "window_closed",
            G_CALLBACK(Preview::on_window_closed), NULL);


    m_timer.start();

}

/**
 * dtor
 */
Preview::~Preview()
{
}

/**
 * Sets the caller pointer 
 * @param DockItem* item
 */
void Preview::init(DockItem* item/*, int &width, int &height, int &windowWidth*/)
{

    if (item == NULL)
        return;

    if (item->m_items.size() == 0)
        return;

    m_previewtems.clear();
    this->m_instancename = item->m_items.at(0)->m_instancename;

    for (DockItem* child : item->m_items) {

        if (child->m_window == NULL || child->m_xid == 0)
            continue;

        DockItem* newchild = new DockItem();

        //the name of window , or a fallback name if no name is available.
        newchild->m_titlename = wnck_window_get_name(child->m_window);
        newchild->m_window = child->m_window;
        newchild->m_xid = child->m_xid;

        m_previewtems.push_back(newchild);
    }

    m_currentIndex = 0;
    m_initialItemMax = (int) m_previewtems.size();

    // Sort by title name
    int size = (int) m_previewtems.size();
    int i, m, j ;

    for (i = 0; i < size - 1; i = i + 1) {
        m = i;
        for ( j = i + 1; j < size; j = j + 1) {

            std::string s1 = m_previewtems.at(j)->m_titlename.c_str();
            std::string s2 = m_previewtems.at(m)->m_titlename.c_str();

            s1 = s1.substr(0, 40);
            s2 = s2.substr(0, 40);

            std::string a = Utilities::stringToLower(s1.c_str());
            std::string b = Utilities::stringToLower(s2.c_str());

            if (a < b) {
                m = j;
            }
        }
        std::swap(m_previewtems.at(i), m_previewtems.at(m));
    }
}

void Preview::Activate(DockItem* item, int dockitemscount, int index)
{
    m_isActive = false;
    m_canLeave = false;

    init(item);

    m_dockItemIndex = index;
    m_dockItemsCount = dockitemscount;
    // update position

    // save the current window size
    m_previewWidth = DEF_PREVIEW_WIDTH;
    m_previewHeight = DEF_PREVIEW_HEIGHT;

    int itemsize = (int) m_previewtems.size();

    // read the new geometry for m_previewWidth, m_previewHeight
    DockPosition::getPreviewItemGeometry(itemsize, m_previewWidth, m_previewHeight);
    int windowWidth = (m_previewWidth * itemsize) + DEF_PREVIEW_RIGHT_MARGING;

    // Calculates the center position of the then window
    // resizes and center it.
    DockPosition::updatePreviewPosition(m_thisInstance,
            m_dockItemsCount,
            m_dockItemIndex,
            windowWidth,
            m_previewWidth,
            m_previewHeight);


    // After remove the window we set the index to 0. 
    m_currentIndex = 0;

    show_all();
    m_isActive = true;
    m_isVisible = true;

}

/**
 * handles on_enter_notify_event 
 * Event triggered by pointer enter widget area.
 * The signal will be emitted when the pointer enters the widget's window.
 * This signal will be sent to the grab widget if there is one.
 * 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 * @param crossing_event
 * @return true to stop other handlers from being invoked for the event. false to propagate the event further
 */
bool Preview::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    m_canLeave = true;
    m_mouseIn = true;
    m_dockpanelReference->m_previewWindowActive = true;
    return true;
}

/**
 * handles on_leave_notify_event 
 * Event triggered by pointer leaving widget area.
 * The signal_leave_notify_event() will be emitted when the pointer leaves the widget's window.
 * To receive this signal, the Gdk::Window associated to the widget needs to enable the Gdk::LEAVE_NOTIFY_MASK mask.
 * This signal will be sent to the grab widget if there is one.
 * 
 * @param crossing_event
 * @return true to stop other handlers from being invoked for the event. false to propagate the event further
 */
bool Preview::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    if (!m_canLeave) {
        return true;
    }

    this->hideMe();

    // tell the caller that we are leaving... 
    m_dockpanelReference->previewWindowClosed();
    return true;
}

/**
 * Hide the window and reset values;
 */
void Preview::hideMe()
{
    hide();

    m_canLeave = true;
    m_isActive = false;
    m_isVisible = false;
    m_mouseIn = false;
    m_dockpanelReference->m_previewWindowActive = false;

    // g_object_unref static items
    for (DockItem* item : m_previewtems) {
        if (item->m_scaledPixbuf != nullptr) {
            g_object_unref(item->m_scaledPixbuf);
        }
    }

    m_previewtems.clear();
}

/**
 * Timeout handler to regenerate the frame. 
 * force to redraw the entire content.
 * 
 * If  the flag m_canLeave is set to FALSE we need to check the mouseY coordinate.
 * If the mouseY value is less the the window height then we hide the Preview  
 */
bool Preview::on_timeoutDraw()
{
    if (!m_isVisible)
        return true;

    if (!m_canLeave) {

        int mouseX;
        int mouseY;

        if (Utilities::getMousePosition(mouseX, mouseY)) {
            if (mouseY < (MonitorGeometry::getAppWindowTopPosition() - m_previewHeight))
                this->hideMe();
        }
    }

    Gtk::Widget::queue_draw();
    return true;
}

/**
 * get the item index from mouse coordinates.
 * @param x
 * @param y
 * @return the DockItem index 
 */
int Preview::getIndex(int x, int y)
{

    int col = 0;
    int idx = 0;

    // g_print("%d %d \n",x,m_previewWidth + 16 );
    for (DockItem* item : m_previewtems) {
        if (item->m_window == NULL)
            continue;

        if (x > col && x < col + (m_previewWidth + 16))
            return idx;

        idx++;
        col += m_previewWidth;
    }

    return -1;
}

/**
 * handles the mouse scroll. 
 * @param GdkEventScroll event
 * @return true to stop other handlers,false to propagate the event further.
 */
bool Preview::on_scroll_event(GdkEventScroll * event)
{
    int count = m_previewtems.size();
    if (count == 0)
        return false;

    WnckWindow *itemWindow;
    int index = m_currentIndex;

    if ((int) event->delta_y == (int) 1) {
        index++;
    } else if ((int) event->delta_y == (int) - 1) {
        index--;
    }

    if (index < 0) {
        index = 0;
    } else if (index >= count) {
        index = count - 1;
    }

    m_currentIndex = index;

    itemWindow = m_previewtems.at(m_currentIndex)->m_window;
    WindowControl::ActivateWindow(itemWindow);

    // Event has been handled
    return true;
}

/**
 * If a window opens then we need to check if it is the same instance.
 * This can happen wen a Window open a dialog asking for cancel or continue.
 * In this case we need to hide the preview.
 * 
 * @param WnckScreen* screen
 * @param WnckWindow* window
 * @param gpointer data
 */
void Preview::on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data)
{
    if (!m_mouseIn)
        return;

    // there is no way to check the instance name or the Application name
    // they are often different...
    // So, we do the preview hiding for any window that arrives. 
    // it should work in the most cases...
    m_isActive = false;
    WindowControl::hideWindow(m_thisInstance);

}

/**
 * Handles window close.
 * Wen a Window gets close we remove it from the vector list.
 * calculate the new position and size and in case that the window size has change 
 * we resize all the windows ind the preview. 
 * 
 * @param WnckScreen screen
 * @param WnckWindow window
 * @param gpointer data
 */
void Preview::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data)
{

    if ((int) m_previewtems.size() == 0) {
        return;
    }

    // don't draw anything
    m_isActive = false;

    int idx = 0;
    int cxid = wnck_window_get_xid(window);
    for (idx = (int) m_previewtems.size() - 1; idx >= 0; idx--) {
        DockItem* item = m_previewtems.at(idx);
        if (cxid == item->m_xid) {
            m_previewtems.erase(m_previewtems.begin() + idx);
            break;
        }
    }

    m_canLeave = false;

    // save the current window size
    int oldresizeWidth = m_previewWidth;
    int oldresizeHeight = m_previewHeight;

    int itemsize = (int) m_previewtems.size();

    // read the new geometry for m_previewWidth, m_previewHeight
    DockPosition::getPreviewItemGeometry(itemsize, m_previewWidth, m_previewHeight);
    int windowWidth = (m_previewWidth * itemsize) + DEF_PREVIEW_RIGHT_MARGING;

    // Calculates the center position of the then window
    // resizes and center it.
    DockPosition::updatePreviewPosition(m_thisInstance,
            m_dockItemsCount,
            m_dockItemIndex,
            windowWidth,
            m_previewWidth,
            m_previewHeight);


    // After remove the window we set the index to 0. 
    m_currentIndex = 0;

    // in case that the window size has change we need to resize all the windows. 
    if (oldresizeHeight != m_previewHeight || oldresizeWidth != m_previewWidth) {

        for (DockItem* item : m_previewtems) {

            if (item->m_isDynamic)
                continue;

            item->m_imageLoadedRequired = true;
            item->m_timerStartSet = false;
            item->m_isDynamic = false;
        }
    }

    // OK we can use the render again
    m_isActive = true;

}

/**
 * handles Mouse button press : process mouse button event
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further.
 * @param event
 * @return true to stop other handlers,false to propagate the event further.
 * here we acivate the window and also close the window. 
 */
bool Preview::on_button_press_event(GdkEventButton *event)
{

    if ((event->type == GDK_BUTTON_PRESS)) {
        // Check if the event is a left button click.
        if (event->button == 1) {
            if (m_currentIndex < 0)
                return true;

            DockItem *item = m_previewtems.at(m_currentIndex);

            // Handle close preview window
            int pos_x = (m_previewWidth - 5) + (m_previewWidth * m_currentIndex);
            if (event->x >= pos_x && event->x <= pos_x + DEF_PREVIEW_LEFT_MARGING && // FIXTHIS: use rectangle instead.
                    event->y >= 19 && event->y <= 19 + DEF_PREVIEW_LEFT_MARGING) {

                wnck_window_close(item->m_window, gtk_get_current_event_time());
                m_isActive = false;

                return true;
            }

            // Activate and/or minimize the window
            WindowControl::ActivateWindow(item->m_window);

            // reload the image and checks if its have movement.
            // In this case it will change from static to Dynamic.
            // This can happen when a browser play a video and gets minimized and 
            // stops playing. When it gets unminimized should play again in the preview.
            if (!item->m_isDynamic) {
                item->m_timerStartSet = false;
                item->m_imageLoadedRequired = true;
            }

            // The event has been handled.
            return true;
        }
    }
    return true;
}

/**
 * handles on_motion_notify_event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 * @param event GdkEventMotion
 * @return true/false
 */
bool Preview::on_motion_notify_event(GdkEventMotion*event)
{
    m_currentIndex = getIndex(event->x, event->y);
    return true;
}

/**
 * Render the Preview
 * @param Cairo::Context cr
 * @return Gtk::Window::on_draw(cr)
 */
bool Preview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    cr->set_source_rgba(1.0, 1.0, 1.8, 0.8);
    cr->paint();

    cr->set_source_rgba(0.0, 0.0, 0.8, 0.4);
    cr->rectangle(0, 0, this->get_width(), this->get_height());
    cr->fill();
    cr->set_line_width(1.0);


    if (!m_isActive) {
        return Gtk::Window::on_draw(cr);
    }


    if (m_previewtems.size() < 1) {
        this->hideMe();
        return Gtk::Window::on_draw(cr);
    }



    int idx = 0;
    for (DockItem* item : m_previewtems) {
        if (item->m_window == NULL || item->m_xid == 0 || !item->visible)
            continue;

        int pos_x = 20 + (m_previewWidth * idx);
        int pos_y = 16;
        int pos_width = m_previewWidth - DEF_PREVIEW_LEFT_MARGING;
        int pos_height = 20;


        cr->set_line_width(1);
        cr->set_source_rgba(1.0, 1.0, 1.8, 0.2);
        if (item->m_imageLoadedRequired)
            cr->set_source_rgba(1.0, 1.0, 1.0, 1.2);

        Utilities::RoundedRectangle(cr,
                DEF_PREVIEW_LEFT_MARGING + (m_previewWidth * idx),
                16, m_previewWidth,
                m_previewHeight - DEF_PREVIEW_RIGHT_MARGING, 2.0);

        cr->stroke();


        // draw title the clipping rectangle
        cr->set_source_rgba(1.0, 1.0, 1.0, 0.0);
        cr->rectangle(pos_x, pos_y + 2, pos_width, pos_height);
        cr->clip_preserve();
        cr->stroke();

        auto layout = create_pango_layout(item->m_titlename);
        layout->set_font_description(font);
        cr->set_source_rgba(1.0, 1.0, 1.0, 1.0); // white text
        cr->move_to(pos_x, pos_y);
        layout->show_in_cairo_context(cr);
        cr->reset_clip(); // Reset the clipping 

        // selector
        if (m_currentIndex >= 0) {

            // rectangle background selector
            pos_x = DEF_PREVIEW_LEFT_MARGING + (m_previewWidth * m_currentIndex);
            pos_y = 16;
            pos_width = m_previewWidth + 1;
            pos_height = m_previewHeight - DEF_PREVIEW_RIGHT_MARGING;


            cr->set_source_rgba(1.0, 1.0, 1.8, 0.08);
            Utilities::RoundedRectangle(cr, pos_x, pos_y, pos_width, pos_height, 2.0);
            cr->fill();

            // rectangle frame selector
            cr->set_line_width(2);
            cr->set_source_rgba(1.0, 1.0, 1.0, 0.9);
            Utilities::RoundedRectangle(cr, pos_x, pos_y, pos_width, pos_height, 2.0);
            cr->stroke();


            // Close rectangle
            cr->set_source_rgba(1.0, 1.0, 1.0, 1);
            cr->set_source_rgba(0.337, 0.612, 0.117, 1.0); // green
            pos_x = (m_previewWidth - 5) + (m_previewWidth * m_currentIndex);
            cr->rectangle(pos_x, 18, DEF_PREVIEW_LEFT_MARGING, DEF_PREVIEW_LEFT_MARGING);
            cr->fill();
            cr->stroke();

            // Close X text
            cr->set_source_rgba(1.0, 1.0, 1.0, 1); //white
            cr->move_to(pos_x + 3, DEF_PREVIEW_RIGHT_MARGING - 1);
            cr->show_text("X");

        }

        if (item->m_imageLoadedRequired) {
            GdkPixbuf *scaledpb = getScaledPixbuf(item);

            if (scaledpb == nullptr) {
                continue;
            }

            // show the loaded image. 
            showPreview(cr, scaledpb, idx);

            if (!item->m_timerStartSet) {
                item->m_timerStartSet = true;
                item->m_timer.start();
            }

            // Checks if the image have movement.
            if (item->isMovementDetected(scaledpb) && !item->m_isDynamic) {
                item->m_isDynamic = true;
            }

            // if no movement has been detected, means that the image 
            // is static and we don't need to reload it again 
            if (item->m_timer.elapsed() > 0.2 && !item->m_isDynamic) {
                item->m_scaledPixbuf = scaledpb;
                // item->m_timerStartSet = false;
                item->m_timer.stop();
                item->m_imageLoadedRequired = false;
            }

            // if the image is static do not unreference the scaledpb.
            if (item->m_imageLoadedRequired)
                g_object_unref(scaledpb);

        } else {
            // show the loaded static image. 
            showPreview(cr, item->m_scaledPixbuf, idx);

        }

        idx++;
    }
    return Gtk::Window::on_draw(cr);
}

/**
 * Paint the scaledpb GdkPixbuf to the cairo context.
 * @param Cairo::Context cr
 * @param GdkPixbuf* scaledpb
 * @param the current idx
 */
void Preview::showPreview(const Cairo::RefPtr<Cairo::Context>& cr, GdkPixbuf* scaledpb, int idx)
{
    Glib::RefPtr<Gdk::Pixbuf> preview = IconLoader::PixbufConvert(scaledpb);
    Gdk::Cairo::set_source_pixbuf(cr, preview, (m_previewWidth * idx) +
            20, DEF_PREVIEW_PIXBUF_TOP);

    cr->paint();
}

/**
 * Transfers image data from a GdkWindow and converts it to an RGB(A) 
 * representation inside a GdkPixbuf. In other words, copies image data 
 * from a server-side drawable to a client-side RGB(A) buffer. 
 * This allows to efficiently read individual pixels on the client side.
 *  
 * Create a new GdkPixbuf containing a copy of src scaled to dest_width x dest_height . 
 * Leaves src unaffected. interp_type should be GDK_INTERP_NEAREST 
 * if you want maximum speed (but when scaling down GDK_INTERP_NEAREST is usually unusably ugly). 
 * The default interp_type should be GDK_INTERP_BILINEAR which offers reasonable quality and speed.
 * 
 * @param DockItem*  item
 * @return GdkPixbuf* 
 * 
 */
GdkPixbuf* Preview::getScaledPixbuf(DockItem* item)
{
    GdkWindow *wm_window = gdk_x11_window_foreign_new_for_display(
            gdk_display_get_default(), item->m_xid);
    GdkRectangle boundingbox;
    gdk_window_get_frame_extents(wm_window, &boundingbox);


    GdkPixbuf *pb = gdk_pixbuf_get_from_window(wm_window,
            0, 0, boundingbox.width - 10, boundingbox.height - 30);


    if (pb == NULL) {
        return nullptr;
    }

    int height = m_previewHeight - DEF_PREVIEW_SCALE_HEIGHT_OFFSET;
    int width = m_previewWidth - DEF_PREVIEW_SCALE_WIDTH_OFFSET;
    int scale_heght = height;

    int windowheight = gdk_window_get_height(wm_window);

    int heightHalf = (height / 2);

    if (windowheight < 300)
        scale_heght = heightHalf;
    if (windowheight < 200)
        scale_heght = heightHalf - 20;
    if (windowheight < 100)
        scale_heght = heightHalf - 40;
    if (windowheight < 50)
        scale_heght = heightHalf - 60;

    if (scale_heght < 10)
        scale_heght = height;

    // offers reasonable quality and speed.
    GdkPixbuf* scaledpb = gdk_pixbuf_scale_simple(pb, width, scale_heght,
            GDK_INTERP_BILINEAR);

    g_object_unref(pb);

    return scaledpb;

}
