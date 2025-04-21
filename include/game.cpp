
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
#include "game.hpp"

const float Game::WORLD_SIZE = 20.0f;

Game::Game() : player(0.0f, 0.5f, 0.0f),
               cameraDistance(5.0f),
               cameraHeight(2.0f),
               cameraAngle(0.0f),
               gameMode(0),
               currentMap(MapType::MAIN),
               lastFrameTime(0.0f),
               deltaTime(0.0f),
               showPortalMessage(false),
               inDungeon(false),
               lastMouseX(0),
               lastMouseY(0),
               mouseLeftDown(false),
               mouseRightDown(false),
               mouseSensitivity(0.2f),
               topDownView(false)
{
    // Inicializar cores do céu
    skyColor[0] = 0.4f;
    skyColor[1] = 0.6f;
    skyColor[2] = 0.9f;

    // Inicializar tooltip
    skillTooltip.visible = false;
    skillTooltip.showConfirmation = false;
    skillTooltip.width = 250.0f;
    skillTooltip.height = 150.0f;

    initObjects();
}

float Game::lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

bool Game::isUnderWater(float x, float z)
{
    std::vector<std::pair<float, float>> lakeCenters = {
        {5.0f, 5.0f},
        {-7.0f, -3.0f},
        {8.0f, -6.0f},
        {-4.0f, 7.0f}};
    float radius = 3.5f;

    for (const auto &center : lakeCenters)
    {
        float cx = center.first;
        float cz = center.second;

        float dist = std::sqrt((x - cx) * (x - cx) + (z - cz) * (z - cz));
        if (dist < radius)
        {
            // Calcular altura da borda
            float maxEdgeHeight = -1000.0f;
            for (int angle = 0; angle < 360; angle += 10)
            {
                float rad = angle * M_PI / 180.0f;
                float bx = cx + cos(rad) * radius;
                float bz = cz + sin(rad) * radius;
                float h = getTerrainHeight(bx, bz);
                if (h > maxEdgeHeight)
                    maxEdgeHeight = h;
            }

            float waterHeight = maxEdgeHeight - 0.02f;
            float terrainY = getTerrainHeight(x, z);
            if (terrainY < waterHeight)
                return true;
        }
    }
    return false;
}

bool Game::isInTrail(float x, float z)
{
    float trailWidth = 2.0f;
    for (const auto &p : trailCurvePoints)
    {
        float dx = x - p.x;
        float dz = z - p.z;
        float distSq = dx * dx + dz * dz;
        if (distSq < trailWidth * trailWidth)
            return true;
    }

    // Verificar clareiras
    for (const auto &c : trailClearings)
    {
        float dx = x - c.x;
        float dz = z - c.z;
        float distSq = dx * dx + dz * dz;
        if (distSq < 4.0f * 4.0f) // raio das clareiras
            return true;
    }

    return false;
}

bool Game::hasGrass(float x, float z)
{
    if (isInTrail(x, z))
    {
        return false; // Sem grama na trilha
    }

    for (const GrassPatch &patch : grassPatches)
    {
        float dx = x - patch.x;
        float dz = z - patch.z;
        if (std::sqrt(dx * dx + dz * dz) < patch.radius)
        {
            return true; // Dentro de alguma mancha de grama
        }
    }

    return false;
}

void Game::generateBranch(TrailPoint origin, float baseDirection, float safeMargin)
{
    float direction = baseDirection + ((rand() % 2 == 0) ? M_PI / 3 : -M_PI / 3); // 60° esquerda ou direita
    int branchLength = 8 + rand() % 5;                                            // comprimento variável

    TrailPoint current = origin;

    for (int i = 0; i < branchLength; ++i)
    {
        float length = 1.5f;
        TrailPoint next;
        next.x = current.x + cos(direction) * length;
        next.z = current.z + sin(direction) * length;
        next.y = getTerrainHeight(next.x, next.z);

        if (fabs(next.x) > safeMargin || fabs(next.z) > safeMargin)
            break;

        trailCurvePoints.push_back(next);
        if (i % 4 == 0)
            trailClearings.push_back(next);

        direction += sin(i * 0.3f) * (M_PI / 18); // zigue-zague leve
        current = next;
    }
}

void Game::generateNaturalTrail(float worldSize)
{
    trailCurvePoints.clear();
    trailClearings.clear();

    float minX = -worldSize;
    float maxX = worldSize;
    float startZ = (rand() % 2000 / 1000.0f - 1.0f) * worldSize; // aleatório entre -worldSize e worldSize
    float endZ = (rand() % 2000 / 1000.0f - 1.0f) * worldSize;

    float step = 1.0f;
    float offset = 0.0f;
    float curveAmplitude = 4.0f;

    for (float x = minX; x <= maxX; x += step)
    {
        float t = (x - minX) / (maxX - minX);
        float z = (1.0f - t) * startZ + t * endZ + std::sin(offset + t * 10.0f) * curveAmplitude;

        if (isUnderWater(x, z))
            continue;

        TrailPoint p;
        p.x = x;
        p.z = z;
        p.y = getTerrainHeight(x, z);
        trailCurvePoints.push_back(p);

        if ((int)(x * 10) % 40 == 0)
        {
            trailClearings.push_back(p);
        }
    }
}

void Game::loadMainMap()
{
    currentMap = MapType::MAIN;
    gameObjects.clear();

    // Céu azul
    skyColor[0] = 0.4f;
    skyColor[1] = 0.7f;
    skyColor[2] = 1.0f;

    // Jogador no centro
    float x = 0.0f, z = 0.0f;
    float y = getTerrainHeight(x, z) + 0.3f;
    player.setPosition(x, y, z);

    // Exemplo de árvores, inimigos, etc.
    for (int i = 0; i < 10; i++)
    {
        float ox = rand() % 20 - 10;
        float oz = rand() % 20 - 10;
        float oy = getTerrainHeight(ox, oz);
        gameObjects.push_back(std::make_unique<StaticObject>(ox, oy, oz, 0.4f, ObjectType::TREE, 0.3f, 0.7f, 0.2f));
    }

    // Adiciona o portal para a dungeon
    float px = 8.0f, pz = -5.0f;
    float py = getTerrainHeight(px, pz);
    gameObjects.push_back(std::make_unique<Portal>(x, y + 0.2f, z, 0.4f, 0.0f, 0.0f, MapType::DUNGEON));
}

