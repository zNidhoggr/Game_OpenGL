// Sky.hpp
// não teve tempo de implementar
#pragma once
#include <GL/gl.h>
#include <vector>
#include <cmath>

enum class MapType {
    DEFAULT,
    PARASIDE,
    BOSS,
    DUNGEON_ONE_LEVEL
};

class Sky {
public:
    enum class SkyType {
        SKYBOX,
        DOME
    };

    Sky(MapType initialMap = MapType::DEFAULT);
    ~Sky();

    void loadTextures();
    void draw(const float cameraPos[3]) const;
    void toggleSkyType();
    void update(float gameTime);
    void setMap(MapType map);

private:
    void drawSkybox(const float cameraPos[3]) const;
    void drawSkyDome(const float cameraPos[3]) const;

    GLuint skyboxTextures[6];  // Right, Left, Top, Bottom, Front, Back
    GLuint skyTextureDefault;
    GLuint skyTextureParadise;
    GLuint skyTextureDungeon1;

    SkyType currentType;
    MapType currentMap;
    float skyColor[3];
    float worldSize;

    static constexpr float DAY_LENGTH = 600.0f; // 10 minutos
};