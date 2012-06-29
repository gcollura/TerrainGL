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

const int       TERRAIN_REGIONS_COUNT = 4;

const float     HEIGHTMAP_ROUGHNESS = 1.2f;
const float     HEIGHTMAP_SCALE = 2.f;
const float     HEIGHTMAP_TILING_FACTOR = 12.0f;
const int       HEIGHTMAP_SIZE = 128;
const int       HEIGHTMAP_GRID_SPACING = 16;

const float     CAMERA_FOVX = 90.0f;
const float     CAMERA_ZFAR = HEIGHTMAP_SIZE * HEIGHTMAP_GRID_SPACING * HEIGHTMAP_SCALE;
const float     CAMERA_ZNEAR = 0.1f;
const float     CAMERA_Y_OFFSET = 25.0f;
const Vector3   CAMERA_ACCELERATION (400.0f, 400.0f, 400.0f);
const Vector3   CAMERA_VELOCITY (100.0f, 100.0f, 100.0f);

GameWindow::GameWindow () {

    m_textureManager = new TextureManager ();
    m_textureManager->addResourceDirectory ("data/");

    // Terrain region 1.
    m_regions[0].set (-5.0f, 15.0f, 0, "data/textures/dirt.png");
    // Terrain region 2.
    m_regions[1].set (16.0f, 49.0f, 0, "data/textures/grass2.png");
    // Terrain region 3.
    m_regions[2].set (30.0f, 105.0f, 0, "data/textures/rock2.png");
    // Terrain region 4.
    m_regions[3].set (106.0f, 255.0f, 0, "data/textures/snow.jpg");

    // Light directions
    m_lightDir[0] = 0.0f;
    m_lightDir[1] = 2.5f;
    m_lightDir[2] = 0.5f;
    m_lightDir[3] = 0.0f;

    setMouseCursorVisible (false);

}

GameWindow::~GameWindow () {

    if (m_textureManager)
        delete m_textureManager;
    m_textureManager = 0;

    for (int i = 0; i < TERRAIN_REGIONS_COUNT; i++) {
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

bool GameWindow::init () {

    create (sf::VideoMode::getFullscreenModes ()[0], "TerrainGL",
            sf::Style::Fullscreen, sf::ContextSettings (32, 0, 4));

    m_clock.restart ();

    m_splashscreen.setTexture (m_textureManager->getTexture ("data/splashscreen.jpg"));

    initGL ();
    if (!OpenGLSupportsGLVersion(2, 0))
        throw error ("This application requires OpenGL 2.0 support.");
    else
        Console::message ("It supports GL 2.0");

    // Setup textures.

    if (!(m_nullTexture = createNullTexture (2, 2)))
        throw error ("failed to create null texture.");

    for (int i = 0; i < TERRAIN_REGIONS_COUNT; i++) {
        if (!(m_regions[i].texture = loadTexture (m_regions[i].filename)))
            Console::error ("Failed to load texture: " + m_regions[i].filename);
    }

    m_disableColorMaps = false;
    m_showWeb = false;
    // Setup shaders.

    std::string infoLog;

    if (!(m_terrainShader = loadShaderProgram ("data/shaders/terrain.frag",
                                               "data/shaders/terrain.vert", infoLog)))
        throw std::runtime_error ("Failed to load shader: terrain.glsl.\n" + infoLog);

    // Setup terrain.

    if (!m_terrain.create (HEIGHTMAP_SIZE, HEIGHTMAP_GRID_SPACING, HEIGHTMAP_SCALE))
        throw std::runtime_error ("Failed to create terrain.");

    generateTerrain ();

    if (!m_terrain2.create ("data/heightmap.jpg", 1.f, 2.0f))
        throw error ("Failed to load heightmap: data/heightmap.bmp");
    // Setup camera.

    Vector3 pos;

    pos.x = 100;
    pos.z = 10;
    pos.y = m_terrain2.heightAt (pos.x, pos.z) + CAMERA_Y_OFFSET;

    m_camera.setBehavior (Camera::CAMERA_BEHAVIOR_FIRST_PERSON);
    m_camera.setPosition (pos);
    m_camera.setAcceleration (CAMERA_ACCELERATION);
    m_camera.setVelocity (CAMERA_VELOCITY);

    m_camera.perspective (CAMERA_FOVX,
                         static_cast<float> (getSize ().x) / static_cast<float> (getSize ().y),
                         CAMERA_ZNEAR, CAMERA_ZFAR);
    m_camera.lookAt (Vector3 (512 / 2.f, 100, 512 / 2.f));

    float upperBounds = (HEIGHTMAP_SIZE * HEIGHTMAP_GRID_SPACING - (2.0f * HEIGHTMAP_GRID_SPACING));
    float lowerBounds = static_cast<float> (HEIGHTMAP_GRID_SPACING);

    m_cameraBoundsMax.x = upperBounds;
    m_cameraBoundsMax.y = CAMERA_ZFAR;
    m_cameraBoundsMax.z = upperBounds;

    m_cameraBoundsMin.x = lowerBounds;
    m_cameraBoundsMin.y = 0.0f;
    m_cameraBoundsMin.z = lowerBounds;

    return true;
}

bool GameWindow::initGL () {

    m_maxAnisotrophy = 5.f;

    return true;

}

void GameWindow::bindTexture (GLuint texture, GLuint unit) {

    glActiveTexture (GL_TEXTURE0 + unit);
    glEnable (GL_TEXTURE_2D);
    glBindTexture (GL_TEXTURE_2D, texture);
}

GLuint GameWindow::compileShader (GLenum type, const GLchar *g_source, GLint length) {

    // Compiles the shader given it's source code. Returns the shader object.
    // A std::string object containing the shader's info log is thrown if the
    // shader failed to compile.
    //
    // 'type' is either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
    // 'g_source' is a C style string containing the shader's source code.
    // 'length' is the length of 'pszSource'.

    GLuint shader = glCreateShader(type);

    if (shader) {
        GLint compiled = 0;

        glShaderSource(shader, 1, &g_source, &length);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled) {
            GLsizei infoLogSize = 0;
            string infoLog;

            glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &infoLogSize);
            infoLog.resize (infoLogSize);
            glGetShaderInfoLog (shader, infoLogSize, &infoLogSize, &infoLog[0]);

            throw infoLog;
        }
    }

    return shader;
}

