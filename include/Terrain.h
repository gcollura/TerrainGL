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

#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <SFML/Graphics.hpp>
#include <Math.h>

class HeightMap {

public:
    HeightMap ();
    ~HeightMap ();

    float getHeightScale () const {
        return m_heightScale;
    }

    int getSize () const {
        return m_size;
    }

    int getGridSpacing () const {
        return m_gridSpacing;
    }

    const float *getHeights () const {
        return &m_heights[0];
    }

    bool create (int size, int gridSpacing, float scale);
    void destroy ();

    void generateDiamondSquareFractal (float roughness);

    float heightAt (float x, float z) const;

    float heightAtPixel (int x, int z) const {
        return m_heights[z * m_size + x];
    }

    void normalAt (float x, float z, Vector3 &n) const;
    void normalAtPixel (int x, int z, Vector3 &n) const;

private:
    void blur (float amount);
    unsigned int heightIndexAt (int x, int z) const;
    void smooth ();

    int m_size;
    int m_gridSpacing;
    float m_heightScale;
    std::vector<float> m_heights;
};


class Terrain {

public:
    Terrain ();
    virtual ~Terrain ();

    bool create (int size, int gridSpacing, float scale);
    void destroy ();
    void draw ();
    bool generateUsingDiamondSquareFractal (float roughness);
    void update (const Vector3 &cameraPos);

    const HeightMap &getHeightMap () const {
        return m_heightMap;
    }

    HeightMap &getHeightMap () {
        return m_heightMap;
    }

protected:
    virtual bool terrainCreate (int size, int gridSpacing, float scale);
    virtual void terrainDestroy ();
    virtual void terrainDraw ();
    virtual void terrainUpdate (const Vector3 &cameraPos);

private:
    struct Vertex {
        float x, y, z;
        float nx, ny, nz;
        float s, t;
    };

    bool generateIndices ();
    bool generateVertices ();

    bool use16BitIndices () const {
        return m_totalVertices <= 65536;
    }

    unsigned int m_vertexBuffer;
    unsigned int m_indexBuffer;
    int m_totalVertices;
    int m_totalIndices;
    HeightMap m_heightMap;
};

#endif // TERRAIN_H
