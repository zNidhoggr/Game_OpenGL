#ifndef STATIC_OBJECT_HPP
#define STATIC_OBJECT_HPP

#include "GameObject.hpp"
#include <GL/glut.h>

// Classe que representa objetos estáticos no cenário
class StaticObject : public GameObject {
private:
    GLfloat color[3]; // Cor para objetos como ITEM

public:
    // Construtor
    StaticObject(float x, float y, float z, float size, ObjectType type,
                 float colorR, float colorG, float colorB);

    // Método de desenho (sobrescreve GameObject)
    void draw() override;
};

#endif
