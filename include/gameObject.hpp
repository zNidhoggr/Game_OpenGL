#pragma once

#include <cmath> // pra usar std::sqrt

// Enum que representa o tipo do objeto (define tu mesmo em outro lugar)

class GameObject {
protected:
    float x, y, z;       // Posição
    float size;          // Tamanho
    ObjectType type;     // Tipo de objeto
    bool active;         // Se está ativo no mundo
    bool collidable;     // Se colide com o jogador

public:
    GameObject(float x, float y, float z, float size, ObjectType type, bool collidable = true);
    virtual ~GameObject();

    virtual void update(float deltaTime);
    virtual void draw() = 0;

    bool checkCollision(const GameObject& other) const;

    // Getters e setters
    float getX() const;
    float getY() const;
    float getZ() const;
    float getSize() const;
    ObjectType getType() const;
    bool isActive() const;
    bool isCollidable() const;

    void setPosition(float newX, float newY, float newZ);
    void setActive(bool state);
    void setCollidable(bool value);
};
