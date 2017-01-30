/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DragDropWindow.h
 * Author: yoo
 *
 * Created on January 29, 2017, 10:58 AM
 */

#ifndef DRAGDROPWINDOW_H
#define DRAGDROPWINDOW_H

#include <gtkmm/window.h>
#include <gdkmm/pixbuf.h>

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
    bool m_active;
    bool mouseMove(int x, int y);
    Glib::RefPtr<Gdk::Pixbuf> m_icon;
    Gdk::Point m_mousePoint;
};

#endif /* DRAGDROPWINDOW_H */

