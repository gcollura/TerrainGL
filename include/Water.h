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

#ifndef WATER_H
#define WATER_H

#define	MOD	0xff
#define RESOLUTION 64

#include <SFML/Graphics.hpp>
#include <OpenGL.h>

class Water {
public:
    Water ();
    void render ();
    inline void setWireFrame (bool value) {
        m_wireFrame = value;
    }
    inline void setShowNormals (bool value) {
        m_normals = value;
    }

private:
    bool genTextures ();
    float z (const float x, const float y, const float t);
    void initNoise ();
    float noise (const float, const float, const float, const float);

    sf::Clock m_clock;
    GLuint m_textureId;

    float m_surface[6 * RESOLUTION * (RESOLUTION + 1)];
    float m_normal[6 * RESOLUTION * (RESOLUTION + 1)];
    bool m_wireFrame;
    bool m_normals;

};

#endif // WATER_H