void Game::loadDungeonMap()
{
    currentMap = MapType::DUNGEON;
    gameObjects.clear();
    skyColor[0] = 0.05f;
    skyColor[1] = 0.05f;
    skyColor[2] = 0.1f;

    // Limpa grid
    for (int i = 0; i < DUNGEON_WIDTH; i++)
        for (int j = 0; j < DUNGEON_HEIGHT; j++)
            dungeonGrid[i][j] = false;

    // Gera um "caminho" simples entre salas conectadas
    int x = 5, z = 5;
    dungeonGrid[x][z] = true;
    for (int i = 0; i < 15; i++)
    {
        int dir = rand() % 4;
        switch (dir)
        {
        case 0:
            if (x > 1)
                x--;
            break;
        case 1:
            if (x < DUNGEON_WIDTH - 2)
                x++;
            break;
        case 2:
            if (z > 1)
                z--;
            break;
        case 3:
            if (z < DUNGEON_HEIGHT - 2)
                z++;
            break;
        }
        dungeonGrid[x][z] = true;
    }

    // Cria as salas no mapa
    for (int i = 0; i < DUNGEON_WIDTH; i++)
    {
        for (int j = 0; j < DUNGEON_HEIGHT; j++)
        {
            if (dungeonGrid[i][j])
            {
                float worldX = (i - DUNGEON_WIDTH / 2) * 5.0f;
                float worldZ = (j - DUNGEON_HEIGHT / 2) * 5.0f;
                float y = getTerrainHeight(worldX, worldZ);

                // Coloca muralhas em volta de salas que não têm vizinhos
                float tileSize = 5.0f; // mesmo valor usado para espaçar salas

                /*for (int d = 0; d < 4; ++d) {
                    int dx = (d == 0) - (d == 1);
                    int dz = (d == 2) - (d == 3);
                    int ni = i + dx;
                    int nj = j + dz;

                    // se a sala vizinha não existe, coloca parede
                    if (ni < 0 || nj < 0 || ni >= DUNGEON_WIDTH || nj >= DUNGEON_HEIGHT || !dungeonGrid[ni][nj]) {
                        float wallX = worldX + dx * (tileSize / 2.0f);
                        float wallZ = worldZ + dz * (tileSize / 2.0f);
                        float wallY = getTerrainHeight(wallX, wallZ);

                        float wallWidth = tileSize;      // Largura completa do espaço
                        float wallHeight = 2.5f;
                        float wallDepth = 0.5f;          // Parede fina

                        gameObjects.push_back(std::make_unique<StaticObject>(
                            wallX, wallY + wallHeight / 2.0f, wallZ, wallWidth, WALL, 0.4f, 0.4f, 0.4f
                        ));
                    }
                }*/

                // Muralha contínua ao redor da dungeon, exceto onde há o portal
                for (int i = 0; i < DUNGEON_WIDTH; ++i)
                {
                    for (int j = 0; j < DUNGEON_HEIGHT; ++j)
                    {
                        bool isEdge = (i == 0 || j == 0 || i == DUNGEON_WIDTH - 1 || j == DUNGEON_HEIGHT - 1);
                        if (isEdge)
                        {
                            float wx = (i - DUNGEON_WIDTH / 2) * 5.0f;
                            float wz = (j - DUNGEON_HEIGHT / 2) * 5.0f;
                            float wy = getTerrainHeight(wx, wz);

                            // Exceção: deixar espaço para o portal
                            bool isPortalGap = (i == DUNGEON_WIDTH / 2 && j == DUNGEON_HEIGHT - 1);
                            if (isPortalGap)
                                continue;

                            // Define escalas diferentes para blocos horizontais e verticais
                            float scaleX = (j == 0 || j == DUNGEON_HEIGHT - 1) ? 2.5f : 0.5f;
                            float scaleZ = (i == 0 || i == DUNGEON_WIDTH - 1) ? 2.5f : 0.5f;

                            gameObjects.push_back(std::make_unique<StaticObject>(
                                wx, wy + 1.0f, wz, 2.5f, WALL, scaleX, 1.0f, scaleZ));
                        }
                    }
                }

                // Obstáculos ou paredes podem ser adicionados aqui
                if (rand() % 3 == 0)
                {
                    gameObjects.push_back(std::make_unique<StaticObject>(
                        worldX + 1.0f, y, worldZ + 1.0f, 0.6f, ObjectType::WALL,
                        0.3f, 0.3f, 0.3f));
                }

                if (rand() % 4 == 0)
                {
                    gameObjects.push_back(std::make_unique<StaticObject>(
                        worldX + 0.5f, y + 0.3f, worldZ + 0.5f, 0.3f, ITEM, 0.9f, 0.8f, 0.1f));
                }

                // Inimigos
                if (rand() % 2 == 0)
                {
                    gameObjects.push_back(std::make_unique<Enemy>(
                        worldX, y + 0.3f, worldZ, 0.5f, 2));
                }
            }
        }
    }

    // Portal de volta ao mapa principal
    float exitX = 0.0f, exitZ = -20.0f;
    float exitY = getTerrainHeight(exitX, exitZ);
    gameObjects.push_back(std::make_unique<Portal>(
        exitX, exitY, exitZ, 0.4f, 0.0f, 0.0f, MapType::MAIN));

    // Jogador no centro da dungeon
    float startX = 0.0f, startZ = 0.0f;
    float startY = getTerrainHeight(startX, startZ) + 0.3f;
    player.setPosition(startX, startY, startZ);
}

