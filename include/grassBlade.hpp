#ifndef GRASSBLADE_HPP
#define GRASSBLADE_HPP

#include "GameObject.hpp"
#include <GL/glut.h>
#include <cmath>

class GrassBlade : public GameObject {
public:
    GrassBlade(float x, float y, float z);

    void draw() override;
};

#endif // GRASSBLADE_HPP
