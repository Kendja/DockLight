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

#include "DragDropWindow.h"
#include "Utilities.h"
#include "Defines.h"

DragDropWindow::DragDropWindow()
: Gtk::Window(Gtk::WindowType::WINDOW_POPUP)
{
    set_size_request(DEF_ICONSIZE, DEF_ICONSIZE);

    GdkScreen *screen;
    GdkVisual *visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }

    Glib::signal_timeout().
            connect(sigc::mem_fun(*this, &DragDropWindow::on_timeoutDraw), DEF_FRAMERATE);
}

DragDropWindow::~DragDropWindow()
{
    g_print("Drag and drop destroy\n");
}

void DragDropWindow::Hide()
{
    this->m_icon = NULLPB;
    m_active = false;
    this->hide();
}

void DragDropWindow::Show(Glib::RefPtr<Gdk::Pixbuf> icon, int size, Gdk::Point mousePoint)
{
    this->m_icon = icon;
    this->m_mousePoint.set_x(mousePoint.get_x());
    this->m_mousePoint.set_y(mousePoint.get_y());

    resize(size, size);
    if (mouseMove(m_mousePoint.get_x(), m_mousePoint.get_y())) {
        m_active = true;
        this->show_now();
    }

}

bool DragDropWindow::mouseMove(int x, int y)
{
    int mouseX;
    int mouseY;
    bool found = Utilities::getMousePosition(mouseX, mouseY);

    if (found)
        this->move(mouseX - x, mouseY - y);

    return found;
}

bool DragDropWindow::on_timeoutDraw()
{
    if (!m_active)
        return true;

    if (mouseMove(m_mousePoint.get_x(), m_mousePoint.get_y())) {
        Gtk::Widget::queue_draw();
    }

    return true;
}

bool DragDropWindow::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (!m_active)
        return true;

    Gdk::Cairo::set_source_pixbuf(cr, this->m_icon, 5, DEF_ICONTOPMARGIN);
    cr->paint();

    return true;
}

