#include "LauncherWindow.h"
#include "Utilities.h"
#include "Defines.h"
#include "DockPosition.h"
#include "DockItem.h"

LaucherButtonBox::LaucherButtonBox(bool horizontal,
        const Glib::ustring& title,
        gint spacing,
        Gtk::ButtonBoxStyle layout)
: Gtk::Frame(title),
m_Button_createLauncher("Create Launcher"),
m_Button_testLauncher("Test Launcher"),
m_Button_Cancel("Cancel")
{
    Gtk::ButtonBox* bbox = nullptr;

    if (horizontal)
        bbox = Gtk::manage(new Gtk::ButtonBox(Gtk::ORIENTATION_HORIZONTAL));
    else
        bbox = Gtk::manage(new Gtk::ButtonBox(Gtk::ORIENTATION_VERTICAL));

    bbox->set_border_width(5);

    add(*bbox);

    /* Set the appearance of the Button Box */
    bbox->set_layout(layout);
    bbox->set_spacing(spacing);

    bbox->add(m_Button_createLauncher);
    bbox->add(m_Button_testLauncher);
    bbox->add(m_Button_Cancel);


}

LauncherWindow::LauncherWindow()
: //Gtk::Window(Gtk::WindowType::WINDOW_POPUP),
m_VBox(Gtk::ORIENTATION_VERTICAL),
m_VPaned(Gtk::ORIENTATION_VERTICAL),
m_ButtonBox(Gtk::ORIENTATION_VERTICAL),
m_labelFile("Command: "),
m_Button_File("Choose command"),
m_Button_Icon("Choose Icon"),
m_Button_create("Create Launcher"),
m_Button_test("Test Launcher"),
m_labeInfo("Create Launcher."),
m_labelName("Name: "),
m_labelComment("Comment: "),
m_labelIcon("Icon: "),
m_labelCategories("Categories: "),
m_Button_close("Close")
{
    set_title("DockLight Launcher");

    set_size_request(440, 300);
    this->set_resizable(false);

    //this->set_decorated(false);
    /* Sets the border width of the window. */
    set_border_width(12);

    m_labelComment.set_margin_left(4);
    m_labelFile.set_margin_left(4);
    m_labelCategories.set_margin_left(4);

    add(m_VBox);

    m_VBox.add(m_FrameTop);
    m_FrameTop.add(m_labeInfo);
    m_FrameTop.set_size_request(-1, 50);
    m_FrameTop.set_margin_bottom(10);

    m_VBox.add(m_Frame);
    m_Frame.add(m_grid);
    m_VBox.add(m_HBox);


    LaucherButtonBox* lb = new LaucherButtonBox(true, "", 5,
            Gtk::BUTTONBOX_SPREAD);

    m_HBox.pack_start(*Gtk::manage(
            //new LaucherButtonBox(true, "", 5,
            //Gtk::BUTTONBOX_SPREAD)
            lb
            ),
            Gtk::PACK_EXPAND_WIDGET);


    m_EntryName.set_max_length(60);
    m_EntryComment.set_max_length(150);
    m_EntryIcon.set_max_length(150);
    m_EntryFile.set_max_length(MAX_INPUT);
    m_EntryCategories.set_max_length(MAX_INPUT);



    std::string docklightIcon = Utilities::getExecPath(DEF_ICONFILE);
    this->set_icon_from_file(docklightIcon.c_str());

    m_grid.attach(m_labelName, 0, 0, 1, 1);
    m_grid.attach(m_EntryName, 1, 0, 1, 1);


    m_grid.attach(m_labelComment, 0, 1, 1, 1);
    m_grid.attach(m_EntryComment, 1, 1, 2, 1);

    m_grid.attach(m_labelFile, 0, 3, 1, 1);
    m_grid.attach(m_EntryFile, 1, 3, 1, 1);
    m_grid.attach(m_Button_File, 2, 3, 1, 1);


    m_grid.attach(m_labelIcon, 0, 6, 1, 1);
    m_grid.attach(m_EntryIcon, 1, 6, 1, 1);
    m_grid.attach(m_Button_Icon, 2, 6, 1, 1);

    m_grid.attach(m_labelCategories, 0, 8, 1, 1);
    m_grid.attach(m_EntryCategories, 1, 8, 2, 1);

    m_grid.set_row_spacing(4);
    m_grid.set_column_spacing(4);


    // lb->m_Button_createLauncher.set_sensitive(false);

    lb->m_Button_Cancel.signal_pressed().connect(sigc::mem_fun(*this, &LauncherWindow::close));
    lb->m_Button_createLauncher.signal_pressed().connect(sigc::mem_fun(*this, &LauncherWindow::on_button_createLauncher_clicked));
    lb->m_Button_testLauncher.signal_pressed().connect(sigc::mem_fun(*this, &LauncherWindow::on_button_testLauncher_clicked));


    m_Button_File.signal_clicked().connect(sigc::mem_fun(*this,
            &LauncherWindow::on_button_file_clicked));

    m_Button_Icon.signal_clicked().connect(sigc::mem_fun(*this,
            &LauncherWindow::on_button_icon_clicked));




    // int x,y;
    //  DockPosition::getCenterScreenPos(440,300,x,y );
    // move(x,y);
    m_grid.show_all();


}

