//
//  Copyright (C) 2011 Giulio Collura
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

#include <System.h>

const string Color::White = "";
const string Color::Cyan = "";
const string Color::Yellow = "";
const string Color::Red = "";
const string Color::Green = "";

Console::Console () {

}

Console::~Console () {

}

void Console::write (string message, LogLevel level) {

    string color;

    switch (level) {
    case DEFAULT:
        color = Color::White;
        break;
    case MESSAGE:
        color = Color::Cyan;
        break;
    case WARNING:
        color = Color::Yellow;
        break;
    case ERROR:
        color = Color::Red;
        break;
    case DEBUGG:
        color = Color::Green;
        break;
    }

    std::cout << color << message;

}

void Console::writeLine (string message, LogLevel level) {

    write (message, level);
    std::cout << std::endl;

}

void Console::message (string message) {

    string intro = "[MESSAGE " + getTime () + "] ";
    writeLine (intro + message, MESSAGE);

}

void Console::message (int number) {
    message (toString (number));
}

void Console::message (float number) {
    message (toString (number));
}

void Console::warning (string message) {

    string intro = "[WARNING " + getTime () + "] ";
    writeLine (intro + message, WARNING);
}

void Console::error (string message) {

    string intro = "[ERROR " + getTime () + "] ";
    writeLine (intro + message, ERROR);
}

void Console::debug (string message) {
#if DEBUG
    string intro = "[DEBUG " + getTime () + "] ";
    writeLine (intro + message, DEBUGG);
#endif
}

string Console::toString (const int number) {

    std::stringstream ss;
    ss << number;
    return ss.str ();

}

string Console::toString (const float number) {

    std::stringstream ss;
    ss << number;
    return ss.str ();

}


string Console::getTime () {

    time_t rawtime;
    time (&rawtime);
    string time = ctime (&rawtime);
    // Remove trailing endline
    time = time.substr (0, time.length() - 1);

    return time;
}