GLuint GameWindow::linkShaders (GLuint vertShader, GLuint fragShader) {
    // Links the compiled vertex and/or fragment shaders into an executable
    // shader program. Returns the executable shader object. If the shaders
    // failed to link into an executable shader program, then a std::string
    // object is thrown containing the info log.

    GLuint program = glCreateProgram ();

    if (program) {
        GLint linked = 0;

        if (vertShader)
            glAttachShader(program, vertShader);

        if (fragShader)
            glAttachShader(program, fragShader);

        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &linked);

        if (!linked) {
            GLsizei infoLogSize = 0;
            string infoLog;

            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);
            infoLog.resize(infoLogSize);
            glGetProgramInfoLog(program, infoLogSize, &infoLogSize, &infoLog[0]);

            throw infoLog;
        }

        // Mark the two attached shaders for deletion. These two shaders aren't
        // deleted right now because both are already attached to a shader
        // program. When the shader program is deleted these two shaders will
        // be automatically detached and deleted.

        if (vertShader)
            glDeleteShader(vertShader);

        if (fragShader)
            glDeleteShader(fragShader);
    }

    return program;
}

GLuint GameWindow::loadShaderProgram (string fragFilename, string vertFilename, string &infoLog) {

    infoLog.clear();

    GLuint program = 0, fragShader, vertShader;
    string bufferFrag, bufferVert;

    // Read the text file containing the GLSL shader program.
    bufferFrag = System::readTextFile (fragFilename);
    bufferVert = System::readTextFile (vertFilename);

    try {

        if (bufferFrag.length () > 0) {
            fragShader = compileShader (GL_FRAGMENT_SHADER, bufferFrag.c_str (), bufferFrag.length ());
        }

        if (bufferVert.length () > 0) {
            vertShader = compileShader (GL_VERTEX_SHADER, bufferVert.c_str (), bufferVert.length ());
        }

        program = linkShaders (vertShader, fragShader);
    } catch (const std::string &errors) {
        infoLog = errors;
        Console::warning (infoLog);
    }

    return program;
}

GLuint GameWindow::createNullTexture (int g_width, int g_height) {

    // Create an empty white texture. This texture is applied to models
    // that don't have any texture maps. This trick allows the same shader to
    // be used to draw the model with and without textures applied.

    int pitch = ((g_width * 32 + 31) & ~31) >> 3; // align to 4-byte boundaries
    std::vector<GLubyte> pixels (pitch * g_height, 255);
    GLuint texture = 0;

    glGenTextures (1, &texture);
    glBindTexture (GL_TEXTURE_2D, texture);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, g_width, g_height, 0, GL_BGRA,
                  GL_UNSIGNED_BYTE, &pixels[0]);

    glBindTexture (GL_TEXTURE_2D, 0);

    return texture;
}

GLuint GameWindow::loadTexture (string filename) {
    return loadTexture (filename, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR,
                        GL_REPEAT, GL_REPEAT);
}

