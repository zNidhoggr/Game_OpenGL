#include "game.hpp"

const float Game::WORLD_SIZE = 25.0f;

Game::Game() : player(0.0f, 0.5f, 0.0f),
               cameraDistance(5.0f),
               hud(player),
               cameraHeight(2.0f),
               cameraAngle(0.0f),
               gameMode(STATE_GAME::PLAYING_EXPLORER),
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
    loader.loadModel("./src/objs/espada.obj", nullptr,
                     {getPlayer().getX(), getPlayer().getY(), getPlayer().getZ() - 0.4f},
                     {270.0f, 1.0f, 0.0f, 0.0f},
                     {0.03f, 0.03f, 0.03f},
                     {0.1f, 0.4f, 0.9f},      // Cor do modelo
                     {0.2f, 0.2f, 0.2f},      // Cor ambiente
                     {0.0f, 0.2f, 1.0f},      // Cor difusa
                     {0.001f, 0.001f, 0.01f}, // Cor especular
                     1.0f, ModelType::OBJ);   // Brilho
    loader.loadModel("./src/objs/cat_meme.obj", "./src/objs/maxwell_the_cat_dingus.mtl",
                     {getPlayer().getX() + 4.0f, getPlayer().getY(), getPlayer().getZ() + 4.0f},
                     {0.0f, 0.0f, 0.0f, 0.0f},
                     {0.05f, 0.05f, 0.05f},
                     {-1.0f, -1.0f, -1.0f}, // Cor do modelo
                     {-1.0f, -1.0f, -1.0f}, // Cor ambiente
                     {-1.0f, -1.0f, -1.0f}, // Cor difusa
                     {-1.0f, -1.0f, -1.0f}, // Cor especular
                     0.0f, ModelType::OBJ); // Brilho
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
    float startZ = (rand() % 2000 / 100.0f - 1.0f) * worldSize; // aleatório entre -worldSize e worldSize
    float endZ = (rand() % 2000 / 100.0f - 1.0f) * worldSize;

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
    float worldX = (DUNGEON_WIDTH / 2) * 5.0f;
    float worldZ = (DUNGEON_HEIGHT / 2) * 5.0f;
    // float y = getTerrainHeight(worldX, worldZ);

    // Adiciona o portal para a DUNGEON_ONE_LEVEL
    float px = 8.0f, pz = -5.0f;
    float py = getTerrainHeight(px, pz);
    gameObjects.push_back(std::make_unique<Portal>(x, y + 0.2f, z, 0.4f, 0.0f, 0.0f, MapType::DUNGEON_ONE_LEVEL));
}
void Game::loadHouseInterior()
{
    currentMap = MapType::HOUSE; // Define o tipo de mapa como o interior da casa
    gameObjects.clear();         // Limpa objetos anteriores

    // Definindo a altura e a posição da casa
    float houseX = 0.0f, houseZ = 0.0f;
    float houseY = getTerrainHeight(houseX, houseZ) + 0.3f; // Ajuste a altura conforme necessário
    gameObjects.push_back(std::make_unique<StaticObject>(houseX, houseY, houseZ, 0.4f, ObjectType::WALL, 0.7f, 0.7f, 0.7f));
    gameObjects.push_back(std::make_unique<StaticObject>(houseX + 5.0f, houseY, houseZ, 0.4f, ObjectType::WALL, 0.7f, 0.7f, 0.7f));
    gameObjects.push_back(std::make_unique<StaticObject>(houseX, houseY, houseZ - 5.0f, 0.4f, ObjectType::WALL, 0.7f, 0.7f, 0.7f));
    gameObjects.push_back(std::make_unique<StaticObject>(houseX + 5.0f, houseY, houseZ - 5.0f, 0.4f, ObjectType::WALL, 0.7f, 0.7f, 0.7f));
    gameObjects.push_back(std::make_unique<StaticObject>(houseX + 2.5f, houseY + 3.0f, houseZ - 2.5f, 0.4f, ObjectType::ROOF, 0.5f, 0.5f, 0.5f));
    gameObjects.push_back(std::make_unique<StaticObject>(houseX + 2.0f, houseY + 0.2f, houseZ - 2.5f, 0.4f, ObjectType::TABLE, 0.4f, 0.2f, 0.1f));
    gameObjects.push_back(std::make_unique<StaticObject>(houseX + 1.5f, houseY + 0.2f, houseZ - 3.0f, 0.4f, ObjectType::CHAIR, 0.6f, 0.3f, 0.1f));

    gameObjects.push_back(std::make_unique<StaticObject>(houseX + 1.0f, houseY + 1.0f, houseZ - 5.1f, 0.4f, ObjectType::WINDOW, 0.9f, 0.9f, 0.9f));

    gameObjects.push_back(std::make_unique<StaticObject>(houseX + 2.0f, houseY + 0.2f, houseZ - 5.0f, 0.4f, ObjectType::DOOR, 0.7f, 0.3f, 0.1f));

    gameObjects.push_back(std::make_unique<StaticObject>(houseX + 3.0f, houseY + 2.0f, houseZ - 2.5f, 0.4f, ObjectType::LAMP, 1.0f, 1.0f, 0.5f));
}

void Game::loadDungeonMap()
{
    currentMap = MapType::DUNGEON_ONE_LEVEL;
    gameObjects.clear();
    skyColor[0] = 0.05f;
    skyColor[1] = 0.05f;
    skyColor[2] = 0.1f;

    for (int i = 0; i < DUNGEON_WIDTH; i++)
        for (int j = 0; j < DUNGEON_HEIGHT; j++)
            dungeonGrid[i][j] = false;

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
                float tileSize = 20.0f; // mesmo valor usado para espaçar salas

                // Muralha contínua ao redor da DUNGEON_ONE_LEVEL, exceto onde há o portal
                for (int i = 0; i < DUNGEON_WIDTH; ++i)
                {
                    for (int j = 0; j < DUNGEON_HEIGHT; ++j)
                    {
                        bool isEdge = (i == 0 || j == 0 || i == DUNGEON_WIDTH - 1 || j == DUNGEON_HEIGHT - 1);
                        if (isEdge)
                        {
                            float wx = (i - DUNGEON_WIDTH / 2) * 10.0f;
                            float wz = (j - DUNGEON_HEIGHT / 2) * 10.0f;
                            float wy = getTerrainHeight(wx, wz);

                            // Exceção: deixar espaço para o portal
                            bool isPortalGap = (i == DUNGEON_WIDTH / 2 && j == DUNGEON_HEIGHT - 1);
                            if (isPortalGap)
                                continue;

                            // Define escalas diferentes para blocos horizontais e verticais
                            float scaleX = (j == 0 || j == DUNGEON_HEIGHT - 1) ? 2.5f : 0.5f;
                            float scaleZ = (i == 0 || i == DUNGEON_WIDTH - 1) ? 2.5f : 0.5f;

                            gameObjects.push_back(std::make_unique<StaticObject>(
                                wx + 10.0f, wy + 10.0f, wz, 2.5f, WALL, scaleX, 1.0f, scaleZ));
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
                    float worldX = (DUNGEON_WIDTH / 2) * 5.0f;
                }
            }
        }
    }

    // Portal de volta ao mapa principal
    float exitX = 0.0f, exitZ = -20.0f;
    float exitY = getTerrainHeight(exitX, exitZ);
    gameObjects.push_back(std::make_unique<Portal>(
        exitX, exitY, exitZ, 0.4f, 0.0f, 0.0f, MapType::DUNGEON_TWO_LEVEL));

    // Jogador no centro da DUNGEON_ONE_LEVEL
    float startX = 0.0f, startZ = 0.0f;
    float startY = getTerrainHeight(startX, startZ) + 0.3f;
    player.setPosition(startX, startY, startZ);
}

void Game::loadDungeonMap_Level2()
{
    currentMap = MapType::DUNGEON_TWO_LEVEL;
    gameObjects.clear();
    skyColor[0] = 0.05f;
    skyColor[1] = 0.05f;
    skyColor[2] = 0.1f;

    // Limpa grid
    for (int i = 0; i < DUNGEON_WIDTH; i++)
        for (int j = 0; j < DUNGEON_HEIGHT; j++)
            dungeonGrid[i][j] = false;

    // Gera um "caminho" mais complexo e ramificado
    int x = 5, z = 5;
    dungeonGrid[x][z] = true;
    for (int i = 0; i < 30; i++)
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

        // Adiciona ramificações aleatórias
        if (rand() % 3 == 0)
        {
            int branchX = x + (rand() % 3 - 1);
            int branchZ = z + (rand() % 3 - 1);
            if (branchX >= 0 && branchX < DUNGEON_WIDTH && branchZ >= 0 && branchZ < DUNGEON_HEIGHT)
                dungeonGrid[branchX][branchZ] = true;
        }
    }

    // Gerar salas e obstáculos
    for (int i = 0; i < DUNGEON_WIDTH; i++)
    {
        for (int j = 0; j < DUNGEON_HEIGHT; j++)
        {
            if (dungeonGrid[i][j])
            {
                float worldX = (i - DUNGEON_WIDTH / 2) * 5.0f;
                float worldZ = (j - DUNGEON_HEIGHT / 2) * 5.0f;
                float y = getTerrainHeight(worldX, worldZ);

                if (rand() % 4 == 0) // Obstáculos
                {
                    gameObjects.push_back(std::make_unique<StaticObject>(worldX, y, worldZ, 0.6f, ObjectType::WALL, 0.3f, 0.3f, 0.3f));
                }

                if (rand() % 1 == 0) // Inimigos
                {
                    gameObjects.push_back(std::make_unique<Enemy>(worldX, y + 0.3f, worldZ, 0.5f, 3));
                }
            }
        }
    }

    // Portal de saída
    float exitX = 0.0f, exitZ = -20.0f;
    float exitY = getTerrainHeight(exitX, exitZ);
    gameObjects.push_back(std::make_unique<Portal>(exitX, exitY, exitZ, 0.4f, 0.0f, 0.0f, MapType::DUNGEON_THREE_LEVEL));

    // Jogador no centro
    float startX = 0.0f, startZ = 0.0f;
    float startY = getTerrainHeight(startX, startZ) + 0.3f;
    player.setPosition(startX, startY, startZ);
}

