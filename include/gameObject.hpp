#pragma once

#include <cmath> 

class GameObject {
protected:
    float x, y, z;      
    float size;       
    ObjectType type;     
    bool active;      
    bool collidable;     

public:
    GameObject(float x, float y, float z, float size, ObjectType type, bool collidable = true);
    virtual ~GameObject();

    virtual void update(float deltaTime);
    virtual void draw() = 0;

    bool checkCollision(const GameObject& other) const;
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
