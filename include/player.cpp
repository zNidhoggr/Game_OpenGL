#include "player.hpp"
#include <GL/glut.h>
#include <GL/gl.h>
#include <cmath>
#include <iostream>

Player::Player(float x, float y, float z)
    : GameObject(x, y, z, 0.3f, ObjectType::NPC),
      rotY(0.0f), health(100.0f), maxHealth(100.0f),
      level(1), experience(0), experienceToNextLevel(100),
      attackCooldown(1.0f), attackTimer(0.0f), 
      upPressed(false), downPressed(false),
      leftPressed(false), rightPressed(false),
      wPressed(false), sPressed(false),
      aPressed(false), dPressed(false),
      upAndNotDown(false), downAndNotW(false),
      leftAndNotRight(false), rightAndNotLeft(false),
      wAndNotS(false), sAndNotW(false),
      aAndNotD(false), dAndNotA(false) {}

void Player::update(float deltaTime)
{
    // Update attack cooldown timer
    if (attackTimer > 0)
    {
        attackTimer -= deltaTime;
        if (attackTimer < 0)
            attackTimer = 0;
    }

    // Update movement state flags
    wAndNotS = wPressed && !sPressed;
    sAndNotW = sPressed && !wPressed;
    aAndNotD = aPressed && !dPressed;
    dAndNotA = dPressed && !aPressed;

    // Process movement based on flags
    handleMovement();
}

void Player::handleMovement()
{
    // Apply movement based on current movement speed
    float actualSpeed = getMovementSpeed();
    
    if (wAndNotS)
        moveForward();
    else if (sAndNotW)
        moveBackward();

    if (aAndNotD)
        strafeLeft();
    else if (dAndNotA)
        strafeRight();
}

void Player::draw()
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // Player material properties
    GLfloat ambient[] = {0.0f, 0.0f, 0.3f, 1.0f};
    GLfloat diffuse[] = {0.1f, 0.5f, 0.8f, 0.8f};
    GLfloat specular[] = {0.9f, 0.4f, 0.4f, 1.0f};
    GLfloat shininess = 64.0f;

    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    // Draw player body
    glutSolidCube(0.6f);

    // Draw player head
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glutSolidSphere(0.2f, 10, 10);
    glPopMatrix();

    glPopMatrix();
}

void Player::moveForward()
{
    // Calculate movement vector based on player's rotation
    float radians = rotY * M_PI / 180.0f;
    x += (getMovementSpeed() + (running? 0.05f: 0.0f)) * std::sin(radians);
    z += (getMovementSpeed() + (running? 0.05f: 0.0f))  * std::cos(radians);
}

void Player::moveBackward()
{
    float radians = rotY * M_PI / 180.0f;
    x -= (getMovementSpeed() + (running? 0.05f: 0.0f)) * std::sin(radians);
    z -= (getMovementSpeed() + (running? 0.05f: 0.0f))  * std::cos(radians);
}

void Player::strafeLeft()
{
    float radians = rotY * M_PI / 180.0f;
    x -= (getMovementSpeed() + (running? 0.05f: 0.0f)) * std::cos(radians);
    z += (getMovementSpeed() + (running? 0.05f: 0.0f)) * std::sin(radians);
}

void Player::strafeRight()
{
    float radians = rotY * M_PI / 180.0f;
    x += (getMovementSpeed() + (running? 0.05f: 0.0f)) * std::cos(radians);
    z -= (getMovementSpeed() + (running? 0.05f: 0.0f)) * std::sin(radians);
}

void Player::rotateRight(float delta)
{
    rotY += delta;
}

void Player::rotateLeft(float delta)
{
    rotY -= delta;
}

bool Player::attack()
{
    if (attackTimer <= 0)
    {
        attackTimer = attackCooldown;
        return true;
    }
    return false;
}

