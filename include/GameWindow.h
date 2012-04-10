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
#include <OpenGL.h>
#include <TextureManager.h>
#include <Terrain.h>
#include <Terrain2.h>
#include <Camera.h>
#include <System.h>
#include <Water.h>


struct TerrainRegion {

    float min;
    float max;
    GLuint texture;
    string filename;

    inline void set (float g_min, float g_max,
                     GLuint g_texture, string g_filename) {
        min = g_min;
        max = g_max;
        texture = g_texture;
        filename = g_filename;
    }

};

struct BackScene {
    GLuint texture;
    string filename;

    inline void set (GLuint g_texture, string g_filename) {
        texture = g_texture;
        filename = g_filename;
    }
};

class GameWindow : public sf::RenderWindow {
public:
    GameWindow ();
    virtual ~GameWindow ();

    void run ();
    void saveScreenshot (string destDir, string filename);

protected:
    void render ();

private:
    bool init ();
    bool initGL ();
    void keyboardEvent (sf::Keyboard::Key key);
    void getInput ();

    GLuint compileShader (GLenum type, const GLchar* g_source, GLint length);
    GLuint createNullTexture (int g_width, int g_height);
    void generateTerrain ();
    GLuint linkShaders (GLuint vertShader, GLuint fragShader);
    GLuint loadShaderProgram (string fragFilename, string vertFilename, string &infoLog);
    GLuint loadTexture (string filename);
    GLuint loadTexture (string filename, GLint magFilter, GLint minFilter,
                        GLint wrapS, GLint wrapT);
    void bindTexture (GLuint texture, GLuint unit);
    void renderTerrain ();
    void updateTerrainShaderParameters ();
    void updateCameraPosition ();

    sf::Clock m_clock;

    TextureManager *m_textureManager;
    sf::Sprite m_splashscreen;

    int m_ScreenNum;
    float m_maxAnisotrophy;
    bool m_disableColorMaps;
    float m_lightDir[4];
    GLuint m_nullTexture;
    GLuint m_terrainShader;
    Terrain m_terrain;
    Terrain2 m_terrain2;
    Water m_water;
    Camera m_camera;
    Vector3 m_cameraBoundsMax;
    Vector3 m_cameraBoundsMin;

    TerrainRegion m_regions[4];
    BackScene m_background[6];

};

#endif // GAMEWINDOW_H
