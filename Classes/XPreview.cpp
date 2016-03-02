/* 
 * File:   XPreview.cpp
 * Author: yoo
 * 
 * Created on February 3, 2016, 5:48 PM
 */

#include "XPreview.h"
#include <cairo.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
//#include <codecvt>

//#include <gtkmm/window.h>



#define WIN_WIDTH 384
#define WIN_HEIGHT 384

XPreview::XPreview() :
Gtk::Window(Gtk::WindowType::WINDOW_POPUP), m_active(false)
{
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
    //Window(WINDOW_POPUP);

    set_resizable(true);
    //            
    //    //this->Window::ty
    //    // this->set_state_flags(StateFlags )
    //    //this->set_decorated(false);
    //    set_default_size(400, 200);
    //
    //    Gtk::Label *label = Gtk::manage(new Gtk::Label("New Popup Window"));
    //
    //    add(*label);

    //how_all();
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
void XPreview::setPanelLocation(panel_locationType panelLocation)
{
       this->m_panelLocation =  panelLocation;
}

bool XPreview::on_enter_notify_event(GdkEventCrossing* crossing_event)
{

    m_mouseIn = true;
    return true;
}

void XPreview::hideMe()
{
    if (!m_mouseIn)
        return;

    this->hide();
    m_mouseIn = false;
}

bool XPreview::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    this->m_active = false;
    this->hideMe();
    return true;
}

bool XPreview::on_timeoutDraw()
{
    queue_draw();
    return true;
}

void XPreview::on_menuCloseWindow_event()
{

}

/****************************************************************
 *
 * handles Mouse button press : process mouse button event
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further.
 ****************************************************************/
bool XPreview::on_button_press_event(GdkEventButton *event)
{



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

/****************************************************************
 * 
 * get the item index from mouse coordinates.
 * 
 ****************************************************************/
int XPreview::getIndex(int x, int y)
{
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

/****************************************************************
 * 
 * handles on_motion_notify_event 
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further. 
 ****************************************************************/
bool XPreview::on_motion_notify_event(GdkEventMotion*event)
{
    m_currentIndex = getIndex(event->x, event->y);
    if (m_currentIndex < 1)
        return true;

    // Gtk::Widget::queue_draw();
    return true;
}

bool XPreview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (m_item == NULL) {
        return true;
    }
    Pango::FontDescription font;
    font.set_family("System");
    font.set_weight(Pango::WEIGHT_NORMAL);

    cr->set_source_rgb(0.5, 0.5, 0.5);
    cr->set_source_rgba(0.0, 0.0, 0.8, 0.4); // partially translucent
    cr->rectangle(0, 0, this->get_width(), this->get_height());
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
        int chlength = utf8Legth(t->m_appname.c_str());
        int length = t->m_appname.length(); // // number of bytes not including null

        // TODO: fix the utf8/unicode problem
        /*
        if( chlength > 16  )
        {
             char utfbuffer[DEF_UTF8MAX];
            // guarantee null-termination for Utf8
            utf8cpy(utfbuffer, t->m_appname.c_str(), 4);
            caption = utfbuffer;
            caption += "...";
        }
         */
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
            Utilities::RoundedRectangle(cr,pos_x, pos_y, pos_width, pos_height,2.0);
            cr->fill();

            // rectangle frame selector
            cr->set_source_rgba(1.0, 1.0, 1.0, 0.3); // partially translucent
            //cr->rectangle(pos_x, pos_y, pos_width, pos_height);
            Utilities::RoundedRectangle(cr,pos_x, pos_y, pos_width, pos_height,2.0);
            
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

        // get the preview for the window.
        GdkWindow *wm_window = gdk_x11_window_foreign_new_for_display(gdk_display_get_default(), t->m_xid);
        GdkRectangle real_coordinates;
        gdk_window_get_frame_extents(wm_window, &real_coordinates);
        // create the image
        GdkPixbuf *pb = gdk_pixbuf_get_from_window(wm_window,
                0, 0, real_coordinates.width, real_coordinates.height);


        //Gdk::Window::
        //wm_window.>
        // Gdk::Window* juan;
        //Glib::RefPtr<Gdk::Window> ramon;
        //auto cssd = Glib::RefPtr<Gdk::Window>::cast_dynamic(wm_window);

        //Glib::RefPtr<Gdk::Window>refWindow =  Glib::RefPtr<Gdk::Window>::cast_dynamic( wm_window);

        //Glib::RefPtr<Gdk::Window> refWindow( wm_window );

        //Glib::RefPtr<Gdk::Window> ccccc = reinterpret_cast<Gdk::Window*>(wm_window);

        // Glib::RefPtr<Gdk::Window>::cast_dynamic(Glib::RefPtr<wm_window>);

        //Gtk::Window  gtkwin2;
        // Gdk::Pixbuf::create(Glib::RefPtr<Window>)
        //dynamic_cast<Gtk::Container*>(m_InfoBar.get ... gtkmm arranges widgets ...

        // Glib::RefPtr<Window>::cast_dynamic(t->m_xid); 
        //auto cc = static_cast<Window>(t->m_xid);
        //Glib::RefPtr<Window> gccw = Glib::RefPtr<Window>::cast_dynamic(gtkwin2);
        //auto bbb = Gdk::Pixbuf::create( ramon,0, 0, real_coordinates.width, real_coordinates.height);

        // Glib::RefPtr<Gdk::Pixbuf> pic;
        //pic->scale(

        GdkPixbuf *scaledpb = gdk_pixbuf_scale_simple(pb,
                DEF_PREVIEW_PIXBUF_WIDTH,
                DEF_PREVIEW_PIXBUF_HEIGHT,
                GDK_INTERP_BILINEAR);

        Glib::RefPtr<Gdk::Pixbuf> screenshot = PixbufConvert(scaledpb);

        Gdk::Cairo::set_source_pixbuf(cr, screenshot, (DEF_PREVIEW_WIDTH * idx) + 20, DEF_PREVIEW_PIXBUF_TOP);
        cr->paint();

        g_object_unref(scaledpb);
        g_object_unref(pb);

        idx++;

    }
    return true;

}

