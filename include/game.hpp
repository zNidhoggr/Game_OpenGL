#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <memory>
#include <cmath>
#include <iostream>
#include <GL/glut.h>
#include <windows.h>

#include "data.hpp"
#include "player.hpp"
#include "skillTree.hpp"
#include "staticObject.hpp"
#include "enemy.hpp"
#include "camera.cpp"
#include "grassBlade.hpp"
#include "HUD.cpp"
#include "meshLoader.hpp"
#include "AudioManager.hpp"
#include "Boss.hpp"

class Game
{
private:
    // === Atributos principais ===
    Player player;
    Camera camera;
    STATE_GAME gameMode;
    HUD hud;
    MeshLoader loader;
    std::vector<std::unique_ptr<GameObject>> gameObjects;
    std::map<unsigned char, bool> keyStates;
    AudioManager sound;
    ALuint buffer, source;
    Volume volume;

    Game(); // Construtor privado

    // === Câmera ===
    float cameraDistance;
    float cameraHeight;
    float cameraAngle;

    // === Tempo ===
    float lastFrameTime;
    float deltaTime;

    // === Estados do jogo ===
    // 0: exploração, 1: combate, 2: menu de habilidades
    bool topDownView;
    MapType currentMap;
    bool inDungeon;
    bool showPortalMessage;

    // === Mouse ===
    int lastMouseX, lastMouseY;
    bool mouseLeftDown;
    bool mouseRightDown;
    float mouseSensitivity;
    bool isOpenHouse = false;

    // === DUNGEON_ONE_LEVEL ===
    static const int DUNGEON_WIDTH = 10;
    static const int DUNGEON_HEIGHT = 10;
    bool dungeonGrid[DUNGEON_WIDTH][DUNGEON_HEIGHT];

    // === Trilhas e vegetação ===
    std::vector<TrailPoint> trailCurvePoints;
    std::vector<TrailPoint> trailClearings;
    std::vector<GrassPatch> grassPatches;

    // === Árvore de habilidades ===
    std::vector<SkillNode> skillNodes;
    SkillTooltip skillTooltip;

    // === Ambiente ===
    static const float WORLD_SIZE;
    GLfloat skyColor[3];

    // === Métodos auxiliares privados ===
    float lerp(float a, float b, float t);
    bool isUnderWater(float x, float z);
    bool isInTrail(float x, float z);
    bool hasGrass(float x, float z);
    void saveScreenshotBMP(const char *filename, int width, int height);
    void shareScreenshot(int width, int height);

public:
    // === Inicialização e carregamento ===
    void loadMainMap();
    void loadDungeonMap();
    void loadDungeonMap_Level2();
    void loadDungeonMap_Level3();
    void loadDungeonMap_Boss();
    void loadParasideMap();
    void loadHouseInterior();
    void initObjects();
    void init();

    // === Geração Procedural ===
    void generateBranch(TrailPoint origin, float baseDirection, float safeMargin);
    void generateNaturalTrail(float worldSize);
    void addPortalNearEdge(float worldSize, float margin, float destX, float destZ, MapType map);

    // === Atualização e lógica ===
    void update();
    void checkCollisions();
    void constrainPlayer();
    void updateMoviment();

    // === Renderização ===
    void render();
    void drawGround();
    void drawLakes();

    // -- Árvore de habilidades --
    void drawSkillTree();
    void calculateSkillTreeLayout();
    void drawSkillTreeConnections();
    void drawSkillTreeNodes();
    void drawSkillIcon(float x, float y, SkillType type);
    void drawSkillTooltip();
    void animateSwordAttack(float currentTime, MeshLoader &loader, const Player &player, const Camera &camera);

    // -- Formas auxiliares --
    void checkPortals();
    void adjustPlayerHeight();
    void updatePlayerAttackState();

    // -- Mouse Picking --
    void checkSkillTreeClick(int x, int y);
    void handleButtonMenuClick(int x, int y);
    void updateSkillNodeHover(int x, int y);
    void updateButtonMenuHover(int x, int y);

    // === Entrada do usuário ===
    void handleKeyboard(unsigned char key, int x, int y);
    void handleSpecialKeyPress(int key, int x, int y);
    void handleSpecialKeys(int key);
    void handleMouse(int button, int state, int x, int y);
    void handleMouseClick(int button, int state, int x, int y);
    void handlePassiveMouseMotion(int x, int y);
    void handleMouseMotion(int x, int y);
    void handleIdle();
    void handleKeyPress(unsigned char key, int x, int y);
    void handleKeyUp(unsigned char key, int x, int y);
    void handleJoystick(unsigned int btn, int x, int y, int z);

    // === Callbacks GLUT ===
    static void initCallback();
    static void displayCallback();
    static void reshapeCallback(int w, int h);
    static void keyboardCallback(unsigned char key, int x, int y);
    static void specialCallback(int key, int x, int y);
    static void mouseCallback(int button, int state, int x, int y);
    static void motionCallback(int x, int y);
    static void passiveMotionCallback(int x, int y);
    static void timerCallback(int value);
    static void KeyUpCallback(unsigned char key, int x, int y);
    static void JoystickCallback(unsigned int btn, int x, int y, int z);
    static void mouseWheelCallback(int wheel, int direction, int x, int y);

    // === Getters / Setters ===

    MapType getCurrentMap() const { return currentMap; }
    Player &getPlayer();
    STATE_GAME getGameMode() const;
    ACTION_BUTTON button_action = ACTION_BUTTON::NONE;

    float getTerrainHeight(float x, float z);
    void setGameMode(STATE_GAME mode);
    bool getShowPortalMessage() { return showPortalMessage; }
    void setShowPortalMessage(bool show) { showPortalMessage = show; }
    // Getters and Setters
    bool getTopDownView() const { return topDownView; }
    void setTopDownView(bool view) { topDownView = view; }

    void setCurrentMap(MapType map) { currentMap = map; }

    bool isInDungeon() const { return inDungeon; }
    void setInDungeon(bool dungeon) { inDungeon = dungeon; }

    bool getShowPortalMessage() const { return showPortalMessage; }
    static Game &GetInstance(); // Singleton público

    int joystickActive = 0;
    int joystickButton = 0;
    int buttonMask = 0;
    int joystickX = 0, joystickY = 0;
    int MousseActvive = 1;
    bool isSoundPlaying = false;
    bool isAttacking = false;
    float attackProgress = 0.0f;
    bool soundNoEnemie = false;
    bool openPortal = false;
    int inimigos = 0;
    bool isAnyEnemyActive = false;
    bool isMoving = false;
    bool BossDefeat = false;
};

#endif // GAME_HPP
