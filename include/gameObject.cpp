#include "gameObject.hpp"
#include "data.hpp"

GameObject::GameObject(float x, float y, float z, float size, ObjectType type, bool collidable)
    : x(x), y(y), z(z), size(size), type(type), active(true), collidable(collidable) {}

GameObject::~GameObject() {}

void GameObject::update(float deltaTime) {

}

bool GameObject::checkCollision(const GameObject& other) const {
    float dx = x - other.x;
    float dz = z - other.z;
    float distance = std::sqrt(dx * dx + dz * dz);
    return distance < (size + other.size);
}

float GameObject::getX() const { return x; }
float GameObject::getY() const { return y; }
float GameObject::getZ() const { return z; }
float GameObject::getSize() const { return size; }
ObjectType GameObject::getType() const { return type; }
bool GameObject::isActive() const { return active; }
bool GameObject::isCollidable() const { return collidable; }

void GameObject::setPosition(float newX, float newY, float newZ) {
    x = newX;
    y = newY;
    z = newZ;
}

void GameObject::setActive(bool state) {
    active = state;
}

void GameObject::setCollidable(bool value) {
    collidable = value;
}
