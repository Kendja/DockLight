#ifndef ABOUT_H
#define ABOUT_H

#include <gtkmm.h>

class About : public Gtk::AboutDialog
{
private:
    Glib::ustring m_appName, m_appVersion;
    std::vector<Glib::ustring> m_authors;
    std::vector<Glib::ustring> m_artists;
    gboolean m_isInitSet;
    void init();
    void signal_about_dlg_response(int response_id);
    
    
public:
    About();
    void show();

};

#endif /* ABOUT_H */

