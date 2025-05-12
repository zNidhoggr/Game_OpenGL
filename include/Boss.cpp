#include "Boss.hpp"
#include <iostream>
#include <cmath>
#include <GL/glut.h>

Boss::Boss(float x, float y, float z, float size, int level)
    : GameObject(x, y, z, size, ObjectType::BOSS),
      health(4000.0f * level), maxHealth(4000.0f * level),
      level(level), moveSpeed(0.025f),
      attackDamage(30.0f * level), attackRange(1.0f), attackCooldown(1.0f),
      attackTimer(0.0f), specialAttackCooldown(8.0f), specialAttackTimer(0.0f),
      enraged(false), enragedThreshold(0.3f),
      detectionRange(30.0f), isCombatActive(false),
      experienceGiven(false), experienceValue(100.0f * level),
      // Initialize animation variables here instead of in class declaration
      timer(0.0f), toggle(false), toggleCount(0), toggleInterval(1.60f),
      pauseTimer(0.0f), paused(false), level_two(0)
{
}

void Boss::update(float deltaTime)
{
    // Return early if not active
    if (!active)
        return;

    // Handle the toggle/timing system
    if (paused)
    {
        pauseTimer += deltaTime;
        if (pauseTimer >= 14.0f)
        {
            paused = false;
            pauseTimer = 0.0f;
            toggleCount = 0;
            if (level_two == 0)
            {
                level_two = 1;
                toggleInterval = 2.6f; // New rhythm for level 2
            }
            else if (level_two == 1)
            {
                level_two = 0;
                toggleInterval = 1.60f; // Return to original rhythm after second pause
            }
        }
    }
    else
    {
        timer += deltaTime;
        if (timer >= toggleInterval)
        {
            toggle = !toggle;
            timer = 0.0f;
            toggleCount++;

            // Level 1 logic
            if (level_two == 0)
            {
                if (toggleCount == 21)
                {
                    toggleInterval = 25.0f;
                }
                else if (toggleCount == 22)
                {
                    paused = true;
                    toggleCount = 0;
                }
            }
            // Level 2 logic
            else if (level_two == 1)
            {
                if (toggleCount == 2)
                {
                    toggleInterval = 5.0f;
                }
                else if (toggleCount == 25)
                {
                    paused = true;
                    toggleCount = 0;
                }
            }
        }
    }

    // Update attack timers
    if (attackTimer > 0)
        attackTimer -= deltaTime;
    if (attackTimer < 0)
        attackTimer = 0;

    if (specialAttackTimer > 0)
        specialAttackTimer -= deltaTime;
    if (specialAttackTimer < 0)
        specialAttackTimer = 0;

    // Check for enrage condition
    if (!enraged && health / maxHealth <= enragedThreshold)
    {
        enraged = true;
        moveSpeed *= 1.5f;
        attackDamage *= 1.2f;
    }
}

void Boss::moveTowardsPlayer(const Player &player, float deltaTime, MeshLoader &loader)
{
    if (!active)
        return;

    float dx = player.getX() - x;
    float dz = player.getZ() - z;
    float dist = std::sqrt(dx * dx + dz * dz);

    isCombatActive = (dist < detectionRange);

    if (isCombatActive && dist > attackRange)
    {
        x += (dx / dist) * moveSpeed * deltaTime * 60.0f * (1.0f + (2.5f * toggle));
        z += (dz / dist) * moveSpeed * deltaTime * 60.0f * (1.0f + (2.5f * toggle));
        y = this->getTerrainHeight(x, z) + 0.5f; // Bosses are a bit higher

        // Update the model's position in the loader
        loader.updateModelTranslationXById(1, x);
        loader.updateModelTranslationYById(1, y);
        loader.updateModelTranslationZById(1, z);
    }

    // Update model rotation based on toggle state
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float speed = toggle ? 1000.0f : 100.0f;
    loader.updateModelRotationAngleById(1, currentTime * speed);
    loader.updateModelRotationYById(1, 1.0f);
}

bool Boss::attackPlayer(Player &player, float deltaTime)
{
    if (!active)
        return false;

    float dx = player.getX() - (x);
    float dz = player.getZ() - (z);
    float dist = std::sqrt(dx * dx + dz * dz);

    // Perform basic attack if in range and cooldown is complete
    if (dist <= attackRange && attackTimer <= 0)
    {
        player.takeDamage(toggle ? attackDamage * 1.75f : attackDamage, AttackType::PHYSICAL);
        attackTimer = attackCooldown;
        return true;
    }

    return false;
}

bool Boss::specialAttack(Player &player)
{
    if (!active)
        return false;

    float dx = player.getX() - x;
    float dz = player.getZ() - z;
    float dist = std::sqrt(dx * dx + dz * dz);

    // Perform special attack if in extended range and cooldown is complete
    if (dist <= attackRange * 1.5f && specialAttackTimer <= 0)
    {
        specialAttackTimer = specialAttackCooldown;
        player.takeDamage(attackDamage * 2.0f, AttackType::MAGIC);
        return true;
    }

    return false;
}