void Game::loadDungeonMap_Level3()
{
    currentMap = MapType::DUNGEON_THREE_LEVEL;
    gameObjects.clear();
    skyColor[0] = 0.05f;
    skyColor[1] = 0.05f;
    skyColor[2] = 0.1f;

    // Limpa grid
    for (int i = 0; i < DUNGEON_WIDTH; i++)
        for (int j = 0; j < DUNGEON_HEIGHT; j++)
            dungeonGrid[i][j] = false;

    // Gera um "caminho" mais complexo entre salas
    int x = 5, z = 5;
    dungeonGrid[x][z] = true;
    for (int i = 0; i < 20; i++)
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

    // Gerar inimigos e obstáculos
    for (int i = 0; i < DUNGEON_WIDTH; i++)
    {
        for (int j = 0; j < DUNGEON_HEIGHT; j++)
        {
            if (dungeonGrid[i][j])
            {
                float worldX = (i - DUNGEON_WIDTH / 2) * 5.0f;
                float worldZ = (j - DUNGEON_HEIGHT / 2) * 5.0f;
                float y = getTerrainHeight(worldX, worldZ);

                if (rand() % 2 == 0) // Mais inimigos
                {
                    gameObjects.push_back(std::make_unique<Enemy>(worldX, y + 0.3f, worldZ, 0.5f, 3));
                }

                if (rand() % 2 == 0) // Obstáculos
                {
                    gameObjects.push_back(std::make_unique<StaticObject>(worldX + 1.0f, y, worldZ + 1.0f, 0.6f, ObjectType::WALL, 0.3f, 0.3f, 0.3f));
                }
            }
        }
    }

    // Portal de saída
    float exitX = 0.0f, exitZ = -20.0f;
    float exitY = getTerrainHeight(exitX, exitZ);
    gameObjects.push_back(std::make_unique<Portal>(exitX, exitY, exitZ, 0.4f, 0.0f, 0.0f, MapType::BOSS));

    // Jogador no centro
    float startX = 0.0f, startZ = 0.0f;
    float startY = getTerrainHeight(startX, startZ) + 0.3f;
    player.setPosition(startX, startY, startZ);
}

void Game::loadDungeonMap_Boss()
{
    currentMap = MapType::BOSS;
    gameObjects.clear();
    skyColor[0] = 0.1f;
    skyColor[1] = 0.02f;
    skyColor[2] = 0.02f;

    // Limpa grid
    for (int i = 0; i < DUNGEON_WIDTH; i++)
        for (int j = 0; j < DUNGEON_HEIGHT; j++)
            dungeonGrid[i][j] = false;

    // Arena central
    for (int i = 3; i < DUNGEON_WIDTH - 3; i++)
        for (int j = 3; j < DUNGEON_HEIGHT - 3; j++)
            dungeonGrid[i][j] = true;

    // Paredes ao redor
    for (int i = 0; i < DUNGEON_WIDTH; i++)
    {
        for (int j = 0; j < DUNGEON_HEIGHT; j++)
        {
            if (!dungeonGrid[i][j])
            {
                float worldX = (i - DUNGEON_WIDTH / 2) * 5.0f;
                float worldZ = (j - DUNGEON_HEIGHT / 2) * 5.0f;
                float y = getTerrainHeight(worldX, worldZ);

                gameObjects.push_back(std::make_unique<StaticObject>(
                    worldX, y, worldZ, 0.6f, ObjectType::WALL, 0.3f, 0.3f, 0.3f));
            }
        }
    }

    float exitX = 0.0f, exitZ = -20.0f;
    float exitY = getTerrainHeight(exitX, exitZ);
    gameObjects.push_back(std::make_unique<Portal>(exitX, exitY, exitZ, 0.4f, 0.0f, 0.0f, MapType::PARASIDE));

    gameObjects.push_back(std::unique_ptr<Boss>(std::make_unique<Boss>((DUNGEON_WIDTH / 2) * 1.0f, 1.0f, (DUNGEON_HEIGHT / 2) * 1.0f, 0.6f, 1)));

    float startX = 0.0f, startZ = 10.0f;
    float startY = getTerrainHeight(startX, startZ) + 0.3f;
    player.setPosition(startX, startY, startZ);
}

void Game::loadParasideMap()
{
    currentMap = MapType::PARASIDE;
    gameObjects.clear();

    // Céu azul claro para um efeito paradisíaco
    skyColor[0] = 0.7f;
    skyColor[1] = 0.9f;
    skyColor[2] = 1.0f;

    // Limpa grid
    for (int i = 0; i < DUNGEON_WIDTH; i++)
        for (int j = 0; j < DUNGEON_HEIGHT; j++)
            dungeonGrid[i][j] = false;

    // Área central mais aberta para o checkpoint
    float centerX = 0.0f;
    float centerZ = 0.0f;

    // Área gramada com árvores e flores
    for (int i = 2; i < DUNGEON_WIDTH - 2; i++)
    {
        for (int j = 2; j < DUNGEON_HEIGHT - 2; j++)
        {
            float worldX = (i - DUNGEON_WIDTH / 2) * 5.0f;
            float worldZ = (j - DUNGEON_HEIGHT / 2) * 5.0f;
            float distToCenter = sqrt(pow(worldX - centerX, 2) + pow(worldZ - centerZ, 2));

            // Mantém o centro mais limpo para o checkpoint
            if (distToCenter < 5.0f)
                continue;

            float y = getTerrainHeight(worldX, worldZ);

            // Adiciona árvores com densidade menor (mais espaçadas)
            if (rand() % 8 == 0)
            {
                gameObjects.push_back(std::make_unique<StaticObject>(
                    worldX, y, worldZ, 1.0f, ObjectType::TREE, 0.8f, 3.5f, 0.8f));
            }

            // Adiciona pedras pequenas espalhadas
            else if (rand() % 15 == 0)
            {
                gameObjects.push_back(std::make_unique<StaticObject>(
                    worldX, y, worldZ, 0.5f, ObjectType::ROCK, 0.7f, 0.5f, 0.7f));
            }
        }
    }

    // Adiciona o checkpoint com fogueira no centro
    float checkpointX = centerX;
    float checkpointZ = centerZ;
    float checkpointY = getTerrainHeight(checkpointX, checkpointZ);

    // Adiciona pedras em círculo para a fogueira
    float stoneDiameter = 0.5f;
    int numStones = 8;
    float radius = 1.5f;

    for (int i = 0; i < numStones; i++)
    {
        float angle = i * (2 * M_PI / numStones);
        float stoneX = checkpointX + radius * cos(angle);
        float stoneZ = checkpointZ + radius * sin(angle);
        float stoneY = getTerrainHeight(stoneX, stoneZ);

        gameObjects.push_back(std::make_unique<StaticObject>(
            stoneX, stoneY, stoneZ, 0.3f, ObjectType::ROCK, stoneDiameter, stoneDiameter * 0.7f, stoneDiameter));
    }

    // Adiciona a fogueira no centro
    gameObjects.push_back(std::make_unique<StaticObject>(
        checkpointX, checkpointY, checkpointZ, 0.8f, ObjectType::BONFIRE, 1.0f, 1.0f, 1.0f));

    // Adiciona alguns troncos ao redor da fogueira
    for (int i = 0; i < 3; i++)
    {
        float angle = i * (2 * M_PI / 3);
        float logX = checkpointX + 3.0f * cos(angle);
        float logZ = checkpointZ + 3.0f * sin(angle);
        float logY = getTerrainHeight(logX, logZ);

        gameObjects.push_back(std::make_unique<StaticObject>(
            logX, logY, logZ, 0.5f, ObjectType::ROCK, 1.5f, 0.5f, 0.5f));
    }

    // Portal para voltar ao mapa principal
    float exitX = 0.0f, exitZ = -20.0f;
    float exitY = getTerrainHeight(exitX, exitZ);
    gameObjects.push_back(std::make_unique<Portal>(
        exitX, exitY, exitZ, 0.4f, 0.0f, 0.0f, MapType::MAIN));

    // Adiciona algumas pedras destacando o caminho para o portal
    for (int i = 1; i <= 5; i++)
    {
        float pathX = exitX + 2.0f;
        float pathZ = exitZ + i * 3.0f;
        float pathY = getTerrainHeight(pathX, pathZ);

        gameObjects.push_back(std::make_unique<StaticObject>(
            pathX, pathY, pathZ, 0.3f, ObjectType::ROCK, 0.6f, 0.4f, 0.6f));

        pathX = exitX - 2.0f;
        gameObjects.push_back(std::make_unique<StaticObject>(
            pathX, pathY, pathZ, 0.3f, ObjectType::ROCK, 0.6f, 0.4f, 0.6f));
    }

    // Posicionamento inicial do jogador próximo à fogueira
    float startX = checkpointX + 1.0f;
    float startZ = checkpointZ;
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
    for (int i = 0; i < 10; i++)
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
                float worldX = (DUNGEON_WIDTH / 2) * 5.0f;
                float worldZ = (DUNGEON_HEIGHT / 2) * 5.0f;
                // float y = getTerrainHeight(worldX, worldZ);
            }
        }
    }

    // Portal para a DUNGEON_ONE_LEVEL
    addPortalNearEdge(WORLD_SIZE, 1.5f, -15.0f, -15.0f, MapType::MAIN); // DUNGEON_ONE_LEVEL 1
    addPortalNearEdge(WORLD_SIZE, 1.5f, 15.0f, -15.0f, MapType::MAIN);  // DUNGEON_ONE_LEVEL 2
    addPortalNearEdge(WORLD_SIZE, 1.5f, -15.0f, 15.0f, MapType::MAIN);  // DUNGEON_ONE_LEVEL 3
    addPortalNearEdge(WORLD_SIZE, 1.5f, 0.0f, 0.0f, MapType::MAIN);     // Retorno ao centro
}

