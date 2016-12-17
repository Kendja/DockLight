//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on November 20, 2015, 12:17 PM 
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

#include "TitleWindow.h"

TitleWindow::TitleWindow() :
Gtk::Window(Gtk::WindowType::WINDOW_POPUP),
m_active(false),
m_HBox(Gtk::ORIENTATION_HORIZONTAL, 5),
m_Width(0),
m_Label("", false)
{
    Gtk::Window::set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_TOOLTIP);
    m_HBox.set_margin_left(6);
    m_HBox.set_margin_right(6);
    m_HBox.set_margin_top(6);
    m_HBox.set_margin_bottom(6);


    override_background_color(Gdk::RGBA("black"));

    m_Label.override_color(Gdk::RGBA("white"), Gtk::STATE_FLAG_NORMAL);
    add(m_HBox);
    m_HBox.add(m_Label);
    m_HBox.show();
    //gtk_label_set_justify(GTK_LABEL(m_Label.gobj()), GTK_JUSTIFY_CENTER);
    /*
        Gtk::Label::Label 	( 	const Glib::ustring &  	label,
                    Align  	xalign,
                    Align  	yalign = ALIGN_CENTER,
                    bool  	mnemonic = false 
            ) 	
     */
    //Glib::signal_timeout().connect( sigc::mem_fun(*this, &TitleWindow::on_timeout), 1000/60 );

}

TitleWindow::~TitleWindow()
{
    //dtor
}

/**
 * Set the tooltip-text to show
 *
 * @param text the text to show
 */
void TitleWindow::setText(const Glib::ustring text)
{
    m_Label.set_text(text);


}
int xx = 0;

void TitleWindow::setPosition(int x, int y)
{


    show();
    if (xx != x) {
        move(x, y);
        xx = x;
        resize(10, get_height());
        //resize_to_geometry(10, get_height());

    }

    m_Width = get_width();





}

int TitleWindow::getCurrentWidth()
{
    return m_Width;
}


/*
bool TitleWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();
    
 //   cr->set_source_rgba(0.337, 0.612, 0.117, 0.9);   // green
    cr->set_source_rgba(1.0, 1.0, 1.8, 0.8); // partially translucent
    cr->paint();

    
    
    return TRUE;
}

bool TitleWindow::on_timeout()
{
    
    // force our program to redraw the entire clock.
    auto win = get_window();
    if (win)
    {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                get_allocation().get_height());
        win->invalidate_rect(r, false);
    }
    return true;
}
 */