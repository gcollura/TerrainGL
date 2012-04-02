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

#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <SFML/Graphics.hpp>
#include <GL/gl.h>
#include <TextureManager.h>

struct TerrainRegion {
    float min;
    float max;
    GLuint texture;
    std::string filename;
};

class GameWindow : public sf::RenderWindow {
public:
    GameWindow ();
    virtual ~GameWindow ();

    void run ();
    void saveScreenshot (std::string destDir, std::string filename);

private:
    void setup ();
    void keyboardEvent (sf::Keyboard::Key key);

    sf::Clock m_clock;

    TextureManager *m_textureManager;
    sf::Sprite m_background;
    sf::Sprite m_splashscreen;
    int m_ScreenNum;
};

#endif // GAMEWINDOW_H