void Game::addPortalNearEdge(float worldSize, float margin, float destX, float destZ, MapType map)
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
    gameObjects.push_back(std::make_unique<Portal>(x, y + 0.2f, z, 0.4f, destX, destZ, map));
}

void Game::update()
{
    static float lastFrameTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    static bool gameOverSoundPlayed = false;

    if (player.getHealth() == 0)
    {
        gameMode = STATE_GAME::GAME_OVER;
        if (sound.isAudioPlaying(0))
            sound.stopAudioRepeter(0);
        if (sound.isAudioPlaying(9))
            sound.stopAudioRepeter(9);
        if (sound.isAudioPlaying(1))
            sound.stopAudioRepeter(1);
        if (sound.isAudioPlaying(6))
            sound.stopAudioRepeter(6);
        if (sound.isAudioPlaying(8))
            sound.stopAudioRepeter(8);
        if (sound.isAudioPlaying(15))
            sound.stopAudioRepeter(15);
        if (sound.isAudioPlaying(19))
            sound.stopAudioRepeter(19);
        if (sound.isAudioPlaying(18))
            sound.stopAudioRepeter(18);
        if (sound.isAudioPlaying(17))
            sound.stopAudioRepeter(17);
        sound.playAudioRepeter(20, volume.musica);
    }
    else
    {
        sound.stopAudioRepeter(20);
    }

    constrainPlayer();
    animateSwordAttack(currentTime, loader, getPlayer(), camera);

    if (currentMap == MapType::MAIN && gameMode != STATE_GAME::GAME_OVER)
    {
        if (sound.isAudioPlaying(19))
            sound.stopAudioRepeter(19);
        if (!sound.isAudioPlaying(6))
        {
            sound.playAudioRepeter(6, volume.ambient);
        }
        if (gameMode == STATE_GAME::COMBAT)
        {
            if (!sound.isAudioPlaying(1))
            {
            sound.playAudioRepeter(1, volume.musica);
            sound.stopAudioRepeter(0);
            sound.stopAudioRepeter(8);
            }
        }
        if (gameMode == STATE_GAME::PLAYING_EXPLORER)
        {
            if (!sound.isAudioPlaying(0))
            {
            sound.playAudioRepeter(0, volume.musica);
            sound.stopAudioRepeter(1);
            }
        }
    }
    if (currentMap == MapType::DUNGEON_ONE_LEVEL && gameMode != STATE_GAME::GAME_OVER)
    {
        if (!sound.isAudioPlaying(8))
        {
            sound.playAudioRepeter(8, volume.ambient);
            sound.stopAudioRepeter(0);
            sound.stopAudioRepeter(6);
        }
    }
    if (currentMap == MapType::DUNGEON_TWO_LEVEL && gameMode != STATE_GAME::GAME_OVER)
    {
        if (!sound.isAudioPlaying(9))
        {
            sound.playAudioRepeter(9, volume.musica);
            sound.stopAudioRepeter(0);
            sound.stopAudioRepeter(6);
        }
    }
    if (currentMap == MapType::PARASIDE && gameMode != STATE_GAME::GAME_OVER)
    {
        if (!sound.isAudioPlaying(19))
        {
            sound.playAudioRepeter(19, volume.musica);
            sound.stopAudioRepeter(0);
            sound.stopAudioRepeter(9);
        }
    }
    if (currentMap == MapType::BOSS && gameMode != STATE_GAME::GAME_OVER)
    {
        if (!sound.isAudioPlaying(15) && !BossDefeat && gameMode != STATE_GAME::GAME_OVER)
        {
            sound.playAudioRepeter(15, volume.musica);
            sound.stopAudioRepeter(0);
            sound.stopAudioRepeter(6);
            sound.stopAudioRepeter(9);
        }
        if (BossDefeat)
        {
            sound.stopAudioRepeter(15);
        }
    }

    // Lógica principal do jogo (combate e exploração)
    if (gameMode == STATE_GAME::COMBAT || gameMode == STATE_GAME::PLAYING_EXPLORER)
    {
        // Atualiza movimentação do jogador
        this->updateMoviment();
        player.update(deltaTime);

        bool isAnyEnemyActive = false;
        int quant_enemies = 0;

        // Atualiza todos os objetos do jogo e verifica inimigos
        for (auto &object : gameObjects)
        {
            object->update(deltaTime);
            const std::string typeName = typeid(*object).name();

            if (typeName == "2Boss")
                std::cout << "Object Type: " << typeName << std::endl;

            Enemy *enemy = dynamic_cast<Enemy *>(object.get());
            if (enemy)
            {
                if (enemy->isActive())
                {
                    quant_enemies++;
                    enemy->moveTowardsPlayer(player, deltaTime);

                    // Verifica ataque do inimigo ao jogador
                    if (enemy->attackPlayer(player, deltaTime))
                    {
                        std::cout << "Você foi atacado! Vida restante: " << player.getHealth() << std::endl;
                    }

                    if (enemy->isInCombat())
                    {
                        enemy->setDetectionRange(20.f);
                        isAnyEnemyActive = true;
                    }
                }
                else
                {
                    // Corrigido: Verifica se o XP ainda não foi dado
                    if (!enemy->isExperienceGiven())
                    {
                        int xp = static_cast<int>(enemy->getExperienceValue());
                        player.addExperience(xp);
                        enemy->markExperienceAsGiven(); // Marca que o XP já foi concedido
                    }
                }
            }
            if (currentMap == MapType::BOSS)
            {
                Boss *boss = dynamic_cast<Boss *>(object.get());
                if (boss)
                {
                    if (boss->isActive())
                    {
                        boss->moveTowardsPlayer(player, deltaTime, loader);

                        // Verifica ataque do inimigo ao jogador
                        if (boss->attackPlayer(player, deltaTime))
                        {
                            std::cout << "Você foi atacado! Vida restante: " << player.getHealth() << std::endl;
                        }

                        if (boss->isInCombat())
                        {
                        }
                    }
                    else
                    {
                        // Corrigido: Verifica se o XP ainda não foi dado
                        if (!boss->isExperienceGiven())
                        {
                            BossDefeat = true;
                            int xp = static_cast<int>(boss->getExperienceValue() * 10.f);
                            player.addExperience(xp);
                            sound.playAudio(18, volume.efeitos);
                            boss->markExperienceAsGiven(); // Marca que o XP já foi concedido
                        }
                    }
                }
            }
        }
        inimigos = quant_enemies;

        // Atualiza o modo de jogo com base nos inimigos ativos
        if (isAnyEnemyActive)
        {
            sound.stopAudioRepeter(0);
            gameMode = STATE_GAME::COMBAT;

            if (currentMap != MapType::DUNGEON_THREE_LEVEL && currentMap != MapType::BOSS)
            {
                sound.playAudioRepeter(1, volume.musica);
            }
            else
            {
                sound.playAudioRepeter(9, volume.musica);
            }
        }
        else
        {
            gameMode = STATE_GAME::PLAYING_EXPLORER;

            if (currentMap != MapType::DUNGEON_THREE_LEVEL && currentMap != MapType::BOSS)
            {
                sound.stopAudioRepeter(1);
            }
            else
            {
                sound.playAudioRepeter(9, volume.musica);
            }

            if (currentMap == MapType::MAIN)
            {
                sound.playAudioRepeter(0, volume.musica);
            }
        }
        // Verificação de colisões
        checkCollisions();

        // Controle do estado de ataque do jogador
        if ((player.getAttackTimer() / player.getAttackCooldown()) > 0)
        {
            isAttacking = true;
            attackProgress = 0.0f;
        }
        else
        {
            isAttacking = false;
            attackProgress = 0.0f;
        }

        // Ajuste suave da altura do jogador baseado no terreno
        float targetY = getTerrainHeight(player.getX(), player.getZ()) + 0.3f;
        float currentY = player.getY();
        float smoothY = lerp(currentY, targetY, 0.1f);
        player.setPosition(player.getX(), smoothY, player.getZ());

        // Lógica para portais
        showPortalMessage = false;

        for (auto &obj : gameObjects)
        {
            Portal *portal = dynamic_cast<Portal *>(obj.get());
            if (portal && portal->playerIsNearby(player))
            {
                if (!isAnyEnemyActive && quant_enemies == 0)
                {
                    showPortalMessage = true;

                    if (GetAsyncKeyState(VK_RETURN) & 0x8000)
                    {
                        portal->teleport(player, *this);
                        break;
                    }
                }
                else
                {
                    showPortalMessage = false;
                    if (!sound.isAudioPlaying(3))
                    {
                        sound.playAudio(3, volume.efeitos);
                    }
                }
            }
        }

        // Verifica se todos os inimigos foram derrotados
        if (quant_enemies == 0 && !isAnyEnemyActive && !soundNoEnemie)
        {
            sound.playAudio(13, volume.efeitos);
            soundNoEnemie = true; // Marca que o som foi tocado
        }
        else if (quant_enemies > 0)
        {
            soundNoEnemie = false; // Reseta para tocar novamente se novos inimigos aparecerem
        }
    }
}
void Game::animateSwordAttack(float deltaTime, MeshLoader &loader, const Player &player, const Camera &camera)
{
    float startX = player.getX();
    float startY = player.getY() + 1.0f;
    float startZ = player.getZ() - 0.4f;
    if (isAttacking)
    {
        float attackDuration = 1.0f;
        attackProgress += deltaTime / attackDuration;

        if (attackProgress > 1.0f)
        {
            attackProgress = 0.0f;
            isAttacking = false;
        }

        loader.updateModelRotationAngleById(0, 95.0f);
        loader.updateModelRotationYById(0, 0.1f);
        loader.updateModelRotationYById(0, 0.2f);
        loader.updateModelTranslationXById(0, startX + 0.2f);
        loader.updateModelTranslationYById(0, startY);
        loader.updateModelTranslationZById(0, startZ + 0.8f);

        loader.updateModelRotationXById(0, std::sin(deltaTime * 4));
        loader.updateModelTranslationXById(0, startX - (0.5f * (std::sin(deltaTime * 4.0f) - 0.5f)));
        loader.updateModelTranslationYById(0, startY - 0.3f);
        loader.updateModelTranslationZById(0, startZ + 0.2f + 0.3f * (std::sin(deltaTime) - 0.5f));
    }
    else

    {
        float angleRad = player.getRotY() * M_PI / 180.0f;
        float camX = player.getX() - std::sin(angleRad + camera.getRotationY()) * 1.0f;
        float camZ = player.getZ() - std::cos(angleRad + camera.getRotationY()) * 1.0f;

        loader.updateModelRotationAngleById(0, 270.0f);
        loader.updateModelTranslationXById(0, camX);
        loader.updateModelTranslationYById(0, player.getY());
        loader.updateModelTranslationZById(0, camZ);
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
    camera.applyView(player);
    // gui.drawGround(currentMap, WORLD_SIZE, trailCurvePoints, trailClearings);
    drawGround();
    drawLakes();

    // Desenhar objetos
    for (auto &object : gameObjects)
    {
        if (object->isActive())
        {
            if (auto boss = dynamic_cast<Boss *>(object.get()))
            {
                // if (currentMap == MapType::BOSS)
                // {
                boss->drawForLoader(loader);
                // }
            }
            else
            {
                object->draw(); // Se não for um Boss, apenas desenha
            }
        }
    }

    loader.drawForId(0);

    loader.updateModelTranslationXById(0, player.getX());
    loader.updateModelTranslationYById(0, player.getY());
    loader.updateModelTranslationZById(0, player.getZ() - 0.4f);

    player.draw();
    // Desenhar HUD
    hud.drawHUD(player, gameMode, showPortalMessage, isOpenHouse);

    if (this->getGameMode() == STATE_GAME::SKILL_TREE)
    {
        hud.drawSkillTree(skillNodes, skillTooltip);
    }

    if (this->getGameMode() == STATE_GAME::MENU || this->getGameMode() == STATE_GAME::GAME_OVER)
    {
        hud.drawMainHUD(player, gameMode, button_action, volume);
    }
}

void Game::drawGround()
{
    const float step = 1.0f;
    const float size = WORLD_SIZE;
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat shininess;

    if (currentMap == MapType::DUNGEON_ONE_LEVEL)
    {
        ambient[0] = 0.2f;
        ambient[1] = 0.15f;
        ambient[2] = 0.15f;
        ambient[3] = 1.0f;
        diffuse[0] = 0.4f;
        diffuse[1] = 0.2f;
        diffuse[2] = 0.2f;
        diffuse[3] = 1.0f;
        specular[0] = 0.15f;
        specular[1] = 0.1f;
        specular[2] = 0.1f;
        specular[3] = 1.0f;
        shininess = 12.0f;
    }

    if (currentMap == MapType::DUNGEON_TWO_LEVEL)
    {
        ambient[0] = 0.15f;
        ambient[1] = 0.2f;
        ambient[2] = 0.3f;
        ambient[3] = 1.0f;
        diffuse[0] = 0.25f;
        diffuse[1] = 0.3f;
        diffuse[2] = 0.4f;
        diffuse[3] = 1.0f;
        specular[0] = 0.1f;
        specular[1] = 0.15f;
        specular[2] = 0.2f;
        specular[3] = 1.0f;
        shininess = 15.0f;
    }

    if (currentMap == MapType::DUNGEON_THREE_LEVEL)
    {
        ambient[0] = 0.3f;
        ambient[1] = 0.1f;
        ambient[2] = 0.2f;
        ambient[3] = 1.0f;
        diffuse[0] = 0.35f;
        diffuse[1] = 0.15f;
        diffuse[2] = 0.25f;
        diffuse[3] = 1.0f;
        specular[0] = 0.2f;
        specular[1] = 0.05f;
        specular[2] = 0.1f;
        specular[3] = 1.0f;
        shininess = 18.0f;
    }

    if (currentMap == MapType::BOSS)
    {
        ambient[0] = 0.05f;
        ambient[1] = 0.05f;
        ambient[2] = 0.05f;
        ambient[3] = 1.0f;
        diffuse[0] = 0.5f;
        diffuse[1] = 0.05f;
        diffuse[2] = 0.05f;
        diffuse[3] = 1.0f;
        specular[0] = 0.3f;
        specular[1] = 0.3f;
        specular[2] = 0.3f;
        specular[3] = 1.0f;
        shininess = 20.0f;
    }

    if (currentMap == MapType::BOSS)
    {
        ambient[0] = 0.2f;
        ambient[1] = 0.2f;
        ambient[2] = 0.2f;
        ambient[3] = 1.0f;
        diffuse[0] = 0.7f;
        diffuse[1] = 0.2f;
        diffuse[2] = 0.2f;
        diffuse[3] = 1.0f;
        specular[0] = 0.4f;
        specular[1] = 0.4f;
        specular[2] = 0.4f;
        specular[3] = 1.0f;
        shininess = 25.0f;
    }

    if (currentMap == MapType::PARASIDE)
    {
        ambient[0] = 0.4f;
        ambient[1] = 0.4f;
        ambient[2] = 0.4f;
        ambient[3] = 1.0f;
        diffuse[0] = 0.6f;
        diffuse[1] = 0.8f;
        diffuse[2] = 0.6f;
        diffuse[3] = 1.0f;
        specular[0] = 0.3f;
        specular[1] = 0.3f;
        specular[2] = 0.3f;
        specular[3] = 1.0f;
        shininess = 10.0f;
    }

    if (currentMap == MapType::MAIN)
    {
        ambient[0] = 0.1f;
        ambient[1] = 0.6f;
        ambient[2] = 0.35f;
        ambient[3] = 1.0f;
        diffuse[0] = 0.1f;
        diffuse[1] = 0.3f;
        diffuse[2] = 0.1f;
        diffuse[3] = 1.0f;
        specular[0] = 0.05f;
        specular[1] = 0.05f;
        specular[2] = 0.05f;
        specular[3] = 1.0f;
        shininess = 5.0f;
    }

    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

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

void Game::calculateSkillTreeLayout()
{
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    const auto &skills = player.getSkillTree().getSkills();

    skillNodes.clear();

    // Definir layout básico com nós de habilidades básicas no centro
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    float baseRadius = 30.0f;
    float nodeSpacing = 150.0f;

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
void Game::checkCollisions()
{
    static float lastHousePosX = -1.0f;
    static float lastHousePosZ = -1.0f;

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
                sound.playAudio(4, volume.efeitos);
                player.addExperience(10000);
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
    key = std::tolower(key);
    keyStates[key] = true;
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
    if (gameMode == STATE_GAME::SKILL_TREE)
    {
        if (key == 'k')
        {
            sound.playAudio(5, volume.UI);
            gameMode = STATE_GAME::PLAYING_EXPLORER;
            return;
        }
        if (key >= '1' && key <= '9')
        {
            int skillIndex = key - '1';
            const auto &skills = player.getSkillTree().getSkills();
            sound.playAudio(5, volume.UI);

            if (skillIndex < static_cast<int>(skills.size()))
            {

                std::string skillName = skills[skillIndex]->getName();
                if (player.getSkillTree().useSkillPoint(skillName))
                {
                    sound.playAudio(10, volume.UI);
                }
                else
                    std::cout << "Não foi possível melhorar a habilidade." << std::endl;
            }
        }

        return;
    }

    if (this->getGameMode() == STATE_GAME::MENU)
    {
        if (key == 9)
        {
            sound.playAudio(5, volume.UI);
            gameMode = STATE_GAME::PLAYING_EXPLORER;
            return;
        }
        if (key == 'k')
        {
            sound.playAudio(5, volume.UI);
            gameMode = STATE_GAME::SKILL_TREE;
            return;
        }

        return;
    }

    switch (key)
    {
    case 't':
        sound.playAudio(5, volume.UI);
        camera.toggleTopDown(gameMode);
        break;
    case 'p':
        shareScreenshot(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        sound.playAudio(16, volume.UI);
        break;
    case ' ':
        if (player.attack())
        {
            static bool isAttacking = false;
            static float attackProgress = 0.1f;
            const float attackSpeed = 0.2f;
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
                        isAttacking = true;
                        attackProgress = 0.0f;
                        enemy->takeDamage(player.getAttackDamage() * 0.8f, AttackType::PHYSICAL);
                        sound.playAudio(2, volume.efeitos);
                        break;
                    }
                }
                Boss *cat = dynamic_cast<Boss *>(object.get());
                if (cat && cat->isActive())
                {
                    float dx = cat->getX() - player.getX();
                    float dz = cat->getZ() - player.getZ();
                    float dist = std::sqrt(dx * dx + dz * dz);
                    if (dist < 2.0f)
                    {
                        isAttacking = true;
                        attackProgress = 0.0f;
                        cat->takeDamage(player.getAttackDamage() * 0.7f, AttackType::PHYSICAL);
                        sound.playAudio(2, volume.efeitos);
                        break;
                    }
                }
            }
        }

    case 'x':
            if (player.attack())
            {
                for (auto &object : gameObjects)
                {
                    Enemy *enemy = dynamic_cast<Enemy *>(object.get());
                    if (enemy && enemy->isActive())
                    {
                        float dx = enemy->getX() - player.getX();
                        float dz = enemy->getZ() - player.getZ();
                        float dist = std::sqrt(dx * dx + dz * dz);
                        if (dist < 4.0f)
                        {
                            sound.playAudio(14, volume.efeitos);
                            float damage = player.getAttackDamage() * 0.5f;
                            enemy->takeDamage(damage, AttackType::FIRE);
                        }
                    }
                }
            }
        if (currentMap == MapType::BOSS)
        {
            if (player.attack())
            {
                for (auto &object : gameObjects)
                {
                    Boss *boss = dynamic_cast<Boss *>(object.get());
                    if (boss && boss->isActive())
                    {
                        float dx = boss->getX() - player.getX();
                        float dz = boss->getZ() - player.getZ();
                        float dist = std::sqrt(dx * dx + dz * dz);
                        if (dist < 3.0f)
                        {
                            isAttacking = true;
                            attackProgress = 0.0f;
                            boss->takeDamage(player.getAttackDamage() * 0.4f, AttackType::PHYSICAL);
                            sound.playAudio(2, volume.efeitos);
                            break;
                        }
                    }
                }
            }
        }
        break;

    case 'k':
        sound.playAudio(5, volume.UI);
        gameMode = (gameMode == STATE_GAME::SKILL_TREE)
                       ? STATE_GAME::PLAYING_EXPLORER
                       : STATE_GAME::SKILL_TREE;
        break;

    case 9: // Tecla TAB
        sound.playAudio(5, volume.UI);
        gameMode = (gameMode == STATE_GAME::MENU) ? STATE_GAME::PLAYING_EXPLORER : STATE_GAME::MENU;
        break;

    case 27: // Tecla ESC
        exit(0);
        break;

    default:
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
    case 112:
        player.toggleRunning();
        break;
    }
}

void Game::handleJoystick(unsigned int btn, int x, int y, int z)
{
    static unsigned int previousButtonMask = 0;
    unsigned int buttonMask = btn;
    static int currentNodeIndex = 0;
    static int hoverIndexButton = 0;

    hud.setHoveredButton(hoverIndexButton);

    if ((x != 0 || y != 0) && (gameMode == STATE_GAME::COMBAT || gameMode == STATE_GAME::PLAYING_EXPLORER))
    {
        sound.playAudioRepeter(7, volume.ambient);
    }
    else
    {
        sound.stopAudioRepeter(7);
    }

    float normalizedX = static_cast<float>(x) / 32768.0f;
    float normalizedY = static_cast<float>(y) / 32768.0f;
    float normalizedZ = static_cast<float>(z) / 32768.0f;
    float moveSpeed = 3.0f;
    float deltaX = normalizedX * moveSpeed;
    float deltaZ = -normalizedY * moveSpeed;

    if (gameMode == STATE_GAME::TOP_VIEW_MAP)
    {
        float panSpeed = 0.2f;
        camera.updatePanWithMouse(deltaX * panSpeed * 500, -deltaZ * panSpeed * 500);
    }
    if (gameMode == STATE_GAME::PLAYING_EXPLORER || gameMode == STATE_GAME::COMBAT)
    {
        float camAngleRad = camera.getRotationY();
        float forwardX = sin(camAngleRad);
        float forwardZ = cos(camAngleRad);
        float rightX = -cos(camAngleRad);
        float rightZ = sin(camAngleRad);

        // Usando a velocidade do jogador
        float speed = player.getMovementSpeed() * 10.0f;
        float moveX = (deltaX * rightX + deltaZ * forwardX) * speed;
        float moveZ = (deltaX * rightZ + deltaZ * forwardZ) * speed;

        player.setPosition(player.getX() + moveX, player.getY(), player.getZ() + moveZ);
        camera.setPosX(player.getX());
        camera.setPosZ(player.getZ());
        camera.setRotationY(camera.getRotationY() + (-normalizedZ));
    }
    if (!skillNodes.empty() && gameMode == STATE_GAME::SKILL_TREE)
    {
        const auto &node = skillNodes[currentNodeIndex];

        skillTooltip.visible = true;
        skillTooltip.showConfirmation = true;
        skillTooltip.skillIndex = node.skillIndex;
        skillTooltip.x = node.x + 20;
        skillTooltip.y = node.y - skillTooltip.height / 2;

        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

        if (skillTooltip.x + skillTooltip.width > windowWidth)
            skillTooltip.x = windowWidth - skillTooltip.width - 10;

        if (skillTooltip.y < 10)
            skillTooltip.y = 10;
        else if (skillTooltip.y + skillTooltip.height > windowHeight)
            skillTooltip.y = windowHeight - skillTooltip.height - 10;
    }

    if ((buttonMask & JOYSTICK_DOWN) && !(previousButtonMask & JOYSTICK_DOWN))
    {
        std::cout << "Botão DOWN pressionado." << std::endl;
    }
    else if (!(buttonMask & JOYSTICK_DOWN) && (previousButtonMask & JOYSTICK_DOWN))
    {
        std::cout << "Botão DOWN liberado." << std::endl;
    }

    if ((buttonMask & JOYSTICK_X) && !(previousButtonMask & JOYSTICK_X))
    {
        if ((gameMode == STATE_GAME::COMBAT || gameMode == STATE_GAME::PLAYING_EXPLORER))
        {
            if (player.attack())
            {
                static bool isAttacking = false;
                static float attackProgress = 0.1f;
                const float attackSpeed = 0.2f;
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
                            isAttacking = true;
                            attackProgress = 0.0f;
                            enemy->takeDamage(player.getAttackDamage(), AttackType::PHYSICAL);
                            sound.playAudio(2, volume.efeitos);
                            break;
                        }
                    }
                    Boss *boss = dynamic_cast<Boss *>(object.get());
                    if (boss && boss->isActive())
                    {
                        float dx = boss->getX() - player.getX();
                        float dz = boss->getZ() - player.getZ();
                        float dist = std::sqrt(dx * dx + dz * dz);
                        if (dist < 2.0f)
                        {
                            isAttacking = true;
                            attackProgress = 0.0f;
                            boss->takeDamage(player.getAttackDamage() * 0.9f, AttackType::PHYSICAL);
                            sound.playAudio(2, volume.efeitos);
                            break;
                        }
                    }
                }
            }
        }
        if (gameMode == STATE_GAME::SKILL_TREE)
        {
            if (skillTooltip.visible && skillTooltip.showConfirmation)
            {
                sound.playAudio(5, volume.UI);
                const auto &skills = player.getSkillTree().getSkills();
                std::string skillName = skills[skillTooltip.skillIndex]->getName();
                if (player.getSkillTree().useSkillPoint(skillName))
                {
                    sound.playAudio(10, volume.UI);
                }
                skillTooltip.visible = false;
            }
        }
        if (gameMode == STATE_GAME::MENU || gameMode == STATE_GAME::GAME_OVER)
        {
            sound.playAudio(5, volume.UI);
            if (hoverIndexButton >= 0 && hoverIndexButton < static_cast<int>(hud.getButtonMenu().size()))
            {
                button_action = hud.getButtonMenu()[hoverIndexButton].destino;
                printf("button_action: %d\n", button_action);

                switch (button_action)
                {
                case ACTION_BUTTON::EXIT:
                    exit(0);
                    break;

                case ACTION_BUTTON::VOLUME_AMBIENT_DECREASE:
                    volume.ambient = std::max(0.0f, volume.ambient - 0.05f);
                    break;

                case ACTION_BUTTON::VOLUME_AMBIENT_INCREASE:
                    volume.ambient = std::min(1.0f, volume.ambient + 0.05f);
                    break;

                case ACTION_BUTTON::VOLUME_EFFECTS_DECREASE:
                    volume.efeitos = std::max(0.0f, volume.efeitos - 0.05f);
                    break;

                case ACTION_BUTTON::VOLUME_EFFECTS_INCREASE:
                    volume.efeitos = std::min(1.0f, volume.efeitos + 0.05f);
                    break;

                case ACTION_BUTTON::VOLUME_MUSIC_DECREASE:
                    volume.musica = std::max(0.0f, volume.musica - 0.05f);
                    break;

                case ACTION_BUTTON::VOLUME_MUSIC_INCREASE:
                    volume.musica = std::min(1.0f, volume.musica + 0.05f);
                    break;

                case ACTION_BUTTON::VOLUME_UI_DECREASE:
                    volume.UI = std::max(0.0f, volume.UI - 0.05f);
                    break;

                case ACTION_BUTTON::VOLUME_UI_INCREASE:
                    volume.UI = std::min(1.0f, volume.UI + 0.05f);
                    break;
                case ACTION_BUTTON::RESET_ALL:
                    gameObjects.clear();
                    player.reset();
                    player.setPosition(0.0f, getTerrainHeight(0.0f, 0.0f) + 0.3f, 0.0f);
                    initObjects();
                    gameMode = STATE_GAME::PLAYING_EXPLORER;
                    currentMap = MapType::MAIN;
                    button_action = ACTION_BUTTON::NONE;
                    break;

                default:
                    break;
                }

                // Update sound volumes
                sound.setVolume(0, volume.musica);
                sound.setVolume(1, volume.musica);
                sound.setVolume(2, volume.efeitos);
                sound.setVolume(3, volume.efeitos);
                sound.setVolume(4, volume.efeitos);
                sound.setVolume(5, volume.UI);
                sound.setVolume(6, volume.ambient);
                sound.setVolume(7, volume.ambient);
                sound.setVolume(8, volume.ambient);
                sound.setVolume(9, volume.ambient);
                sound.setVolume(10, volume.musica);
                sound.setVolume(11, volume.musica);
                sound.setVolume(12, volume.UI);
                sound.setVolume(13, volume.UI);
                sound.setVolume(14, volume.efeitos);
                sound.setVolume(15, volume.musica);
                sound.setVolume(16, volume.UI);
                sound.setVolume(17, volume.efeitos);
                sound.setVolume(18, volume.efeitos);
                sound.setVolume(19, volume.musica);
                sound.setVolume(20, volume.musica);
            }
        }
    }
    else if (!(buttonMask & JOYSTICK_X) && (previousButtonMask & JOYSTICK_X))
    {
        std::cout << "Botão X liberado." << std::endl;
    }

    if ((buttonMask & JOYSTICK_CIRCLE) && !(previousButtonMask & JOYSTICK_CIRCLE))
    {
        for (auto &obj : gameObjects)
        {
            Portal *portal = dynamic_cast<Portal *>(obj.get());
            if (portal && portal->playerIsNearby(player))
            {
                if (!isAnyEnemyActive && inimigos == 0)
                {
                    showPortalMessage = true;
                    portal->teleport(player, *this);
                    break;
                }
                else
                {
                    showPortalMessage = false;
                    if (!sound.isAudioPlaying(3))
                    {
                        sound.playAudio(3, volume.efeitos);
                    }
                }
            }
        }
    }
    else if (!(buttonMask & JOYSTICK_CIRCLE) && (previousButtonMask & JOYSTICK_CIRCLE))
    {
        std::cout << "Botão CIRCLE liberado." << std::endl;
    }

    if ((buttonMask & JOYSTICK_TRIANGLE) && !(previousButtonMask & JOYSTICK_TRIANGLE) && gameMode != STATE_GAME::TOP_VIEW_MAP)
    {
        if (gameMode == STATE_GAME::SKILL_TREE)
        {
            sound.playAudio(5, volume.UI);
            gameMode = STATE_GAME::PLAYING_EXPLORER;
        }
        else
        {
            sound.playAudio(5, volume.UI);
            gameMode = STATE_GAME::SKILL_TREE;
        }
    }
    else if (!(buttonMask & JOYSTICK_TRIANGLE) && (previousButtonMask & JOYSTICK_TRIANGLE))
    {
        std::cout << "Botão TRIANGLE liberado." << std::endl;
    }

    if ((buttonMask & JOYSTICK_QUAD) && !(previousButtonMask & JOYSTICK_QUAD))
    {
        if (currentMap == MapType::BOSS || gameMode == STATE_GAME::COMBAT)
        {
            if (player.attack())
            {
                for (auto &object : gameObjects)
                {
                    Boss *boss = dynamic_cast<Boss *>(object.get());
                    if (boss && boss->isActive())
                    {
                        float dx = boss->getX() - player.getX();
                        float dz = boss->getZ() - player.getZ();
                        float dist = std::sqrt(dx * dx + dz * dz);
                        if (dist < 4.0f)
                        {
                            isAttacking = true;
                            attackProgress = 0.0f;
                            boss->takeDamage(player.getAttackDamage() * 0.4f, AttackType::FIRE);
                            sound.playAudio(14, volume.efeitos);
                        }
                    }
                    Enemy *enemy = dynamic_cast<Enemy *>(object.get());
                    if (enemy && enemy->isActive())
                    {
                        float dx = enemy->getX() - player.getX();
                        float dz = enemy->getZ() - player.getZ();
                        float dist = std::sqrt(dx * dx + dz * dz);
                        if (dist < 4.0f)
                        {
                            isAttacking = true;
                            attackProgress = 0.0f;
                            enemy->takeDamage(player.getAttackDamage() * 0.6f, AttackType::FIRE);
                            sound.playAudio(14, volume.efeitos);
                        }
                    }
                }
            }
        }
    }
    else if (!(buttonMask & JOYSTICK_QUAD) && (previousButtonMask & JOYSTICK_QUAD))
    {
        std::cout << "Botão QUAD liberado." << std::endl;
    }

    if ((buttonMask & JOYSTICK_L1) && !(previousButtonMask & JOYSTICK_L1))
    {
        hoverIndexButton = (hoverIndexButton > 0) ? hoverIndexButton - 1 : hud.getButtonMenu().size() - 1;
    }
    else if (!(buttonMask & JOYSTICK_L1) && (previousButtonMask & JOYSTICK_L1))
    {
        std::cout << "Botão L1 liberado." << std::endl;
    }

    if ((buttonMask & JOYSTICK_R1) && !(previousButtonMask & JOYSTICK_R1))
    {
        hoverIndexButton = (hoverIndexButton < hud.getButtonMenu().size() - 1) ? hoverIndexButton + 1 : 0;
    }
    else if (!(buttonMask & JOYSTICK_R1) && (previousButtonMask & JOYSTICK_R1))
    {
        std::cout << "Botão R1 liberado." << std::endl;
    }

    if ((buttonMask & JOYSTICK_L3) && !(previousButtonMask & JOYSTICK_L3))
    {
        player.setSpeed((player.getMovementSpeed() < 0.1f) ? 0.13f : 0.08f);
        std::cout << player.getMovementSpeed() << std::endl;
    }
    else if (!(buttonMask & JOYSTICK_L3) && (previousButtonMask & JOYSTICK_L3))
    {
    }

    if ((buttonMask & JOYSTICK_R3) && !(previousButtonMask & JOYSTICK_R3))
    {
        std::cout << "Botão R3 pressionado." << std::endl;
    }
    else if (!(buttonMask & JOYSTICK_R3) && (previousButtonMask & JOYSTICK_R3))
    {
        std::cout << "Botão R3 liberado." << std::endl;
    }

    if ((buttonMask & JOYSTICK_START) && !(previousButtonMask & JOYSTICK_START))
    {
        int width = glutGet(GLUT_WINDOW_WIDTH);
        int height = glutGet(GLUT_WINDOW_HEIGHT);
        shareScreenshot(width, height);
        sound.playAudio(16, volume.UI);
    }
    else if (!(buttonMask & JOYSTICK_START) && (previousButtonMask & JOYSTICK_START))
    {
        std::cout << "Botão START liberado." << std::endl;
    }

    if ((buttonMask & JOYSTICK_R2))
    {
        if (gameMode == STATE_GAME::TOP_VIEW_MAP)
        {
            camera.updateTopDownZoom(+1);
        }
        if (gameMode == STATE_GAME::COMBAT || gameMode == STATE_GAME::PLAYING_EXPLORER)
        {
            camera.updateZoom(+1);
        }
        if (gameMode == STATE_GAME::SKILL_TREE && !(previousButtonMask & JOYSTICK_R2))
        {
            sound.playAudio(5, volume.UI);
            currentNodeIndex = (currentNodeIndex - 1 + skillNodes.size()) % skillNodes.size();
        }
    }

    if ((buttonMask & JOYSTICK_OPT) && !(previousButtonMask & JOYSTICK_OPT) && gameMode != STATE_GAME::TOP_VIEW_MAP)
    {
        sound.playAudio(5, volume.UI);
        gameMode = (gameMode == STATE_GAME::MENU) ? STATE_GAME::PLAYING_EXPLORER : STATE_GAME::MENU;
    }
    else if (!(buttonMask & JOYSTICK_OPT) && (previousButtonMask & JOYSTICK_OPT))
    {
        std::cout << "Botão OPT liberado." << std::endl;
    }

    if ((buttonMask & JOYSTICK_L2))
    {
        if (gameMode == STATE_GAME::TOP_VIEW_MAP)
        {
            camera.updateTopDownZoom(-1);
        }
        if (gameMode == STATE_GAME::PLAYING_EXPLORER || gameMode == STATE_GAME::COMBAT)
        {
            camera.updateZoom(-1);
        }
        if (gameMode == STATE_GAME::SKILL_TREE && !(previousButtonMask & JOYSTICK_L2))
        {
            sound.playAudio(5, volume.UI);
            currentNodeIndex = (currentNodeIndex + 1) % skillNodes.size();
        }
    }

    if ((buttonMask & JOYSTICK_ICON_BUTTON) && !(previousButtonMask & JOYSTICK_ICON_BUTTON))
    {
        std::cout << "Botão ICON_BUTTON pressionado." << std::endl;
    }
    else if (!(buttonMask & JOYSTICK_ICON_BUTTON) && (previousButtonMask & JOYSTICK_ICON_BUTTON))
    {
        exit(0);
    }

    if ((buttonMask & JOYSTICK_TOUCH) && !(previousButtonMask & JOYSTICK_TOUCH))
    {
        sound.playAudio(5, volume.UI);
        camera.toggleTopDown(gameMode);
    }
    else if (!(buttonMask & JOYSTICK_TOUCH) && (previousButtonMask & JOYSTICK_TOUCH))
    {
        std::cout << "Botão TOUCH liberado." << std::endl;
    }

    // Atualiza o estado anterior dos botões
    previousButtonMask = buttonMask;
}
// Adicionar estes métodos à classe Game
void Game::handleMouseClick(int button, int state, int x, int y)
{
    lastMouseX = x;
    lastMouseY = y;
    if (button == GLUT_LEFT_BUTTON)
    {
        mouseLeftDown = (state == GLUT_DOWN);
        if (mouseLeftDown && gameMode == STATE_GAME::SKILL_TREE)
            checkSkillTreeClick(x, y);
        if (this->getGameMode() == STATE_GAME::MENU ||
            this->getGameMode() == STATE_GAME::GAME_OVER)
            handleButtonMenuClick(x, y);
    }
    else if (button == GLUT_RIGHT_BUTTON)
    {
        mouseRightDown = (state == GLUT_DOWN);
    }
    if (button == 3)
    {
        if (gameMode == STATE_GAME::TOP_VIEW_MAP)
            camera.updateTopDownZoom(+1);
        else
            camera.updateZoom(+1);
    }
    else if (button == 4)
    {
        if (gameMode == STATE_GAME::TOP_VIEW_MAP)
            camera.updateTopDownZoom(-1);
        else
            camera.updateZoom(-1);
    }
}

