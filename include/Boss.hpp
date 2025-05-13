#ifndef BOSS_HPP
#define BOSS_HPP

#include "GameObject.hpp"
#include "Player.hpp"
#include "MeshLoader.hpp"
#include <vector>

class Boss : public GameObject {
private:
    float health;
    float maxHealth;
    int level;
    float moveSpeed;
    float attackDamage;
    float attackRange;
    float attackCooldown;
    float attackTimer;
    float specialAttackCooldown;
    float specialAttackTimer;
    bool enraged;
    float enragedThreshold;
    float detectionRange;
    bool isCombatActive;
    bool experienceGiven;
    float experienceValue;
    float timer;
    bool toggle;
    int toggleCount;
    float toggleInterval;
    float pauseTimer;
    bool paused;
    int level_two;
    void drawHealthBar();
    
public:
    Boss(float x, float y, float z, float size, int level);
    void update(float deltaTime);
    void moveTowardsPlayer(const Player &player, float deltaTime, MeshLoader &loader);
    bool attackPlayer(Player &player, float deltaTime);
    bool specialAttack(Player &player);
    void takeDamage(float amount, const AttackType& attack);
    void draw() override;  
    void drawForLoader(MeshLoader &loader);
    float getTerrainHeight(float x, float z);
    bool isExperienceGiven() const;
    void markExperienceAsGiven();
    float getHealth() const;
    float getMaxHealth() const;
    int getLevel() const;
    bool isInCombat() const;
    float getExperienceValue() const;
    bool isEnraged() const;    
};

#endif 