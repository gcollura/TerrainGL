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

#include <Terrain2.h>
#include <Math.h>
#include <System.h>
#include <cassert>

int listId = -1;

Terrain2::Terrain2 () {

    // Set Pointers To NULL
    m_totalVertices = 0;
    m_vertexBuffer = 0;

}

Terrain2::~Terrain2 () {

    GLuint nBuffers[2] = {m_vertexBuffer};
    glDeleteBuffers (2, nBuffers);

}

bool Terrain2::create (std::string filename, GLfloat heightScale, GLfloat resolution) {
    terrainCreate (filename, heightScale, resolution);
}

float Terrain2::heightAt (int x, int y) {

    float flR = m_textureImage.getPixel (x, y).r;    // Get The Red Component
    float flG = m_textureImage.getPixel (x, y).g;    // Get The Green Component
    float flB = m_textureImage.getPixel (x, y).b;    // Get The Blue Component

    // Calculate The Height Using The Luminance Algorithm
    float height = 0.299f * flR + 0.587f * flG + 0.114f * flB;

    // Smooth the height
    height /= log (flR + flG + flB);
    height *= abs (sin (flR + flG + flB)) + 1;
    return height*5;
}

void Terrain2::normalAtPixel (int x, int z, Vector3 &n) {

    // Returns the normal at the specified location on the height map.
    // The normal is calculated using the properties of the height map.
    // This approach is much quicker and more elegant than triangulating the
    // height map and averaging triangle surface normals.

    if (x > 0 && x < m_size - 1)
        n.x = heightAt(x - 1, z) - heightAt(x + 1, z);
    else if (x > 0)
        n.x = 2.0f * (heightAt(x - 1, z) - heightAt(x, z));
    else
        n.x = 2.0f * (heightAt(x, z) - heightAt(x + 1, z));

    if (z > 0 && z < m_size - 1)
        n.z = heightAt(x, z - 1) - heightAt(x, z + 1);
    else if (z > 0)
        n.z = 2.0f * (heightAt (x, z - 1) - heightAt (x, z));
    else
        n.z = 2.0f * (heightAt (x, z) - heightAt (x, z + 1));

    n.y = 2.0f * m_resolution;
    n.normalize();
}

float Terrain2::normalAt (int x, int y, Vector3 &n) {
    // Given a (x, z) position on the rendered height map this method
    // calculates the exact normal of the height map at that (x, z) position
    // using bilinear interpolation.

    x /= static_cast<float>(m_resolution);
    y /= static_cast<float>(m_resolution);

    assert(x >= 0.0f && x < float(m_size));
    assert(y >= 0.0f && y < float(m_size));

    long ix = Math::floatToLong(x);
    long iy = Math::floatToLong(y);

    float percentX = x - static_cast<float>(ix);
    float percentY = y - static_cast<float>(iy);

    Vector3 topLeft;
    Vector3 topRight;
    Vector3 bottomLeft;
    Vector3 bottomRight;

    normalAtPixel(ix, iy, topLeft);
    normalAtPixel(ix + 1, iy, topRight);
    normalAtPixel(ix, iy + 1, bottomLeft);
    normalAtPixel(ix + 1, iy + 1, bottomRight);

    n = Math::bilerp (topLeft, topRight, bottomLeft, bottomRight, percentX, percentY);
    n.normalize ();
}

void Terrain2::render () {

    terrainDraw ();

}

bool Terrain2::terrainCreate (std::string filename, float heightScale, float resolution) {

    if (!m_textureImage.loadFromFile (filename))
        return false;
    sf::Vector2u imageSize = m_textureImage.getSize ();
    m_resolution = resolution;
    m_heightScale = heightScale;

    // Initialize the vertex buffer object.

    m_totalVertices = imageSize.x * imageSize.y;
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_totalVertices,0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Initialize the index buffer object.

    m_totalIndices = (imageSize.x - 1) * (imageSize.y * 2 + 1);
    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

    int indexSize = use16BitIndices() ? sizeof(unsigned short) : sizeof(unsigned int);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * m_totalIndices, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return generateIndices() && generateVertices ();
}