void Game::updateMoviment()
{
    isMoving = false;

    if (keyStates['w'])
    {
        player.moveForward();
        isMoving = true;
    }
    if (keyStates['s'])
    {
        player.moveBackward();
        isMoving = true;
    }
    if (keyStates['a'])
    {
        player.strafeRight();
        isMoving = true;
    }
    if (keyStates['d'])
    {
        player.strafeLeft();
        isMoving = true;
    }
    if (keyStates['q'])
    {
        player.rotateLeft();
    }
    if (keyStates['e'])
    {
        player.rotateRight();
    }
    if (isMoving && !isSoundPlaying)
    {
        sound.playAudioRepeter(7, volume.ambient);
        isSoundPlaying = true;
    }
    else if (!isMoving && isSoundPlaying)
    {
        sound.stopAudioRepeter(7); // Pare o som quando não estiver se movendo
        isSoundPlaying = false;
    }
}

void Game::handleMouseMotion(int x, int y)
{
    int deltaX = x - lastMouseX;
    int deltaY = y - lastMouseY;
    int directionX = (deltaX > 0) ? 1 : (deltaX < 0) ? -1
                                                     : 0;
    int directionY = (deltaY > 0) ? 1 : (deltaY < 0) ? -1
                                                     : 0;
    float zoomSpeed = 0.05f;

    if (mouseRightDown && (gameMode == STATE_GAME::COMBAT || gameMode == STATE_GAME::PLAYING_EXPLORER))
    {
        player.rotateRight(-deltaX * mouseSensitivity);
        camera.adjustHeight(-deltaY);
    }
    if (mouseRightDown && gameMode == STATE_GAME::TOP_VIEW_MAP)
    {
        const int width = glutGet(GLUT_WINDOW_WIDTH);
        const int height = glutGet(GLUT_WINDOW_HEIGHT);
        const int panX = -deltaX / 2;
        const int panY = -deltaY / 2;
        camera.updatePanWithMouse(panX, panY);
    }

    if (gameMode == STATE_GAME::SKILL_TREE)
        updateSkillNodeHover(x, y);
    if (this->getGameMode() == STATE_GAME::MENU ||
        this->getGameMode() == STATE_GAME::GAME_OVER)
        updateButtonMenuHover(x, y);

    lastMouseX = x;
    lastMouseY = y;
}

