#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include "data.hpp"
#include "portal.cpp"
#include "skill.cpp"
#include "staticObject.cpp"
#include "grassBlade.cpp"
#include "player.cpp"
#include "enemy.cpp"
#include "game.cpp"
#include "gameObject.cpp"

float getTerrainHeight(float x, float z)
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

const int DUNGEON_WIDTH = 10;
const int DUNGEON_HEIGHT = 10;
bool dungeonGrid[DUNGEON_WIDTH][DUNGEON_HEIGHT]; 
const int MAP_SIZE = 100;
float heightMap[MAP_SIZE][MAP_SIZE];
bool trailMap[MAP_SIZE][MAP_SIZE] = {false};
bool lakeMap[MAP_SIZE][MAP_SIZE] = {false};


SkillTooltip skillTooltip;

std::vector<TrailPoint> trailCurvePoints;
std::vector<TrailPoint> trailClearings;
std::vector<TrailPoint> trailPoints;
std::vector<GrassPatch> grassPatches;


// void Portal::teleport(Player &player, Game &game)
// {
//     if (destinationMap == MapType::DUNGEON)
//     {
//         game.loadDungeonMap();
//     }
//     else if (destinationMap == MapType::MAIN)
//     {
//         game.loadMainMap();
//     }
//     else
//     {
//         float y = getTerrainHeight(destinationX, destinationZ) + 0.3f;
//         player.setPosition(destinationX, y, destinationZ);
//     }
// }

void display() { Game::displayCallback(); }
void reshape(int w, int h) { Game::reshapeCallback(w, h); }
void keyboard(unsigned char key, int x, int y) { Game::keyboardCallback(key, x, y); }
void specialKeys(int key, int x, int y) { Game::specialCallback(key, x, y); }
void update(int value) { Game::timerCallback(value); }
void mouse(int button, int state, int x, int y) { Game::mouseCallback(button, state, x, y); }
void motion(int x, int y) { Game::motionCallback(x, y); }
void passiveMotion(int x, int y) { Game::passiveMotionCallback(x, y); }

void init()
{
    glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat light_position[] = {0.0f, 10.0f, 0.0f, 1.0f};
    GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    srand(static_cast<unsigned int>(time(nullptr)));

}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("RPG 3D em C++ com OpenGL");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(16, update, 0);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passiveMotion);

    init();
    glutMainLoop();

    return 0;
}
