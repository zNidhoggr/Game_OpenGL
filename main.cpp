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
#include "textureLoader.cpp"
#include "light.cpp"

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

unsigned int texturaJogador;
unsigned int texturaGrama;
unsigned int texturaPortal;
unsigned int texturaParaside;
unsigned int texturaBoss;
unsigned int texturaDungeon3;
unsigned int texturaDungeon2;
unsigned int texturaDungeon1;
unsigned int textureWood;
unsigned int textureTrunk;
unsigned int textureRock;
unsigned int textureItem;
unsigned int textureWall;
unsigned int textureLeaves;
unsigned int textureHouseWall;
unsigned int textureRoof;
unsigned int textureDoor;
unsigned int textureBrick;
unsigned int texturaJogadorCabeca;
unsigned int textureFloor;

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

    Light::initLight();

    texturaJogador = loadTexture("src/textures/player.png");
    texturaJogadorCabeca = loadTexture("src/textures/player.png");
    texturaGrama = loadTexture("src/textures/grass.png");
    texturaPortal = loadTexture("src/textures/portal.png");
    texturaParaside = loadTexture("src/textures/grass.png");
    texturaBoss = loadTexture("src/textures/dungeon.png");
    texturaDungeon3 = loadTexture("src/textures/dungeon.png");
    texturaDungeon2 = loadTexture("src/textures/dungeon.png");
    texturaDungeon1 = loadTexture("src/textures/dungeon.png");

    textureWood = loadTexture("src/textures/wood.png");
    textureTrunk = loadTexture("src/textures/wood.png");
    textureRock = loadTexture("src/textures/rock.png");
    textureItem = loadTexture("src/textures/gold.png");
    textureWall = loadTexture("src/textures/wall.png");
    textureLeaves = loadTexture("src/textures/wall.png");
    textureHouseWall = loadTexture("src/textures/house_wall.png");
    textureRoof = loadTexture("src/textures/roof.png");

    textureDoor = loadTexture("src/textures/door.png");
    textureBrick = loadTexture("src/textures/wall.png");
    
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
    glutFullScreen(); 
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