bool XPreview::on_drawX(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (m_item == NULL) {
        return true;
    }

    // Fill the background with gray
    cr->set_source_rgb(0.5, 0.5, 0.5);

    cr->set_source_rgba(0.0, 0.0, 0.8, 0.4); // partially translucent
    cr->rectangle(0, 0, this->get_width(), this->get_height());
    cr->fill_preserve();
    //  double m_radius = 5.0;

    cr->set_line_width(1.0);
    //
    //    cairo_text_extents_t te;
    //    char letter[2];
    //    *letter = '\0';

    //cairo_text_extents(cr, letter, &te);

    //  Cairo::TextExtents textents;
    // Cairo::FontExtents fextents;

    //Glib::RefPtr<Pango::Layout> layout = Pango::Layout::create(cr);

    Pango::FontDescription font;
    font.set_family("System");
    font.set_weight(Pango::WEIGHT_NORMAL);
    int text_width = 0;
    int text_height = 0;

    //te->width;

    // cr->select_font_face("System", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
    // cr->set_font_size(11);

    int idx = 0;

    int maxtextlength = 23;
    // int maxtextlength_selected = 34;

    std::string caption = "";
    for (auto t : *m_item->m_items) {

        if (t->m_window == NULL)
            continue;

        // get the current window title;
        t->m_appname = wnck_window_get_name(t->m_window);
        int length = t->m_appname.length(); // // number of bytes not including null
        //   size_t bytescount =  strlen(t->m_appname.c_str()); // number of bytes not including null

        // http://developer.gnome.org/pangomm/unstable/classPango_1_1Layout.html
        //  auto layout = create_pango_layout(t->m_appname);
        //  layout->set_font_description(font);
        // get the text dimensions (it updates the variables -- by reference)
        //  layout->get_pixel_size(text_width, text_height);

        // layout->set_text(t->m_appname);
        int stringWidth, stringHeight;
        // layout->get_pixel_size(stringWidth, stringHeight);

        int sizechars = utf8Legth(t->m_appname.c_str());
        //  t->m_appname ="gtkmm Cairo  \340\270\204\340\271\211\340\270\231\340\270\253\340\270\262\340\270\224\340\271\211\340\270\247\340\270\242 Google - Mozilla Firefox";

        // t->m_appname="sdasda แคช  ใกล้เคียง  sd sdas - แคช  ใกล้เคียง";

        //    cr->get_text_extents(t->m_appname.c_str(), textents);
        // cr->get_font_extents(fextents);


        //  int textWidth = (int) textents.width;


        // layout->set_text(t->m_appname);
        //  int stringWidth, stringHeight;
        // layout->get_pixel_size(stringWidth, stringHeight);
        if (1 == 2/*sizechars > maxtextlength*/) {
            //if (length > maxtextlength) {
            // TODO: use this lib http://site.icu-project.org/
            //https://randomascii.wordpress.com/2013/04/03/stop-using-strncpy-already/
            char utfbuffer[DEF_UTF8MAX];

            // guarantee null-termination for Utf8
            // g_utf8_strncpy(utfbuffer, t->m_appname.c_str(), maxtextlength);
            utf8cpy(utfbuffer, t->m_appname.c_str(), 10);

            //utfbuffer[maxtextlength - 1] = '\0';
            caption = utfbuffer;
            caption += "...";

            //int mw = textWidth/length;
            //int mw = textWidth/20;

            //int cf = mw ;
            //   double gg = textents.x_bearing - textents.width;


            //int cf = (mw * length)/10;
            // char buffer [33];
            //  sprintf(buffer,"%d",mw);
            //std::string num = itoa(mw,buffer,10); 

            //  caption = t->m_appname.substr(0, maxtextlength ) + "...";

            // they do not guarantee null-termination Utf8

            // caption = t->m_appname.substr(0,maxtextlength);
        } else {
            caption = t->m_appname;
        }

        // cr->set_source_rgb(1.0, 1.0, 1.0);
        // cr->move_to(20 + (DEF_PREVIEW_WIDTH * idx), 30);

        // char* invalid = utf8::find_invalid(caption.c_str(), utf_invalid.c_str() + 6);
        //assert (invalid == utf_invalid + 5);
        //caption = "sdasda แคช  ใกล้เคียง  sd sdas - แคช  ใกล้เคียง";

        // caption ="gtkmm Cairo  \340\270\204\340\271\211\340\270\231\340\270\253\340\270\262\340\270\224\340\271\211\340\270\247\340\270\242 Google - Mozilla Firefox";
        // char utfbuffer[DEF_UTF8MAX];
        // utfbuffer[maxtextlength - 1] = '\0';
        //strncpy(utfbuffer, caption.c_str(),maxtextlength);
        // g_utf8_strncpy(utfbuffer,caption.c_str(),maxtextlength);
        //  g_print("%s\n",utfbuffer);
        // caption = utfbuffer;


        // caption = t->m_appname.substr(0,maxtextlength);
        // caption = caption.substr(0, maxtextlength );
        //  fix_utf8_string(caption);
        //caption[maxtextlength]='\0';

        // std::u8
        // fix_utf8_string(caption);
        //cr->show_text(caption);

        draw_rectangle(cr, 20 + (DEF_PREVIEW_WIDTH * idx), 10, 60, 30);
        drawText(cr, 20 + (DEF_PREVIEW_WIDTH * idx), 18, caption);
        cr->reset_clip();

        //cr->save();
        /*
        Pango::FontDescription font;
        font.set_family("System");
        font.set_weight(Pango::WEIGHT_NORMAL);
        int text_width = 0;
        int text_height = 0;

        auto layout = create_pango_layout(caption);
        layout->set_font_description(font);

        layout->get_pixel_size(text_width, text_height);
        
        cr->move_to(20 + (DEF_PREVIEW_WIDTH * idx), 18);
        layout->show_in_cairo_context(cr);
         */

        if (m_currentIndex >= 0) {

            // rectangle background selector
            double pos_x = 14 + (DEF_PREVIEW_WIDTH * m_currentIndex);
            double pos_y = 16;
            double pos_width = DEF_PREVIEW_WIDTH + 1;
            double pos_height = DEF_PREVIEW_HEIGHT - 30;

            cr->set_line_width(2);
            cr->set_source_rgba(1.0, 1.0, 1.8, 0.1); // partially translucent
            cr->rectangle(pos_x, pos_y, pos_width, pos_height);
            cr->fill();

            // rectangle frame selector
            cr->set_source_rgba(1.0, 1.0, 1.0, 0.3); // partially translucent
            cr->rectangle(pos_x, pos_y, pos_width, pos_height);
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


        // get the preview for the xid window
        GdkWindow *wm_window = gdk_x11_window_foreign_new_for_display(gdk_display_get_default(), t->m_xid);
        GdkRectangle real_coordinates;

        gdk_window_get_frame_extents(wm_window, &real_coordinates);
        GdkPixbuf *pb = gdk_pixbuf_get_from_window(wm_window,
                0, 0, real_coordinates.width, real_coordinates.height);

        GdkPixbuf *scaledpb = gdk_pixbuf_scale_simple(pb,
                DEF_PREVIEW_PIXBUF_WIDTH,
                DEF_PREVIEW_PIXBUF_HEIGHT,
                GDK_INTERP_BILINEAR);

        Glib::RefPtr<Gdk::Pixbuf> screenshot = PixbufConvert(scaledpb);

        Gdk::Cairo::set_source_pixbuf(cr, screenshot, (DEF_PREVIEW_WIDTH * idx) + 20, DEF_PREVIEW_PIXBUF_TOP);
        cr->paint();

        idx++;
        g_object_unref(scaledpb);
        g_object_unref(pb);

    }

    return true;
}




void XPreview::setXid(DockItem * item)
{
    m_active = true;
    m_currentIndex = 0;
    this->m_item = item;


}

Glib::RefPtr<Gdk::Pixbuf>
XPreview::PixbufConvert(GdkPixbuf * icon)
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

XPreview::~XPreview()
{
}

