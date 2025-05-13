#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "GameObject.hpp"
#include "Player.hpp"
#include <GL/glut.h>
#include <cmath>

class Enemy : public GameObject {
private:
    float health;
    float maxHealth;
    int level;
    float moveSpeed;
    float attackDamage;
    float attackRange;
    float attackCooldown;
    float attackTimer;
    float detectionRange;
    bool isCombatActive;
    bool experienceGiven = false;

public:
    Enemy(float x, float y, float z, float size, int level);
    void update(float deltaTime) override;
    void moveTowardsPlayer(const Player& player, float deltaTime);
    bool attackPlayer(Player& player, float deltaTime);
    void takeDamage(float amount, const AttackType& attack);
    void draw() override;
    void drawHealthBar();
    float getTerrainHeight(float x, float z);
    bool isExperienceGiven() const;
    void markExperienceAsGiven();
    float getHealth() const;
    float getMaxHealth() const;
    int getLevel() const;
    bool isInCombat() const;
    float getExperienceValue() const;
    float getDetectionRange(){ return detectionRange; }
    void setDetectionRange(float newState){  detectionRange = newState;}
};

#endif 
