//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on February 3, 2016, 5:48 PM
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

#include "XPreview.h"
#include <cairo.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

XPreview::XPreview() :
Gtk::Window(Gtk::WindowType::WINDOW_POPUP), m_active(false) {
    //Gtk::WindowType::WINDOW_POPUP
    // set_opacity(0.9);
    /*
    GdkScreen *screen;
    GdkVisual *visual;

    //gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    
    set_app_paintable(true);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
        //gtk_widget_set_visual(win, visual);

    }
     */



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



    set_app_paintable(true);

    m_item = NULL;
    m_mouseIn = false;
    m_currentIndex = 0;

    set_resizable(true);
    show_all_children();

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &XPreview::on_timeoutDraw), 50);


    m_MenuCloseWindow.set_label("Close this window");
    m_MenuCloseWindow.signal_activate().connect(sigc::mem_fun(*this, &XPreview::on_menuCloseWindow_event));
    m_Menu_Popup.append(m_MenuCloseWindow);
    // Show the menu
    m_Menu_Popup.show_all();
    // Connect the menu to this Widget
    m_Menu_Popup.accelerate(*this);


}

void XPreview::setPanelLocation(panel_locationType panelLocation) {
    this->m_panelLocation = panelLocation;
}

bool XPreview::on_enter_notify_event(GdkEventCrossing* crossing_event) {

    m_mouseIn = true;
    return true;
}

void XPreview::hideMe() {
    if (!m_mouseIn)
        return;

    this->hide();
    m_mouseIn = false;
}

bool XPreview::on_leave_notify_event(GdkEventCrossing* crossing_event) {
    this->m_active = false;
    this->hideMe();
    return true;
}

bool XPreview::on_timeoutDraw() {
    queue_draw();
    return true;
}

void XPreview::on_menuCloseWindow_event() {

}

/*
 *
 * handles Mouse button press : process mouse button event
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further.
 */
bool XPreview::on_button_press_event(GdkEventButton *event) {

    if ((event->type == GDK_BUTTON_PRESS)) {
        // Check if the event is a left button click.
        if (event->button == 1) {

            if (m_currentIndex < 0)
                return true;

            DockItem *item = m_item->m_items->at(m_currentIndex);
            int ct = gtk_get_current_event_time();

            // Handle close preview window
            int pos_x = (DEF_PREVIEW_WIDTH - 5) + (DEF_PREVIEW_WIDTH * m_currentIndex);
            if (event->x >= pos_x && event->x <= pos_x + 14 && // FIXTHIS: use rectangle instead.
                    event->y >= 19 && event->y <= 19 + 14) {

                //int mxp = event->x + (m_currentIndex * DEF_PREVIEW_PIXBUF_WIDTH);
                // g_print("%d/%d %d\n", (int) event->x, (int) event->y, mxp);

                wnck_window_close(item->m_window, (guint32) ct);
                m_item->m_items->erase(m_item->m_items->begin() + m_currentIndex);

                int width = (DEF_PREVIEW_WIDTH * m_item->m_items->size() - 1) + 30;
                this->resize(width, DEF_PREVIEW_HEIGHT);
                return true;
            }

            if (!wnck_window_is_active(item->m_window)) {
                wnck_window_activate(item->m_window, (guint32) ct);
            } else {
                wnck_window_minimize(item->m_window);
            }

            // The event has been handled.
            return true;
        } else if (event->button == 3) {
            //   m_Menu_Popup.popup(event->button, event->time); // Dont Work in the Preview
        }

    }
}

/*
 * 
 * get the item index from mouse coordinates.
 * 
 */
int XPreview::getIndex(int x, int y) {
    if (m_item == NULL) {
        return -1;
    }

    //g_print("%d %d\n", x, m_currentIndex);
    int col = 0;
    int idx = 0;

    for (auto item : *m_item->m_items) {
        if (item->m_window == NULL)
            continue;

        if (x > col && x <= col + (DEF_PREVIEW_WIDTH + 10))
            return idx;

        idx++;
        col += DEF_PREVIEW_WIDTH;
    }

    return -1;

}

/*
 * 
 * handles on_motion_notify_event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 */
bool XPreview::on_motion_notify_event(GdkEventMotion*event) {
    m_currentIndex = getIndex(event->x, event->y);
    if (m_currentIndex < 1)
        return true;

    return true;
}

/*
 * 
 * handles the mouse scroll . 
 * 
 */
bool XPreview::on_scroll_event(GdkEventScroll * event) {
    if (this->m_item == NULL)
        return false;

    int count = this->m_item->m_items->size();
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

    itemWindow = this->m_item->m_items->at(m_currentIndex)->m_window;
    wnck_window_activate(itemWindow, gtk_get_current_event_time());

    // Event has been handled
    return true;
}

static bool pangoset = false;