void Boss::takeDamage(float amount, const AttackType &attack)
{
    health -= amount;
    if (health <= 0)
    {
        health = 0;
        active = false;
    }

    // Save current OpenGL attributes
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);

    // Enable color material for the effect visualization
    glEnable(GL_COLOR_MATERIAL);

    // Create visual effect based on attack type
    switch (attack)
    {
    case AttackType::PHYSICAL:
        std::cout << "Boss hit by physical attack!" << std::endl;
        glColor3f(1.0f, 0.0f, 0.0f);
        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidSphere(0.6f, 16, 16); // Larger red sphere for boss
        glPopMatrix();
        break;

    case AttackType::FIRE:
        std::cout << "Boss hit by fire attack!" << std::endl;
        glColor3f(1.0f, 0.5f, 0.0f);
        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidSphere(0.5f, 16, 16);
        glPopMatrix();
        break;

    case AttackType::ICE:
        std::cout << "Boss hit by ice attack!" << std::endl;
        glColor3f(0.0f, 0.7f, 1.0f);
        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidSphere(0.6f, 16, 16);
        glPopMatrix();
        break;

    case AttackType::POISON:
        std::cout << "Boss hit by poison attack!" << std::endl;
        glColor3f(0.0f, 1.0f, 0.0f);
        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidSphere(0.55f, 16, 16);
        glPopMatrix();
        break;

    case AttackType::MAGIC:
        std::cout << "Boss hit by magic attack!" << std::endl;
        glColor3f(0.5f, 0.0f, 0.5f);
        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidSphere(0.65f, 16, 16);
        glPopMatrix();
        break;

    default:
        std::cout << "Boss hit by unknown attack type!" << std::endl;
    }

    // Restore previous OpenGL state
    glPopAttrib();
}

void Boss::drawForLoader(MeshLoader &loader)
{
    if (!active) return;
    loader.drawForId(1);
    drawHealthBar();
}

void Boss::draw()
{
}

void Boss::drawHealthBar()
{
    // Use window dimensions
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // Switch to orthographic projection for screen-space rendering
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowWidth, 0, windowHeight, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    float healthPercent = health / maxHealth;

    // Bar dimensions constrained to window size
    float barWidth = std::max(100.0f, std::min(windowWidth * 0.5f, windowWidth * 0.5f * healthPercent));
    float barHeight = std::max(10.0f, std::min(windowHeight * 0.05f, windowHeight * 0.05f));

    float barPosX = windowWidth * 0.5f - barWidth * 0.5f;
    float barPosY = windowHeight * 0.9f;

    // Background bar
    glColor3f(0.2f, 0.2f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(barPosX, barPosY);
    glVertex2f(barPosX + barWidth, barPosY);
    glVertex2f(barPosX + barWidth, barPosY + barHeight);
    glVertex2f(barPosX, barPosY + barHeight);
    glEnd();

    // Health bar
    if (enraged){
        glColor3f(0.0f, 0.3f, 1.0f);
    }
    else{
        glColor3f(0.0f, 0.4f + (0.6f * healthPercent), 0.6f + (0.4f * healthPercent));
    }

    glBegin(GL_QUADS);
    glVertex2f(barPosX, barPosY);
    glVertex2f(barPosX + barWidth * healthPercent, barPosY);
    glVertex2f(barPosX + barWidth * healthPercent, barPosY + barHeight);
    glVertex2f(barPosX, barPosY + barHeight);
    glEnd();

    // Draw boss name
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(barPosX + barWidth * 0.5f - 50, barPosY + barHeight + 20);
    const char *bossName = "OIIA OIIA EN";
    for (const char *c = bossName; *c != '\0'; ++c)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

float Boss::getTerrainHeight(float x, float z)
{
    float height = 0.0f;

    // Base terrain height calculation with sine and cosine waves
    height += std::sin(x * 0.1f) * 0.5f;
    height += std::cos(z * 0.1f) * 0.5f;
    height += (std::sin(x * 0.3f + z * 0.5f) * 0.3f);

    // Lake centers defined as depressions in the terrain
    std::vector<std::pair<float, float>> lakeCenters = {
        {5.0f, 5.0f},
        {-7.0f, -3.0f},
        {8.0f, -6.0f},
        {-4.0f, 7.0f}};

    // Apply lake depressions
    for (const auto &center : lakeCenters)
    {
        float dist = std::sqrt((x - center.first) * (x - center.first) +
                               (z - center.second) * (z - center.second));
        if (dist < 3.5f)
        {
            height -= (3.5f - dist) * 0.4f;
        }
    }

    return height;
}

bool Boss::isExperienceGiven() const
{
    return experienceGiven;
}

void Boss::markExperienceAsGiven()
{
    experienceGiven = true;
}

// Getters implementation
float Boss::getHealth() const { return health; }
float Boss::getMaxHealth() const { return maxHealth; }
int Boss::getLevel() const { return level; }
bool Boss::isInCombat() const { return isCombatActive; }
float Boss::getExperienceValue() const { return experienceValue; }
bool Boss::isEnraged() const { return enraged; }