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

#include <Water.h>
#include <cstdlib>
#include <cmath>

Water::Water ()
    : m_wireFrame (false),
      m_normals (false) {
    m_clock.restart ();
    genTextures ();
}

void Water::render () {

    const float t = m_clock.getElapsedTime ().asSeconds ();
    const float delta = 2. / RESOLUTION;
    const unsigned int length = 2 * (RESOLUTION + 1);
    const float xn = (RESOLUTION + 1) * delta + 1;
    unsigned int i;
    unsigned int j;
    float x;
    float y;
    unsigned int indice;
    unsigned int preindice;

    /* Yes, I know, this is quite ugly... */
    float v1x;
    float v1y;
    float v1z;

    float v2x;
    float v2y;
    float v2z;

    float v3x;
    float v3y;
    float v3z;

    float vax;
    float vay;
    float vaz;

    float vbx;
    float vby;
    float vbz;

    float nx;
    float ny;
    float nz;

    float l;
    /* Vertices */
    for (j = 0; j < RESOLUTION; j++) {
        y = (j + 1) * delta - 1;
        for (i = 0; i <= RESOLUTION; i++) {
            indice = 6 * (i + j * (RESOLUTION + 1));

            x = i * delta - 1;
            m_surface[indice + 3] = x;
            m_surface[indice + 4] = z (x, y, t);
            m_surface[indice + 5] = y;
            if (j != 0) {
                /* Values were computed during the previous loop */
                preindice = 6 * (i + (j - 1) * (RESOLUTION + 1));
                m_surface[indice] = m_surface[preindice + 3];
                m_surface[indice + 1] = m_surface[preindice + 4];
                m_surface[indice + 2] = m_surface[preindice + 5];
            } else {
                m_surface[indice] = x;
                m_surface[indice + 1] = z (x, -1, t);
                m_surface[indice + 2] = -1;
            }
        }
    }

    /* Normals */
    for (j = 0; j < RESOLUTION; j++)
        for (i = 0; i <= RESOLUTION; i++) {
            indice = 6 * (i + j * (RESOLUTION + 1));

            v1x = m_surface[indice + 3];
            v1y = m_surface[indice + 4];
            v1z = m_surface[indice + 5];

            v2x = v1x;
            v2y = m_surface[indice + 1];
            v2z = m_surface[indice + 2];

            if (i < RESOLUTION)
            {
                v3x = m_surface[indice + 9];
                v3y = m_surface[indice + 10];
                v3z = v1z;
            }
            else
            {
                v3x = xn;
                v3y = z (xn, v1z, t);
                v3z = v1z;
            }

            vax =  v2x - v1x;
            vay =  v2y - v1y;
            vaz =  v2z - v1z;

            vbx = v3x - v1x;
            vby = v3y - v1y;
            vbz = v3z - v1z;

            nx = (vby * vaz) - (vbz * vay);
            ny = (vbz * vax) - (vbx * vaz);
            nz = (vbx * vay) - (vby * vax);

            l = sqrtf (nx * nx + ny * ny + nz * nz);
            if (l != 0)
            {
                l = 1 / l;
                m_normal[indice + 3] = nx * l;
                m_normal[indice + 4] = ny * l;
                m_normal[indice + 5] = nz * l;
            }
            else
            {
                m_normal[indice + 3] = 0;
                m_normal[indice + 4] = 1;
                m_normal[indice + 5] = 0;
            }


            if (j != 0)
            {
                /* Values were computed during the previous loop */
                preindice = 6 * (i + (j - 1) * (RESOLUTION + 1));
                m_normal[indice] = m_normal[preindice + 3];
                m_normal[indice + 1] = m_normal[preindice + 4];
                m_normal[indice + 2] = m_normal[preindice + 5];
            }
            else
            {
                /* 	    v1x = v1x; */
                v1y = z (v1x, (j - 1) * delta - 1, t);
                v1z = (j - 1) * delta - 1;

                /* 	    v3x = v3x; */
                v3y = z (v3x, v2z, t);
                v3z = v2z;

                vax = v1x - v2x;
                vay = v1y - v2y;
                vaz = v1z - v2z;

                vbx = v3x - v2x;
                vby = v3y - v2y;
                vbz = v3z - v2z;

                nx = (vby * vaz) - (vbz * vay);
                ny = (vbz * vax) - (vbx * vaz);
                nz = (vbx * vay) - (vby * vax);

                l = sqrtf (nx * nx + ny * ny + nz * nz);
                if (l != 0)
                {
                    l = 1 / l;
                    m_normal[indice] = nx * l;
                    m_normal[indice + 1] = ny * l;
                    m_normal[indice + 2] = nz * l;
                }
                else
                {
                    m_normal[indice] = 0;
                    m_normal[indice + 1] = 1;
                    m_normal[indice + 2] = 0;
                }
            }
        }

    /* The ground */
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable (GL_TEXTURE_2D);
    glColor3f (1, 0.9, 0.7);
    glBegin (GL_TRIANGLE_FAN);
    glVertex3f (-1, 0, -1);
    glVertex3f (-1, 0,  1);
    glVertex3f ( 1, 0,  1);
    glVertex3f ( 1, 0, -1);
    glEnd ();

    glTranslatef (0, 0.2, 0);

    /* Render wireframe? */
    if (m_wireFrame)
        glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

    /* The water */
    glEnable (GL_TEXTURE_2D);
    glColor3f (1, 1, 1);
    glEnableClientState (GL_NORMAL_ARRAY);
    glEnableClientState (GL_VERTEX_ARRAY);
    glNormalPointer (GL_FLOAT, 0, m_normal);
    glVertexPointer (3, GL_FLOAT, 0, m_surface);
    for (i = 0; i < RESOLUTION; i++)
        glDrawArrays (GL_TRIANGLE_STRIP, i * length, length);

    /* Draw normals? */
    if (m_normals) {
        glDisable (GL_TEXTURE_2D);
        glColor3f (1, 0, 0);
        glBegin (GL_LINES);
        for (j = 0; j < RESOLUTION; j++)
            for (i = 0; i <= RESOLUTION; i++) {
                indice = 6 * (i + j * (RESOLUTION + 1));
                glVertex3fv (&(m_surface[indice]));
                glVertex3f (m_surface[indice] + m_normal[indice] / 50,
                            m_surface[indice + 1] + m_normal[indice + 1] / 50,
                            m_surface[indice + 2] + m_normal[indice + 2] / 50);
            }

        glEnd ();
    }
}

