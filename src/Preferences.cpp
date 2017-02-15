
#include "Preferences.h"
#include "DockPanel.h"
#include "Configuration.h"
#include  <glibmm/i18n.h>

ExampleRow::ExampleRow(const Glib::ustring& text, int sort_id)
: m_label(text),
m_sort_id(sort_id) {
    if (!text.empty()) {
        add(m_label);
        m_label.show();
    }
}

Preferences::Preferences()
:
m_ScrolledWindow(),
m_VBox_Row(Gtk::ORIENTATION_VERTICAL),
m_HBox_Row(Gtk::ORIENTATION_HORIZONTAL),
m_VBox(Gtk::ORIENTATION_VERTICAL),
m_CheckButton_Autohide(_("Autohide on start"), /* mnemonic= */ false),
m_ListBox(), m_FrameTop(), m_Frame() {

    set_title(_("Preferences"));
    set_default_size(300, 300);

    this->set_resizable(false);
    this->set_keep_above(true);
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DIALOG);


    this->signal_delete_event().
            connect(sigc::mem_fun(*this, &Preferences::on_delete_event));

    //Check button for single click.
    //  m_CheckButton_Autohide.set_active(m_ListBox.get_activate_on_single_click());
    m_CheckButton_Autohide.signal_toggled().
            connect(sigc::mem_fun(*this, &Preferences::on_autohide_click_toggled));

    m_ListBox.signal_row_selected().
            connect(sigc::mem_fun(*this, &Preferences::on_row_selected));

    // Put the ListBox in a ScrolledWindow.
    m_ScrolledWindow.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    m_ScrolledWindow.add(m_ListBox);


    add(m_VBox);
    m_VBox.set_size_request(300, 300);
    m_VBox.set_margin_top(4);
    m_VBox.set_margin_left(4);
    m_VBox.set_margin_right(4);
    m_VBox.set_margin_bottom(4);


    m_VBox.pack_start(m_CheckButton_Autohide, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_ScrolledWindow, true, true, Gtk::PACK_SHRINK);

    show_all();

};

Preferences::~Preferences() {

}
//TODO change this with a Signal handler.

void Preferences::init(DockPanel& panel) {
    this->m_panel = &panel;



    std::string filepath = Utilities::getExecPath(DEF_INITNAME);
    GError *error = NULL;
    GKeyFile *key_file = g_key_file_new();

    gboolean found = g_key_file_load_from_file(key_file,
            filepath.c_str(), GKeyFileFlags::G_KEY_FILE_NONE, &error);

    if (found) {

        if (error) {
            g_critical("Preferences::init:Load configuration fails! %s\n", error->message);
            g_error_free(error);
            error = NULL;
            g_key_file_free(key_file);
            return;
        }
    }

    bool autohide = g_key_file_get_boolean(key_file, "DockLight", "Autohide", &error);
    if (error) {
        g_critical("configuration Key (Autohide) could not be found!! %s\n", error->message);
        g_error_free(error);
        error = NULL;
    }

    m_CheckButton_Autohide.set_active(autohide);




    guint group, key;
    gsize* length;
    gsize num_groups, num_keys;
    gchar** groups, **keys, *value;

    groups = g_key_file_get_groups(key_file, &num_groups);
    //    g_print(".........%d\n", (int) num_groups);
    for (group = 1; group < num_groups; group++) {
        // std::string grpname(group);
        //  g_print("group %u/%u: \t%s\n", group, (num_groups - 1), groups[group]);
        //g_print("group %u,  %d %d\n%s", group, (num_groups - 1), groups[group]);

        auto row = Gtk::manage(new ExampleRow(groups[group], 1));
        m_ListBox.append(*row);


    }




    g_strfreev(groups);
    g_key_file_free(key_file);


}
//TODO change this with a Signal handler.

bool Preferences::on_delete_event(GdkEventAny* event) {


    this->m_panel->m_preferences = nullptr;
    delete(this);
    return true;
}

void Preferences::on_row_selected(Gtk::ListBoxRow* row) {


    ExampleRow* erow = (ExampleRow*) row;
    std::string themename = erow->getLabelText();
    //std::cout << "selected row: " << row->get_index() << themename << std::endl;

    std::string filepath = Utilities::getExecPath(DEF_INITNAME);
    GError *error = NULL;
    GKeyFile *key_file = g_key_file_new();

    gboolean found = g_key_file_load_from_file(key_file,
            filepath.c_str(), GKeyFileFlags::G_KEY_FILE_NONE, &error);

    if (!found) {
        if (error) {
            g_critical("Preferences::init:Load configuration fails! %s\n", error->message);
            g_error_free(error);
            error = NULL;
            g_key_file_free(key_file);
            return;
        }
    }



    g_key_file_set_string(key_file,
            "DockLight",
            "Theme",
            themename.c_str());
    char* value = g_key_file_get_string(key_file, "DockLight", "Theme", &error);

    if (error) {
        g_critical("Preferences::init:Load configuration fails! %s\n", error->message);
        g_error_free(error);
        error = NULL;
        g_key_file_free(key_file);
        return;
    }

    gboolean saved = g_key_file_save_to_file(key_file, filepath.c_str(), &error);
    if (!saved) {

        if (error) {
            g_critical("Preferences::init:SAVED configuration fails! %s\n", error->message);
            g_error_free(error);
            error = NULL;
            g_key_file_free(key_file);
            return;
        }
    }


    g_key_file_free(key_file);
    Configuration::Load();

}

void Preferences::on_autohide_click_toggled() {


    std::string filepath = Utilities::getExecPath(DEF_INITNAME);
    GError *error = NULL;
    GKeyFile *key_file = g_key_file_new();

    gboolean found = g_key_file_load_from_file(key_file,
            filepath.c_str(), GKeyFileFlags::G_KEY_FILE_NONE, &error);

    if (!found) {
        if (error) {
            g_critical("Preferences::init:Load configuration fails! %s\n", error->message);
            g_error_free(error);
            error = NULL;
            g_key_file_free(key_file);
            return;
        }
    }

    g_key_file_set_boolean(key_file,
            "DockLight",
            "Autohide",
            m_CheckButton_Autohide.get_active());

    gboolean saved = g_key_file_save_to_file(key_file, filepath.c_str(), &error);
    if (!saved) {

        if (error) {
            g_critical("Preferences::init:SAVED configuration fails! %s\n", error->message);
            g_error_free(error);
            error = NULL;
            g_key_file_free(key_file);
            return;
        }
    }


    g_key_file_free(key_file);
}
