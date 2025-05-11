#include "portal.hpp"

Portal::Portal(float x, float y, float z, float size, float destX, float destZ, MapType destMap)
    : GameObject(x, y, z, size, ObjectType::PORTAL),
      destinationX(destX), destinationZ(destZ), destinationMap(destMap) {}

void Portal::draw()
{
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    glPushMatrix();
    glTranslatef(x, y, z);

    // Pulsação e cor vibrante
    float scale = 1.0f + 0.1f * sin(time * 3.0f);
    float r = 0.6f + 0.4f * sin(time * 2.0f);
    float g = 0.2f + 0.3f * cos(time * 1.5f);
    float b = 1.0f;

    glColor4f(r, g, b, 0.7f);

    // Anéis giratórios
    for (int i = 0; i < 3; i++)
    {
        glPushMatrix();
        glRotatef(time * 60.0f + i * 120, 0.0f, 1.0f, 0.0f);
        glScalef(scale, 1.0f, scale);
        glutSolidTorus(0.05, size + 0.1f * i, 10, 20);
        glPopMatrix();
    }

    glPopMatrix();
}

bool Portal::playerIsNearby(const Player &player) const
{
    float dx = x - player.getX();
    float dz = z - player.getZ();
    return std::sqrt(dx * dx + dz * dz) < 1.5f;
}

void Portal::teleport(Player &player, Game &game)
{
    if (destinationMap == MapType::MAIN)
    {
        game.loadMainMap();
        return;
    }
    if (destinationMap == MapType::DUNGEON_ONE_LEVEL)
    {
        game.loadDungeonMap();
        return;
    }
    if (destinationMap == MapType::DUNGEON_TWO_LEVEL)
    {
        game.loadDungeonMap_Level2();
        return;
    }
    if (destinationMap == MapType::DUNGEON_THREE_LEVEL)
    {
        game.loadDungeonMap_Level3();
        return;
    }
    if (destinationMap == MapType::BOSS)
    {
        game.loadDungeonMap_Boss();
        return;
    }
    if (destinationMap == MapType::PARASIDE)
    {
        game.loadParasideMap();
        return;
    }
    if (destinationMap == MapType::HOUSE)
    {
        game.loadHouseInterior();
        return;
    }

    float y = getTerrainHeight(destinationX, destinationZ) + 0.3f;
    player.setPosition(destinationX, y, destinationZ);
    game.setCurrentMap(destinationMap);
}

float Portal::getTerrainHeight(float x, float z)
{
    float height = 0.0f;
    height += std::sin(x * 0.1f) * 0.5f;
    height += std::cos(z * 0.1f) * 0.5f;

    height += (std::sin(x * 0.3f + z * 0.5f) * 0.3f);

    std::vector<std::pair<float, float>> lakeCenters = {
        {5.0f, 5.0f},
        {-7.0f, -3.0f},
        {8.0f, -6.0f},
        {-4.0f, 7.0f}};

    for (const auto &center : lakeCenters)
    {
        float dist = std::sqrt((x - center.first) * (x - center.first) + (z - center.second) * (z - center.second));
        if (dist < 3.5f)
        {
            height -= (3.5f - dist) * 0.4f;
        }
    }

    return height;
}