void Game::initObjects()
{
    // Criar árvores aleatórias
    for (int i = 0; i < 20; i++)
    {
        float x = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
        float z = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
        float size = 0.5f + ((float)rand() / RAND_MAX) * 0.5f;
        float y = getTerrainHeight(x, z);

        if (isUnderWater(x, z))
            continue; // ignora objetos submersos
        gameObjects.push_back(std::make_unique<StaticObject>(
            x, y, z, size, TREE, 0.3f, 0.5f, 0.1f));
    }

    // Criar pedras
    for (int i = 0; i < 10; i++)
    {
        float x = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
        float z = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
        float size = 0.3f + ((float)rand() / RAND_MAX) * 0.4f;
        float y = getTerrainHeight(x, z);

        if (isUnderWater(x, z))
            continue; // ignora objetos submersos
        gameObjects.push_back(std::make_unique<StaticObject>(
            x, y, z, size, ROCK, 0.5f, 0.5f, 0.5f));
    }

    // Criar casas
    for (int i = 0; i < 5; i++)
    {
        float x = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
        float z = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
        float size = 1.0f + ((float)rand() / RAND_MAX) * 0.5f;
        float y = getTerrainHeight(x, z);

        if (isUnderWater(x, z))
            continue; // ignora objetos submersos
        gameObjects.push_back(std::make_unique<StaticObject>(
            x, y, z, size, HOUSE, 0.7f, 0.4f, 0.1f));
    }

    // Criar inimigos
    for (int i = 0; i < 5; i++)
    {
        float x = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
        float z = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
        float size = 0.4f + ((float)rand() / RAND_MAX) * 0.3f;
        int level = 1 + rand() % 3; // Inimigos de nível 1 a 3
        float y = getTerrainHeight(x, z);

        if (isUnderWater(x, z))
            continue; // ignora objetos submersos
        gameObjects.push_back(std::make_unique<Enemy>(
            x, y, z, size, level));
    }

    // Criar itens
    for (int i = 0; i < 3; i++)
    {
        float x = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
        float z = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
        float size = 0.2f + ((float)rand() / RAND_MAX) * 0.2f;
        float y = getTerrainHeight(x, z) + 0.3f;

        if (isUnderWater(x, z))
            continue; // ignora objetos submersos
        gameObjects.push_back(std::make_unique<StaticObject>(
            x, y, z, size, ITEM, 0.9f, 0.8f, 0.1f));
    }

    generateNaturalTrail(WORLD_SIZE);

    // Criar manchas
    int numPatches = 20;
    for (int i = 0; i < numPatches; ++i)
    {
        GrassPatch patch;
        patch.x = (rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
        patch.z = (rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
        patch.radius = 2.0f + (rand() % 300) / 100.0f; // raio entre 2 e 5
        grassPatches.push_back(patch);
    }

    // Criar grama com densidade controlada
    for (float x = -WORLD_SIZE; x <= WORLD_SIZE; x += 0.5f)
    {
        for (float z = -WORLD_SIZE; z <= WORLD_SIZE; z += 0.5f)
        {
            if (hasGrass(x, z))
            {
                float offsetX = ((rand() % 100) / 100.0f - 0.5f) * 0.3f;
                float offsetZ = ((rand() % 100) / 100.0f - 0.5f) * 0.3f;
                float y = getTerrainHeight(x + offsetX, z + offsetZ) + 0.01f;

                if (isUnderWater(x, z))
                    continue; // ignora objetos submersos
                gameObjects.push_back(std::make_unique<GrassBlade>(x + offsetX, y, z + offsetZ));
            }
        }
    }

    // Portal para a dungeon
    addPortalNearEdge(WORLD_SIZE, 1.5f, -15.0f, -15.0f); // Dungeon 1
    addPortalNearEdge(WORLD_SIZE, 1.5f, 15.0f, -15.0f);  // Dungeon 2
    addPortalNearEdge(WORLD_SIZE, 1.5f, -15.0f, 15.0f);  // Dungeon 3
    addPortalNearEdge(WORLD_SIZE, 1.5f, 0.0f, 0.0f);     // Retorno ao centro
}

void Game::addPortalNearEdge(float worldSize, float margin, float destX, float destZ)
{
    float edge = worldSize - margin;
    float x = 0.0f, z = 0.0f;

    int side = rand() % 4; // 0: cima, 1: baixo, 2: esquerda, 3: direita

    switch (side)
    {
    case 0: // topo
        x = (rand() % (int)(worldSize * 2)) - worldSize;
        z = edge;
        break;
    case 1: // baixo
        x = (rand() % (int)(worldSize * 2)) - worldSize;
        z = -edge;
        break;
    case 2: // esquerda
        x = -edge;
        z = (rand() % (int)(worldSize * 2)) - worldSize;
        break;
    case 3: // direita
        x = edge;
        z = (rand() % (int)(worldSize * 2)) - worldSize;
        break;
    }

    float y = getTerrainHeight(x, z);
    gameObjects.push_back(std::make_unique<Portal>(x, y + 0.2f, z, 0.4f, destX, destZ, MapType::MAIN));
}

void Game::update()
{
    // Calcular deltaTime
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    // Atualizar o jogador
    player.update(deltaTime);

    // Verificar modo de jogo atual
    if (gameMode != 2)
    {
        gameMode = 0; // Padrão: exploração
    }
    // Atualizar objetos
    for (auto &object : gameObjects)
    {
        object->update(deltaTime);

        // Verificar inimigos para combate
        Enemy *enemy = dynamic_cast<Enemy *>(object.get());
        if (enemy && enemy->isActive())
        {
            // Atualizar comportamento do inimigo
            enemy->moveTowardsPlayer(player, deltaTime);
            if (enemy->attackPlayer(player, deltaTime))
            {
                std::cout << "Você foi atacado! Vida restante: " << player.getHealth() << std::endl;
            }

            // Se algum inimigo estiver em combate, ativar modo de combate
            if (enemy->isInCombat())
            {
                gameMode = 1;
            }

            // Verificar se o jogador derrotou o inimigo
            if (!enemy->isActive())
            {
                player.addExperience(static_cast<int>(enemy->getExperienceValue()));
                std::cout << "Inimigo derrotado! Ganhou " << enemy->getExperienceValue() << " XP." << std::endl;
            }
        }
    }

    // Verificar colisões com objetos
    checkCollisions();

    // Atualizar altura do jogador conforme o terreno
    float targetY = getTerrainHeight(player.getX(), player.getZ()) + 0.3f;
    float currentY = player.getY();
    float smoothY = lerp(currentY, targetY, 0.1f); // 0.1 = suavização
    player.setPosition(player.getX(), smoothY, player.getZ());

    // Manter o jogador dentro dos limites do mundo
    constrainPlayer();

    showPortalMessage = false;

    for (auto &obj : gameObjects)
    {
        Portal *portal = dynamic_cast<Portal *>(obj.get());
        if (portal && portal->playerIsNearby(player))
        {
            showPortalMessage = true;

            if (GetAsyncKeyState(VK_RETURN) & 0x8000)
            {
                portal->teleport(player, *this);
                std::cout << ">> Teleportado para nova área!" << std::endl;
            }
        }
    }
}

void Game::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 100.0f);

    // Configurar câmera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Posicionar câmera em 3ª pessoa
    float camX = player.getX() - cameraDistance * std::sin(player.getRotY() * M_PI / 180.0f);
    float camZ = player.getZ() - cameraDistance * std::cos(player.getRotY() * M_PI / 180.0f);

    if (topDownView)
    {
        gluLookAt(player.getX(), 50.0f, player.getZ(),
                  player.getX(), 0.0f, player.getZ(),
                  0.0f, 0.0f, -1.0f);
    }
    else
    {
        gluLookAt(
            camX, player.getY() + cameraHeight, camZ,    // Posição da câmera
            player.getX(), player.getY(), player.getZ(), // Ponto para onde a câmera olha
            0.0f, 1.0f, 0.0f                             // Vetor "up" (para cima)
        );
    }

    // Desenhar o mundo
    drawGround();

    drawLakes();

    // Desenhar objetos
    for (auto &object : gameObjects)
    {
        if (object->isActive())
        {
            object->draw();
        }
    }

    // Desenhar o jogador
    player.draw();

    // Desenhar HUD
    drawHUD();

    // Se estiver no menu de habilidades, desenhar a árvore
    if (gameMode == 2)
    {
        drawSkillTree();
    }
}

