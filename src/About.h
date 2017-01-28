#ifndef ABOUT_H
#define ABOUT_H

#include <gtkmm.h>

class About : public Gtk::AboutDialog
{
private:
    Glib::ustring m_appName, m_appVersion;
    std::vector<Glib::ustring> m_authors;
    std::vector<Glib::ustring> m_artists;
    std::string m_logoPath;
    gboolean m_isInitSet;
    void init(Window* window);
    void signal_about_dlg_response(int response_id);
    
    
public:
    About();
    void show(Gtk::Window* window);

};

#endif /* ABOUT_H */

