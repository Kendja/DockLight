/* 
 * File:   SessionWindow.h
 * Author: yoo
 *
 * Created on February 21, 2017, 2:42 PM
 */

#ifndef SESSIONWINDOW_H
#define	SESSIONWINDOW_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include "Utilities.h"
#include <libwnck/libwnck.h>

#include <gtkmm/window.h>
#include <gtkmm/widget.h>
#include <gtkmm/grid.h>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/listbox.h>
#include "IconLoader.h"

class DockPanel;

#define DEF_BUTTON_ADDTOLIST 3
#define DEF_BUTTON_OK 4
#define DEF_BUTTON_CANCEL 5

struct sessionGrpData
{
    guint8 pixbuff[ 1024 * 6 ]={0};
    char appname[60]={'\0'};
    char parameters[MAX_INPUT]={'\0'};
    char titlename[MAX_INPUT]={'\0'};
};

class ListRow : public Gtk::ListBoxRow
{
public:
    ListRow(const Glib::ustring& appname,
            const Glib::ustring& parameters,
            const Glib::ustring& titlename,
            Glib::RefPtr<Gdk::Pixbuf> appIcon);

    Glib::ustring get_appname() const
    {
        return m_appname.get_text();
    }

    Glib::ustring get_parameters() const
    {
        return m_parameters.get_text();
    }

    Glib::ustring get_titlename() const
    {
        return m_titlename;
    }
    
    Glib::RefPtr<Gdk::Pixbuf> get_pixbuf()
    {
        return m_image.get_pixbuf();
    }
protected:
    void on_launch_button_clicked(ListRow& row);
private:
    Gtk::Button m_launchButton;
    Gtk::Image m_image;
    Gtk::Grid m_grid;
    Gtk::Frame m_frame;
    Gtk::Entry m_parameters;
    Gtk::Box m_HBox;
    Gtk::Label m_appname;
    
    Glib::ustring m_titlename;

};

class SessionWindow : public Gtk::Window
{
public:

    SessionWindow();
    ~SessionWindow();
    void init(DockPanel& panel, const int id);

protected:
    // Signal handlers.
    bool on_delete_event(GdkEventAny* event);
    void on_button_clicked(guint buttonId);

    static void on_active_window_changed_callback(WnckScreen *screen,
            WnckWindow *previously_active_window, gpointer user_data);

    static WnckWindow* m_activeWindow;
private:
    //signal accessor:
    typedef sigc::signal<void, WnckWindow*> type_signal_getactive;
    type_signal_getactive signal_getactive();
    static type_signal_getactive m_signal_getactive;
    void on_signal_getactive(WnckWindow* window);

    Glib::ustring getFilePath() ;
    void save();
    void addToList();

    WnckWindow* m_window = nullptr;
    DockPanel* m_panel;
    static bool m_deleteSet;
    std::string m_sessiongrpname;

    // Member data.
    Gtk::Grid m_grid;
    Gtk::Box m_VBox;
    Gtk::Box m_HBox;
    Gtk::Box m_HBoxBottom;
    Gtk::Label m_labelAddActive;
    Gtk::Entry m_EntryAppName;
    Gtk::Button m_OkButton;
    Gtk::Button m_CancelButton;
    Gtk::Button m_AddToGroupButton;
    Gtk::ScrolledWindow m_ScrolledWindow;
    Gtk::ListBox m_ListBox;

};

#endif	/* SESSIONWINDOW_H */

