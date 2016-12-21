#include "Preview.h"
#include "Defines.h"
#include "Utilities.h"
#include "DockPanel.h"
#include "IconLoader.h"
#include "MonitorGeometry.h"

#include <gdk/gdkx.h>

Preview::Preview() :
Gtk::Window(Gtk::WindowType::WINDOW_POPUP),
m_mouseIn(false),
m_isActive(false),
m_currentIndex(0),
m_dockItemReference(nullptr)
{
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

    m_timer.start();

}

Preview::~Preview()
{
}

void Preview::setDockItem(DockItem* item)
{
    m_currentIndex = 0;
    m_dockItemReference = item;
    m_isActive = true;
}

bool Preview::on_enter_notify_event(GdkEventCrossing* crossing_event)
{

    m_mouseIn = true;
    m_dockpanelReference->m_previewWindowActive = true;
    return true;
}

bool Preview::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    m_isActive = false;
    this->hideMe();
    m_dockpanelReference->previewWindowClosed();

    return true;
}

void Preview::hideMe()
{
    this->hide();
    m_mouseIn = false;
    m_dockpanelReference->m_previewWindowActive = false;
}

bool Preview::on_timeoutDraw()
{
    queue_draw();
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
    if (m_dockItemReference == nullptr) {
        return -1;
    }

    int col = 0;
    int idx = 0;

    for (DockItem* item : m_dockItemReference->m_items) {
        if (item->m_window == NULL)
            continue;

        if (x > col && x <= col + (DEF_PREVIEW_WIDTH + 10))
            return idx;

        idx++;
        col += DEF_PREVIEW_WIDTH;
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
    if (this->m_dockItemReference == nullptr)
        return false;

    int count = this->m_dockItemReference->m_items.size();
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

    itemWindow = m_dockItemReference->m_items.at(m_currentIndex)->m_window;
    wnck_window_activate(itemWindow, gtk_get_current_event_time());

    // Event has been handled
    return true;
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

            DockItem *item = m_dockItemReference->m_items.at(m_currentIndex);
            int ct = gtk_get_current_event_time();

            // Handle close preview window
            int pos_x = (DEF_PREVIEW_WIDTH - 5) + (DEF_PREVIEW_WIDTH * m_currentIndex);
            if (event->x >= pos_x && event->x <= pos_x + 14 && // FIXTHIS: use rectangle instead.
                    event->y >= 19 && event->y <= 19 + 14) {

                wnck_window_close(item->m_window, (guint32) ct);

                // delete the item. it is a pointer to the DockPanel m_dockitems
                delete( item);
                m_dockItemReference->m_items.erase(m_dockItemReference->m_items.begin() + m_currentIndex);

                hideMe();
                /*
                int width = (DEF_PREVIEW_WIDTH * m_dockItemReference->m_items.size() - 1) + 30;
                this->resize(width, DEF_PREVIEW_HEIGHT);
                
                int centerpos = DockPosition::getCenterPosByCurrentDockItemIndex(
                        m_dockItemReference->m_items.size() , m_dockpanelReference->getCurrentIndex(), width);
                
                g_print("current,%d\n",m_dockpanelReference->getCurrentIndex());
                
                move(centerpos, MonitorGeometry::getScreenHeight() - 
                (DEF_PREVIEW_HEIGHT + DEF_PANELBCKHIGHT + 6));
                */
               

                
                
                

                return true;
            }

            if (!wnck_window_is_active(item->m_window)) {
                wnck_window_activate(item->m_window, (guint32) ct);
            } else {
                wnck_window_minimize(item->m_window);
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
    if (m_currentIndex < 1)
        return true;

    return true;
}

bool Preview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (!m_isActive)
        return false;

    //g_print("Draw %d \n", (int) m_timer.elapsed());

    cr->set_source_rgba(1.0, 1.0, 1.8, 0.8);
    cr->paint();

    cr->set_source_rgba(0.0, 0.0, 0.8, 0.4);
    cr->rectangle(0, 0, this->get_width(), this->get_height());
    cr->fill();
    cr->set_line_width(1.0);

    int idx = 0;

    for (DockItem* item : m_dockItemReference->m_items) {
        if (item->m_window == NULL || item->m_xid == 0)
            continue;

        int pos_x = 20 + (DEF_PREVIEW_WIDTH * idx);
        int pos_y = 16;
        int pos_width = DEF_PREVIEW_WIDTH - 30;
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


        if (m_currentIndex >= 0) {

            // rectangle background selector
            pos_x = 14 + (DEF_PREVIEW_WIDTH * m_currentIndex);
            pos_y = 16;
            pos_width = DEF_PREVIEW_WIDTH + 1;
            pos_height = DEF_PREVIEW_HEIGHT - 30;

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
            pos_x = (DEF_PREVIEW_WIDTH - 5) + (DEF_PREVIEW_WIDTH * m_currentIndex);
            cr->rectangle(pos_x, 19, 14, 14);
            cr->fill();
            cr->stroke();

            // Close X text
            cr->set_source_rgba(1.0, 1.0, 1.0, 1); //white
            cr->move_to(pos_x + 3, 30);
            cr->show_text("X");

            //g_print("%s\n",t->m_appname.c_str());    
        }

        // get the preview for window.
        GdkWindow *wm_window = gdk_x11_window_foreign_new_for_display(
                gdk_display_get_default(), item->m_xid);

        GdkRectangle real_coordinates;
        gdk_window_get_frame_extents(wm_window, &real_coordinates);
        // create the image
        GdkPixbuf *pb = gdk_pixbuf_get_from_window(wm_window,
                0, 0, real_coordinates.width, real_coordinates.height);

        GdkPixbuf *scaledpb = gdk_pixbuf_scale_simple(pb,
                DEF_PREVIEW_PIXBUF_WIDTH,
                DEF_PREVIEW_PIXBUF_HEIGHT,
                GDK_INTERP_BILINEAR);

        Glib::RefPtr<Gdk::Pixbuf> preview = IconLoader::PixbufConvert(scaledpb);
        Gdk::Cairo::set_source_pixbuf(cr, preview, (DEF_PREVIEW_WIDTH * idx) +
                20, DEF_PREVIEW_PIXBUF_TOP);
        cr->paint();

        g_object_unref(scaledpb);
        g_object_unref(pb);

        idx++;
    }


    return Gtk::Window::on_draw(cr);
}