void Terrain2::terrainDestroy()
{
    if (m_vertexBuffer)
    {
        glDeleteBuffers(1, &m_vertexBuffer);
        m_vertexBuffer = 0;
        m_totalVertices = 0;
    }

    if (m_indexBuffer)
    {
        glDeleteBuffers(1, &m_indexBuffer);
        m_indexBuffer = 0;
        m_totalIndices = 0;
    }
}

void Terrain2::terrainDraw()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(6 * sizeof(float)));

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(3 * sizeof(float)));

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));

    if (use16BitIndices())
        glDrawElements(GL_TRIANGLE_STRIP, m_totalIndices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
    else
        glDrawElements(GL_TRIANGLE_STRIP, m_totalIndices, GL_UNSIGNED_INT, BUFFER_OFFSET(0));

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Terrain2::terrainUpdate (const Vector3 &cameraPos) {

}

bool Terrain2::generateIndices() {

    void *pBuffer = 0;
    int size = m_textureImage.getSize ().x;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

    if (!(pBuffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY)))
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return false;
    }

    if (use16BitIndices())
    {
        unsigned short *pIndex = static_cast<unsigned short *>(pBuffer);

        for (int z = 0; z < size - 1; ++z)
        {
            if (z % 2 == 0)
            {
                for (int x = 0; x < size; ++x)
                {
                    *pIndex++ = static_cast<unsigned short>(x + z * size);
                    *pIndex++ = static_cast<unsigned short>(x + (z + 1) * size);
                }

                // Add degenerate triangles to stitch strips together.
                *pIndex++ = static_cast<unsigned short>((size - 1) + (z + 1) * size);
            }
            else
            {
                for (int x = size - 1; x >= 0; --x)
                {
                    *pIndex++ = static_cast<unsigned short>(x + z * size);
                    *pIndex++ = static_cast<unsigned short>(x + (z + 1) * size);
                }

                // Add degenerate triangles to stitch strips together.
                *pIndex++ = static_cast<unsigned short>((z + 1) * size);
            }
        }
    }
    else
    {
        unsigned int *pIndex = static_cast<unsigned int *>(pBuffer);

        for (int z = 0; z < size - 1; ++z)
        {
            if (z % 2 == 0)
            {
                for (int x = 0; x < size; ++x)
                {
                    *pIndex++ = x + z * size;
                    *pIndex++ = x + (z + 1) * size;
                }

                // Add degenerate triangles to stitch strips together.
                *pIndex++ = (size - 1) + (z + 1) * size;
            }
            else
            {
                for (int x = size - 1; x >= 0; --x)
                {
                    *pIndex++ = x + z * size;
                    *pIndex++ = x + (z + 1) * size;
                }

                // Add degenerate triangles to stitch strips together.
                *pIndex++ = (z + 1) * size;
            }
        }
    }

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return true;
}

bool Terrain2::generateVertices() {

    Vertex *pVertices = 0;
    Vertex *pVertex = 0;
    int currVertex = 0;
    int size = m_textureImage.getSize().x;
    int gridSpacing = m_resolution;
    Vector3 normal;

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    pVertices = static_cast<Vertex *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

    if (!pVertices) {
        glBindBuffer (GL_ARRAY_BUFFER, 0);
        return false;
    }

    for (int z = 0; z < size; ++z) {
        for (int x = 0; x < size; ++x) {
            currVertex = z * size + x;
            pVertex = &pVertices[currVertex];

            pVertex->x = static_cast<float>(x * gridSpacing);
            pVertex->y = heightAt (x, z) * m_heightScale;
            if (pVertex->y < 0)
                Console::message ("Less than 0 in " + Console::toString (x) + " " + Console::toString (z));
            pVertex->z = static_cast<float>(z * gridSpacing);

            normalAtPixel(x, z, normal);
            pVertex->nx = normal.x;
            pVertex->ny = normal.y;
            pVertex->nz = normal.z;

            pVertex->s = static_cast<float>(x) / static_cast<float>(size);
            pVertex->t = static_cast<float>(z) / static_cast<float>(size);
        }
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}
