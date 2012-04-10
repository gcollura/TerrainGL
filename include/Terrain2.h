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

#ifndef TERRAIN2_H
#define TERRAIN2_H

#include <OpenGL.h>
#include <Math.h>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

struct Vertex {
    float x, y, z;
    float nx, ny, nz;
    float s, t;
};

class Terrain2 {
public:
    Terrain2 ();
    virtual ~Terrain2 ();

    bool create (std::string filename, GLfloat heightScale, GLfloat resolution);
    void render ();
    float heightAt (int x, int y);
    float normalAt (int x, int y, Vector3 &n);

    GLuint getWidth () const;
    GLuint getHeight () const;

protected:
    virtual bool terrainCreate (std::string filename, float heightScale, float resolution);
    virtual void terrainDestroy ();
    virtual void terrainDraw ();
    virtual void terrainUpdate (const Vector3 &cameraPos);

private:
    void blur (float amount);
    void smooth ();
    void buildVBOs ();
    void normalAtPixel (int x, int z, Vector3 &n);

    GLuint m_totalVertices;
    GLuint m_vertexBuffer;
    GLuint m_indexBuffer;

    GLuint m_totalIndices;
    bool generateIndices ();
    bool generateVertices ();

    bool use16BitIndices () const {
        return m_totalVertices <= 65536;
    }

    int m_size;
    int m_resolution;
    float m_heightScale;
    sf::Image m_textureImage;

};

#endif // TERRAIN2_H
