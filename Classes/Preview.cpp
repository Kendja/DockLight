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

#include <gdk/gdkx.h>


// Static members
std::vector<DockItem*> Preview::m_previewtems;
bool Preview::m_isActive;
/**
 * Handles the Preview 
 * ctor
 */
Preview::Preview() :
Gtk::Window(Gtk::WindowType::WINDOW_POPUP),
m_mouseIn(false),
m_currentIndex(0),
m_initialItemMax(0),
m_previewWidth(DEF_PREVIEW_WIDTH),
m_previewHeight(DEF_PREVIEW_HEIGHT)
{
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
            connect(sigc::mem_fun(*this, &Preview::on_timeoutDraw), 1000 / 60);

    WnckScreen *wnckscreen = wnck_screen_get_default();

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
void Preview::init(DockItem* item, int &width, int &height, int &windowWidth)
{
    m_previewtems.clear();
    for (DockItem* child : item->m_items) {

        if (child->m_window == NULL || child->m_xid == 0)
            continue;

        DockItem* newchild = new DockItem();
       
        newchild->m_window = child->m_window;
        newchild->m_xid = child->m_xid;

        m_previewtems.push_back(newchild);
    }
      
    m_currentIndex = 0;
    int itemsize = m_previewtems.size() ;
    m_initialItemMax = itemsize;
          
    m_previewWidth = DEF_PREVIEW_WIDTH;
    m_previewHeight = DEF_PREVIEW_HEIGHT;
       
    DockPosition::getPreviewItemGeometry(itemsize,m_previewWidth,m_previewHeight );
    width = m_previewWidth;
    height = m_previewHeight;
    windowWidth = (m_previewWidth * itemsize ) + 30;
    
    //g_print("windowWidth %d\n ",windowWidth);
    
    resize(windowWidth, m_previewHeight);
    
    m_isActive = true;
}

/**
 * handles on_enter_notify_event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 * @param crossing_event
 * @return 
 */
bool Preview::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    m_mouseIn = true;
    m_dockpanelReference->m_previewWindowActive = true;
    return true;
}

/**
 * handles on_leave_notify_event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 * @param crossing_event
 * @return 
 */
bool Preview::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    m_isActive = false;
    this->hideMe();
    m_dockpanelReference->previewWindowClosed();

    return true;
}

/**
 * Hide the window and reset values;
 */
void Preview::hideMe()
{
    hide();
    m_mouseIn = false;
    m_dockpanelReference->m_previewWindowActive = false;
    m_previewtems.clear();
}

/**
 * Timeout handler to regenerate the frame. 
 * force to redraw the entire content.
 */
bool Preview::on_timeoutDraw()
{
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

    for (DockItem* item : m_previewtems) {
        if (item->m_window == NULL)
            continue;

        if (x > col && x <= col + (m_previewWidth + 10))
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
    wnck_window_activate(itemWindow, gtk_get_current_event_time());

    // Event has been handled
    return true;
}

/**
 * Handles window close.
 * 
 * @param WnckScreen screen
 * @param WnckWindow window
 * @param gpointer data
 */
void Preview::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data)
{
    m_isActive = false;
    int idx = 0;
    for (DockItem* item : m_previewtems) {
        int cxid = wnck_window_get_xid(window);
        if (cxid == item->m_xid) {
            m_previewtems.erase(m_previewtems.begin() + idx);
            break;
        }
        idx++;
    }

    m_isActive = true;
}

/**
 * handles Mouse button press : process mouse button event
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further.
 * @param event
 * @return true to stop other handlers,false to propagate the event further.
 */