void Game::drawGround()
{
    const float step = 1.0f;
    const float size = WORLD_SIZE;

    if (currentMap == MapType::DUNGEON)
    {
        // Chão cinza na dungeon
        GLfloat ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
        GLfloat diffuse[] = {0.3f, 0.3f, 0.3f, 1.0f};
        GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
        GLfloat shininess = 10.0f;

        glDisable(GL_COLOR_MATERIAL);
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    }
    else
    {
        // Cor normal do chão no mapa principal
        GLfloat ambient[] = {0.1f, 0.6f, 0.35f, 1.0f};
        GLfloat diffuse[] = {0.1f, 0.3f, 0.1f, 1.0f};
        GLfloat specular[] = {0.05f, 0.05f, 0.05f, 1.0f};
        GLfloat shininess = 5.0f;

        glDisable(GL_COLOR_MATERIAL);
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    }

    for (float x = -size; x < size; x += step)
    {
        for (float z = -size; z < size; z += step)
        {
            float y1 = getTerrainHeight(x, z);
            float y2 = getTerrainHeight(x + step, z);
            float y3 = getTerrainHeight(x + step, z + step);
            float y4 = getTerrainHeight(x, z + step);

            glBegin(GL_QUADS);
            glVertex3f(x, y1, z);
            glVertex3f(x + step, y2, z);
            glVertex3f(x + step, y3, z + step);
            glVertex3f(x, y4, z + step);
            glEnd();
        }
    }

    // Trilhas
    GLfloat trailColor[] = {0.5f, 0.4f, 0.2f};
    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, trailColor);

    float trailWidth = 0.5f; // mais sutil
    for (size_t i = 1; i < trailCurvePoints.size(); ++i)
    {
        TrailPoint p1 = trailCurvePoints[i - 1];
        TrailPoint p2 = trailCurvePoints[i];

        p1.y = getTerrainHeight(p1.x, p1.z) + 0.01f;
        p2.y = getTerrainHeight(p2.x, p2.z) + 0.01f;

        float dx = p2.z - p1.z;
        float dz = -(p2.x - p1.x);
        float len = std::sqrt(dx * dx + dz * dz);
        dx /= len;
        dz /= len;

        glBegin(GL_TRIANGLE_STRIP);
        glVertex3f(p1.x + dx * trailWidth, p1.y, p1.z + dz * trailWidth);
        glVertex3f(p1.x - dx * trailWidth, p1.y, p1.z - dz * trailWidth);
        glVertex3f(p2.x + dx * trailWidth, p2.y, p2.z + dz * trailWidth);
        glVertex3f(p2.x - dx * trailWidth, p2.y, p2.z - dz * trailWidth);
        glEnd();
    }

    // Clareiras
    for (const auto &c : trailClearings)
    {
        float radius = 1.5f;
        float segments = 16.0f;
        float centerY = getTerrainHeight(c.x, c.z) + 0.01f;

        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(c.x, centerY, c.z);
        for (int i = 0; i <= segments; ++i)
        {
            float angle = 2.0f * M_PI * i / segments;
            float x = c.x + std::cos(angle) * radius;
            float z = c.z + std::sin(angle) * radius;
            float y = getTerrainHeight(x, z) + 0.01f;
            glVertex3f(x, y, z);
        }
        glEnd();
    }
}

