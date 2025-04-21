#include "staticObject.hpp"

StaticObject::StaticObject(float x, float y, float z, float size, ObjectType type,
                           float colorR, float colorG, float colorB)
    : GameObject(x, y, z, size, type) {
    color[0] = colorR;
    color[1] = colorG;
    color[2] = colorB;
}

void StaticObject::draw() {
    glPushMatrix();
    glTranslatef(x, y, z);

    GLfloat trunkColor[3] = {0.7f, 0.4f, 0.1f};
    GLfloat leavesColor[3] = {0.3f, 0.5f, 0.1f};
    GLfloat roofColor[3] = {0.5f, 0.5f, 0.5f};

    switch (type) {
        case TREE: {
            GLfloat ambient[]  = {0.2f, 0.1f, 0.0f, 1.0f};
            GLfloat diffuse[]  = {0.5f, 0.3f, 0.1f, 1.0f};
            GLfloat specular[] = {0.1f, 0.05f, 0.02f, 1.0f};
            GLfloat shininess  = 10.0f;

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

            GLfloat topambient[]  = {0.1f, 0.2f, 0.1f, 1.0f};
            GLfloat topdiffuse[]  = {0.2f, 0.6f, 0.2f, 1.0f};
            GLfloat topspecular[] = {0.05f, 0.1f, 0.05f, 1.0f};
            GLfloat topshininess  = 5.0f;

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

        case ROCK: {
            GLfloat ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
            GLfloat diffuse[]  = {0.5f, 0.5f, 0.5f, 1.0f};
            GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
            GLfloat shininess  = 5.0f;

            glDisable(GL_COLOR_MATERIAL);
            glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
            glMaterialf(GL_FRONT, GL_SHININESS, shininess);

            glScalef(size, size * 0.7f, size);
            glutSolidSphere(0.5f, 10, 10);
            break;
        }

        case HOUSE: {
            GLfloat ambient[]  = {0.3f, 0.2f, 0.1f, 1.0f};
            GLfloat diffuse[]  = {0.6f, 0.4f, 0.2f, 1.0f};
            GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
            GLfloat shininess  = 10.0f;

            glDisable(GL_COLOR_MATERIAL);
            glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
            glMaterialf(GL_FRONT, GL_SHININESS, shininess);

            glPushMatrix();
            glScalef(size, size * 0.8f, size);
            glTranslatef(0.0f, 0.5f, 0.0f);
            glutSolidCube(1.0f);
            glPopMatrix();

            GLfloat roofAmbient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
            GLfloat roofDiffuse[]  = {0.5f, 0.5f, 0.5f, 1.0f};
            GLfloat roofSpecular[] = {0.05f, 0.05f, 0.05f, 1.0f};
            GLfloat roofShine      = 8.0f;

            glMaterialfv(GL_FRONT, GL_AMBIENT, roofAmbient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, roofDiffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, roofSpecular);
            glMaterialf(GL_FRONT, GL_SHININESS, roofShine);

            glPushMatrix();
            glTranslatef(0.0f, size * 0.8f, 0.0f);
            glBegin(GL_TRIANGLES);
            glVertex3f(-size * 0.6f, 0, -size * 0.6f);
            glVertex3f(size * 0.6f, 0, -size * 0.6f);
            glVertex3f(0, size * 0.6f, 0);

            glVertex3f(size * 0.6f, 0, -size * 0.6f);
            glVertex3f(size * 0.6f, 0, size * 0.6f);
            glVertex3f(0, size * 0.6f, 0);

            glVertex3f(size * 0.6f, 0, size * 0.6f);
            glVertex3f(-size * 0.6f, 0, size * 0.6f);
            glVertex3f(0, size * 0.6f, 0);

            glVertex3f(-size * 0.6f, 0, size * 0.6f);
            glVertex3f(-size * 0.6f, 0, -size * 0.6f);
            glVertex3f(0, size * 0.6f, 0);
            glEnd();
            glPopMatrix();
            break;
        }

        case WALL: {
            glDisable(GL_COLOR_MATERIAL);
            GLfloat ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
            GLfloat diffuse[]  = {0.5f, 0.5f, 0.5f, 1.0f};
            GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
            GLfloat shininess  = 10.0f;

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

        case ITEM: {
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
            glutSolidSphere(size * 0.5f, 8, 8);

            static float angle = 0.0f;
            angle += 0.5f;
            glRotatef(angle, 0.0f, 1.0f, 0.0f);
            glutSolidTetrahedron();
            break;
        }

        case ENEMY:
        case NPC:
        default:
            break;
    }

    glPopMatrix();
}
