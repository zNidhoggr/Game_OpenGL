#include "grassBlade.hpp"

GrassBlade::GrassBlade(float x, float y, float z)
    : GameObject(x, y, z, 0.1f, ObjectType::GRASS, false) {}

void GrassBlade::draw() {
    glPushMatrix();
    glTranslatef(x, y, z);

    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float sway = sin(time * 2.0f + x * 0.5f + z * 0.5f) * 10.0f;

    glRotatef(sway, 0.0f, 0.0f, 1.0f);

    GLfloat ambient[]  = {0.0f, 0.2f, 0.0f, 1.0f};
    GLfloat diffuse[]  = {0.2f, 0.8f, 0.2f, 1.0f};
    GLfloat specular[] = {0.0f, 0.05f, 0.0f, 1.0f};
    GLfloat shininess  = 5.0f;

    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glBegin(GL_TRIANGLES);

    float h = 0.3f;
    float w = 0.02f;

    for (int i = 0; i < 3; ++i) {
        float angle = i * 60.0f * M_PI / 180.0f;
        float dx = std::cos(angle) * w;
        float dz = std::sin(angle) * w;

        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(dx, h, dz);
        glVertex3f(-dx, 0.0f, -dz);
    }

    glEnd();
    glPopMatrix();
}
