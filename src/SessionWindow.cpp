
#include "SessionWindow.h"
#include "DockPanel.h"
#include "WindowControl.h"
#include <glibmm/i18n.h>
#include <cstdio>

WnckWindow* SessionWindow::m_activeWindow;
SessionWindow::type_signal_getactive SessionWindow::m_signal_getactive;
bool SessionWindow::m_deleteSet;

ListRow::ListRow(
        const Glib::ustring& appname,
        const Glib::ustring& parameters,
        const Glib::ustring& titlename,
        Glib::RefPtr<Gdk::Pixbuf> appIcon)
:
m_HBox(Gtk::ORIENTATION_HORIZONTAL),
m_appname(appname),
m_image(appIcon),
m_frame(),
m_launchButton(_("Launch")),
m_grid()
{
    if (!appname.empty()) {

        this->m_titlename = titlename;

        m_parameters.set_text(parameters);
        m_parameters.set_max_length(MAX_INPUT);

        m_HBox.set_margin_left(6);
        m_HBox.set_margin_right(6);
        m_HBox.set_margin_top(6);
        m_HBox.set_margin_bottom(6);

        m_grid.set_row_spacing(4);
        m_grid.set_column_spacing(4);

        this->add(m_frame);
        this->m_frame.add(m_HBox);

        this->m_HBox.add(m_image);
        this->m_HBox.add(m_grid);
        this->m_HBox.add(m_launchButton);

        this->m_parameters.set_hexpand(true);
        this->m_appname.set_halign(Gtk::Align::ALIGN_START);
        this->m_grid.attach(m_appname, 0, 0, 1, 1);
        this->m_grid.attach(m_parameters, 0, 1, 2, 1);

        m_launchButton.signal_clicked().connect(sigc::bind<ListRow&>(
                sigc::mem_fun(*this, &ListRow::on_launch_button_clicked), *this));

    }
}

void ListRow::on_launch_button_clicked(ListRow& row)
{
    Launcher::Launch(row.get_appname().c_str(),
            row.get_parameters().c_str());
}

SessionWindow::SessionWindow()
: Gtk::Window(Gtk::WindowType::WINDOW_TOPLEVEL),
m_VBox(Gtk::ORIENTATION_VERTICAL),
m_HBox(Gtk::ORIENTATION_HORIZONTAL),
m_HBoxBottom(Gtk::ORIENTATION_HORIZONTAL),
m_labelAddActive(_("Application")),
m_AddToGroupButton(_("Add to group list")),
m_OkButton(_("Ok")),
m_CancelButton(_("Cancel")),
m_ScrolledWindow(),
m_ListBox()
{

    m_deleteSet = false;

    set_border_width(10);
    set_default_size(500, 550);

    this->set_resizable(false);
    this->set_keep_above(true);
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DIALOG);

    m_VBox.set_margin_left(6);
    m_VBox.set_margin_right(6);
    m_VBox.set_margin_top(6);
    m_VBox.set_margin_bottom(6);

    m_EntryAppName.set_max_length(60);
    m_EntryAppName.set_hexpand(true);
    m_EntryAppName.set_sensitive(false);

    m_grid.set_row_spacing(4);
    m_grid.set_column_spacing(4);
    m_grid.set_hexpand(true);

    m_ScrolledWindow.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    m_ScrolledWindow.add(m_ListBox);

    add(m_VBox);
    m_VBox.add(m_grid);
    m_VBox.add(m_HBox);

    m_VBox.pack_start(m_ScrolledWindow, true, true, Gtk::PACK_EXPAND_WIDGET);
    m_VBox.add(m_HBoxBottom);

    m_HBox.set_margin_top(10);
    m_HBox.pack_start(m_AddToGroupButton, Gtk::PACK_EXPAND_WIDGET, 0);

    m_grid.attach(m_labelAddActive, 0, 0, 1, 1);
    m_grid.attach(m_EntryAppName, 1, 0, 1, 1);


    m_HBoxBottom.pack_start(m_OkButton, true, true, Gtk::PACK_SHRINK);
    m_HBoxBottom.pack_start(m_CancelButton, true, true, Gtk::PACK_SHRINK);


    m_AddToGroupButton.signal_clicked().connect(sigc::bind<guint>(
            sigc::mem_fun(*this, &SessionWindow::on_button_clicked), DEF_BUTTON_ADDTOLIST));

    m_OkButton.signal_clicked().connect(sigc::bind<guint>(
            sigc::mem_fun(*this, &SessionWindow::on_button_clicked), DEF_BUTTON_OK));

    m_CancelButton.signal_clicked().connect(sigc::bind<guint>(
            sigc::mem_fun(*this, &SessionWindow::on_button_clicked), DEF_BUTTON_CANCEL));


    // Gets the default WnckScreen on the default display.
    WnckScreen *wnckscreen = wnck_screen_get_default();
    g_signal_connect(wnckscreen, "active_window_changed",
            G_CALLBACK(SessionWindow::on_active_window_changed_callback), NULL);

    signal_getactive().connect(sigc::mem_fun(*this,
            &SessionWindow::on_signal_getactive));

    this->show_all();
}

