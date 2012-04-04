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

#include <GameWindow.h>
#include <Math.h>
#include <iostream>

GameWindow::GameWindow () {

    m_textureManager = new TextureManager ();
    m_textureManager->addResourceDirectory ("data/");

    // Terrain region 1.
    m_regions[0].set (0.0f, 50.0f * HEIGHTMAP_SCALE, 0, "content/textures/dirt.jpg");
    // Terrain region 2.
    m_regions[1].set (51.0f * HEIGHTMAP_SCALE, 101.0f * HEIGHTMAP_SCALE, 0, "content/textures/grass.jpg");
    // Terrain region 3.
    m_regions[2].set (102.0f * HEIGHTMAP_SCALE, 203.0f * HEIGHTMAP_SCALE, 0, "content/textures/rock.jpg");
    // Terrain region 4.
    m_regions[3].set (204.0f * HEIGHTMAP_SCALE, 255.0f * HEIGHTMAP_SCALE, 0, "content/textures/snow.jpg");

    // Light directions
    m_lightDir[0] = 0.0f;
    m_lightDir[1] = 1.0f;
    m_lightDir[2] = 0.0f;
    m_lightDir[3] = 0.0f;

}

GameWindow::~GameWindow () {

    if (m_textureManager)
        delete m_textureManager;
    m_textureManager = 0;

    for (int i = 0; i < TERRAIN_REGIONS_COUNT; ++i) {
        if (m_regions[i].texture) {
            glDeleteTextures (1, &m_regions[i].texture);
            m_regions[i].texture = 0;
        }
    }

    if (m_nullTexture) {
        glDeleteTextures (1, &m_nullTexture);
        m_nullTexture = 0;
    }

    if (m_terrainShader) {
        glUseProgram (0);
        glDeleteProgram (m_terrainShader);
        m_terrainShader = 0;
    }

    m_terrain.destroy();

}

void GameWindow::setup () {

    create (sf::VideoMode (1024, 600), "TerrainGL",
            sf::Style::Default, sf::ContextSettings (32, 0, 4));

    m_clock.restart ();

    m_splashscreen.setTexture (m_textureManager->getTexture ("data/splashscreen.jpg"));
    m_background.setTexture (m_textureManager->getTexture ("data/cubemap_posx.jpg"));
}

void GameWindow::saveScreenshot (std::string destDir = "", std::string filename = "") {

    sf::Image image;
    image = this->capture ();

    if (filename == "")
        filename = "Screenshot-" + Console::toString (m_ScreenNum) + ".jpg";


    if (image.saveToFile (destDir + filename))
        Console::message (destDir + filename + " saved!");
    m_ScreenNum++;

}

void GameWindow::keyboardEvent (sf::Keyboard::Key key) {

    if (key == sf::Keyboard::F11)
        saveScreenshot ();
    if (key == sf::Keyboard::Q)
        close ();

}

float radius = 7;
void GameWindow::run () {

    setup ();

    sf::CircleShape circle (radius);
    circle.setFillColor (sf::Color::Blue);

    while (isOpen ()) {
        // Process events
        sf::Event event;
        while (this->pollEvent (event)) {
            // Close window : exit
            if (event.type == sf::Event::Closed)
                close ();
            if (event.type == sf::Event::KeyPressed)
                keyboardEvent (event.key.code);
        }

        this->display ();
        this->clear (sf::Color (0, 0, 100));
        circle.setPosition (sf::Mouse::getPosition (*this).x - radius,
                            sf::Mouse::getPosition (*this).y - radius);

        if (m_clock.getElapsedTime ().asSeconds () < 1)
            this->draw (m_splashscreen);
        else {
            this->draw (m_background);
            this->draw (circle);
        }

    }

}