bool Water::genTextures () {

    /* Texture loading  */
    glGenTextures (1, &m_textureId);
    sf::Image caustic_texture;
    caustic_texture.loadFromFile ("data/reflection.png");

    glBindTexture (GL_TEXTURE_2D, m_textureId);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, caustic_texture.getSize ().x, caustic_texture.getSize ().y,
                  0, GL_RGBA, GL_UNSIGNED_BYTE,  caustic_texture.getPixelsPtr ());
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glEnable (GL_TEXTURE_GEN_S);
    glEnable (GL_TEXTURE_GEN_T);
    glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

}

float Water::z (const float x, const float y, const float t) {

    const float x2 = x - 3;
    const float y2 = y + 1;
    const float xx = x2 * x2;
    const float yy = y2 * y2;
    return ((2 * sinf (20 * sqrtf (xx + yy) - 4 * t) +
             noise (10 * x, 10 * y, t, 0)) / 200);

}

static int permut[256];
static const char gradient[32][4] = {
    { 1,  1,  1,  0}, { 1,  1,  0,  1}, { 1,  0,  1,  1}, { 0,  1,  1,  1},
    { 1,  1, -1,  0}, { 1,  1,  0, -1}, { 1,  0,  1, -1}, { 0,  1,  1, -1},
    { 1, -1,  1,  0}, { 1, -1,  0,  1}, { 1,  0, -1,  1}, { 0,  1, -1,  1},
    { 1, -1, -1,  0}, { 1, -1,  0, -1}, { 1,  0, -1, -1}, { 0,  1, -1, -1},
    {-1,  1,  1,  0}, {-1,  1,  0,  1}, {-1,  0,  1,  1}, { 0, -1,  1,  1},
    {-1,  1, -1,  0}, {-1,  1,  0, -1}, {-1,  0,  1, -1}, { 0, -1,  1, -1},
    {-1, -1,  1,  0}, {-1, -1,  0,  1}, {-1,  0, -1,  1}, { 0, -1, -1,  1},
    {-1, -1, -1,  0}, {-1, -1,  0, -1}, {-1,  0, -1, -1}, { 0, -1, -1, -1},
};

