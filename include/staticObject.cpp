#include "staticObject.hpp"

extern unsigned int textureWood;
extern unsigned int textureTrunk;
extern unsigned int textureRock;
extern unsigned int textureItem;
extern unsigned int textureWall;
extern unsigned int textureLeaves;
extern unsigned int textureHouseWall;
extern unsigned int textureRoof;
extern unsigned int textureDoor;
extern unsigned int textureBrick;
extern unsigned int textureFloor;

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

    GLfloat ambient[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat specular[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat shininess = 10.0f;

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    switch (type)
    {
    case TREE:
    {
        glBindTexture(GL_TEXTURE_2D, textureTrunk);

        GLfloat trunkAmbient[] = {0.3f, 0.15f, 0.05f, 1.0f};
        GLfloat trunkDiffuse[] = {0.7f, 0.4f, 0.2f, 1.0f};
        GLfloat trunkSpecular[] = {0.1f, 0.1f, 0.05f, 1.0f};
        GLfloat trunkShininess = 5.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, trunkAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, trunkDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, trunkSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS, trunkShininess);

        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f);

        GLUquadric *trunkQuad = gluNewQuadric();
        gluQuadricTexture(trunkQuad, GL_TRUE);
        gluQuadricNormals(trunkQuad, GLU_SMOOTH);

        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

        gluCylinder(trunkQuad,
                    size * 0.2,
                    size * 0.15,
                    size * 1.5,
                    12,
                    4);

        gluDeleteQuadric(trunkQuad);
        glPopMatrix();

        glBindTexture(GL_TEXTURE_2D, textureLeaves);

        GLfloat leavesAmbient[] = {0.2f, 0.3f, 0.1f, 1.0f};
        GLfloat leavesDiffuse[] = {0.3f, 0.7f, 0.3f, 0.8f};
        GLfloat leavesSpecular[] = {0.1f, 0.2f, 0.1f, 1.0f};
        GLfloat leavesShininess = 10.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, leavesAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, leavesDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, leavesSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS, leavesShininess);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glPushMatrix();
        glTranslatef(0.0f, size * 1.2f, 0.0f);

        GLUquadric *leavesQuad = gluNewQuadric();
        gluQuadricTexture(leavesQuad, GL_TRUE);
        gluSphere(leavesQuad, size * 0.6f, 24, 24);

        for (int i = 0; i < 3; i++)
        {
            glPushMatrix();
            float angle = i * 120.0f;
            float offset = size * 0.4f;
            glTranslatef(cos(angle) * offset, size * 0.1f, sin(angle) * offset);
            gluSphere(gluNewQuadric(), size * 0.3f, 16, 16);
            glPopMatrix();
        }

        gluDeleteQuadric(leavesQuad);
        glPopMatrix();

        glDisable(GL_BLEND);

        break;
    }

    case ROCK:
    {
        glBindTexture(GL_TEXTURE_2D, textureRock);

        GLfloat rockAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
        GLfloat rockDiffuse[] = {0.5f, 0.5f, 0.5f, 1.0f};
        GLfloat rockSpecular[] = {0.1f, 0.1f, 0.1f, 1.0f};
        GLfloat rockShininess = 1.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, rockAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, rockDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, rockSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS, rockShininess);

        glPushMatrix();
        glScalef(size, size * 0.7f, size);
        GLUquadric *quad = gluNewQuadric();
        gluQuadricTexture(quad, GL_TRUE);
        gluSphere(quad, 0.5f, 16, 16);
        gluDeleteQuadric(quad);
        glPopMatrix();
        break;
    }

    case ITEM:
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureItem);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        GLfloat goldAmbient[] = {0.3f, 0.2f, 0.1f, 1.0f};
        GLfloat goldDiffuse[] = {0.8f, 0.7f, 0.2f, 1.0f};
        GLfloat goldSpecular[] = {0.6f, 0.6f, 0.3f, 1.0f};
        GLfloat goldShininess = 50.0f;
        GLfloat goldEmission[] = {0.1f, 0.1f, 0.0f, 1.0f};

        glMaterialfv(GL_FRONT, GL_AMBIENT, goldAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, goldDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, goldSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS, goldShininess);
        glMaterialfv(GL_FRONT, GL_EMISSION, goldEmission);

        static float floatOffset = 0.0f;
        floatOffset += 0.03f;
        float floatHeight = sin(floatOffset) * 0.05f;

        glPushMatrix();
        glTranslatef(0.0f, floatHeight, 0.0f);
        GLUquadric *quad = gluNewQuadric();
        gluQuadricTexture(quad, GL_TRUE);
        gluQuadricNormals(quad, GLU_SMOOTH);

        glPushMatrix();
        glScalef(1.0f, 0.2f, 1.0f);
        gluSphere(quad, size * 0.5f, 24, 24);
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);
        GLfloat gemAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f};
        GLfloat gemDiffuse[] = {0.9f, 0.8f, 0.1f, 1.0f};
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gemDiffuse);

        static float rotateAngle = 0.0f;
        rotateAngle += 0.8f;
        glRotatef(rotateAngle, 0.0f, 1.0f, 0.0f);
        glutSolidTorus(0.05f, 0.15f, 6, 16);

        glEnable(GL_TEXTURE_2D);
        gluDeleteQuadric(quad);
        glPopMatrix();

        GLfloat noEmission[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, noEmission);
        glDisable(GL_TEXTURE_2D);

        break;
    }

    case HOUSE:
    {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);

        glBindTexture(GL_TEXTURE_2D, textureHouseWall);

        GLfloat wallAmbient[] = {0.4f, 0.3f, 0.2f, 1.0f};
        GLfloat wallDiffuse[] = {0.8f, 0.7f, 0.6f, 1.0f};
        GLfloat wallSpecular[] = {0.1f, 0.1f, 0.1f, 1.0f};
        GLfloat wallShininess = 10.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, wallAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, wallDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, wallSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS, wallShininess);

        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 0.0f);

        float houseWidth = size;
        float houseHeight = size * 0.8f;
        float houseDepth = size * 0.8f;

        for (int i = 0; i < 4; i++)
        {
            glPushMatrix();
            glRotatef(i * 90.0f, 0.0f, 1.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, houseDepth / 2);

            glBegin(GL_QUADS);
            glNormal3f(0.0f, 0.0f, 1.0f);

            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(-houseWidth / 2, 0.0f, 0.0f);
            glTexCoord2f(2.0f, 0.0f);
            glVertex3f(houseWidth / 2, 0.0f, 0.0f);
            glTexCoord2f(2.0f, 2.0f);
            glVertex3f(houseWidth / 2, houseHeight, 0.0f);
            glTexCoord2f(0.0f, 2.0f);
            glVertex3f(-houseWidth / 2, houseHeight, 0.0f);
            glEnd();

            if (i < 3)
            {
                glDisable(GL_TEXTURE_2D);
                GLfloat windowAmbient[] = {0.1f, 0.1f, 0.2f, 0.7f};
                GLfloat windowDiffuse[] = {0.2f, 0.2f, 0.4f, 0.7f};
                glMaterialfv(GL_FRONT, GL_AMBIENT, windowAmbient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, windowDiffuse);

                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                glPushMatrix();
                glTranslatef(0.0f, houseHeight * 0.5f, 0.01f);
                glScalef(0.3f, 0.4f, 1.0f);

                glBegin(GL_QUADS);
                glNormal3f(0.0f, 0.0f, 1.0f);
                glVertex3f(-0.5f, -0.5f, 0.0f);
                glVertex3f(0.5f, -0.5f, 0.0f);
                glVertex3f(0.5f, 0.5f, 0.0f);
                glVertex3f(-0.5f, 0.5f, 0.0f);
                glEnd();

                glPopMatrix();
                glDisable(GL_BLEND);
                glEnable(GL_TEXTURE_2D);
                glMaterialfv(GL_FRONT, GL_AMBIENT, wallAmbient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, wallDiffuse);
            }
            glPopMatrix();
        }

        glBindTexture(GL_TEXTURE_2D, textureFloor);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-houseWidth / 2, 0.0f, -houseDepth / 2);
        glTexCoord2f(2.0f, 0.0f);
        glVertex3f(houseWidth / 2, 0.0f, -houseDepth / 2);
        glTexCoord2f(2.0f, 2.0f);
        glVertex3f(houseWidth / 2, 0.0f, houseDepth / 2);
        glTexCoord2f(0.0f, 2.0f);
        glVertex3f(-houseWidth / 2, 0.0f, houseDepth / 2);
        glEnd();

        glPopMatrix();
        glBindTexture(GL_TEXTURE_2D, textureRoof);

        GLfloat roofAmbient[] = {0.3f, 0.1f, 0.0f, 1.0f};
        GLfloat roofDiffuse[] = {0.6f, 0.2f, 0.1f, 1.0f};
        GLfloat roofSpecular[] = {0.1f, 0.1f, 0.1f, 1.0f};
        GLfloat roofShininess = 5.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, roofAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, roofDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, roofSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS, roofShininess);

        float roofHeight = size * 0.4f;
        float roofOverhang = size * 0.1f;

        glPushMatrix();
        glTranslatef(0.0f, houseHeight, 0.0f);

        glBegin(GL_TRIANGLES);
        glNormal3f(0.0f, roofHeight, houseDepth / 2);
        glTexCoord2f(0.5f, 1.0f);
        glVertex3f(0.0f, roofHeight, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-houseWidth / 2 - roofOverhang, 0.0f, -houseDepth / 2 - roofOverhang);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(houseWidth / 2 + roofOverhang, 0.0f, -houseDepth / 2 - roofOverhang);
        glEnd();

        glBegin(GL_TRIANGLES);
        glNormal3f(0.0f, roofHeight, -houseDepth / 2);
        glTexCoord2f(0.5f, 1.0f);
        glVertex3f(0.0f, roofHeight, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-houseWidth / 2 - roofOverhang, 0.0f, houseDepth / 2 + roofOverhang);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(houseWidth / 2 + roofOverhang, 0.0f, houseDepth / 2 + roofOverhang);
        glEnd();

        glBegin(GL_QUADS);
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(houseWidth / 2 + roofOverhang, 0.0f, -houseDepth / 2 - roofOverhang);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(houseWidth / 2 + roofOverhang, 0.0f, houseDepth / 2 + roofOverhang);
        glTexCoord2f(0.5f, 1.0f);
        glVertex3f(0.0f, roofHeight, 0.0f);
        glTexCoord2f(0.5f, 1.0f);
        glVertex3f(0.0f, roofHeight, 0.0f);

        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-houseWidth / 2 - roofOverhang, 0.0f, houseDepth / 2 + roofOverhang);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-houseWidth / 2 - roofOverhang, 0.0f, -houseDepth / 2 - roofOverhang);
        glTexCoord2f(0.5f, 1.0f);
        glVertex3f(0.0f, roofHeight, 0.0f);
        glTexCoord2f(0.5f, 1.0f);
        glVertex3f(0.0f, roofHeight, 0.0f);
        glEnd();

        glPopMatrix();

        glBindTexture(GL_TEXTURE_2D, textureDoor);
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, -houseDepth / 2 - 0.01f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

        GLfloat doorAmbient[] = {0.4f, 0.3f, 0.2f, 1.0f};
        GLfloat doorDiffuse[] = {0.7f, 0.5f, 0.3f, 1.0f};
        glMaterialfv(GL_FRONT, GL_AMBIENT, doorAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, doorDiffuse);

        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-0.3f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(0.3f, 0.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(0.3f, houseHeight * 0.6f, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-0.3f, houseHeight * 0.6f, 0.0f);
        glEnd();

        glPopMatrix();

        glBindTexture(GL_TEXTURE_2D, textureBrick);
        glPushMatrix();
        glTranslatef(houseWidth * 0.3f, houseHeight + roofHeight * 0.5f, 0.0f);
        glScalef(0.1f, 0.3f, 0.1f);

        GLfloat chimneyAmbient[] = {0.5f, 0.3f, 0.2f, 1.0f};
        GLfloat chimneyDiffuse[] = {0.8f, 0.5f, 0.4f, 1.0f};
        glMaterialfv(GL_FRONT, GL_AMBIENT, chimneyAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, chimneyDiffuse);

        glutSolidCube(1.0f);
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);
        break;
    }

    case WALL:
    {
        glBindTexture(GL_TEXTURE_2D, textureWall);

        GLfloat wallAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
        GLfloat wallDiffuse[] = {0.5f, 0.5f, 0.5f, 1.0f};
        GLfloat wallSpecular[] = {0.1f, 0.1f, 0.1f, 1.0f};
        GLfloat wallShininess = 1.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, wallAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, wallDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, wallSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS, wallShininess);

        glPushMatrix();
        glScalef(1.0f, 1.0f, 0.1f);

        float wallSize = size;
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-wallSize, -wallSize, wallSize);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(wallSize, -wallSize, wallSize);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(wallSize, wallSize, wallSize);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-wallSize, wallSize, wallSize);
        glEnd();
        glPopMatrix();
        break;
    }

    case BONFIRE:
    {
        static float flicker = 0.0f;
        flicker += 0.1f;

        glBindTexture(GL_TEXTURE_2D, textureWood);

        GLfloat woodAmbient[] = {0.3f, 0.15f, 0.05f, 1.0f};
        GLfloat woodDiffuse[] = {0.6f, 0.3f, 0.1f, 1.0f};
        GLfloat woodSpecular[] = {0.1f, 0.05f, 0.02f, 1.0f};
        GLfloat woodShininess = 10.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, woodAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, woodDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, woodSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS, woodShininess);

        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, textureRock);
        glTranslatef(0.0f, -0.1f, 0.0f);
        glutSolidTorus(0.2f, 0.4f, 8, 16);

        glBindTexture(GL_TEXTURE_2D, textureWood);
        for (int i = 0; i < 6; i++)
        {
            glPushMatrix();
            float angle = i * 60.0f;
            float tilt = 15.0f + (i % 2) * 10.0f;

            glRotatef(angle, 0.0f, 1.0f, 0.0f);
            glRotatef(tilt, 0.0f, 0.0f, 1.0f);

            GLUquadric *logQuad = gluNewQuadric();
            gluQuadricTexture(logQuad, GL_TRUE);
            gluQuadricNormals(logQuad, GLU_SMOOTH);

            gluCylinder(logQuad,
                        size * 0.08f,
                        size * 0.05f,
                        size * 0.6f,
                        8,
                        3);

            gluDeleteQuadric(logQuad);
            glPopMatrix();
        }
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GLfloat fireAmbient[] = {0.5f, 0.2f, 0.0f, 0.8f};
        GLfloat fireDiffuse1[] = {1.0f, 0.6f, 0.1f, 0.7f};
        GLfloat fireDiffuse2[] = {1.0f, 0.3f, 0.0f, 0.9f};
        GLfloat fireSpecular[] = {0.8f, 0.5f, 0.2f, 1.0f};
        GLfloat fireShininess = 50.0f;

        glMaterialfv(GL_FRONT, GL_AMBIENT, fireAmbient);
        glMaterialfv(GL_FRONT, GL_SPECULAR, fireSpecular);
        glMaterialf(GL_FRONT, GL_SHININESS, fireShininess);

        glPushMatrix();
        glTranslatef(0.0f, size * 0.3f, 0.0f);

        for (int i = 0; i < 8; i++)
        {
            float scale = (0.6f + 0.4f * sin(flicker + i * 0.5f));
            float heightScale = 1.5f + sin(flicker * 1.3f + i) * 0.5f;
            float offsetX = cos(i * 45.0f) * 0.2f;
            float offsetZ = sin(i * 45.0f) * 0.2f;

            glMaterialfv(GL_FRONT, GL_DIFFUSE, i % 2 == 0 ? fireDiffuse1 : fireDiffuse2);

            glPushMatrix();
            glTranslatef(offsetX, 0.0f, offsetZ);
            glScalef(scale, scale * heightScale, scale);

            glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(0.0f, 0.3f, 0.0f);
            for (int j = 0; j <= 360; j += 30)
            {
                float rad = j * M_PI / 180.0f;
                float width = 0.15f + sin(rad * 3.0f) * 0.05f;
                glVertex3f(cos(rad) * width, 0.0f, sin(rad) * width);
            }
            glEnd();

            glPopMatrix();
        }

        glPointSize(3.0f);
        glBegin(GL_POINTS);
        for (int i = 0; i < 20; i++)
        {
            float sparkFlicker = flicker * 2.0f + i;
            float x = cos(sparkFlicker + i * 18.0f) * 0.1f;
            float y = 0.1f + fabs(sin(sparkFlicker * 0.7f + i)) * 0.3f;
            float z = sin(sparkFlicker + i * 22.0f) * 0.1f;

            glMaterialfv(GL_FRONT, GL_DIFFUSE, i % 3 ? fireDiffuse1 : fireDiffuse2);
            glVertex3f(x, y, z);
        }
        glEnd();

        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0f, size * 0.8f, 0.0f);
        GLfloat smokeAmbient[] = {0.2f, 0.2f, 0.2f, 0.4f};
        GLfloat smokeDiffuse[] = {0.3f, 0.3f, 0.3f, 0.3f};
        glMaterialfv(GL_FRONT, GL_AMBIENT, smokeAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, smokeDiffuse);

        for (int i = 0; i < 3; i++)
        {
            glPushMatrix();
            float smokeScale = 0.3f + sin(flicker * 0.5f + i) * 0.1f;
            float smokeHeight = i * 0.3f + flicker * 0.05f;
            glTranslatef(0.0f, smokeHeight, 0.0f);
            glScalef(smokeScale, smokeScale * 0.5f, smokeScale);
            glutSolidSphere(0.2f, 8, 8);
            glPopMatrix();
        }
        glPopMatrix();

        glDisable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        break;
    }

    case ENEMY:
    case NPC:
    default:
        break;
    }

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}