TEMPLATE = app

CONFIG -= qt
CONFIG += x11 opengl

TARGET = TerrainGL
INCLUDEPATH += include
QMAKE_CXXFLAGS += -std=c++0x -pthread

HEADERS += \
    include/System.h \
    include/Math.h \
    include/GameWindow.h \
    include/TextureManager.h \
    include/Camera.h \
    include/Terrain.h \
    include/OpenGL.h \
    include/Water.h \
    include/Terrain2.h

SOURCES += Main.cpp \
    src/System.cpp \
    src/Math.cpp \
    src/GameWindow.cpp \
    src/TextureManager.cpp \
    src/Camera.cpp \
    src/Terrain.cpp \
    src/OpenGL.cpp \
    src/Water.cpp \
    src/Terrain2.cpp

LIBS += -L/usr/lib \
        -lsfml-window -lsfml-audio \
        -lsfml-system -lsfml-graphics

data.path = ../TerrainGL2-build/data
data.files += data/splashscreen.jpg \
              data/landscape.jpg \
              data/reflection.png \
              data/terrain.bmp \
              data/heightmap.jpg

shaders.path = ../TerrainGL2-build/data/shaders
shaders.files += data/shaders/terrain.frag \
                 data/shaders/terrain.vert

textures.path = ../TerrainGL2-build/data/textures
textures.files += data/textures/dirt.png \
                  data/textures/grass.jpg \
                  data/textures/grass2.png \
                  data/textures/rock.jpg \
                  data/textures/rock2.png \
                  data/textures/snow.jpg \
                  data/textures/sand.jpg

INSTALLS += data shaders textures

OTHER_FILES += \
    LICENSE.txt \
    README.txt

DEFINES += DEBUG
