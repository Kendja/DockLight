//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on January  2017
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//****************************************************************

#include "Configuration.h"
#include "Utilities.h"
#include  "Defines.h" 
#include <locale>

namespace Configuration {

    Theme m_theme;
    bool m_autohide = false;
    bool m_panelScaleOnHover;
    int m_panelLinesBinaryValue = 0;
    int m_previewLinesBinaryValue = 0;

    Color::Color() {
    };

    Color::Color(const double red, const double green, const double blue, const double alpha) {
        set(red, green, blue, alpha);
    }

    void Color::set(const double red, const double green, const double blue, const double alpha) {
        this->red = red;
        this->green = green;
        this->blue = blue;
        this->alpha = alpha;

    }

    ColorWindow::ColorWindow() {
    }

    ColorWindow::ColorWindow(const bool enabled, const Color background, const Color foreground,
            const double lineWith, const double roundedRatious) {
        this->set(enabled, background, foreground, lineWith, roundedRatious);
    }

    void ColorWindow::set(const bool enabled, const Color background, const Color foreground,
            const double lineWith, const double roundedRatious) {

        this->m_enabled = enabled;
        this->m_background = background;
        this->m_foreground = foreground;
        this->m_lineWith = lineWith;
        this->m_roundedRatious = roundedRatious;
    }

    /**
     * Constructs the default theme
     */
    Theme::Theme() {
        Color backColor(0.27, 0.44, 0.58, 1.0);
        Color foreColor(1.0, 1.0, 1.0, 1.0);

        Color backSelector(1.9, 1.0, 1.0, 0.3);
        Color foreSelector(1.1, 1.1, 1.0, 1.0);

        this->m_window = new ColorWindow();
        this->m_panel = new ColorWindow(true, backColor, foreColor, 0.7, 4.0);
        this->m_panelSelector = new ColorWindow(true, backSelector, foreSelector, 1.5, 4.0);
        this->m_panelTitle = new ColorWindow(true, backColor, foreColor, 0.0, 4.0);
        this->m_panelTitleText = new ColorWindow(true, Color(), Color(1.0, 1.0, 1.0, 1.0), 0.0, 0.0);
        this->m_preview = new ColorWindow(true, backColor, foreColor, 0.7, 4.0);
        this->m_previewTitle = new ColorWindow(true, Color(0.0, 0.0, 1.0, 0.0), Color(1.0, 1.0, 1.0, 0.0), 1.0, 0.0);
        this->m_previewTitleText = new ColorWindow(true, Color(), Color(), 0.0, 0.0);
        this->m_previewSelector = new ColorWindow(true, backSelector, foreSelector, 2.0, 4.0);
        this->m_previewSelectorClose = new ColorWindow(true, Color(0.0, 0.50, 0.0, 1.0), foreColor, 4.0, 4.0);

    }

    Theme getTheme() {
        return m_theme;
    };

    void getColorFromString(const std::string& s, bool& enable,
            double& lineWidth, double& roundedRadius,
            Color& background, Color& foreground) {

        if (!Utilities::isNumeric(s)) {
            g_critical("The color string is not numeric: [%s]\n", s.c_str());
        }

        std::string currentLocale = setlocale(LC_NUMERIC,NULL);
        setlocale(LC_NUMERIC,"C");
                
        const int MAXBUFF = 50;
        int maxlength = s.size();
        std::string token = "";
        double values[MAXBUFF] = {0};
        int index = 0;
        for (int i = 0; i < maxlength; i++) {

            if (index < MAXBUFF) {
                char c = s[i];
                if (c != ',' && c != ' ') {
                    token += c;
                }

                if ((c == ',') || i + 1 == maxlength) {

                    double value = 0.0;
                    try {
                       value = std::stod(token);
                       values[ index ] = value;
                       //g_print(" %d........:%f (%s)\n", index, value,token.c_str());
                       
                    } catch (std::invalid_argument) {
                        g_critical("getColorFromString: can't convert the token: %s\n", s.c_str());
                    }


                    token = "";
                    index++;
                }
            }
        }

        lineWidth = values[0];
        roundedRadius = values[1];
        enable = values[2];
        background.red = values[3];
        background.green = values[4];
        background.blue = values[5];
        background.alpha = values[6];
        foreground.red = values[7];
        foreground.green = values[8];
        foreground.blue = values[9];
        foreground.alpha = values[10];

        setlocale(LC_NUMERIC,currentLocale.c_str());


    }


