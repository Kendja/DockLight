#include "About.h"
#include "Utilities.h"
#include "Defines.h"
#include  <glibmm/i18n.h>

About::About()
: m_appName(APPNAME),
m_appVersion(VERSION),
m_isInitSet(false)
{

    this->signal_response().
            connect(sigc::mem_fun(this, &About::signal_about_dlg_response));
}

void About::signal_about_dlg_response(int response_id)
{

    if (Gtk::RESPONSE_CLOSE)
        this->hide();
}

void About::show()
{
    init();
    Gtk::AboutDialog::show();
}

void About::init()
{
    if (m_isInitSet) 
        return;
    
    if (!m_isInitSet) {
        this->m_isInitSet = true;
    }
    

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
    Glib::RefPtr< Gdk::Pixbuf > logo;

    try {
        logo = logo->create_from_file(Utilities::getExecPath("logo.png"));

    } catch (Glib::FileError fex) {
        g_critical("About Logo not found\n");

    } catch (Gdk::PixbufError bex) {
        g_critical("About logo Attachment file PixbufError \n");
    }

    this->set_logo(logo);
    this->set_icon_name(ICONNAME);
    this->set_comments(_("docklight is a lightweight Dock Panel for opening common applications for the GNOME Desktop"));
    this->set_copyright("Copyright © 2016-2017 Juan González");
    this->set_wrap_license(true);


}
