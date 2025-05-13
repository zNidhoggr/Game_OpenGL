
#include "grassBlade.hpp"

GrassBlade::GrassBlade(float x, float y, float z) 
    : GameObject(x, y, z, 0.1f, ObjectType::GRASS, false) {
}

void GrassBlade::draw() {
    glPushMatrix();
    glTranslatef(x, y, z);
    float sway = calculateSway();
    glRotatef(sway, 0.0f, 0.0f, 1.0f);
    setupMaterial();
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < BLADE_SEGMENTS; ++i) {
        drawBladeSegment(i);
    }
    glEnd();
    
    glPopMatrix();
}

float GrassBlade::calculateSway() const {
    float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    return sin(time * SWAY_SPEED + x * 0.5f + z * 0.5f) * SWAY_AMOUNT;
}

void GrassBlade::setupMaterial() const {
    const GLfloat ambient[4] = {0.0f, 0.3f, 0.0f, 1.0f};
    const GLfloat diffuse[4] = {0.4f, 0.7f, 0.4f, 1.0f};
    const GLfloat specular[4] = {0.0f, 0.1f, 0.0f, 1.0f};
    const GLfloat shininess = 1.0f;
    
    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void GrassBlade::drawBladeSegment(int segmentIndex) const {
    float angle = segmentIndex * 60.0f * M_PI / 180.0f;
    
    float dx = std::cos(angle) * WIDTH;
    float dz = std::sin(angle) * WIDTH;
    
    glVertex3f(0.0f, 0.0f, 0.0f);    
    glVertex3f(dx, HEIGHT, dz);     
    glVertex3f(-dx, 0.0f, -dz);  
}