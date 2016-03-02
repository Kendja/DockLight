/* 
 * File:   XPreview.h
 * Author: yoo
 *
 * Created on February 3, 2016, 5:48 PM
 */

#ifndef XPREVIEW_H
#define	XPREVIEW_H

#include <gtkmm.h>
#include <gtk/gtk.h> // user c version for samples
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <gdk/gdkx.h>
#include "DockItem.h"
#include <math.h>
#include <pango/pangocairo.h> // https://developer.gnome.org/pango/stable/pango-Cairo-Rendering.html
#include "Utilities.h"
//#include "utf8.h" //http://utfcpp.sourceforge.net/


class XPreview : public Gtk::Window
{
public:
    //MyPopup(Gtk::WindowType type);
    //XPreview(Gtk::WindowType type, panel_locationType location); //:Gtk::Window(type)
    XPreview(); //:Gtk::Window(type)
    
    
    
    void hideMe();
    bool m_mouseIn;
    bool m_active;
    int m_currentIndex;
    Gtk::Menu m_Menu_Popup;
    Gtk::MenuItem m_MenuCloseWindow;

    void on_menuCloseWindow_event();

    void setXid(DockItem* item);
    int getIndex(int x, int y);

    virtual ~XPreview();
    
    void setPanelLocation(panel_locationType panelLocation);
    
protected:

    panel_locationType m_panelLocation;
 
        
    bool on_button_press_event(GdkEventButton *event);
    bool on_motion_notify_event(GdkEventMotion*event);

    //Override default signal handler:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    bool on_drawX(const Cairo::RefPtr<Cairo::Context>& cr);

    bool on_enter_notify_event(GdkEventCrossing* crossing_event);
    bool on_leave_notify_event(GdkEventCrossing* crossing_event);
    bool on_timeoutDraw();

    Glib::RefPtr<Gdk::Pixbuf> PixbufConvert(GdkPixbuf* icon);

    ////////////////////////
//
//    void fix_utf8_string(std::string& str)
//    {
//        std::string temp;
//        utf8::replace_invalid(str.begin(), str.end(), back_inserter(temp));
//        str = temp;
//    }

    // http://stackoverflow.com/questions/7344683/utf8-aware-strncpy
    char* utf8cpy(char* dst, const char* src, size_t sizeDest)
    {  
        if (sizeDest)
        {
            size_t sizeSrc = strlen(src); // number of bytes not including null
            while (sizeSrc >= sizeDest)
            {

                const char* lastByte = src + sizeSrc; // Initially, pointing to the null terminator.
                while (lastByte-- > src)
                    if ((*lastByte & 0xC0) != 0x80) // Found the initial byte of the (potentially) multi-byte character (or found null).
                        break;

                sizeSrc = lastByte - src;
              
            }
            memcpy(dst, src, sizeSrc);
            dst[sizeSrc] = '\0';
        }
        return dst;
    }

    int utf8Legth(const char* src)
    {
        int count = 0;
            size_t sizeSrc = strlen(src); // number of bytes not including null
            for(int idx = 0 ; idx < sizeSrc; idx++){
            //while (sizeSrc >= 5)
            //{

                const char* lastByte = src + sizeSrc; // Initially, pointing to the null terminator.
                while (lastByte-- > src)
                    if ((*lastByte & 0xC0) != 0x80) // Found the initial byte of the (potentially) multi-byte character (or found null).
                        break;

                sizeSrc = lastByte - src;
                count++;
            }
            
            //memcpy(dst, src, sizeSrc);
           // dst[sizeSrc] = '\0';
        
        return count;
    }
    
    void drawText(const Cairo::RefPtr<Cairo::Context>& cr,
            int x, int y, const std::string text)
    {

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
        cr->move_to(x,  y);
        //cr->rel_move_to(x, text_height + y);
        layout->show_in_cairo_context(cr);
    }

    void draw_text(const Cairo::RefPtr<Cairo::Context>& cr,
            int rectangle_width, int rectangle_height)
    {
        // http://developer.gnome.org/pangomm/unstable/classPango_1_1FontDescription.html
        Pango::FontDescription font;
        cr->set_source_rgb(0.0, 0.0, 0.0);


        font.set_family("System");
        font.set_weight(Pango::WEIGHT_NORMAL);

        // http://developer.gnome.org/pangomm/unstable/classPango_1_1Layout.html
        auto layout = create_pango_layout("We display the text on the window by specifying the position of the text and calling the cairo_show_text() function.");

        layout->set_font_description(font);

        int text_width;
        int text_height;

        //get the text dimensions (it updates the variables -- by reference)
        layout->get_pixel_size(text_width, text_height);

        // Position the text in the middle
        cr->move_to((rectangle_width - text_width) / 2, (rectangle_height - text_height) / 2);

        layout->show_in_cairo_context(cr);
    }

    void draw_rectangle(const Cairo::RefPtr<Cairo::Context>& cr,
           int x, int y, int width, int height)
    {
        
        //cr->save();
        
        cr->set_source_rgba(1.0, 1.0, 1.0, 1.0);
        cr->rectangle(x, y, width, height);
        cr->clip_preserve();
        //cr->clip();
        cr->stroke();
       // cr->restore();

    }

private:

    DockItem* m_item;
};

#endif	/* XPREVIEW_H */

