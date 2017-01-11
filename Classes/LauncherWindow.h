
#ifndef LAUNCHERWINDOW_H
#define LAUNCHERWINDOW_H

#include <gtkmm.h>

class DockItem;

class LaucherButtonBox : public Gtk::Frame
{
public:
  LaucherButtonBox(bool horizontal,
       const Glib::ustring& title,
       gint spacing,
       Gtk::ButtonBoxStyle layout);
Gtk::Button m_Button_createLauncher, m_Button_Cancel, m_Button_testLauncher;
protected:
  
};

class LauncherWindow : public Gtk::Window
{
public:

    LauncherWindow();
    void init(DockItem* dockitem);
    

protected:
    DockItem* m_dockitem;
    
    //Signal handlers:
    void on_button_file_clicked();
    void on_button_icon_clicked();
    void on_button_createLauncher_clicked();
    void on_button_testLauncher_clicked();
    void on_button_CategoriesLink_clicked();
    bool isFormFieldsValid();
    void InvalidFormFieldsMessage();

    //Child widgets:
    Gtk::Grid m_grid;
    
    Gtk::Box m_VBox;
    Gtk::Box m_HBox;
    Gtk::Box m_HBox2;
    
    Gtk::Label m_labelName;
    Gtk::Label m_labelFile;
    Gtk::Label m_labelComment;
    Gtk::Label m_labelIcon;
    Gtk::Label m_labeInfo;
    Gtk::Label m_labelCategories;
    
    
    
    
    Gtk::Entry m_EntryName;
    Gtk::Entry m_EntryComment;
    Gtk::Entry m_EntryFile;
    Gtk::Entry m_EntryIcon;
    Gtk::Entry m_EntryCategories;
    
    
    
    Gtk::ButtonBox m_ButtonBox;
    Gtk::Button m_Button_File, m_Button_CategoriesLink;
    
    Gtk::Button m_Button_Icon;
    Gtk::Button m_Button_close;
    Gtk::Button m_Button_create;
    Gtk::Button m_Button_test;
    
    Gtk::Separator m_separator1;
    
    //Child widgets:
    Gtk::Paned m_VPaned;
    Gtk::Frame m_FrameTop;
    Gtk::Frame m_Frame;
};


#endif /* LAUNCHERWINDOW_H */