void Water::initNoise () {
    unsigned int i = 0;
    while (i < 256)
        permut[i++] = rand () & MOD;
}

/*
** Function finding out the gradient corresponding to the coordinates
*/
static int indice (const int i,
                    const int j,
                    const int k,
                    const int l)
{
    return (permut[(l + permut[(k + permut[(j + permut[i & MOD])
                                & MOD])
                    & MOD])
            & MOD]
            & 0x1f);
}

/*
** Functions computing the dot product of the vector and the gradient
*/
static float product (const float a, const char b) {
    if (b > 0)
        return a;
    if (b < 0)
        return -a;
    return 0;
}

static float dotProduct (const float x1, const char x2,
                         const float y1, const char y2,
                         const float z1, const char z2,
                         const float t1, const char t2) {
    return (product (x1, x2) + product (y1, y2) + product (z1, z2) + product (t1, t2));
}

/*
** Functions computing interpolations
*/
static float spline (const float state) {
    /*
     ** Enhanced spline :
     ** (3x^2 + 2x^3) is not as good as (6x^5 - 15x^4 + 10x^3)
    */
    const float sqr = state * state;
    return state * sqr * (6 * sqr - 15 * state + 10);
}

static float linear (const float start,
                        const float end,
                        const float state) {
    return start + (end - start) * state;
}

