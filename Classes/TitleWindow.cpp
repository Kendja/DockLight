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
/**
 * constructs a window POPUP for teh dock item  title.
 */
TitleWindow::TitleWindow() :
Gtk::Window(Gtk::WindowType::WINDOW_POPUP),
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
    
}
/**
 * Hide the window on destroy
 */
TitleWindow::~TitleWindow()
{
    hide();
}

/**
 * Set the tooltip-text to show
 * @param text the text to show
 */
void TitleWindow::setText(const Glib::ustring text)
{
    m_Label.set_text(text);
  
    resize(10, get_height()); // Trick to auto resize the window
    show_all();
    
    m_Width = get_width();
}
/**
 * get the calculated width
 * @return width of the window;
 */
int TitleWindow::getCurrentWidth()
{
    return m_Width;
}