void LauncherWindow::init(DockItem* dockitem)
{
    m_dockitem = dockitem;

    char message[NAME_MAX];
    sprintf(message, "Launcher for %s could not be found.\nYou need to create a Launcher for this Application.",
            dockitem->getTitle().c_str());

    m_labeInfo.set_text(message);
    m_EntryName.set_text(m_dockitem->getTitle().c_str());
    std::string defaultcomment(m_dockitem->getTitle() + " Application");
    m_EntryComment.set_text(defaultcomment.c_str());
    m_EntryFile.set_text("");
    m_EntryIcon.set_text("");
    m_EntryCategories.set_text("Other");
}

//Signal handlers

void LauncherWindow::on_button_testLauncher_clicked()
{
    if( Utilities::system( m_EntryFile.get_text().c_str() ) == 0 )
        return;
    
    Gtk::MessageDialog dialog(*this, "Launcher test Failed!", false, Gtk::MESSAGE_INFO);
    //dialog.set_icon_name()
    dialog.set_secondary_text("The command you enter is invalid.\n\n"
            "Some Applications are scripts files and others requires Administrator privileges:\n"
            "for scrips files you need to add the bash on the beginning:\n\n"
            "/bin/sh \"/usr/local/scriptfile.sh\"\n"
            "/usr/bin/bash \"/home/yoo/Develop/scriptfile\"\n\n"
            "for Applications that requires Administrator privileges:\n"
            "gksu gparted-pkexec\n\n"
            "Please make sure that the Application starts from the command line.\n"
            "Open a terminal and navigate to the directory where the application is locate\n"
            "and trying the command from there...");

    dialog.run();
}

void LauncherWindow::on_button_createLauncher_clicked()
{

    std::string desktopfile(DEF_INSTALLATIONDATA_DIR + m_dockitem->getDesktopFileName());

    char command[NAME_MAX];
    sprintf(command, "cp %stemplate.desktop %s &",
            DEF_INSTALLATIONDATA_DIR,
            desktopfile.c_str());

    Utilities::exec(command); //TODO: move to launcher


    GError *error = NULL;
    GKeyFile *key_file = g_key_file_new();
    if (FALSE == g_key_file_load_from_file(key_file,
            desktopfile.c_str(),
            GKeyFileFlags::G_KEY_FILE_NONE, &error)) {

        if (error) {
            g_error_free(error);
            error = NULL;
        }
        g_key_file_free(key_file);
        return;
    }


    // Name 
    std::string name(m_EntryName.get_text());
    if (name.empty()) {
        g_key_file_free(key_file);
        return;
    }
    g_key_file_set_string(key_file, "Desktop Entry", "Name", name.c_str());


    // Comment
    std::string comment(m_EntryComment.get_text());
    if (!comment.empty()) {
        g_key_file_set_string(key_file, "Desktop Entry", "Comment", comment.c_str());
    }

    // Command Executable
    std::string executable(m_EntryFile.get_text());
    if (executable.empty()) {
        g_key_file_free(key_file);
        return;
    }
    g_key_file_set_string(key_file, "Desktop Entry", "Exec", executable.c_str());


    // Icon
    std::string iconfile(m_EntryIcon.get_text());
    if (!iconfile.empty()) {
        g_key_file_set_string(key_file, "Desktop Entry", "Icon", iconfile.c_str());
    } else {

        //g_key_file_set_string(key_file, "Desktop Entry", "Icon", "applications-system");
        g_key_file_remove_key(key_file, "Desktop Entry", "Icon", &error);

    }

    // Categories
    std::string categories(m_EntryCategories.get_text());
    if (categories.empty()) {
        g_key_file_free(key_file);
        return;
    }
    g_key_file_set_string(key_file, "Desktop Entry", "Categories", categories.c_str());



    if (FALSE == g_key_file_save_to_file(key_file, desktopfile.c_str(), &error)) {
        if (error) {
            g_error_free(error);
            error = NULL;
        }
        g_key_file_free(key_file);
        return;
    }


    g_key_file_free(key_file);

    sprintf(command, "gksu mv %s %s", desktopfile.c_str(), DEF_LAUNCHER_DIR);
    Utilities::exec(command); // TODO: move to launcher


    Gtk::MessageDialog dialog(*this, "\n\nLauncher created successfully!\n\n", false, Gtk::MESSAGE_INFO);
    dialog.run();
    close();


}

void LauncherWindow::on_button_file_clicked()
{

    Gtk::FileChooserDialog dialog("Please choose a file",
            Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);


    //Add response buttons the the dialog:
    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Open", Gtk::RESPONSE_OK);

    //Show the dialog and wait for a user response:
    int result = dialog.run();

    if (result != Gtk::RESPONSE_OK) {
        dialog.close();
        return;
    }

    std::string filename = dialog.get_filename();

    m_EntryFile.set_text(filename.c_str());

}

void LauncherWindow::on_button_icon_clicked()
{

    Gtk::FileChooserDialog dialog("Please choose a file",
            Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);


    //Add response buttons the the dialog:
    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Open", Gtk::RESPONSE_OK);

    auto filter_text = Gtk::FileFilter::create();
    filter_text->set_name("Image files");
    filter_text->add_mime_type("image/png");
    filter_text->add_mime_type("image/ico");
    filter_text->add_mime_type("image/svg");
    dialog.add_filter(filter_text);

    //Show the dialog and wait for a user response:
    int result = dialog.run();

    if (result != Gtk::RESPONSE_OK) {
        dialog.close();
        return;
    }

    std::string filename = dialog.get_filename();

    m_EntryIcon.set_text(filename.c_str());

}