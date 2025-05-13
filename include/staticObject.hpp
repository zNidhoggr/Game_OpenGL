#ifndef STATIC_OBJECT_HPP
#define STATIC_OBJECT_HPP

#include "GameObject.hpp"
#include <GL/glut.h>

class StaticObject : public GameObject {
private:
    GLfloat color[3]; 

public:
    StaticObject(float x, float y, float z, float size, ObjectType type,
                 float colorR, float colorG, float colorB);

    void draw() override;
};

#endif