GLuint GameWindow::loadTexture (string filename, GLint magFilter, GLint minFilter,
                                GLint wrapS, GLint wrapT) {
    GLuint id = 0;
    sf::Texture texture;
    texture = m_textureManager->getTexture (filename);

    if (true) {

        glGenTextures (1, &id);
        glBindTexture (GL_TEXTURE_2D, id);

        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
        glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

        if (m_maxAnisotrophy > 1) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
                            m_maxAnisotrophy);
        }

        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, texture.getSize ().x, texture.getSize ().y,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, texture.copyToImage ().getPixelsPtr ());

        glBindTexture (GL_TEXTURE_2D, 0);
    }

    return id;
}

void GameWindow::generateTerrain () {
    if (!m_terrain.generateUsingDiamondSquareFractal (HEIGHTMAP_ROUGHNESS))
        throw error ("Failed to generate terrain.");
}

void GameWindow::updateTerrainShaderParameters () {

    GLint handle = -1;

    // Update the terrain tiling factor.

    handle = glGetUniformLocation(m_terrainShader, "tilingFactor");
    glUniform1f(handle, HEIGHTMAP_TILING_FACTOR);

    // Update terrain region 1.

    handle = glGetUniformLocation(m_terrainShader, "region1.max");
    glUniform1f(handle, m_regions[0].max);

    handle = glGetUniformLocation(m_terrainShader, "region1.min");
    glUniform1f(handle, m_regions[0].min);

    // Update terrain region 2.

    handle = glGetUniformLocation(m_terrainShader, "region2.max");
    glUniform1f(handle, m_regions[1].max);

    handle = glGetUniformLocation(m_terrainShader, "region2.min");
    glUniform1f(handle, m_regions[1].min);

    // Update terrain region 3.

    handle = glGetUniformLocation(m_terrainShader, "region3.max");
    glUniform1f(handle, m_regions[2].max);

    handle = glGetUniformLocation(m_terrainShader, "region3.min");
    glUniform1f(handle, m_regions[2].min);

    // Update terrain region 4.

    handle = glGetUniformLocation(m_terrainShader, "region4.max");
    glUniform1f(handle, m_regions[3].max);

    handle = glGetUniformLocation(m_terrainShader, "region4.min");
    glUniform1f(handle, m_regions[3].min);

    // Bind textures.

    glUniform1i(glGetUniformLocation(m_terrainShader, "region1ColorMap"), 0);
    glUniform1i(glGetUniformLocation(m_terrainShader, "region2ColorMap"), 1);
    glUniform1i(glGetUniformLocation(m_terrainShader, "region3ColorMap"), 2);
    glUniform1i(glGetUniformLocation(m_terrainShader, "region4ColorMap"), 3);

}

void GameWindow::saveScreenshot (string destDir = "", string filename = "") {

    sf::Image image;
    image = this->capture ();

    if (filename == "")
        filename = "Screenshot-" + Console::toString (m_ScreenNum) + ".jpg";


    if (image.saveToFile (destDir + filename))
        Console::message (destDir + filename + " saved!");
    m_ScreenNum++;

}

bool b_render = true;
void GameWindow::keyboardEvent (sf::Keyboard::Key key) {

    if (key == sf::Keyboard::F11)
        saveScreenshot ();
    if (key == sf::Keyboard::Q)
        close ();
    if (key == sf::Keyboard::E)
        m_useMouse = !m_useMouse;
    if (key == sf::Keyboard::R)
        b_render = !b_render;
    if (key == sf::Keyboard::P)
        m_showWeb = !m_showWeb;
    if (key == sf::Keyboard::C)
        m_disableColorMaps = !m_disableColorMaps;

    if (key == sf::Keyboard::Space)
        generateTerrain ();

}

void GameWindow::joystickEvent (uint button) {

    if (sf::Joystick::isButtonPressed (0, 3))
        close ();
    if (sf::Joystick::isButtonPressed (0, 4))
        m_camera.moveUpward (5.f);
    if (sf::Joystick::isButtonPressed (0, 5))
        m_camera.moveUpward (-5.f);
    if (sf::Joystick::isButtonPressed (0, 2))
        m_disableColorMaps = !m_disableColorMaps;
    if (sf::Joystick::isButtonPressed (0, 1))
        m_showWeb = !m_showWeb;
}

