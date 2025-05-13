#ifndef PORTAL_HPP
#define PORTAL_HPP

#include "GameObject.hpp"
#include "data.hpp"
#include "game.hpp"
#include "player.hpp"
#include <GL/glut.h>
#include <cmath>

class Portal : public GameObject {
private:
    float destinationX, destinationZ;
    MapType destinationMap;

public:
    Portal(float x, float y, float z, float size, float destX, float destZ, MapType destMap);

    void draw() override;
    bool playerIsNearby(const Player& player) const;
    void teleport(Player &player, Game &game);
    float getTerrainHeight(float x, float z);
};

#endif 