   void Load() {

        std::string filepath = Utilities::getExecPath(DEF_INITNAME);
        GError *error = NULL;
        GKeyFile *key_file = g_key_file_new();

        gboolean found = g_key_file_load_from_file(key_file,
                filepath.c_str(), GKeyFileFlags::G_KEY_FILE_NONE, &error);

        if (found) {

            if (error) {
                g_critical("Load configuration fails! %s\n", error->message);
                g_error_free(error);
                error = NULL;
                g_key_file_free(key_file);
                return;
            }
        }

        m_autohide = g_key_file_get_boolean(key_file, "DockLight", "Autohide", &error);
        if (error) {
            g_critical("configuration Key (Autohide) could not be found!! %s\n", error->message);
            g_error_free(error);
            error = NULL;
            g_key_file_free(key_file);
            return;
        }





        char* themename = g_key_file_get_string(key_file, "DockLight", "Theme", &error);
        if (themename == NULL) {
            if (error) {
                g_print("configuration Key (Theme) could not be found. use default\n");
                g_error_free(error);
                error = NULL;

            }

            g_key_file_free(key_file);
            return;
        }



        bool panelScaleOnHover = g_key_file_get_boolean(key_file, themename, "PanelScaleOnHover", &error);

        if (error) {
            g_print("configuration Key (Theme PanelScaleOnHover) could not be found. use default\n");
            g_error_free(error);
            error = NULL;
            panelScaleOnHover = false;

        }


        int panelLinesBinaryValue = g_key_file_get_integer(key_file, themename, "PanelLinesBinaryValue", &error);

        if (error) {
            g_print("configuration Key (Theme %s PanelLinesBinaryValue ) could not be found. use default\n", themename);
            g_error_free(error);
            error = NULL;
        }

        int panelSelectorLinesBinaryValue = g_key_file_get_integer(key_file, themename, "PanelSelectorLinesBinaryValue", &error);

        if (error) {
            g_print("configuration Key (Theme %s PanelSelectorLinesBinaryValue ) could not be found. use default\n", themename);
            g_error_free(error);
            error = NULL;
        }


        int previewlLinesBinaryValue = g_key_file_get_integer(key_file, themename, "PreviewLinesBinaryValue", &error);

        if (error) {
            g_print("configuration Key (Theme %s PreviewLinesBinaryValue ) could not be found. use default\n", themename);
            g_error_free(error);
            error = NULL;
        }


        int previewSelectorLinesBinaryValue = g_key_file_get_integer(key_file, themename, "PreviewSelectorLinesBinaryValue", &error);

        if (error) {
            g_print("configuration Key (Theme %s PreviewSelectorLinesBinaryValue ) could not be found. use default\n", themename);
            g_error_free(error);
            error = NULL;
        }



        char* windowString = g_key_file_get_string(key_file, themename, "Window", &error);
        if (windowString == NULL) {
            if (error) {
                g_print("configuration Key (Theme %s Window ) could not be found. use default\n", themename);
                g_error_free(error);
                error = NULL;
            }
        }
        char* panelString = g_key_file_get_string(key_file, themename, "Panel", &error);
        if (panelString == NULL) {
            if (error) {
                g_print("configuration Key (Theme %s Panel ) could not be found. use default\n", themename);
                g_error_free(error);
                error = NULL;
            }
        }


        char* PanelTitleString = g_key_file_get_string(key_file, themename, "PanelTitle", &error);
        if (PanelTitleString == NULL) {
            if (error) {
                g_print("configuration Key (Theme %s PanelTitle ) could not be found. use default\n", themename);
                g_error_free(error);
                error = NULL;
            }
        }
        char* PanelTitleTextString = g_key_file_get_string(key_file, themename, "PanelTitleText", &error);
        if (PanelTitleTextString == NULL) {
            if (error) {
                g_print("configuration Key (Theme %s PanelTitleText ) could not be found. use default\n", themename);
                g_error_free(error);
                error = NULL;
            }
        }

        char* previewString = g_key_file_get_string(key_file, themename, "Preview", &error);
        if (previewString == NULL) {
            if (error) {
                g_print("configuration Key (Theme %s Preview ) could not be found. use default\n", themename);
                g_error_free(error);
                error = NULL;
            }
        }



        char* PanelSelectorString = g_key_file_get_string(key_file, themename, "PanelSelector", &error);
        if (PanelSelectorString == NULL) {
            if (error) {
                g_print("configuration Key (Theme %s PanelSelector ) could not be found. use default\n", themename);
                g_error_free(error);
                error = NULL;
            }
        }



        char* PreviewTitleTextString = g_key_file_get_string(key_file, themename, "PreviewTitleText", &error);
        if (PreviewTitleTextString == NULL) {
            if (error) {
                g_print("configuration Key (Theme %s PreviewTitleText ) could not be found. use default\n", themename);
                g_error_free(error);
                error = NULL;
            }
        }

        char* PreviewSelectorString = g_key_file_get_string(key_file, themename, "PreviewSelector", &error);
        if (PreviewSelectorString == NULL) {
            if (error) {
                g_print("configuration Key (Theme %s PreviewSelector ) could not be found. use default\n", themename);
                g_error_free(error);
                error = NULL;
            }
        }

        bool enable;
        double lineWidth;
        double roundedRadius;
        Color background;
        Color foreground;

        m_theme.setPanelScaleOnhover(panelScaleOnHover);


        if (windowString != NULL) {
            getColorFromString(windowString, enable, lineWidth, roundedRadius, background, foreground);
            m_theme.forWindow().set(enable, background, foreground, lineWidth, roundedRadius);
        }
        if (panelString != NULL) {
            getColorFromString(panelString, enable, lineWidth, roundedRadius, background, foreground);
            m_theme.forPanel().set(enable, background, foreground, lineWidth, roundedRadius);
        }
        if (PanelTitleString != NULL) {
            getColorFromString(PanelTitleString, enable, lineWidth, roundedRadius, background, foreground);
            m_theme.forPanelTitle().set(enable, background, foreground, lineWidth, roundedRadius);
        }
        if (PanelTitleTextString != NULL) {
            getColorFromString(PanelTitleTextString, enable, lineWidth, roundedRadius, background, foreground);
            m_theme.forPanelTitleText().set(enable, background, foreground, lineWidth, roundedRadius);

        }
        if (previewString != NULL) {
            getColorFromString(previewString, enable, lineWidth, roundedRadius, background, foreground);
            m_theme.forPreview().set(enable, background, foreground, lineWidth, roundedRadius);
        }

        if (PanelSelectorString != NULL) {
            getColorFromString(PanelSelectorString, enable, lineWidth, roundedRadius, background, foreground);
            m_theme.forPanelSelector().set(enable, background, foreground, lineWidth, roundedRadius);
        }

        if (PreviewTitleTextString != NULL) {
            getColorFromString(PreviewTitleTextString, enable, lineWidth, roundedRadius, background, foreground);
            m_theme.forPreviewTitleText().set(enable, background, foreground, lineWidth, roundedRadius);
        }

        if (PreviewSelectorString != NULL) {
            getColorFromString(PreviewSelectorString, enable, lineWidth, roundedRadius, background, foreground);
            m_theme.forPreviewSelector().set(enable, background, foreground, lineWidth, roundedRadius);
        }


        m_theme.setPanelBinaryValue(panelLinesBinaryValue);

        m_theme.setPanelSelectorBinaryValue(panelSelectorLinesBinaryValue);

        m_theme.setPreviewBinaryValue(previewlLinesBinaryValue);
        m_theme.setPreviewSelectorBinaryValue(previewSelectorLinesBinaryValue);


        g_key_file_free(key_file);
        return;

    }

    bool getAutohide() {
        return m_autohide;
    }

    void setAutohide(bool value) {
        m_autohide = value;
    }




}