void Game::drawLakes()
{
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    std::vector<std::pair<float, float>> lakeCenters = {
        {5.0f, 5.0f},
        {-7.0f, -3.0f},
        {8.0f, -6.0f},
        {-4.0f, 7.0f}};

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);

    for (const auto &center : lakeCenters)
    {
        float cx = center.first;
        float cz = center.second;
        float radius = 3.5f;

        // altura da água
        float minEdgeHeight = 1000.0f;
        for (int angle = 0; angle < 360; angle += 10)
        {
            float rad = angle * M_PI / 180.0f;
            float x = cx + cos(rad) * radius;
            float z = cz + sin(rad) * radius;
            float height = getTerrainHeight(x, z);
            if (height < minEdgeHeight)
            {
                minEdgeHeight = height;
            }
        }
        float waterHeight = minEdgeHeight - 0.02f; // ligeiramente abaixo da borda mais baixa

        // Cor da água
        float depthColor = std::min(1.0f, (waterHeight + 1.0f) / 5.0f);
        glColor4f(0.0f, 0.4f + 0.1f * sin(time), 0.7f, 0.25f);

        // Desenhar tampo superior (superfície da água)
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(cx, waterHeight, cz);
        for (int angle = 0; angle <= 360; angle += 10)
        {
            float rad = angle * M_PI / 180.0f;
            float x = cx + cos(rad) * radius;
            float z = cz + sin(rad) * radius;
            float wave = sin(rad * 4 + time * 2) * 0.02f;

            float terrainHeight = getTerrainHeight(x, z);
            float depth = waterHeight - terrainHeight;

            // Mapeia profundidade para alpha (quanto mais fundo, menos transparente)
            float alpha = std::min(0.5f, std::max(0.15f, depth * 0.8f)); // ajuste fino aqui
            glColor4f(0.2f, 0.6f + 0.1f * sin(time), 0.8f, alpha);

            glVertex3f(x, waterHeight + wave, z);
        }
        glEnd();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void Game::drawHUD()
{
    // Desabilitar iluminação e profundidade para desenhar a HUD
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // Configurar projeção ortogonal para HUD
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // Barra de vida
    float barWidth = 200.0f;
    float barHeight = 20.0f;
    float healthPercent = player.getHealth() / player.getMaxHealth();

    // Fundo da barra
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(10, windowHeight - 30);
    glVertex2f(10 + barWidth, windowHeight - 30);
    glVertex2f(10 + barWidth, windowHeight - 30 - barHeight);
    glVertex2f(10, windowHeight - 30 - barHeight);
    glEnd();

    // Barra de vida atual
    glColor3f(1.0f - healthPercent, healthPercent, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(10, windowHeight - 30);
    glVertex2f(10 + barWidth * healthPercent, windowHeight - 30);
    glVertex2f(10 + barWidth * healthPercent, windowHeight - 30 - barHeight);
    glVertex2f(10, windowHeight - 30 - barHeight);
    glEnd();

    // Barra de experiência
    float expPercent = (float)player.getExperience() / (float)player.getExperienceToNextLevel();

    // Fundo da barra
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(10, windowHeight - 60);
    glVertex2f(10 + barWidth, windowHeight - 60);
    glVertex2f(10 + barWidth, windowHeight - 60 - barHeight);
    glVertex2f(10, windowHeight - 60 - barHeight);
    glEnd();

    // Barra de experiência atual
    glColor3f(0.2f, 0.4f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(10, windowHeight - 60);
    glVertex2f(10 + barWidth * expPercent, windowHeight - 60);
    glVertex2f(10 + barWidth * expPercent, windowHeight - 60 - barHeight);
    glVertex2f(10, windowHeight - 60 - barHeight);
    glEnd();

    // Barra de cooldown de ataque
    float cooldownPercent = player.getAttackTimer() / player.getAttackCooldown();

    if (cooldownPercent > 0)
    {
        // Fundo da barra
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(10, windowHeight - 90);
        glVertex2f(10 + barWidth, windowHeight - 90);
        glVertex2f(10 + barWidth, windowHeight - 90 - barHeight);
        glVertex2f(10, windowHeight - 90 - barHeight);
        glEnd();

        // Barra de cooldown atual
        glColor3f(0.8f, 0.8f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(10, windowHeight - 90);
        glVertex2f(10 + barWidth * (1.0f - cooldownPercent), windowHeight - 90);
        glVertex2f(10 + barWidth * (1.0f - cooldownPercent), windowHeight - 90 - barHeight);
        glVertex2f(10, windowHeight - 90 - barHeight);
        glEnd();
    }

    // Texto da HUD
    glColor3f(1.0f, 1.0f, 1.0f);

    char buffer[128];
    sprintf(buffer, "Vida: %.1f/%.1f", player.getHealth(), player.getMaxHealth());
    drawText(15, windowHeight - 25, buffer);

    sprintf(buffer, "Nivel: %d   XP: %d/%d", player.getLevel(),
            player.getExperience(), player.getExperienceToNextLevel());
    drawText(15, windowHeight - 55, buffer);

    if (cooldownPercent > 0)
    {
        sprintf(buffer, "Ataque: %.1f", player.getAttackCooldown() - player.getAttackTimer());
        drawText(15, windowHeight - 85, buffer);
    }
    else
    {
        glColor3f(0.0f, 1.0f, 0.0f);
        drawText(15, windowHeight - 85, "Ataque Pronto!");
    }

    // Status do jogo
    if (gameMode == 1)
    {
        glColor3f(1.0f, 0.2f, 0.2f);
        drawText(windowWidth / 2 - 50, windowHeight - 25, "COMBATE");
        drawText(10, 20, "Pressione 1 para atacar");
    }
    else if (gameMode == 2)
    {
        glColor3f(0.2f, 0.7f, 1.0f);
        drawText(windowWidth / 2 - 60, windowHeight - 25, "MENU DE HABILIDADES");
        drawText(10, 20, "Use as teclas numericas (1-9) para melhorar habilidades");
    }
    else
    {
        glColor3f(0.2f, 1.0f, 0.2f);
        drawText(windowWidth / 2 - 60, windowHeight - 25, "EXPLORACAO");
    }

    // Pontos de habilidade
    if (player.getSkillTree().getSkillPoints() > 0)
    {
        glColor3f(1.0f, 1.0f, 0.0f);
        sprintf(buffer, "Pontos de Habilidade: %d (Pressione K para abrir menu)",
                player.getSkillTree().getSkillPoints());
        drawText(windowWidth - 400, windowHeight - 25, buffer);
    }

    // Controles
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(windowWidth - 200, 80, "Controles:");
    drawText(windowWidth - 200, 60, "W,A,S,D - Movimento");
    drawText(windowWidth - 200, 40, "Q,E - Rotacao");
    drawText(windowWidth - 200, 20, "Espaco - Ataque");

    if (showPortalMessage)
    {
        glColor3f(1.0f, 1.0f, 0.0f);
        drawText(glutGet(GLUT_WINDOW_WIDTH) / 2 - 100, 100, "Pressione [Enter] para entrar");
    }

    // Restaurar matrizes e estados
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // Reativar estados
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void Game::drawSkillTree()
{

    // Calcular layout da árvore de habilidades
    calculateSkillTreeLayout();

    // Desabilitar iluminação e profundidade para desenhar o menu
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Configurar projeção ortogonal para menu
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // Fundo semi-transparente
    glColor4f(0.0f, 0.0f, 0.2f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(50, 50);
    glVertex2f(windowWidth - 50, 50);
    glVertex2f(windowWidth - 50, windowHeight - 50);
    glVertex2f(50, windowHeight - 50);
    glEnd();

    // Título
    glColor3f(1.0f, 1.0f, 0.0f);
    drawText(windowWidth / 2 - 80, windowHeight - 80, "ARVORE DE HABILIDADES");

    // Pontos disponíveis
    char buffer[128];
    sprintf(buffer, "Pontos Disponiveis: %d", player.getSkillTree().getSkillPoints());
    drawText(windowWidth / 2 - 80, windowHeight - 110, buffer);

    // Desenhar conexões entre nós
    drawSkillTreeConnections();

    // Desenhar nós da árvore de habilidades
    drawSkillTreeNodes();

    // Desenhar tooltip para nós clicados
    drawSkillTooltip();

    // Instrução para sair
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(windowWidth / 2 - 100, 80, "Pressione K para voltar ao jogo");
    drawText(windowWidth / 2 - 140, 60, "Use o mouse para selecionar habilidades");

    // Restaurar matrizes e estados
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // Reativar estados
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}

void Game::calculateSkillTreeLayout()
{
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    const auto &skills = player.getSkillTree().getSkills();

    // Limpar nós existentes
    skillNodes.clear();

    // Definir layout básico com nós de habilidades básicas no centro
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    float baseRadius = 30.0f;
    float nodeSpacing = 150.0f;

    // Disposição em estrela das habilidades básicas (0-3)
    float angles[4] = {0, 90, 180, 270};
    for (int i = 0; i < 4; i++)
    {
        float angle = angles[i] * M_PI / 180.0f;
        float x = centerX + cos(angle) * nodeSpacing;
        float y = centerY + sin(angle) * nodeSpacing;

        SkillNode node;
        node.x = x;
        node.y = y;
        node.radius = baseRadius;
        node.skillIndex = i;
        node.hovering = false;

        skillNodes.push_back(node);
    }

    // Habilidades avançadas (4-7) como filhos das básicas
    for (int i = 0; i < 4; i++)
    {
        float angle = angles[i] * M_PI / 180.0f;
        float x = centerX + cos(angle) * nodeSpacing * 2.0f;
        float y = centerY + sin(angle) * nodeSpacing * 2.0f;

        SkillNode node;
        node.x = x;
        node.y = y;
        node.radius = baseRadius * 0.9f;
        node.skillIndex = i + 4;
        node.hovering = false;

        skillNodes.push_back(node);
    }
}

void Game::drawSkillTreeConnections()
{
    glLineWidth(2.0f);

    const auto &skills = player.getSkillTree().getSkills();

    // Desenhar linhas conectando habilidades básicas (0-3) com avançadas (4-7)
    for (int i = 0; i < 4; i++)
    {
        int parentIndex = i;
        int childIndex = i + 4;

        // Determinar cor da linha baseada no estado das habilidades
        if (skills[childIndex]->getLevel() > 0)
        {
            // Conexão ativa
            glColor3f(0.2f, 0.8f, 0.2f);
        }
        else if (skills[parentIndex]->getLevel() > 0)
        {
            // Conexão disponível
            glColor3f(0.8f, 0.8f, 0.2f);
        }
        else
        {
            // Conexão indisponível
            glColor3f(0.5f, 0.5f, 0.5f);
        }

        glBegin(GL_LINES);
        glVertex2f(skillNodes[parentIndex].x, skillNodes[parentIndex].y);
        glVertex2f(skillNodes[childIndex].x, skillNodes[childIndex].y);
        glEnd();
    }
}

void Game::drawSkillTreeNodes()
{
    const auto &skills = player.getSkillTree().getSkills();

    // Desenhar cada nó
    for (size_t i = 0; i < skillNodes.size(); i++)
    {
        const auto &node = skillNodes[i];
        const auto &skill = skills[node.skillIndex];

        // Determinar cor do nó baseada no estado da habilidade
        if (skill->getLevel() == 0)
        {
            if (skill->canLearn() && player.getSkillTree().getSkillPoints() > 0)
            {
                // Nó disponível para aprendizado
                glColor4f(0.8f, 0.8f, 0.2f, node.hovering ? 0.9f : 0.7f);
            }
            else
            {
                // Nó indisponível
                glColor4f(0.5f, 0.5f, 0.5f, node.hovering ? 0.9f : 0.7f);
            }
        }
        else if (skill->getLevel() == skill->getMaxLevel())
        {
            // Nó com nível máximo
            glColor4f(1.0f, 0.6f, 0.0f, node.hovering ? 0.9f : 0.7f);
        }
        else
        {
            if (skill->canLearn() && player.getSkillTree().getSkillPoints() > 0)
            {
                // Nó com nível parcial e pode melhorar
                glColor4f(0.2f, 0.8f, 0.2f, node.hovering ? 0.9f : 0.7f);
            }
            else
            {
                // Nó com nível parcial
                glColor4f(0.2f, 0.6f, 0.8f, node.hovering ? 0.9f : 0.7f);
            }
        }

        // Desenhar círculo do nó
        drawFilledCircle(node.x, node.y, node.radius, 20);

        // Borda do nó
        if (node.hovering)
        {
            glLineWidth(2.5f);
            glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
        }
        else
        {
            glLineWidth(1.5f);
            glColor4f(0.8f, 0.8f, 0.8f, 0.7f);
        }
        drawCircle(node.x, node.y, node.radius, 20);

        // Desenhar ícone ou indicador de nível dentro do nó
        char levelText[8];
        sprintf(levelText, "%d/%d", skill->getLevel(), skill->getMaxLevel());

        // Centralizar texto
        float textWidth = strlen(levelText) * 8.0f;
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(node.x - textWidth / 2, node.y - 6, levelText);

        // Desenhar ícone baseado no tipo de habilidade
        drawSkillIcon(node.x, node.y - 20, skill->getType());
    }
}

void Game::drawSkillIcon(float x, float y, SkillType type)
{
    glLineWidth(2.0f);
    glColor3f(1.0f, 1.0f, 1.0f);

    float iconSize = 8.0f;

    switch (type)
    {
    case ATTACK:
        // Ícone de espada
        glBegin(GL_LINES);
        glVertex2f(x - iconSize, y + iconSize);
        glVertex2f(x + iconSize, y - iconSize);
        glVertex2f(x - iconSize / 2, y - iconSize / 2);
        glVertex2f(x + iconSize / 2, y + iconSize / 2);
        glEnd();
        break;

    case DEFENSE:
        // Ícone de escudo
        drawCircle(x, y, iconSize, 8);
        break;

    case MAGIC:
        // Ícone de estrela
        drawStar(x, y, iconSize, 5);
        break;

    case SPEED:
        // Ícone de velocidade
        glBegin(GL_LINES);
        glVertex2f(x - iconSize, y);
        glVertex2f(x + iconSize, y);
        glVertex2f(x + iconSize / 2, y - iconSize / 2);
        glVertex2f(x + iconSize, y);
        glVertex2f(x + iconSize / 2, y + iconSize / 2);
        glVertex2f(x + iconSize, y);
        glEnd();
        break;
    }
}

void Game::drawSkillTooltip()
{
    if (!skillTooltip.visible)
        return;

    const auto &skills = player.getSkillTree().getSkills();
    const auto &skill = skills[skillTooltip.skillIndex];

    // Fundo da tooltip
    glColor4f(0.1f, 0.1f, 0.3f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(skillTooltip.x, skillTooltip.y);
    glVertex2f(skillTooltip.x + skillTooltip.width, skillTooltip.y);
    glVertex2f(skillTooltip.x + skillTooltip.width, skillTooltip.y + skillTooltip.height);
    glVertex2f(skillTooltip.x, skillTooltip.y + skillTooltip.height);
    glEnd();

    // Borda
    glColor4f(0.8f, 0.8f, 0.9f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(skillTooltip.x, skillTooltip.y);
    glVertex2f(skillTooltip.x + skillTooltip.width, skillTooltip.y);
    glVertex2f(skillTooltip.x + skillTooltip.width, skillTooltip.y + skillTooltip.height);
    glVertex2f(skillTooltip.x, skillTooltip.y + skillTooltip.height);
    glEnd();

    // Título da habilidade
    glColor3f(1.0f, 1.0f, 0.0f);
    drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 25, skill->getName().c_str());

    // Informações da habilidade
    glColor3f(1.0f, 1.0f, 1.0f);
    char buffer[128];

    // Tipo de habilidade
    const char *typeStr = "";
    switch (skill->getType())
    {
    case ATTACK:
        typeStr = "Ataque";
        break;
    case DEFENSE:
        typeStr = "Defesa";
        break;
    case MAGIC:
        typeStr = "Magia";
        break;
    case SPEED:
        typeStr = "Velocidade";
        break;
    }
    sprintf(buffer, "Tipo: %s", typeStr);
    drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 45, buffer);

    // Nível atual e máximo
    sprintf(buffer, "Nivel: %d/%d", skill->getLevel(), skill->getMaxLevel());
    drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 65, buffer);

    // Valor atual
    sprintf(buffer, "Valor: %.1f", skill->getValue());
    drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 85, buffer);

    // Custo
    sprintf(buffer, "Custo: 1 ponto de habilidade");
    drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 105, buffer);

    // Disponibilidade
    if (!skill->canLearn())
    {
        glColor3f(1.0f, 0.3f, 0.3f);
        if (skill->getLevel() >= skill->getMaxLevel())
        {
            drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 125, "Nivel maximo atingido");
        }
        else
        {
            drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 125, "Pre-requisitos nao cumpridos");
        }
    }
    else if (player.getSkillTree().getSkillPoints() <= 0)
    {
        glColor3f(1.0f, 0.3f, 0.3f);
        drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 125, "Pontos insuficientes");
    }

    // Botões de confirmação
    if (skillTooltip.showConfirmation && skill->canLearn() && player.getSkillTree().getSkillPoints() > 0)
    {
        // Botão Sim
        glColor4f(0.2f, 0.7f, 0.2f, 0.8f);
        glBegin(GL_QUADS);
        glVertex2f(skillTooltip.x + 30, skillTooltip.y + 20);
        glVertex2f(skillTooltip.x + 100, skillTooltip.y + 20);
        glVertex2f(skillTooltip.x + 100, skillTooltip.y + 45);
        glVertex2f(skillTooltip.x + 30, skillTooltip.y + 45);
        glEnd();

        // Botão Não
        glColor4f(0.7f, 0.2f, 0.2f, 0.8f);
        glBegin(GL_QUADS);
        glVertex2f(skillTooltip.x + 150, skillTooltip.y + 20);
        glVertex2f(skillTooltip.x + 220, skillTooltip.y + 20);
        glVertex2f(skillTooltip.x + 220, skillTooltip.y + 45);
        glVertex2f(skillTooltip.x + 150, skillTooltip.y + 45);
        glEnd();

        // Textos dos botões
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(skillTooltip.x + 45, skillTooltip.y + 30, "Aprender");
        drawText(skillTooltip.x + 175, skillTooltip.y + 30, "Cancelar");
    }
}

