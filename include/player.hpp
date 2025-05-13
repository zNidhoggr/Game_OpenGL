#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "GameObject.hpp"
#include "skillTree.cpp"
#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <GL/gl.h>

class Player : public GameObject
{
private:
    float rotY;
    float health;
    float maxHealth;
    int level;
    int experience;
    int experienceToNextLevel;
    SkillTree skillTree;
    float attackCooldown;
    float attackTimer;

    bool upPressed;
    bool downPressed;
    bool leftPressed;
    bool rightPressed;

    bool wPressed;
    bool sPressed;
    bool aPressed;
    bool dPressed;

    bool upAndNotDown;
    bool downAndNotW;
    bool leftAndNotRight;
    bool rightAndNotLeft;

    bool wAndNotS;
    bool sAndNotW;
    bool aAndNotD;
    bool dAndNotA;

    float MOVEMENT_SPEED = 0.08f;
    static constexpr float ROTATION_SPEED = 1.0f;

    bool running = false;

public:
    Player(float x, float y, float z);

    void update(float deltaTime) override;
    void draw() override;

    void moveForward();
    void moveBackward();
    void strafeLeft();
    void strafeRight();
    void rotateRight(float delta = ROTATION_SPEED);
    void rotateLeft(float delta = ROTATION_SPEED);

    bool attack();
    void takeDamage(float amount, const AttackType& attack);
    void heal(float amount);
    void addExperience(int xp);
    void levelUp();
    void handleMovement();

    float getRotY() const;
    float getHealth() const;
    float getMaxHealth() const;
    int getLevel() const;
    int getExperience() const;
    int getExperienceToNextLevel() const;
    float getAttackCooldown() const;
    float getAttackTimer() const;
    float getAttackDamage() const;
    float getAttackDamageMagic() const;
    float getMovementSpeed() const;

    void reset();

    bool getIsRunning(){ return running; }
    void toggleRunning() { running = running? false: true; }
    void setIsRunnung(bool newState) {  running = newState; }
    void setHealth(float newState) { health = newState; }

    SkillTree &getSkillTree();
    float speed;
    void setSpeed(float movementSpeed);
};

#endif 