void GameWindow::getInput () {

    if (sf::Keyboard::isKeyPressed (sf::Keyboard::A))
        m_camera.strafeRight (-1.f);
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::D))
        m_camera.strafeRight (1.f);
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::W))
        m_camera.moveForward (1.f);
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::S))
        m_camera.moveForward (-1.f);
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::Z))
        m_camera.moveUpward (5.f);
    if (sf::Keyboard::isKeyPressed (sf::Keyboard::X))
        m_camera.moveUpward (-5.f);

    if (sf::Joystick::isConnected (0)) {

        float xAx = sf::Joystick::getAxisPosition (0, sf::Joystick::X);
        if (xAx < -10.f || xAx > 10.f)
            m_camera.strafeRight (xAx / 100.f);
        float yAx = sf::Joystick::getAxisPosition (0, sf::Joystick::Y);
        if (yAx < -10.f || yAx > 10.f)
            m_camera.moveForward (-yAx / 100.f);
        float uAx = sf::Joystick::getAxisPosition (0, sf::Joystick::U);
        if (uAx < -10.f || uAx > 10.f)
            m_camera.rotateSmoothly (-uAx / 100.f, 0, 0);
        float vAx = sf::Joystick::getAxisPosition (0, sf::Joystick::V);
        if (vAx < -10.f || vAx > 10.f)
            m_camera.rotateSmoothly (0, -vAx / 100.f, 0);

    }

    if (m_useMouse) {
        sf::Vector2i centerPos (this->getSize ().x / 2, this->getSize ().y / 2);

        // Calculate deltas: d = xf - xi
        float dX = (sf::Mouse::getPosition (*this).x - centerPos.x) / 10.f;
        float dY = (sf::Mouse::getPosition (*this).y - centerPos.y) / 10.f;

        m_camera.rotateSmoothly (-dX, -dY, 0);
        sf::Mouse::setPosition (centerPos, *this);
    }

}

void GameWindow::updateCameraPosition () {

    const Vector3 &pos = m_camera.getPosition();
    Vector3 newPos (pos);

    if (pos.y <= m_terrain2.heightAt (pos.x, pos.z) + CAMERA_Y_OFFSET)
        newPos.y = m_terrain.getHeightMap ().heightAt (newPos.x, newPos.z) + CAMERA_Y_OFFSET;

    m_camera.setPosition (newPos);

}

void GameWindow::renderTerrain () {

    glUseProgram (m_terrainShader);
    updateTerrainShaderParameters ();

    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
    glLightfv (GL_LIGHT0, GL_POSITION, m_lightDir);

    if (m_disableColorMaps) {
        bindTexture (m_nullTexture, 0);
        bindTexture (m_nullTexture, 1);
        bindTexture (m_nullTexture, 2);
        bindTexture (m_nullTexture, 3);
    } else {
        bindTexture (m_regions[0].texture, 0);
        bindTexture (m_regions[1].texture, 1);
        bindTexture (m_regions[2].texture, 2);
        bindTexture (m_regions[3].texture, 3);
    }

    //m_terrain.draw ();
    if (m_showWeb)
        glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);

    m_terrain2.render ();
    glScalef (512, 1, 512);
    //m_water.render ();

    for (int i = 4; i >= 0; --i) {
        glActiveTexture (GL_TEXTURE0 + i);
        glBindTexture (GL_TEXTURE_2D, 0);
        glDisable (GL_TEXTURE_2D);
    }

    glDisable (GL_LIGHT0);
    glDisable (GL_LIGHTING);

    glUseProgram (0);
}

void GameWindow::render () {

    glEnable (GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    //glDepthMask(GL_TRUE);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glViewport (0, 0, getSize ().x, getSize ().y);
    glClearColor (0.3f, 0.5f, 0.6f, 0.0f);
    glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glMultMatrixf (&m_camera.getProjectionMatrix()[0][0]);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glMultMatrixf (&m_camera.getViewMatrix()[0][0]);

    renderTerrain ();



}

void GameWindow::run () {

    init ();

    while (isOpen ()) {
        // Process events
        sf::Event event;
        while (this->pollEvent (event)) {
            // Close window : exit
            if (event.type == sf::Event::Closed)
                close ();
            if (event.type == sf::Event::KeyPressed)
                keyboardEvent (event.key.code);
            if (event.type == sf::Event::JoystickButtonPressed)
                joystickEvent (event.joystickButton.button);
            if (event.type == sf::Event::JoystickConnected)
                Console::message ("Joystick with " + Console::toString ((int) sf::Joystick::getButtonCount (0))
                                  + " buttons connected");

        }
        getInput ();
        //updateCameraPosition ();

        pushGLStates ();
        if (m_clock.getElapsedTime ().asSeconds () < 1)
            this->draw (m_splashscreen);
        else {
            popGLStates ();
            if (b_render)
                render ();
        }
        this->display ();
    }

}
