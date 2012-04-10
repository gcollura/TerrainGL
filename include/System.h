//
//  Copyright (C) 2012 Giulio Collura
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

#ifndef CONSOLE_H
#define CONSOLE_H

#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <stdexcept>

typedef std::string string;
typedef std::runtime_error error;

enum LogLevel {
    DEFAULT,
    MESSAGE,
    WARNING,
    ERROR,
    DEBUGG // Different from DEBUG (macro)
};

struct Color {
    static const string White;
    static const string Cyan;
    static const string Yellow;
    static const string Red;
    static const string Green;
};

class Console {

public:
    Console();
    virtual ~Console();

    static void writeLine (string message,
                           LogLevel level = DEFAULT);
    static void write (string message, LogLevel level = DEFAULT);
    static void message (string message);
    static void message (int number);
    static void message (float number);
    static void error (string message);
    static void warning (string message);
    static void debug (string message);

    static string toString (const int number);
    static string toString (const float number);

private:
    static string getTime ();
};

class System {

public:
    static string readTextFile (string filename);
    static bool writeTextFile (string filename, string text);
    static bool writeTextFile (string filename, char* text);
};

#endif // CONSOLE_H
