//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on January , 2017
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


#ifndef DRAGDROPWINDOW_H
#define DRAGDROPWINDOW_H

#include <gtkmm/window.h>
#include <gdkmm/pixbuf.h>
#include "Defines.h"


class DragDropWindow : public Gtk::Window
{
public:
    DragDropWindow();
    ~DragDropWindow();
    void Show(Glib::RefPtr<Gdk::Pixbuf> icon, int size, Gdk::Point mousePoint);
    void Hide();
protected:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
    bool on_timeoutDraw();
private:
    bool m_active=false;
    bool mouseMove(int x, int y);
    Glib::RefPtr<Gdk::Pixbuf> m_icon=NULLPB;
    Gdk::Point m_mousePoint;
};

#endif /* DRAGDROPWINDOW_H */