/*
** Noise function, returning the Perlin Noise at a given point
*/
float Water::noise (const float x,
                    const float y,
                    const float z,
                    const float t) {
    /* The unit hypercube containing the point */
    const int x1 = (int) (x > 0 ? x : x - 1);
    const int y1 = (int) (y > 0 ? y : y - 1);
    const int z1 = (int) (z > 0 ? z : z - 1);
    const int t1 = (int) (t > 0 ? t : t - 1);
    const int x2 = x1 + 1;
    const int y2 = y1 + 1;
    const int z2 = z1 + 1;
    const int t2 = t1 + 1;

    /* The 16 corresponding gradients */
    const char * g0000 = gradient[indice (x1, y1, z1, t1)];
    const char * g0001 = gradient[indice (x1, y1, z1, t2)];
    const char * g0010 = gradient[indice (x1, y1, z2, t1)];
    const char * g0011 = gradient[indice (x1, y1, z2, t2)];
    const char * g0100 = gradient[indice (x1, y2, z1, t1)];
    const char * g0101 = gradient[indice (x1, y2, z1, t2)];
    const char * g0110 = gradient[indice (x1, y2, z2, t1)];
    const char * g0111 = gradient[indice (x1, y2, z2, t2)];
    const char * g1000 = gradient[indice (x2, y1, z1, t1)];
    const char * g1001 = gradient[indice (x2, y1, z1, t2)];
    const char * g1010 = gradient[indice (x2, y1, z2, t1)];
    const char * g1011 = gradient[indice (x2, y1, z2, t2)];
    const char * g1100 = gradient[indice (x2, y2, z1, t1)];
    const char * g1101 = gradient[indice (x2, y2, z1, t2)];
    const char * g1110 = gradient[indice (x2, y2, z2, t1)];
    const char * g1111 = gradient[indice (x2, y2, z2, t2)];

    /* The 16 vectors */
    const float dx1 = x - x1;
    const float dx2 = x - x2;
    const float dy1 = y - y1;
    const float dy2 = y - y2;
    const float dz1 = z - z1;
    const float dz2 = z - z2;
    const float dt1 = t - t1;
    const float dt2 = t - t2;

    /* The 16 dot products */
    const float b0000 = dotProduct(dx1, g0000[0], dy1, g0000[1],
                                 dz1, g0000[2], dt1, g0000[3]);
    const float b0001 = dotProduct(dx1, g0001[0], dy1, g0001[1],
                                 dz1, g0001[2], dt2, g0001[3]);
    const float b0010 = dotProduct(dx1, g0010[0], dy1, g0010[1],
                                 dz2, g0010[2], dt1, g0010[3]);
    const float b0011 = dotProduct(dx1, g0011[0], dy1, g0011[1],
                                 dz2, g0011[2], dt2, g0011[3]);
    const float b0100 = dotProduct(dx1, g0100[0], dy2, g0100[1],
                                 dz1, g0100[2], dt1, g0100[3]);
    const float b0101 = dotProduct(dx1, g0101[0], dy2, g0101[1],
                                 dz1, g0101[2], dt2, g0101[3]);
    const float b0110 = dotProduct(dx1, g0110[0], dy2, g0110[1],
                                 dz2, g0110[2], dt1, g0110[3]);
    const float b0111 = dotProduct(dx1, g0111[0], dy2, g0111[1],
                                 dz2, g0111[2], dt2, g0111[3]);
    const float b1000 = dotProduct(dx2, g1000[0], dy1, g1000[1],
                                 dz1, g1000[2], dt1, g1000[3]);
    const float b1001 = dotProduct(dx2, g1001[0], dy1, g1001[1],
                                 dz1, g1001[2], dt2, g1001[3]);
    const float b1010 = dotProduct(dx2, g1010[0], dy1, g1010[1],
                                 dz2, g1010[2], dt1, g1010[3]);
    const float b1011 = dotProduct(dx2, g1011[0], dy1, g1011[1],
                                 dz2, g1011[2], dt2, g1011[3]);
    const float b1100 = dotProduct(dx2, g1100[0], dy2, g1100[1],
                                 dz1, g1100[2], dt1, g1100[3]);
    const float b1101 = dotProduct(dx2, g1101[0], dy2, g1101[1],
                                 dz1, g1101[2], dt2, g1101[3]);
    const float b1110 = dotProduct(dx2, g1110[0], dy2, g1110[1],
                                 dz2, g1110[2], dt1, g1110[3]);
    const float b1111 = dotProduct(dx2, g1111[0], dy2, g1111[1],
                                 dz2, g1111[2], dt2, g1111[3]);

    /* Then the interpolations, down to the result */
    const float idx1 = spline (dx1);
    const float idy1 = spline (dy1);
    const float idz1 = spline (dz1);
    const float idt1 = spline (dt1);

    const float b111 = linear (b1110, b1111, idt1);
    const float b110 = linear (b1100, b1101, idt1);
    const float b101 = linear (b1010, b1011, idt1);
    const float b100 = linear (b1000, b1001, idt1);
    const float b011 = linear (b0110, b0111, idt1);
    const float b010 = linear (b0100, b0101, idt1);
    const float b001 = linear (b0010, b0011, idt1);
    const float b000 = linear (b0000, b0001, idt1);

    const float b11 = linear (b110, b111, idz1);
    const float b10 = linear (b100, b101, idz1);
    const float b01 = linear (b010, b011, idz1);
    const float b00 = linear (b000, b001, idz1);

    const float b1 = linear (b10, b11, idy1);
    const float b0 = linear (b00, b01, idy1);

    return linear (b0, b1, idx1);
}
