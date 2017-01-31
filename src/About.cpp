#include "About.h"
#include "Utilities.h"
#include "Defines.h"
#include "DockPosition.h"
#include  <glibmm/i18n.h>
#include  "../config.h"


About::About()
: m_appName(PACKAGE_NAME),
m_appVersion(VERSION),
m_isInitSet(false),
m_logoPath(Utilities::getExecPath(DEF_LOGONAME))
{
    this->signal_response().
            connect(sigc::mem_fun(this, &About::signal_about_dlg_response));

}

void About::signal_about_dlg_response(int response_id)
{

    if (Gtk::RESPONSE_CLOSE)
        this->hide();
}

void About::show(Gtk::Window* gtkwindow)
{
    Window *window = static_cast<Gtk::Window*> (gtkwindow);
    init(window);
    
    this->show_all();
    int x, y;
    DockPosition::getCenterScreenPos(this->get_width(), this->get_height(), x, y);
    this->move(x, y);
    
   
    Gtk::AboutDialog::show();
}

void About::init(Window* window)
{
    if (m_isInitSet)
        return;

    if (!m_isInitSet) {
        this->m_isInitSet = true;
    }

    this->set_transient_for(*window);

    this->set_name(m_appName);
    this->set_version(m_appVersion);
    this->m_authors.push_back("Juan González https://github.com/yoosamui/docklight");
    this->set_authors(m_authors);
    this->m_artists.push_back("Juan Álvarez https://github.com/yoosamui/docklight");
    this->set_artists(m_artists);
    this->set_translator_credits("Juan Álvarez\nSuppaluk Srisaeng");
    this->set_website_label("docklight on github");
    this->set_website("https://github.com/yoosamui/docklight");
    
    this->set_license_type(Gtk::License::LICENSE_GPL_3_0);
    //this->set_license("nullptr");

    Glib::RefPtr< Gdk::Pixbuf > logo;
    
    
    try {
        logo = logo->create_from_file(this->m_logoPath);

    } catch (Glib::FileError fex) {
        g_critical("About Logo not found\n");

    } catch (Gdk::PixbufError bex) {
        g_critical("About logo Attachment file PixbufError \n");
    }

    this->set_logo(logo);
     

    std::string docklightIcon = Utilities::getExecPath(DEF_ICONNAME);
    this->set_icon_from_file(docklightIcon.c_str());


    this->set_comments(_("This program is a lightweight Dock Launcher for opening common applications for the GNOME Desktop"));
    this->set_copyright("Copyright © 2016-2017 Juan González");
    this->set_wrap_license(true);

    
}
