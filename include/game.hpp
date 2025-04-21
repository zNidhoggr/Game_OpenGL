#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <memory>
#include <cmath>
#include <iostream>
#include <GL/glut.h>
#include <windows.h> // Para GetAsyncKeyState
#include "data.hpp"
#include "player.hpp"
#include "skillTree.hpp"
#include "staticObject.hpp"
#include "enemy.hpp"
#include "grassBlade.hpp"

class Game {
private:
    Player player;
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    Game();
    
    float cameraDistance;
    float cameraHeight;
    float cameraAngle;
    
    int gameMode;  // 0: exploração, 1: combate, 2: menu de habilidades
    
    static const float WORLD_SIZE;
    float lastFrameTime;
    float deltaTime;
    
    // Dungeon
    MapType currentMap;
    
    bool showPortalMessage;
    bool inDungeon;
    
    // Variáveis para controle do mouse
    int lastMouseX, lastMouseY;
    bool mouseLeftDown;
    bool mouseRightDown;
    float mouseSensitivity;
    
    // Para mouse picking na árvore de habilidades
    std::vector<SkillNode> skillNodes;
    
    // Constantes para cores
    GLfloat skyColor[3];
    
    // Tooltip para habilidades
    SkillTooltip skillTooltip;
    
    // Para dungeon grid
    static const int DUNGEON_WIDTH = 10;
    static const int DUNGEON_HEIGHT = 10;
    bool dungeonGrid[10][10];
    
    // Para trilhas e vegetação
    std::vector<TrailPoint> trailCurvePoints;
    std::vector<TrailPoint> trailClearings;
    std::vector<GrassPatch> grassPatches;
    
public:
    bool topDownView;
    
    // Métodos de terreno e ambiente
    float lerp(float a, float b, float t);
    bool isUnderWater(float x, float z);
    bool isInTrail(float x, float z);
    bool hasGrass(float x, float z);
    
    // Métodos de geração
    void generateBranch(TrailPoint origin, float baseDirection, float safeMargin);
    void generateNaturalTrail(float worldSize);
    void addPortalNearEdge(float worldSize, float margin, float destX, float destZ);
    
    // Métodos de carregamento de mapas
    void loadMainMap();
    void loadDungeonMap();
    void initObjects();
    
    // Métodos de atualização e colisão
    void update();
    void checkCollisions();
    void constrainPlayer();
    
    // Métodos de renderização
    void render();
    void drawHUD();
    void drawGround();
    void drawLakes();
    void drawSkillTree();
    void checkSkillTreeClick(int x, int y);
    void updateSkillNodeHover(int x, int y);

    void calculateSkillTreeLayout();
    void drawSkillTreeConnections();
    void drawSkillTreeNodes();
    void drawSkillIcon(float x, float y, SkillType type) ;
    void drawSkillTooltip();
    void drawFilledCircle(float x, float y, float radius, int segments);
    void drawCircle(float x, float y, float radius, int segments);
    void drawStar(float x, float y, float radius, int points);
    void drawText(float x, float y, const char* string);
    
    // Métodos de entrada
    void handleKeyboard(unsigned char key, int x, int y);
    void handleSpecialKeyPress(int key, int x, int y);
    void handleMouseClick(int button, int state, int x, int y);
    void handlePassiveMouseMotion(int x, int y);
    void handleSpecialKeys(int key);
    void handleMouse(int button, int state, int x, int y);
    void handleMouseMotion(int x, int y);
    void handleKeyPress(unsigned char key, int x, int y);
    
    // Getters/Setters
    float getTerrainHeight(float x, float z);
    MapType getCurrentMap() const { return currentMap; }
    void setCurrentMap(MapType map) { currentMap = map; }

    Player& getPlayer();          // Retorna referência ao jogador
    int getGameMode() const;      // Retorna o modo de jogo atual
    void setGameMode(int mode);

    static Game& GetInstance(); // Singleton

    static void mouseCallback(int button, int state, int x, int y);
    static void motionCallback(int x, int y);
    static void passiveMotionCallback(int x, int y);
    static void displayCallback();
    static void reshapeCallback(int w, int h);
    static void keyboardCallback(unsigned char key, int x, int y);
    static void specialCallback(int key, int x, int y);
    static void timerCallback(int value);

};

#endif // GAME_HPP