bool XPreview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {

    if (m_item == NULL) {
        return true;
    }

    cr->set_source_rgba(1.0, 1.0, 1.8, 0.8); // partially translucent
    cr->paint();

    Pango::FontDescription font;

    if (!pangoset) {
        font.set_family("System");
        font.set_weight(Pango::WEIGHT_NORMAL);
        pangoset = true;
    }
    // cr->set_source_rgb(0.5, 0.5, 0.5);
    cr->set_source_rgba(0.0, 0.0, 0.8, 0.4); // partially translucent
    //  cr->set_source_rgba(0.0, 0.0, 0.0, 1.0); // partially translucent
    cr->rectangle(0, 0, this->get_width(), this->get_height());
    // Utilities::RoundedRectangle(cr,0, 0, this->get_width(), this->get_height(),12.0);
    cr->fill();
    cr->set_line_width(1.0);


    int idx = 0;
    for (auto t : *m_item->m_items) {

        if (t->m_window == NULL)
            continue;

        int pos_x = 20 + (DEF_PREVIEW_WIDTH * idx);
        int pos_y = 16;
        int pos_width = DEF_PREVIEW_WIDTH - 30;
        int pos_height = 20;

        // draw the clipping rectangle
        cr->set_source_rgba(1.0, 1.0, 1.0, 0.0);
        cr->rectangle(pos_x, pos_y, pos_width, pos_height);
        cr->clip_preserve();
        cr->stroke();

        // get the current window title;
        t->m_appname = wnck_window_get_name(t->m_window);
        std::string caption = t->m_appname;
        int length = t->m_appname.length(); // // number of bytes not including null

        // Draw the pango text
        auto layout = create_pango_layout(caption);
        layout->set_font_description(font);

        int text_width;
        int text_height;
        layout->get_pixel_size(text_width, text_height);

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
            cr->set_source_rgba(1.0, 1.0, 1.8, 0.1); // partially translucent
            //cr->rectangle(pos_x, pos_y, pos_width, pos_height);
            Utilities::RoundedRectangle(cr, pos_x, pos_y, pos_width, pos_height, 2.0);
            cr->fill();

            // rectangle frame selector
            cr->set_source_rgba(1.0, 1.0, 1.0, 0.3); // partially translucent
            //cr->rectangle(pos_x, pos_y, pos_width, pos_height);
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
        // get the preview for t->m_xid the window.
        GdkWindow *wm_window =
                gdk_x11_window_foreign_new_for_display(gdk_display_get_default(),
                t->m_xid);

        GdkRectangle real_coordinates;
        gdk_window_get_frame_extents(wm_window, &real_coordinates);
        // create the image
        GdkPixbuf *pb = gdk_pixbuf_get_from_window(wm_window,
                0, 0, real_coordinates.width, real_coordinates.height);

        GdkPixbuf *scaledpb = gdk_pixbuf_scale_simple(pb,
                DEF_PREVIEW_PIXBUF_WIDTH,
                DEF_PREVIEW_PIXBUF_HEIGHT,
                GDK_INTERP_BILINEAR);

        Glib::RefPtr<Gdk::Pixbuf> screenshot = PixbufConvert(scaledpb);

        Gdk::Cairo::set_source_pixbuf(cr, screenshot, (DEF_PREVIEW_WIDTH * idx) +
                20, DEF_PREVIEW_PIXBUF_TOP);
        cr->paint();

        g_object_unref(scaledpb);
        g_object_unref(pb);

        idx++;

    }
    return true;

}

void XPreview::setXid(DockItem * item) {
    m_active = true;
    m_currentIndex = 0;
    this->m_item = item;


}

Glib::RefPtr<Gdk::Pixbuf>
XPreview::PixbufConvert(GdkPixbuf * icon) {
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

void XPreview::drawText(const Cairo::RefPtr<Cairo::Context>& cr,
        int x, int y, const std::string text) {
    // http://developer.gnome.org/pangomm/unstable/classPango_1_1FontDescription.html
    Pango::FontDescription font;
    font.set_family("Sans");
    font.set_weight(Pango::WEIGHT_NORMAL);

    // http://developer.gnome.org/pangomm/unstable/classPango_1_1Layout.html
    auto layout = create_pango_layout(text);
    // layout->set_height(40);

    Cairo::TextExtents textents;
    layout->set_font_description(font);
    int text_width;
    int text_height;

    //get the text dimensions (it updates the variables -- by reference)
    layout->get_pixel_size(text_width, text_height);
    cr->get_text_extents("พ", textents);

    cr->set_source_rgb(1.0, 1.0, 1.0);
    // Position the text in the middle
    //cr->move_to(x, textents.y_bearing+ textents.height + y);
    cr->move_to(x, y);
    //cr->rel_move_to(x, text_height + y);
    layout->show_in_cairo_context(cr);
}

void XPreview::draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr,
        int x, int y, int width, int height) {

    //cr->save();

    cr->set_source_rgba(1.0, 1.0, 1.0, 1.0);
    cr->rectangle(x, y, width, height);
    cr->clip_preserve();
    //cr->clip();
    cr->stroke();
    // cr->restore();

}

XPreview::~XPreview() {
}

