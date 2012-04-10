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

#include <cassert>
#include <cstring>
#include <cstdio>
#include <OpenGL.h>

bool OpenGLExtensionSupported (const char *pszExtensionName) {

    const unsigned char *pszExtensions = NULL;
    const unsigned char *pszStart;
    unsigned char *pszWhere, *pszTerminator;

    // Extension names should not have spaces
    pszWhere = (unsigned char *) strchr( pszExtensionName, ' ' );
    if( pszWhere || *pszExtensionName == '\0' )
        return false;

    // Get Extensions String
    pszExtensions = glGetString( GL_EXTENSIONS );

    // Search The Extensions String For An Exact Copy
    pszStart = pszExtensions;
    for(;;)
    {
        pszWhere = (unsigned char *) strstr( (const char *) pszStart, pszExtensionName );
        if( !pszWhere )
            break;
        pszTerminator = pszWhere + strlen( pszExtensionName );
        if( pszWhere == pszStart || *( pszWhere - 1 ) == ' ' )
            if( *pszTerminator == ' ' || *pszTerminator == '\0' )
                return true;
        pszStart = pszTerminator;
    }
    return false;
}

void OpenGLGetGLVersion(int &major, int &minor) {
    static int majorGL = 0;
    static int minorGL = 0;

    if (!majorGL && !minorGL) {
        const char *pszVersion = reinterpret_cast<const char *>(glGetString(GL_VERSION));

        if (pszVersion)
            sscanf(pszVersion, "%d.%d", &majorGL, &minorGL);
    }

    major = majorGL;
    minor = minorGL;
}

void OpenGLGetGLSLVersion (int &major, int &minor) {
    static int majorGLSL = 0;
    static int minorGLSL = 0;

    if (!majorGLSL && !minorGLSL) {
        int majorGL = 0;
        int minorGL = 0;

        OpenGLGetGLVersion (majorGL, minorGL);

        if (majorGL >= 2) {
            const char *pszShaderVersion = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));

            if (pszShaderVersion)
                sscanf(pszShaderVersion, "%d.%d", &majorGLSL, &minorGLSL);
        } else {
            const char *pszExtension = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

            if (pszExtension) {
                if (strstr(pszExtension, "GL_ARB_shading_language_100")) {
                    majorGLSL = 1;
                    minorGLSL = 0;
                }
            }
        }
    }

    major = majorGLSL;
    minor = minorGLSL;
}

bool OpenGLSupportsGLVersion (int major, int minor) {
    static int majorGL = 0;
    static int minorGL = 0;

    if (!majorGL && !minorGL)
        OpenGLGetGLVersion(majorGL, minorGL);

    if (majorGL > major)
        return true;

    if (majorGL == major && minorGL >= minor)
        return true;

    return false;
}

bool OpenGLSupportsGLSLVersion(int major, int minor) {
    static int majorGLSL = 0;
    static int minorGLSL = 0;

    if (!majorGLSL && !minorGLSL)
        OpenGLGetGLSLVersion(majorGLSL, minorGLSL);

    if (majorGLSL > major)
        return true;

    if (majorGLSL == major && minorGLSL >= minor)
        return true;

    return false;
}

#define LOAD_ENTRYPOINT(name, var, type) \
    if (!var) \
    { \
        var = reinterpret_cast<type>(wglGetProcAddress(name)); \
        assert(var != 0); \
    }