void Player::takeDamage(float amount, const AttackType &attack)
{
    // Calculate damage with defense reduction
    float defenseValue = skillTree.getSkillValue(DEFENSE);
    float damageReduction = defenseValue / (defenseValue + 100.0f);
    amount *= (1.0f - damageReduction);

    // Apply damage to health
    health -= amount;
    if (health < 0)
        health = 0;

    // Save current state for proper OpenGL state management
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
    
    // Enable color material for the effect visualization
    glEnable(GL_COLOR_MATERIAL);
    
    // Create effect based on attack type
    switch (attack)
    {
    case AttackType::PHYSICAL:
        std::cout << "Displaying physical effect: impact!" << std::endl;
        glColor3f(1.0f, 0.0f, 0.0f);
        glPushMatrix();
        glTranslatef(x, y, z); // Position at player location
        glutSolidSphere(0.4f, 16, 16);   // Red sphere (physical impact)
        glPopMatrix();
        break;

    case AttackType::FIRE:
        std::cout << "Displaying fire effect: flame burst!" << std::endl;
        glColor3f(1.0f, 0.5f, 0.0f);
        glPushMatrix();
        glTranslatef(x, y, z); // Position at player location
        glutSolidSphere(0.3f, 16, 16); // Orange sphere (fire)
        glPopMatrix();
        break;

    case AttackType::ICE:
        std::cout << "Displaying ice effect: freezing!" << std::endl;
        glColor3f(0.0f, 0.7f, 1.0f);
        glPushMatrix();
        glTranslatef(x, y, z); // Position at player location
        glutSolidSphere(0.4f, 16, 16); // Blue sphere (ice)
        glPopMatrix();
        break;

    case AttackType::POISON:
        std::cout << "Displaying poison effect: toxic pools!" << std::endl;
        glColor3f(0.0f, 1.0f, 0.0f);
        glPushMatrix();
        glTranslatef(x, y, z); // Position at player location
        glutSolidSphere(0.35f, 16, 16); // Green sphere (poison)
        glPopMatrix();
        break;

    case AttackType::MAGIC:
        std::cout << "Displaying magic effect: magic glow!" << std::endl;
        glColor3f(0.5f, 0.0f, 0.5f);
        glPushMatrix();
        glTranslatef(x, y, z); // Position at player location
        glutSolidSphere(0.45f, 16, 16); // Purple sphere (magic)
        glPopMatrix();
        break;

    default:
        std::cout << "Unknown effect type!" << std::endl;
    }
    
    // Restore previous OpenGL state
    glPopAttrib();
}

void Player::heal(float amount)
{
    health += amount;
    if (health > maxHealth)
        health = maxHealth;
}

void Player::addExperience(int xp)
{
    experience += xp;
    while (experience >= experienceToNextLevel)
    {
        levelUp();
    }
}

void Player::levelUp()
{
    level++;
    experience -= experienceToNextLevel;
    experienceToNextLevel = level * 100;
    maxHealth += 10.0f;
    health = maxHealth;
    skillTree.addSkillPoint();

    std::cout << "Level increased to " << level << "! Gained a skill point." << std::endl;
}

// Getters
float Player::getRotY() const { return rotY; }
float Player::getHealth() const { return health; }
float Player::getMaxHealth() const { return maxHealth; }
int Player::getLevel() const { return level; }
int Player::getExperience() const { return experience; }
int Player::getExperienceToNextLevel() const { return experienceToNextLevel; }
float Player::getAttackCooldown() const { return attackCooldown; }
float Player::getAttackTimer() const { return attackTimer; }

float Player::getAttackDamage() const
{
    float baseAttack = 30.0f;
    float attackBonus = skillTree.getSkillValue(ATTACK);
    return baseAttack + attackBonus;
}

float Player::getMovementSpeed() const
{
    float baseSpeed = MOVEMENT_SPEED;
    float speedBonus = skillTree.getSkillValue(SPEED) * 0.01f;
    return baseSpeed * (1.0f + speedBonus);
}

SkillTree &Player::getSkillTree()
{
    return skillTree;
}

void Player::setSpeed(float movementSpeed)
{
    this->MOVEMENT_SPEED = movementSpeed;
}

void Player::reset()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    rotY = 0.0f;

    health = maxHealth = 100.0f;
    level = 1;
    experience = 0;
    experienceToNextLevel = 100;

    attackCooldown = 1.0f;
    attackTimer = 0.0f;

    upPressed = downPressed = leftPressed = rightPressed = false;
    wPressed = sPressed = aPressed = dPressed = false;

    upAndNotDown = downAndNotW = leftAndNotRight = rightAndNotLeft = false;
    wAndNotS = sAndNotW = aAndNotD = dAndNotA = false;

    skillTree.reset(); // Se o skillTree tiver uma função de reset

    std::cout << "Player resetado." << std::endl;
}
