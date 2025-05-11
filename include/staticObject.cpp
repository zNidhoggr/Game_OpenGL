#include "staticObject.hpp"

StaticObject::StaticObject(float x, float y, float z, float size, ObjectType type,
                           float colorR, float colorG, float colorB)
    : GameObject(x, y, z, size, type)
{
    color[0] = colorR;
    color[1] = colorG;
    color[2] = colorB;
}
void StaticObject::draw()
{
    glPushMatrix();
    glTranslatef(x, y, z);

    GLfloat trunkColor[3] = {0.7f, 0.4f, 0.1f};
    GLfloat leavesColor[3] = {0.3f, 0.5f, 0.1f};
    GLfloat roofColor[3] = {0.5f, 0.5f, 0.5f};

    switch (type)
    {
    case TREE:
    {
        GLfloat ambient[] = {0.2f, 0.1f, 0.0f, 1.0f};
        GLfloat diffuse[] = {0.5f, 0.3f, 0.1f, 1.0f};
        GLfloat specular[] = {0.1f, 0.05f, 0.02f, 1.0f};
        GLfloat shininess = 10.0f;

        glDisable(GL_COLOR_MATERIAL);
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);

        glPushMatrix();
        glScalef(0.3f, 1.0f, 0.3f);
        glTranslatef(0.0f, 0.5f, 0.0f);
        glutSolidCube(size);
        glPopMatrix();

        GLfloat topambient[] = {0.1f, 0.2f, 0.1f, 1.0f};
        GLfloat topdiffuse[] = {0.2f, 0.6f, 0.2f, 1.0f};
        GLfloat topspecular[] = {0.05f, 0.1f, 0.05f, 1.0f};
        GLfloat topshininess = 5.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, topambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, topdiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, topspecular);
        glMaterialf(GL_FRONT, GL_SHININESS, topshininess);

        glPushMatrix();
        glTranslatef(0.0f, size * 0.8f, 0.0f);
        glutSolidSphere(size * 0.6f, 10, 10);
        glPopMatrix();
        break;
    }

    case ROCK:
    {
        GLfloat ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
        GLfloat diffuse[] = {0.5f, 0.5f, 0.5f, 1.0f};
        GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
        GLfloat shininess = 5.0f;

        glDisable(GL_COLOR_MATERIAL);
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);

        glScalef(size, size * 0.7f, size);
        glutSolidSphere(0.5f, 10, 10);
        break;
    }

    case ITEM:
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
        glutSolidSphere(size * 0.5f, 8, 8);

        static float angle = 0.0f;
        angle += 0.5f;
        glRotatef(angle, 0.0f, 1.0f, 0.0f);
        glutSolidTetrahedron();
        break;
    }
    case HOUSE:
    {
        GLfloat ambient[] = {0.3f, 0.2f, 0.1f, 1.0f};
        GLfloat diffuse[] = {0.6f, 0.4f, 0.2f, 1.0f};
        GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
        GLfloat shininess = 10.0f;

        glDisable(GL_COLOR_MATERIAL);
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);

        // Base da casa
        glPushMatrix();
        glScalef(size, size * 0.8f, size);
        glTranslatef(0.0f, 0.5f, 0.0f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Telhado (Pirâmide)
        glPushMatrix();
        glTranslatef(0.0f, size * 0.8f, 0.0f);
        glRotatef(270.0f, 1.0f, 0.0f, 0.0f);
        glutSolidCone(size * 0.7f, size * 0.5f, 4, 4);
        glPopMatrix();

        // Mobiliário - Mesa
        glPushMatrix();
        glTranslatef(0.0f, 0.3f, size * 0.2f);             // Posição central da mesa
        glScalef(0.5f, 0.1f, 0.3f);                        // Proporções da mesa
        GLfloat tableAmbient[] = {0.5f, 0.3f, 0.1f, 1.0f}; // Cor da mesa
        glMaterialfv(GL_FRONT, GL_AMBIENT, tableAmbient);
        glutSolidCube(1.0f); // Usando o cubo para a mesa
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-0.3f, 0.2f, size * 0.15f);           // Posição de uma cadeira próxima à mesa
        glScalef(0.2f, 0.4f, 0.2f);                        // Proporções da cadeira
        GLfloat chairAmbient[] = {0.3f, 0.2f, 0.1f, 1.0f}; // Cor da cadeira
        glMaterialfv(GL_FRONT, GL_AMBIENT, chairAmbient);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.3f, 0.0f, size * 0.3f);              // Posicionando o animal dentro da casa
        glScalef(0.3f, 0.3f, 0.3f);                         // Proporção do animal
        GLfloat animalAmbient[] = {0.7f, 0.5f, 0.2f, 1.0f}; // Cor do animal (poderia ser um cachorro ou gato)
        glMaterialfv(GL_FRONT, GL_AMBIENT, animalAmbient);
        glutSolidSphere(1.0f, 10, 10); // Usando esfera para simular um animal
        glPopMatrix();

        // Planta
        glPushMatrix();
        glTranslatef(-0.5f, 0.2f, size * 0.4f);            // Posição da planta dentro da casa
        glScalef(0.1f, 0.3f, 0.1f);                        // Proporções da planta
        GLfloat plantAmbient[] = {0.0f, 0.6f, 0.0f, 1.0f}; // Cor da planta (verde)
        glMaterialfv(GL_FRONT, GL_AMBIENT, plantAmbient);
        glutSolidCone(0.2f, 0.5f, 4, 4); // Usando cone para representar a planta
        glPopMatrix();

        break;
    }
    case WALL:
    {
        glDisable(GL_COLOR_MATERIAL);
        GLfloat ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
        GLfloat diffuse[] = {0.5f, 0.5f, 0.5f, 1.0f};
        GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
        GLfloat shininess = 10.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);

        glPushMatrix();
        glScalef(1.0f, 1.0f, 0.1f);
        glutSolidCube(size);
        glPopMatrix();
        break;
    }

    case DOOR:
    {
        GLfloat ambient[] = {0.3f, 0.1f, 0.0f, 1.0f};  // Cor da porta
        GLfloat diffuse[] = {0.6f, 0.3f, 0.1f, 1.0f};  // Cor difusa da porta
        GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f}; // Cor especular
        GLfloat shininess = 10.0f;                     // Brilho

        glDisable(GL_COLOR_MATERIAL);
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);

        glPushMatrix();
        glTranslatef(x, y, z);

        // Desenhando a porta
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f);
        glScalef(0.3f, 0.7f, 0.05f);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0f, 0.35f, 0.0f);
        glScalef(0.05f, 0.7f, 0.05f);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPopMatrix();
        break;
    }

    case ROOF: // Telhado
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, roofColor);

        // Desenha o telhado (pirâmide ou cone)
        glPushMatrix();
        glTranslatef(0.0f, size * 0.8f, 0.0f);
        glRotatef(270.f, 1.0f, 0.0f, 0.0f);
        glutSolidCone(size * 0.7f, size * 0.5f, 4, 4);
        glPopMatrix();
        break;
    }

    case WINDOW: // Janela
    {
        GLfloat windowColor[3] = {0.9f, 0.9f, 0.9f}; // Cor da janela
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, windowColor);

        // Desenha a janela
        glPushMatrix();
        glTranslatef(0.3f, 0.4f, size * 0.51f);
        glScalef(0.2f, 0.2f, 0.05f);
        glutSolidCube(1.0f);
        glPopMatrix();
        break;
    }

    case TABLE: // Mesa
    {
        GLfloat tableColor[3] = {0.4f, 0.2f, 0.1f}; // Cor da mesa
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tableColor);

        // Desenha a mesa
        glPushMatrix();
        glTranslatef(0.0f, size * 0.5f, 0.0f);
        glScalef(0.6f, 0.2f, 0.3f); // Tamanho da mesa
        glutSolidCube(1.0f);
        glPopMatrix();
        break;
    }
   case BONFIRE:
{
    static float flicker = 0.0f;
    flicker += 0.05f;

    // Definindo material dos troncos
    GLfloat trunkAmbient[] = {0.2f, 0.1f, 0.05f, 1.0f};
    GLfloat trunkDiffuse[] = {0.4f, 0.2f, 0.1f, 1.0f};
    GLfloat trunkSpecular[] = {0.1f, 0.05f, 0.02f, 1.0f};
    GLfloat trunkShininess = 5.0f;

    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, trunkAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, trunkDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, trunkSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, trunkShininess);

    // Desenhando os troncos da fogueira
    glPushMatrix();
    for (int i = 0; i < 5; i++)
    {
        glPushMatrix();
        glRotatef(i * 72, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.1f, 0.0f, 0.0f);
        glScalef(0.05f, 0.2f, 0.05f);
        glutSolidCube(size);
        glPopMatrix();
    }
    glPopMatrix();

    // Definindo material das chamas
    GLfloat fireAmbient[] = {0.3f, 0.1f, 0.0f, 1.0f};
    GLfloat fireDiffuse1[] = {1.0f, 0.5f, 0.0f, 1.0f};
    GLfloat fireDiffuse2[] = {1.0f, 0.2f, 0.0f, 1.0f};
    GLfloat fireSpecular[] = {0.5f, 0.3f, 0.1f, 1.0f};
    GLfloat fireShininess = 30.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, fireAmbient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, fireSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, fireShininess);

    // Desenhando as chamas
    glPushMatrix();
    for (int i = 0; i < 5; i++)
    {
        float scale = (0.5f + 0.5f * sin(flicker + i));
        glMaterialfv(GL_FRONT, GL_DIFFUSE, i % 2 == 0 ? fireDiffuse1 : fireDiffuse2);

        glPushMatrix();
        glScalef(scale, scale * 1.5f, scale);
        glutSolidSphere(0.3f, 10, 10);
        glPopMatrix();
    }
    glPopMatrix();

    break;
}


    case CHAIR: // Cadeira
    {
        GLfloat chairColor[3] = {0.6f, 0.3f, 0.1f}; // Cor da cadeira
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, chairColor);

        // Desenha a cadeira
        glPushMatrix();
        glTranslatef(0.0f, size * 0.5f, 0.0f);
        glScalef(0.3f, 0.4f, 0.3f); // Tamanho da cadeira
        glutSolidCube(1.0f);
        glPopMatrix();
        break;
    }

    case LAMP: // Lâmpada
    {
        GLfloat lampColor[3] = {1.0f, 1.0f, 0.5f}; // Cor da lâmpada
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, lampColor);

        // Desenha a lâmpada (esfera com haste)
        glPushMatrix();
        glTranslatef(0.0f, size * 0.8f, 0.0f);
        glutSolidSphere(size * 0.5f, 10, 10); // Cabeça da lâmpada
        glPopMatrix();
        break;
    }

    case ENEMY:
    case NPC:
    default:
        break;
    }

    glPopMatrix();
}
