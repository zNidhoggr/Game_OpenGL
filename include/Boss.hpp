#ifndef BOSS_HPP
#define BOSS_HPP

#include "GameObject.hpp"
#include "Player.hpp"
#include "MeshLoader.hpp"  // Fixed capitalization for consistency
#include <vector>

class Boss : public GameObject {
private:
    // Core stats
    float health;
    float maxHealth;
    int level;
    
    // Combat attributes
    float moveSpeed;
    float attackDamage;
    float attackRange;
    float attackCooldown;
    float attackTimer;
    float specialAttackCooldown;
    float specialAttackTimer;
    
    // Behavior states
    bool enraged;
    float enragedThreshold;
    float detectionRange;
    bool isCombatActive;
    
    // Experience system
    bool experienceGiven;
    float experienceValue;
    
    // Animation/behavior control
    float timer;
    bool toggle;
    int toggleCount;
    float toggleInterval;
    float pauseTimer;
    bool paused;
    int level_two;
    
    // Helper drawing method
    void drawHealthBar();
    
public:
    // Constructor
    Boss(float x, float y, float z, float size, int level);
    
    // Main update function
    void update(float deltaTime);
    
    // Movement and combat
    void moveTowardsPlayer(const Player &player, float deltaTime, MeshLoader &loader);
    bool attackPlayer(Player &player, float deltaTime);
    bool specialAttack(Player &player);
    void takeDamage(float amount, const AttackType& attack);
    
    // Render functions
    void draw() override;  // Add override keyword for clarity
    void drawForLoader(MeshLoader &loader);
    
    // Terrain interaction
    float getTerrainHeight(float x, float z);
    
    // Experience system
    bool isExperienceGiven() const;
    void markExperienceAsGiven();
    
    // Getters
    float getHealth() const;
    float getMaxHealth() const;
    int getLevel() const;
    bool isInCombat() const;
    float getExperienceValue() const;
    bool isEnraged() const;
    
    // Initialize animation variables in constructor rather than in class declaration
};

#endif // BOSS_HPP