//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on November 20, 2015, 12:17 PM 
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

#include "Utilities.h"

Utilities::Utilities()
{
}

Utilities::Utilities(const Utilities& orig)
{
}

Utilities::~Utilities()
{
}

/*
 * getExecPath()
 *
 * return the executable path.
 * printf( getExecPath())
 * output:
 * /sample/path
 */
std::string Utilities::getExecPath()
{
    char result[ PATH_MAX ];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string path = result;
    std::size_t found = path.find_last_of("/\\");
    path = path.substr(0, found);

    return path; 
}

/*
 * getExecPath(const std::string str)
 *
 * return the executable path and concatenate the given string.
 * printf( getExecPath("test.txt"))
 * output:
 * /sample/path/test.txt
 */
std::string Utilities::getExecPath(const std::string str)
{
    char result[ PATH_MAX ];
    sprintf(result,"%s/%s",Utilities::getExecPath().c_str(),str.c_str());
    
    return result;
}

/*
 * std::vector<std::string> 
 * split(const std::string &text, char sep)
 *
 * return the tokens by delimeter separator parameter.
 * split("this_is_a_test,"_"))
 * output:
 * this
 * is
 * a
 * test
 */
std::vector<std::string> Utilities::split(const std::string &text, char sep)
{
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
    return tokens;
}

/*
 * RoundedRectangle helper
 */
void Utilities::RoundedRectangle(const Cairo::RefPtr<Cairo::Context>& cr,
            double x, double y, double width, double height, double radius)
    {
        // radius can be no larger than half our height or width
        radius = std::min(radius, std::min(width / 2, height / 2));
        cr->move_to(x + radius, y);
        cr->arc(x + width - radius, y + radius, radius, M_PI * 1.5, M_PI * 2);
        cr->arc(x + width - radius, y + height - radius, radius, 0, M_PI * .5);
        cr->arc(x + radius, y + height - radius, radius, M_PI * .5, M_PI);
        cr->arc(x + radius, y + radius, radius, M_PI, M_PI * 1.5);

    }



std::string Utilities::removeExtension(std::string text, const char* extension)
{
    // find extension
    std::size_t found = text.find(extension);
    if (found != std::string::npos) {
        // let's replace the extension with an empty string:
        text.replace(text.find(extension),
                text.length(), "");
    }
    return text;
}

std::string Utilities::removeExtension(std::string text, const std::string extensions[])
{
    // find extensions
    for (int i = 0; i < extensions->size() - 1; i++) {
        std::string e = extensions[i];
        std::size_t found = text.find(e);
        if (found != std::string::npos) {
            // let's replace the extension with an empty string:
            text.replace(text.find(e),
                    text.length(), "");
        }
    }
    return text;
}


/*
 * 
 * stringToLower helper.
 * 
 */
std::string Utilities::stringToLower(const char* strp)
{
    std::string str = strp;
    transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str;
}

