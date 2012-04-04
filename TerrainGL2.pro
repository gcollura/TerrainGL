TEMPLATE = app

CONFIG -= qt
CONFIG += x11 opengl

TARGET = TerrainGL
INCLUDEPATH += include

HEADERS += \
    include/System.h \
    include/Math.h \
    include/GameWindow.h \
    include/TextureManager.h \
    include/Camera.h \
    include/Terrain.h \
    include/OpenGL.h

SOURCES += Main.cpp \
    src/System.cpp \
    src/Math.cpp \
    src/GameWindow.cpp \
    src/TextureManager.cpp \
    src/Camera.cpp \
    src/Terrain.cpp

LIBS += -L/usr/lib \
        -lsfml-window -lsfml-audio \
        -lsfml-system -lsfml-graphics

data.path = ../TerrainGL2-build/data
data.files += data/splashscreen.jpg \
              data/cubemap_posx.jpg

shaders.path = ../TerrainGL2-build/data/shaders
shaders.files += data/shaders/terrain.frag \
                 data/shaders/terrain.vert

textures.path = ../TerrainGL2-build/data/textures
textures.files += data/textures/dirt.jpg \
                  data/textures/grass.jpg \
                  data/textures/rock.jpg \
                  data/textures/snow.jpg

INSTALLS += data shaders textures

OTHER_FILES += \
    LICENSE.txt \
    README.txt

DEFINES += DEBUG
