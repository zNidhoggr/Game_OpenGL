#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <vector>
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
#include "mesh.hpp"
#include "meshLoader.cpp"
#include "enemy.cpp"
#include "game.cpp"
#include "Boss.cpp"
#include "gameObject.cpp"

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

void display() { Game::displayCallback(); }
void reshape(int w, int h) { Game::reshapeCallback(w, h); }
void keyboard(unsigned char key, int x, int y) { Game::keyboardCallback(key, x, y); }
void specialKeys(int key, int x, int y) { Game::specialCallback(key, x, y); }
void update(int value) { Game::timerCallback(value); }
void mouse(int button, int state, int x, int y) { Game::mouseCallback(button, state, x, y); }
void motion(int x, int y) { Game::motionCallback(x, y); }
void passiveMotion(int x, int y) { Game::passiveMotionCallback(x, y); }
void keyboardUp(unsigned char key, int x, int y) { Game::KeyUpCallback(key, x, y); }
void Joystick(unsigned int buttonMask, int x, int y, int z) { Game::JoystickCallback(buttonMask, x, y, z); }

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
    Game::initCallback();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("RPG DA SHOPEE");
    // glutFullScreen(); 
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutKeyboardUpFunc(keyboardUp);
    glutTimerFunc(16, update, 0);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passiveMotion);
    glutJoystickFunc(Joystick, 16);
    glutMainLoop();

    return 0;
}
