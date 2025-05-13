#include "portal.hpp"

extern unsigned int texturaPortal;

Portal::Portal(float x, float y, float z, float size, float destX, float destZ, MapType destMap)
    : GameObject(x, y, z, size, ObjectType::PORTAL),
      destinationX(destX), destinationZ(destZ), destinationMap(destMap) {}

void Portal::draw()
{
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

    glPushMatrix();
    glTranslatef(x, y, z);

    // Pulsação
    float scale = 1.0f + 0.1f * sin(time * 3.0f);
    float r = 0.6f + 0.4f * sin(time * 2.0f);
    float g = 0.2f + 0.3f * cos(time * 1.5f);
    float b = 1.0f;

    // Salvar estado atual
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    // Configuração para blending (transparência)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Desabilitar iluminação temporariamente para desenhar o centro do portal
    glDisable(GL_LIGHTING);
    
    // Textura para o centro do portal (disco)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaPortal); // Você precisa ter essa textura carregada
    
    // Configurar modo da textura para DECAL (substituir a cor sem iluminação)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    // Desenhar o disco central com textura
    glPushMatrix();
    glScalef(scale, 1.0f, scale);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotacionar para ficar no plano xz
    
    // Desenhar o disco com coordenadas de textura
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(r, g, b, 0.8f);
    glTexCoord2f(0.5f, 0.5f);
    glVertex3f(0.0f, 0.0f, 0.0f); // Centro do disco
    
    // Vértices do disco com coordenadas de textura
    const int numSegments = 32;
    for(int i = 0; i <= numSegments; i++) {
        float angle = i * (2.0f * M_PI / numSegments);
        float texX = 0.5f + 0.5f * cos(angle);
        float texY = 0.5f + 0.5f * sin(angle);
        float x = size * cos(angle);
        float z = size * sin(angle);
        
        glTexCoord2f(texX, texY);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
    glPopMatrix();
    
    // Rotação de textura para efeito de vórtex
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.5f, 0.5f, 0.0f); // Mover para o centro da textura
    glRotatef(time * 30.0f, 0.0f, 0.0f, 1.0f); // Rotacionar a textura
    glTranslatef(-0.5f, -0.5f, 0.0f); // Voltar da origem
    glMatrixMode(GL_MODELVIEW);
    
    // Desabilitar textura para os anéis
    glDisable(GL_TEXTURE_2D);
    
    // Efeito de material brilhante para os anéis
    GLfloat materialAmbient[] = {r * 0.3f, g * 0.3f, b * 0.3f, 0.7f};
    GLfloat materialDiffuse[] = {r, g, b, 0.7f};
    GLfloat materialSpecular[] = {1.0f, 1.0f, 1.0f, 0.7f};
    GLfloat materialShininess = 50.0f;
    
    // Reativar iluminação para os anéis
    glEnable(GL_LIGHTING);
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);
    
    // Anéis giratórios com iluminação
    for (int i = 0; i < 3; i++)
    {
        glPushMatrix();
        float ringAngle = time * 60.0f + i * 120;
        glRotatef(ringAngle, 0.0f, 1.0f, 0.0f);
        glScalef(scale, 1.0f, scale);
        glutSolidTorus(0.05, size + 0.1f * i, 10, 20);
        glPopMatrix();
    }
    
    // Restaurar a matriz de textura
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    // Restaurar todos os atributos
    glPopAttrib();
    
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
    if (destinationMap == MapType::MAIN){
        game.loadMainMap();
        return;
    }if (destinationMap == MapType::DUNGEON_ONE_LEVEL){
        game.loadDungeonMap();
        return;
    }if (destinationMap == MapType::DUNGEON_TWO_LEVEL){
        game.loadDungeonMap_Level2();
        return;
    }if (destinationMap == MapType::DUNGEON_THREE_LEVEL){
        game.loadDungeonMap_Level3();
        return;
    }if (destinationMap == MapType::BOSS){
        game.loadDungeonMap_Boss();
        return;
    }if (destinationMap == MapType::PARASIDE){
        game.loadParasideMap();
        return;
    }

    float y = getTerrainHeight(destinationX, destinationZ) + 0.3f;
    player.setPosition(destinationX, y, destinationZ);
    game.setCurrentMap(destinationMap);
}

float Portal::getTerrainHeight(float x, float z){
    float height = 0.0f;
    height += std::sin(x * 0.1f) * 0.5f;
    height += std::cos(z * 0.1f) * 0.5f;
    height += (std::sin(x * 0.3f + z * 0.5f) * 0.3f);

    std::vector<std::pair<float, float>> lakeCenters = {
        {5.0f, 5.0f},
        {-7.0f, -3.0f},
        {8.0f, -6.0f},
        {-4.0f, 7.0f}};

    for (const auto &center : lakeCenters){
        float dist = std::sqrt((x - center.first) * (x - center.first) + (z - center.second) * (z - center.second));
        if (dist < 3.5f){
            height -= (3.5f - dist) * 0.4f;
        }
    }

    return height;
}
