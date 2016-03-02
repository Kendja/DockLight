/* 
 * File:   Utilities.cpp
 * Author: yoo
 * 
 * Created on February 22, 2016, 4:40 PM
 */

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
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string path = result;
    std::size_t found = path.find_last_of("/\\");
    path = path.substr(0, found);

    return path + "/" + str;
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