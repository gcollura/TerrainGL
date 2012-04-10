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

#ifndef OPENGL_H
#define OPENGL_H

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>

// Macro to return the byte offset into a buffer object in machine units.
// This is used primarily for Buffer Objects.
inline const GLubyte *BUFFER_OFFSET(size_t bytes) {
    return reinterpret_cast<const GLubyte *>(0) + bytes;
}

// Determines whether an OpenGL extension is supported.
extern bool OpenGLExtensionSupported (const char *pszExtensionName);

// Returns the highest supported GL version.
extern void OpenGLGetGLVersion (int &major, int &minor);

// Returns the highest supported GLSL version.
extern void OpenGLGetGLSLVersion (int &major, int &minor);

// Determines whether the specified GL version is supported.
extern bool OpenGLSupportsGLVersion (int major, int minor);

// Determines whether the specified GLSL version is supported.
extern bool OpenGLSupportsGLSLVersion (int major, int minor);

#endif // OPENGL_H
