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

public:
    Enemy(float x, float y, float z, float size, int level);

    void update(float deltaTime) override;
    void moveTowardsPlayer(const Player& player, float deltaTime);
    bool attackPlayer(Player& player, float deltaTime);
    void takeDamage(float amount);
    void draw() override;
    void drawHealthBar();
    float getTerrainHeight(float x, float z);

    // Getters
    float getHealth() const;
    float getMaxHealth() const;
    int getLevel() const;
    bool isInCombat() const;
    float getExperienceValue() const;
};

#endif // ENEMY_HPP