void Game::handlePassiveMouseMotion(int x, int y)
{
    lastMouseX = x;
    lastMouseY = y;

    if (gameMode == STATE_GAME::SKILL_TREE)
        updateSkillNodeHover(x, y);
    if (this->getGameMode() == STATE_GAME::MENU)
        updateButtonMenuHover(x, y);
}

void Game::checkSkillTreeClick(int x, int y)
{
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    y = windowHeight - y; // Inverter coordenada Y para corresponder ao nosso sistema

    // Verificar se clicou no botão de confirmação, se a tooltip estiver visível
    if (skillTooltip.visible && skillTooltip.showConfirmation)
    {
        sound.playAudio(5, volume.UI);
        // Verificar se clicou no botão "Sim"
        if (x >= skillTooltip.x + 30 &&
            x <= skillTooltip.x + 100 &&
            y >= skillTooltip.y + 20 &&
            y <= skillTooltip.y + 45)
        {
            sound.playAudio(5, volume.UI);
            const auto &skills = player.getSkillTree().getSkills();
            std::string skillName = skills[skillTooltip.skillIndex]->getName();
            if (player.getSkillTree().useSkillPoint(skillName))
            {
                sound.playAudio(10, volume.efeitos);
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

    for (const auto &node : skillNodes)
    {
        float dx = x - node.x;
        float dy = y - node.y;
        float distSq = dx * dx + dy * dy;

        if (distSq < node.radius * node.radius)
        {
            skillTooltip.visible = true;
            skillTooltip.skillIndex = node.skillIndex;
            skillTooltip.x = x + 20; // Posicionar a tooltip ao lado do mouse
            skillTooltip.y = y - skillTooltip.height / 2;
            skillTooltip.showConfirmation = true;
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
    skillTooltip.visible = false;
}

void Game::handleKeyUp(unsigned char key, int x, int y)
{
    key = std::tolower(key);
    keyStates[key] = false;
}

void Game::handleButtonMenuClick(int x, int y)
{
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    float mouseYGL = windowHeight - y;

    for (auto &botao : hud.getButtonMenu())
    {
        bool dentroX = x >= botao.x && x <= (botao.x + botao.width);
        bool dentroY = mouseYGL >= botao.y && mouseYGL <= (botao.y + botao.height);

        if (dentroX && dentroY)
        {
            sound.playAudio(5, volume.UI);
            button_action = botao.destino;
            float volumeChange = 0.05f;
            bool volumeChanged = false;

            switch (button_action)
            {
            case ACTION_BUTTON::EXIT:
                exit(0);
                break;

            case ACTION_BUTTON::VOLUME_AMBIENT_DECREASE:
                volume.ambient = std::max(0.0f, volume.ambient - volumeChange);
                volumeChanged = true;
                break;

            case ACTION_BUTTON::VOLUME_AMBIENT_INCREASE:
                volume.ambient = std::min(1.0f, volume.ambient + volumeChange);
                volumeChanged = true;
                break;

            case ACTION_BUTTON::VOLUME_EFFECTS_DECREASE:
                volume.efeitos = std::max(0.0f, volume.efeitos - volumeChange);
                volumeChanged = true;
                break;

            case ACTION_BUTTON::VOLUME_EFFECTS_INCREASE:
                volume.efeitos = std::min(1.0f, volume.efeitos + volumeChange);
                volumeChanged = true;
                break;

            case ACTION_BUTTON::VOLUME_MUSIC_DECREASE:
                volume.musica = std::max(0.0f, volume.musica - volumeChange);
                volumeChanged = true;
                break;

            case ACTION_BUTTON::VOLUME_MUSIC_INCREASE:
                volume.musica = std::min(1.0f, volume.musica + volumeChange);
                volumeChanged = true;
                break;

            case ACTION_BUTTON::VOLUME_UI_DECREASE:
                volume.UI = std::max(0.0f, volume.UI - volumeChange);
                volumeChanged = true;
                break;

            case ACTION_BUTTON::VOLUME_UI_INCREASE:
                volume.UI = std::min(1.0f, volume.UI + volumeChange);
                volumeChanged = true;
                break;
            case ACTION_BUTTON::RESET_ALL:
                gameObjects.clear();
                player.reset();
                player.setPosition(0.0f, getTerrainHeight(0.0f, 0.0f) + 0.3f, 0.0f);
                initObjects();
                gameMode = STATE_GAME::PLAYING_EXPLORER;
                currentMap = MapType::MAIN;
                button_action = ACTION_BUTTON::NONE;
                break;
            }

            if (volumeChanged)
            {
                sound.setVolume(0, volume.musica);
                sound.setVolume(1, volume.musica);
                sound.setVolume(2, volume.efeitos);
                sound.setVolume(3, volume.efeitos);
                sound.setVolume(4, volume.efeitos);
                sound.setVolume(5, volume.UI);
                sound.setVolume(6, volume.ambient);
                sound.setVolume(7, volume.ambient);
                sound.setVolume(8, volume.ambient);
                sound.setVolume(9, volume.ambient);
                sound.setVolume(10, volume.musica);
                sound.setVolume(11, volume.musica);
                sound.setVolume(12, volume.UI);
                sound.setVolume(13, volume.UI);
                sound.setVolume(14, volume.efeitos);
                sound.setVolume(15, volume.musica);
                sound.setVolume(16, volume.UI);
                sound.setVolume(17, volume.efeitos);
                sound.setVolume(18, volume.efeitos);
                sound.setVolume(19, volume.musica);
                sound.setVolume(20, volume.musica);
            }
        }
    }
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

void Game::saveScreenshotBMP(const char *filename, int width, int height)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        printf("Erro ao abrir o arquivo para salvar.\n");
        return;
    }

    // Calcula padding para garantir que cada linha tem um múltiplo de 4 bytes
    int padding = (4 - (width * 3) % 4) % 4;
    int rowSize = width * 3 + padding;
    int imageSize = rowSize * height;

    BMPHeader bmpHeader = {
        {'B', 'M'},
        static_cast<unsigned int>(54 + imageSize),
        0,
        0,
        54};

    BMPInfoHeader bmpInfoHeader = {
        40,
        width,
        height,
        1,
        24,
        0,
        static_cast<unsigned int>(imageSize),
        2835,
        2835,
        0,
        0};

    fwrite(&bmpHeader, sizeof(BMPHeader), 1, file);
    fwrite(&bmpInfoHeader, sizeof(BMPInfoHeader), 1, file);

    // Aloca buffer para os pixels do OpenGL
    std::vector<GLubyte> pixels(3 * width * height);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Aloca buffer para a imagem rotacionada 180 graus
    std::vector<GLubyte> rotatedPixels(3 * width * height);

    // Rotaciona a imagem 180 graus (espelhando horizontalmente e verticalmente)
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Posição original
            int srcPos = (y * width + x) * 3;

            int destPos = ((height - 1 - y) * width + (width - 1 - x)) * 3;

            rotatedPixels[destPos] = pixels[srcPos + 2];     // B
            rotatedPixels[destPos + 1] = pixels[srcPos + 1]; // G
            rotatedPixels[destPos + 2] = pixels[srcPos];     // R
        }
    }

    unsigned char padBytes[3] = {0, 0, 0};

    for (int y = 0; y < height; y++)
    {
        fwrite(&rotatedPixels[y * width * 3], 3, width, file);

        if (padding > 0)
        {
            fwrite(padBytes, 1, padding, file);
        }
    }

    fclose(file);
}

void Game::shareScreenshot(int width, int height)
{
    // Gerar nome do arquivo com data e hora
    time_t now = time(nullptr);
    struct tm *t = localtime(&now);
    char filename[128];
    strftime(filename, sizeof(filename), "screenshot_%Y_%m_%d_%H-%M-%S.bmp", t);

    saveScreenshotBMP(filename, width, height);
}
void Game::updateButtonMenuHover(int x, int y)
{
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    float mouseYGL = windowHeight - y; // Ajusta Y para o sistema de coordenadas do OpenGL

    int hoveredID = -1; // Nenhum botão em hover inicialmente

    for (size_t i = 0; i < hud.getButtonMenu().size(); ++i)
    {
        auto &botao = hud.getButtonMenu()[i];
        float bx = botao.x;
        float by = botao.y;
        float bw = botao.width;
        float bh = botao.height;

        bool dentroX = (x >= bx && x <= (bx + bw));
        bool dentroY = (mouseYGL >= by && mouseYGL <= (by + bh));

        botao.hovering = dentroX && dentroY;
        if (botao.hovering)
            hoveredID = static_cast<int>(i);
    }

    hud.setHoveredButton(hoveredID);
}

void Game::init()
{
    if (!sound.initOpenAL())
    {
        return;
    }
    else
    {
        sound.carregarAudios(
            {
                "omnia_poetree_theme.wav",        // 0
                "city_of_ingrigues.wav",          // 1
                "punch.wav",                      // 2
                "swipe.wav",                      // 3
                "classiccoin.wav",                // 4
                "one_beep.wav",                   // 5
                "birds_backgorund.wav",           // 6
                "walk-on-grass.wav",              // 7
                "dark-void-ambience.wav",         // 8
                "ambient-dark-2.wav",             // 9
                "up-habilidade.wav",              // 10
                "bury-the-light-deep-within.wav", // 11
                "horror.wav",                     // 12
                "no-enemie.wav",                  // 13
                "espacial-attack.wav",            // 14
                "cat_meme.wav",                   // 15
                "print_window.wav",               // 16
                "step.wav",                       // 17
                "pum.wav",                        // 18
                "bomfire.wav",                    // 19
                "game_over.wav"                   // 20
            });
    }
}
// Getters e setters
Player &Game::getPlayer() { return player; }
STATE_GAME Game::getGameMode() const { return gameMode; }
void Game::setGameMode(STATE_GAME mode) { gameMode = mode; }

void Game::initCallback() { GetInstance().init(); }
void Game::mouseCallback(int button, int state, int x, int y) { GetInstance().handleMouseClick(button, state, x, y); }
void Game::motionCallback(int x, int y) { GetInstance().handleMouseMotion(x, y); }
void Game::passiveMotionCallback(int x, int y) { GetInstance().handlePassiveMouseMotion(x, y); }
void Game::displayCallback()
{
    GetInstance().render();
    glutSwapBuffers();
}
void Game::reshapeCallback(int w, int h) { glViewport(0, 0, w, h); }
void Game::keyboardCallback(unsigned char key, int x, int y) { GetInstance().handleKeyPress(key, x, y); }

void Game::specialCallback(int key, int x, int y)
{
    GetInstance().handleSpecialKeyPress(key, x, y);
}

void Game::KeyUpCallback(unsigned char key, int x, int y)
{
    GetInstance().handleKeyUp(key, x, y);
}
void Game::JoystickCallback(unsigned int key, int x, int y, int z)
{
    GetInstance().handleJoystick(key, x, y, z);
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

float Game::getTerrainHeight(float x, float z)
{
    float height = 0.0f;

    // Colinas suaves
    height += std::sin(x * 0.1f) * 0.5f;
    height += std::cos(z * 0.1f) * 0.5f;
    height += (std::sin(x * 0.3f + z * 0.5f) * 0.3f);

    // Lista de centros de lagos
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