bool Preview::on_button_press_event(GdkEventButton *event)
{

    if ((event->type == GDK_BUTTON_PRESS)) {
        // Check if the event is a left button click.
        if (event->button == 1) {

            if (m_currentIndex < 0)
                return true;

            DockItem *item = m_previewtems.at(m_currentIndex);
            int ct = gtk_get_current_event_time();

            // Handle close preview window
            int pos_x = (m_previewWidth - 5) + (m_previewWidth * m_currentIndex);
            if (event->x >= pos_x && event->x <= pos_x + 14 && // FIXTHIS: use rectangle instead.
                    event->y >= 19 && event->y <= 19 + 14) {

                wnck_window_close(item->m_window, (guint32) ct);
                m_isActive = false;

                // delete the item here, so the Draw method can't paint it anymore
                m_previewtems.erase(m_previewtems.begin() + m_currentIndex);
                m_currentIndex = getIndex(event->x, event->y);

                return true;
            }

//            if( wnck_window_is_active( item->m_window )){
//                wnck_window_minimize(item->m_window);
//                return true;
//            }
                
            wnck_window_activate(item->m_window, ct);
            
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
    if (m_currentIndex < 1)
        return true;

   // DockItem* item = m_previewtems.at(m_currentIndex);
   // wnck_window_activate(item->m_window, gtk_get_current_event_time());
            
    return true;
}

/**
 * Render the Preview
 * @param Cairo::Context cr
 * @return Gtk::Window::on_draw(cr)
 */
bool Preview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
   
    if (!m_isActive)
        return Gtk::Window::on_draw(cr);


    if (m_previewtems.size() < 1) {
        this->hideMe();
        return Gtk::Window::on_draw(cr);
    }

    // debug
    //g_print("Draw %d \n", (int) m_timer.elapsed());

    cr->set_source_rgba(1.0, 1.0, 1.8, 0.8);
    cr->paint();

    cr->set_source_rgba(0.0, 0.0, 0.8, 0.4);
    cr->rectangle(0, 0, this->get_width(), this->get_height());
    cr->fill();
    cr->set_line_width(1.0);

    int idx = 0;

    for (int i = 0; i < m_initialItemMax; i++) {
//
//        cr->set_line_width(1);
//        cr->set_source_rgba(1.0, 1.0, 1.8, 0.2);
//        Utilities::RoundedRectangle(cr, 14 + (m_previewWidth * i), 16, m_previewWidth + 1, m_previewHeight - 30, 2.0);
//        cr->fill();

        
        cr->set_line_width(1);
        cr->set_source_rgba(1.0, 1.0, 1.8, 0.2);
        Utilities::RoundedRectangle(cr,
                DEF_PREVIEW_LEFT_MARGING + (m_previewWidth * i), 
                16, m_previewWidth,
                m_previewHeight - DEF_PREVIEW_RIGHT_MARGING , 2.0);
        
         cr->stroke();
         

    }
  
    for (DockItem* item : m_previewtems) {
        if (item->m_window == NULL || item->m_xid == 0 || !item->visible)
            continue;

        int pos_x = 20 + (m_previewWidth * idx);
        int pos_y = 16;
        int pos_width = m_previewWidth - DEF_PREVIEW_LEFT_MARGING;
        int pos_height = 20;

        // draw title the clipping rectangle
        cr->set_source_rgba(1.0, 1.0, 1.0, 0.0);
        cr->rectangle(pos_x, pos_y + 2, pos_width, pos_height);
        cr->clip_preserve();
        cr->stroke();

        // get the current window title and Draw the pango text.
        m_title = wnck_window_get_name(item->m_window);
        auto layout = create_pango_layout(m_title);
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

            cr->set_line_width(2);
            cr->set_source_rgba(1.0, 1.0, 1.8, 0.1);
            Utilities::RoundedRectangle(cr, pos_x, pos_y, pos_width, pos_height, 2.0);
            cr->fill();

            // rectangle frame selector
            cr->set_source_rgba(1.0, 1.0, 1.0, 0.3);
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
            cr->move_to(pos_x + 3, DEF_PREVIEW_RIGHT_MARGING-1);
            cr->show_text("X");
              
        }
          

        
        // get the preview for the window.
        GdkWindow *wm_window = gdk_x11_window_foreign_new_for_display(
                gdk_display_get_default(), item->m_xid);

        // Obtains the bounding box of the window, including window manager 
        // titlebar/borders if any.
        GdkRectangle boundingbox;
        gdk_window_get_frame_extents(wm_window, &boundingbox);
        
        // create the image
        GdkPixbuf *pb = gdk_pixbuf_get_from_window(wm_window,
                0, 0, boundingbox.width-10, boundingbox.height-24);

        // FIXME: Scale don't work well
        int height = m_previewHeight - DEF_PREVIEW_SCALE_HEIGHT_OFFSET;
        int width = m_previewWidth - DEF_PREVIEW_SCALE_WIDTH_OFFSET;
        int scale_heght = height;
        
        int windowheight = gdk_window_get_height (wm_window);
        if( windowheight < 200){

            scale_heght = height/2;
        }
                
        GdkPixbuf *scaledpb = gdk_pixbuf_scale_simple(pb,
                width,
                scale_heght,
                GDK_INTERP_BILINEAR);

        Glib::RefPtr<Gdk::Pixbuf> preview = IconLoader::PixbufConvert(scaledpb);
        Gdk::Cairo::set_source_pixbuf(cr, preview, (m_previewWidth * idx) +
                20, DEF_PREVIEW_PIXBUF_TOP);
        cr->paint();

        // unreferenced release memory. 
        g_object_unref(scaledpb);
        g_object_unref(pb);
        
        idx++;
    }


    return Gtk::Window::on_draw(cr);
}
