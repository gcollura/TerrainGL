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
    include/Camera.h

SOURCES += Main.cpp \
    src/System.cpp \
    src/Math.cpp \
    src/GameWindow.cpp \
    src/TextureManager.cpp \
    src/Camera.cpp

LIBS += -L/usr/lib \
        -lsfml-window -lsfml-audio \
        -lsfml-system -lsfml-graphics

data.path = ../TerrainGL2-build/data
data.files += data/texture.jpg \
              data/splashscreen.jpg \
              data/cubemap_posx.jpg

shaders.path = ../TerrainGL2-build/data/shaders
shaders.files += data/shaders/terrain.frag \
                 data/shaders/terrain.vert

INSTALLS += data shaders

OTHER_FILES += \
    LICENSE.txt

DEFINES += DEBUG
