#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "GameObject.hpp"
#include "skillTree.cpp"
#include <iostream>
#include <cmath>
#include <GL/glut.h>

class Player : public GameObject {
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

    static constexpr float MOVEMENT_SPEED = 0.1f;
    static constexpr float ROTATION_SPEED = 5.0f;

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
    void takeDamage(float amount);
    void heal(float amount);
    void addExperience(int xp);
    void levelUp();

    // Getters
    float getRotY() const;
    float getHealth() const;
    float getMaxHealth() const;
    int getLevel() const;
    int getExperience() const;
    int getExperienceToNextLevel() const;
    float getAttackCooldown() const;
    float getAttackTimer() const;
    float getAttackDamage() const;
    float getMovementSpeed() const;

 
    SkillTree& getSkillTree();
};

#endif // PLAYER_HPP