SessionWindow::~SessionWindow()
{
    auto children = m_ListBox.get_children();
    for (int i = children.size() - 1; i >= 0; i--) {
        ListRow* row = (ListRow*) m_ListBox.get_row_at_index(i);
        delete row;
    }

}

void SessionWindow::init(DockPanel& panel, const int id)
{
    this->m_panel = &panel;
    char buff[100];
    sprintf(buff, _("Session group %d"), id);
    this->set_title(buff);

    sprintf(buff, "Session-group-%d", id);
    m_sessiongrpname = buff;
    m_deleteSet = false;

    size_t result;
    FILE* f;
    f = fopen(getFilePath().c_str(), "rb");
    if (!f)
        return;

    struct sessionGrpData st;
    GdkPixbufLoader *loader;
    GdkPixbuf *pixbuf;

    while (1) {
        result = fread(&st, sizeof (st), 1, f);

        if (feof(f) != 0)
            break;

        if (result == 0)
            g_critical("SessionWindow::init: Error reading file> fread\n");

        loader = gdk_pixbuf_loader_new();
        gdk_pixbuf_loader_write(loader, st.pixbuff, sizeof (st.pixbuff), NULL);
        pixbuf = gdk_pixbuf_loader_get_pixbuf(loader);
        auto gpixbuff = IconLoader::PixbufConvert(pixbuf);

        auto row = Gtk::manage(new ListRow(
                st.appname,
                st.parameters,
                st.titlename,
                gpixbuff));

        m_ListBox.append(*row);
    }

    m_ListBox.show_all();
    fclose(f);

}

bool SessionWindow::on_delete_event(GdkEventAny* event)
{
    m_deleteSet = true;

    this->m_panel->m_sessionWindow = nullptr;
    delete(this);

    return false;
}

void SessionWindow::on_button_clicked(guint buttonId)
{
    switch (buttonId)
    {
        case DEF_BUTTON_ADDTOLIST: this->addToList();
            break;
        case DEF_BUTTON_OK: this->save();
            break;
        case DEF_BUTTON_CANCEL: this->close();
            break;
    }
}

void SessionWindow::addToList()
{
    Glib::RefPtr<Gdk::Pixbuf> appIcon = NULLPB;
    appIcon = IconLoader::GetWindowIcon(m_window);
    appIcon = appIcon->scale_simple(DEF_ICONSIZE,
            DEF_ICONSIZE, Gdk::INTERP_BILINEAR);

    std::string the_appname;
    std::string the_instancename;
    std::string the_groupname;
    std::string the_titlename;

    if (Launcher::getAppNameByWindow(m_window,
            the_appname,
            the_instancename,
            the_groupname,
            the_titlename) == FALSE) {
        return;
    }

    auto row = Gtk::manage(new ListRow(
            m_EntryAppName.get_text(), "", the_appname, appIcon));

    m_ListBox.append(*row);
    m_ListBox.show_all();
}

Glib::ustring SessionWindow::getFilePath()
{
    char filename[PATH_MAX];
    std::string thispath = Utilities::getExecPath();
    sprintf(filename, "%s/%s/%s.bin", thispath.c_str(),
            DEF_ATTACHMENTDIR, m_sessiongrpname.c_str());

    return filename;
}

void SessionWindow::save()
{
    FILE* f;
    f = fopen(getFilePath().c_str(), "wb");
    if (!f)
        return;

    gchar* iconBuffer;
    gsize buffer_size;
    GError *error = NULL;
    struct sessionGrpData st;
    int i;
    int maxrows = m_ListBox.get_children().size();
    for (i = 0; i < maxrows; i++) {
        ListRow* row = (ListRow*) m_ListBox.get_row_at_index(i);
        Glib::RefPtr<Gdk::Pixbuf> appIcon = row->get_pixbuf();
        GdkPixbuf *pixbuf = appIcon->gobj();
        if (!gdk_pixbuf_save_to_buffer(pixbuf, &iconBuffer,
                &buffer_size, "png", &error, NULL, "100", NULL)) {

            if (error) {
                g_error_free(error);
                error = NULL;
            }

            continue;
        }

        std::string appname = row->get_appname();
        std::string parameters = row->get_parameters();
        std::string titlename = row->get_titlename();


        memcpy(st.pixbuff, iconBuffer, buffer_size);
        strcpy(st.appname, appname.c_str());
        strcpy(st.parameters, parameters.c_str());
        strcpy(st.titlename, titlename.c_str());

        fwrite(&st, sizeof (st), 1, f);
    }

    fclose(f);
    this->close();
}

SessionWindow::type_signal_getactive SessionWindow::signal_getactive()
{
    return m_signal_getactive;
}

void SessionWindow::on_signal_getactive(WnckWindow* window)
{
    if (m_deleteSet)
        return;

    if (!WNCK_IS_WINDOW(window))
        return;


    std::string appname = Launcher::getAppNameByWindow(window);
    if (appname.empty()) {
        m_EntryAppName.set_text(_("Click here to get the Application name!"));
        return;
    }

    m_EntryAppName.set_text(appname);
    m_window = window;

}

void SessionWindow::on_active_window_changed_callback(WnckScreen *screen,
        WnckWindow *previously_active_window, gpointer user_data)
{
    if (m_deleteSet)
        return;

    m_signal_getactive.emit(previously_active_window);
}
