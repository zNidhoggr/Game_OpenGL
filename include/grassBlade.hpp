#ifndef GRASS_BLADE_H
#define GRASS_BLADE_H

#include "GameObject.hpp" 
#include <GL/glut.h> 
#include <cmath>

class GrassBlade : public GameObject {
private:
    static constexpr float HEIGHT = 0.3f;
    static constexpr float WIDTH = 0.02f;
    static constexpr int BLADE_SEGMENTS = 3;
    static constexpr float SWAY_SPEED = 2.0f;
    static constexpr float SWAY_AMOUNT = 10.0f;

public:
    GrassBlade(float x, float y, float z);
    virtual void draw() override;
    
private:
    void setupMaterial() const;
    void drawBladeSegment(int segmentIndex) const;
    float calculateSway() const;
};

#endif 