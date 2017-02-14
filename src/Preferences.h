/* 
 * File:   Preferences.h
 * Author: yoo
 *
 * Created on February 13, 2017, 6:12 PM
 */

#ifndef PREFERENCES_H
#define	PREFERENCES_H

#include <gtkmm.h>

class DockPanel;

class Preferences : public Gtk::Window {
public:

    Preferences();
    virtual ~Preferences();

    //TODO change this with a Signal handler.
    void init(DockPanel& panel);

protected:
        // Signal handlers.
    bool on_delete_event(GdkEventAny* event);
    void on_row_selected(Gtk::ListBoxRow* row);
    void on_autohide_click_toggled();

private:

    DockPanel* m_panel;


    // Member data.
    Gtk::Grid m_grid;
    Gtk::Box m_VBox;
    Gtk::Frame m_FrameTop;
    Gtk::Frame m_Frame;
    
    
     
     
    Gtk::Box m_VBox1;
    Gtk::Box m_VBox2;
    Gtk::Box m_VBox_Row;
    Gtk::Box m_HBox_Row;
  
    Gtk::ListBox m_ListBox;
    Gtk::Label m_Label_Row;
    Gtk::CheckButton m_CheckButton_Row1;
    Gtk::CheckButton m_CheckButton_Row2;

    Gtk::CheckButton m_CheckButton_Autohide;
    
    Gtk::ScrolledWindow m_ScrolledWindow;
};



class ExampleRow : public Gtk::ListBoxRow
{
public:
  ExampleRow(const Glib::ustring& text, int sort_id);

  Glib::ustring get_text() const { return m_label.get_text(); }
  void set_text(const Glib::ustring & text) { m_label.set_text(text); }

  int get_sort_id() const { return m_sort_id; }
  void set_sort_id(int sort_id) { m_sort_id = sort_id; }

  std::string getLabelText(){
      return m_label.get_text();
  }
private:

    Gtk::Label m_label;
  int m_sort_id;
};



#endif	/* PREFERENCES_H */