// Funções auxiliares de desenho
void Game::drawFilledCircle(float x, float y, float radius, int segments)
{
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++)
    {
        float theta = 2.0f * M_PI * i / segments;
        float px = x + radius * cosf(theta);
        float py = y + radius * sinf(theta);
        glVertex2f(px, py);
    }
    glEnd();
}

void Game::drawCircle(float x, float y, float radius, int segments)
{
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++)
    {
        float theta = 2.0f * M_PI * i / segments;
        float px = x + radius * cosf(theta);
        float py = y + radius * sinf(theta);
        glVertex2f(px, py);
    }
    glEnd();
}

void Game::drawStar(float x, float y, float radius, int points)
{
    float innerRadius = radius * 0.4f;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < points * 2; i++)
    {
        float theta = M_PI * i / points;
        float r = (i % 2 == 0) ? radius : innerRadius;
        float px = x + r * cosf(theta);
        float py = y + r * sinf(theta);
        glVertex2f(px, py);
    }
    glEnd();
}

void Game::drawText(float x, float y, const char *string)
{
    // Posiciona o texto
    glRasterPos2f(x, y);

    // Desenha cada caractere
    for (const char *c = string; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void Game::checkCollisions()
{
    for (auto &object : gameObjects)
    {
        if (!object->isActive() || !object->isCollidable())
            continue;

        if (player.checkCollision(*object))
        {
            if (object->getType() == ITEM)
            {
                object->setActive(false);
                player.heal(10.0f);
                player.addExperience(10000);
                std::cout << "Item coletado! +10 XP, +10 Vida" << std::endl;
                continue;
            }

            float dx = player.getX() - object->getX();
            float dz = player.getZ() - object->getZ();
            float dist = std::sqrt(dx * dx + dz * dz);

            if (dist > 0.1f)
            {
                float overlap = (player.getSize() + object->getSize()) - dist;
                float x = player.getX() + (dx / dist) * overlap * 1.1f;
                float z = player.getZ() + (dz / dist) * overlap * 1.1f;
                player.setPosition(x, player.getY(), z);
            }
            else
            {
                float x = player.getX() + ((float)rand() / RAND_MAX * 2.0f - 1.0f) * 0.1f;
                float z = player.getZ() + ((float)rand() / RAND_MAX * 2.0f - 1.0f) * 0.1f;
                player.setPosition(x, player.getY(), z);
            }
        }
    }
}

void Game::constrainPlayer()
{
    // Manter o jogador dentro dos limites do mundo
    float x = player.getX();
    float z = player.getZ();

    if (x > WORLD_SIZE)
        x = WORLD_SIZE;
    if (x < -WORLD_SIZE)
        x = -WORLD_SIZE;
    if (z > WORLD_SIZE)
        z = WORLD_SIZE;
    if (z < -WORLD_SIZE)
        z = -WORLD_SIZE;

    player.setPosition(x, player.getY(), z);
}

void Game::handleKeyPress(unsigned char key, int x, int y)
{
    if (gameMode == 2)
    {
        // Menu de habilidades
        if (key == 'k')
        {                 // K
            gameMode = 0; // Voltar para exploração
        }
        else if (key >= '1' && key <= '9')
        {
            int skillIndex = key - '1';
            const auto &skills = player.getSkillTree().getSkills();

            if (skillIndex < static_cast<int>(skills.size()))
            {
                std::string skillName = skills[skillIndex]->getName();
                if (player.getSkillTree().useSkillPoint(skillName))
                {
                    std::cout << "Habilidade melhorada: " << skillName << std::endl;
                }
                else
                {
                    std::cout << "Não foi possível melhorar a habilidade." << std::endl;
                }
            }
        }
        return;
    }

    // Controles normais
    switch (key)
    {
    case 'w':
    case 'W':
        player.moveForward();
        break;
    case 's':
    case 'S':
        player.moveBackward();
        break;
    case 't':
        topDownView = !topDownView;
        break;
    case 'a':
    case 'A':
        player.strafeLeft();
        break;
    case 'd':
    case 'D':
        player.strafeRight();
        break;
    case 'q':
    case 'Q':
        player.rotateLeft();
        break;
    case 'e':
    case 'E':
        player.rotateRight();
        break;
    case 'c':
    case 'C':
        // Alternar distância da câmera
        cameraDistance = (cameraDistance == 5.0f) ? 2.0f : 5.0f;
        break;
    case ' ':
        // Atacar
        if (player.attack())
        {
            std::cout << "Atacando!" << std::endl;

            for (auto &object : gameObjects)
            {
                Enemy *enemy = dynamic_cast<Enemy *>(object.get());
                if (enemy && enemy->isActive())
                {
                    float dx = enemy->getX() - player.getX();
                    float dz = enemy->getZ() - player.getZ();
                    float dist = std::sqrt(dx * dx + dz * dz);

                    if (dist < 2.0f)
                    {
                        enemy->takeDamage(player.getAttackDamage());
                        std::cout << "Dano causado em inimigo: " << player.getAttackDamage() << std::endl;
                    }
                }
            }
        }
        break;
    case '1':
        // Ataque alternativo
        if (gameMode == 1 && player.attack())
        {
            std::cout << "Ataque Especial!" << std::endl;

            for (auto &object : gameObjects)
            {
                Enemy *enemy = dynamic_cast<Enemy *>(object.get());
                if (enemy && enemy->isActive())
                {
                    float dx = enemy->getX() - player.getX();
                    float dz = enemy->getZ() - player.getZ();
                    float dist = std::sqrt(dx * dx + dz * dz);

                    if (dist < 3.0f)
                    {                                                   // Alcance maior
                        float damage = player.getAttackDamage() * 1.5f; // Dano aumentado
                        enemy->takeDamage(damage);
                        std::cout << "Dano crítico causado: " << damage << std::endl;
                    }
                }
            }
        }
        break;
    case 'k':
    case 'K': // K
        // Alternar para o menu de habilidades
        gameMode = (gameMode == 2) ? 0 : 2;
        break;
    case 27: // ESC
        exit(0);
        break;
    }
}

void Game::handleSpecialKeyPress(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_UP:
        cameraHeight += 0.1f;
        break;
    case GLUT_KEY_DOWN:
        cameraHeight -= 0.1f;
        if (cameraHeight < 0.5f)
            cameraHeight = 0.5f;
        break;
    }
}

// Adicionar estes métodos à classe Game
void Game::handleMouseClick(int button, int state, int x, int y)
{
    lastMouseX = x;
    lastMouseY = y;

    if (button == GLUT_LEFT_BUTTON)
    {
        mouseLeftDown = (state == GLUT_DOWN);

        // Verificar clique na árvore de habilidades
        if (mouseLeftDown && gameMode == 2)
        {
            checkSkillTreeClick(x, y);
        }
    }
    else if (button == GLUT_RIGHT_BUTTON)
    {
        mouseRightDown = (state == GLUT_DOWN);
    }
}

void Game::handleMouseMotion(int x, int y)
{
    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;

    if (mouseRightDown)
    {
        // Rotacionar a câmera com o mouse direito
        player.rotateRight(deltaX * mouseSensitivity);

        // Ajustar altura da câmera com movimento vertical do mouse
        cameraHeight -= deltaY * mouseSensitivity * 0.05f;
        if (cameraHeight < 0.5f)
            cameraHeight = 0.5f;
        if (cameraHeight > 5.0f)
            cameraHeight = 5.0f;
    }

    if (gameMode == 2)
    {
        // Atualizar estado de hover nos nós da árvore de habilidades
        updateSkillNodeHover(x, y);
    }

    lastMouseX = x;
    lastMouseY = y;
}

void Game::handlePassiveMouseMotion(int x, int y)
{
    lastMouseX = x;
    lastMouseY = y;

    if (gameMode == 2)
    {
        // Atualizar estado de hover nos nós da árvore de habilidades
        updateSkillNodeHover(x, y);
    }
}

void Game::checkSkillTreeClick(int x, int y)
{
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    y = windowHeight - y; // Inverter coordenada Y para corresponder ao nosso sistema

    // Verificar se clicou no botão de confirmação, se a tooltip estiver visível
    if (skillTooltip.visible && skillTooltip.showConfirmation)
    {
        // Verificar se clicou no botão "Sim"
        if (x >= skillTooltip.x + 30 &&
            x <= skillTooltip.x + 100 &&
            y >= skillTooltip.y + 20 &&
            y <= skillTooltip.y + 45)
        {

            const auto &skills = player.getSkillTree().getSkills();
            std::string skillName = skills[skillTooltip.skillIndex]->getName();
            if (player.getSkillTree().useSkillPoint(skillName))
            {
                std::cout << "Habilidade melhorada: " << skillName << std::endl;
            }
            skillTooltip.visible = false;
            return;
        }

        // Verificar se clicou no botão "Não"
        if (x >= skillTooltip.x + 150 &&
            x <= skillTooltip.x + 220 &&
            y >= skillTooltip.y + 20 &&
            y <= skillTooltip.y + 45)
        {

            skillTooltip.visible = false;
            return;
        }
    }

    // Verificar clique em uma habilidade para mostrar a tooltip
    for (const auto &node : skillNodes)
    {
        float dx = x - node.x;
        float dy = y - node.y;
        float distSq = dx * dx + dy * dy;

        if (distSq < node.radius * node.radius)
        {
            // Mostrar tooltip
            skillTooltip.visible = true;
            skillTooltip.skillIndex = node.skillIndex;
            skillTooltip.x = x + 20; // Posicionar a tooltip ao lado do mouse
            skillTooltip.y = y - skillTooltip.height / 2;
            skillTooltip.showConfirmation = true;

            // Garantir que a tooltip fique dentro da tela
            int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
            if (skillTooltip.x + skillTooltip.width > windowWidth)
                skillTooltip.x = windowWidth - skillTooltip.width - 10;

            if (skillTooltip.y < 10)
                skillTooltip.y = 10;
            else if (skillTooltip.y + skillTooltip.height > windowHeight)
                skillTooltip.y = windowHeight - skillTooltip.height - 10;

            return;
        }
    }

    // Se clicou fora dos nós e fora dos botões, fechar a tooltip
    skillTooltip.visible = false;
}

void Game::updateSkillNodeHover(int x, int y)
{
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    y = windowHeight - y; // Inverter coordenada Y

    for (auto &node : skillNodes)
    {
        float dx = x - node.x;
        float dy = y - node.y;
        float distSq = dx * dx + dy * dy;

        node.hovering = (distSq < node.radius * node.radius);
    }
}

// Getters e setters
Player& Game::getPlayer() {
    return player;
}

int Game::getGameMode() const {
    return gameMode;
}

void Game::setGameMode(int mode) {
    gameMode = mode;
}

// Responders de eventos
void Game::mouseCallback(int button, int state, int x, int y)
{
    GetInstance().handleMouseClick(button, state, x, y);
}

void Game::motionCallback(int x, int y)
{
    GetInstance().handleMouseMotion(x, y);
}

void Game::passiveMotionCallback(int x, int y)
{
    GetInstance().handlePassiveMouseMotion(x, y);
}
void Game::displayCallback()
{
    GetInstance().render();
    glutSwapBuffers();
}

void Game::reshapeCallback(int w, int h)
{
    glViewport(0, 0, w, h);
}

void Game::keyboardCallback(unsigned char key, int x, int y)
{
    GetInstance().handleKeyPress(key, x, y);
}

void Game::specialCallback(int key, int x, int y)
{
    GetInstance().handleSpecialKeyPress(key, x, y);
}

void Game::timerCallback(int value)
{
    GetInstance().update();
    glutPostRedisplay();
    glutTimerFunc(16, timerCallback, 0); // 60 FPS aproximadamente
}

// Singleton para facilitar callbacks
Game &Game::GetInstance()
{
    static Game instance;
    return instance;
}

float Game::getTerrainHeight(float x, float z) {
    float height = 0.0f;

    // Colinas suaves
    height += std::sin(x * 0.1f) * 0.5f;
    height += std::cos(z * 0.1f) * 0.5f;

    // Ruído sutil
    height += (std::sin(x * 0.3f + z * 0.5f) * 0.3f);

    // Lista de centros de lagos
    std::vector<std::pair<float, float>> lakeCenters = {
        {5.0f, 5.0f},
        {-7.0f, -3.0f},
        {8.0f, -6.0f},
        {-4.0f, 7.0f}
    };

    for (const auto& center : lakeCenters) {
        float dist = std::sqrt((x - center.first)*(x - center.first) + (z - center.second)*(z - center.second));
        if (dist < 3.5f) {
            height -= (3.5f - dist) * 0.4f;
        }
    }

    return height;
}