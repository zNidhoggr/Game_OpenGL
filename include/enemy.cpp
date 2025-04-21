#include "Enemy.hpp"

Enemy::Enemy(float x, float y, float z, float size, int level)
    : GameObject(x, y, z, size, ObjectType::ENEMY),
      health(50.0f * level), maxHealth(50.0f * level),
      level(level), moveSpeed(0.05f), attackDamage(5.0f * level),
      attackRange(1.0f), attackCooldown(2.0f), attackTimer(0.0f),
      detectionRange(5.0f), isCombatActive(false) {}

void Enemy::update(float deltaTime) {
    if (!active) return;

    if (attackTimer > 0) {
        attackTimer -= deltaTime;
        if (attackTimer < 0) attackTimer = 0;
    }
}

void Enemy::moveTowardsPlayer(const Player& player, float deltaTime) {
    if (!active) return;

    float dx = player.getX() - x;
    float dz = player.getZ() - z;
    float dist = std::sqrt(dx * dx + dz * dz);

    isCombatActive = (dist < detectionRange);

    if (isCombatActive && dist > attackRange) {
        x += (dx / dist) * moveSpeed * deltaTime * 60.0f;
        z += (dz / dist) * moveSpeed * deltaTime * 60.0f;
        y = this->getTerrainHeight(x, z) + 0.3f;
    }
}

bool Enemy::attackPlayer(Player& player, float deltaTime) {
    if (!active) return false;

    float dx = player.getX() - x;
    float dz = player.getZ() - z;
    float dist = std::sqrt(dx * dx + dz * dz);

    if (dist <= attackRange && attackTimer <= 0) {
        player.takeDamage(attackDamage);
        attackTimer = attackCooldown;
        return true;
    }

    return false;
}

void Enemy::takeDamage(float amount) {
    health -= amount;
    if (health <= 0) {
        health = 0;
        active = false;
    }
}

void Enemy::draw() {
    if (!active) return;

    glPushMatrix();
    glTranslatef(x, y, z);

    GLfloat ambient[]  = {0.3f, 0.0f, 0.0f, 1.0f};
    GLfloat diffuse[]  = {0.8f, 0.1f, 0.1f, 1.0f};
    GLfloat specular[] = {0.9f, 0.4f, 0.4f, 1.0f};
    GLfloat shininess  = 64.0f;

    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glPushMatrix();
    glScalef(size, size, size);
    glutSolidCube(0.8f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, size * 0.6f, 0.0f);
    glutSolidSphere(size * 0.3f, 8, 8);
    glPopMatrix();

    drawHealthBar();

    glPopMatrix();
}

void Enemy::drawHealthBar() {
    glDisable(GL_LIGHTING);

    float barWidth = size * 1.5f;
    float barHeight = size * 0.2f;
    float barPosY = size * 1.2f;

    float healthPercent = health / maxHealth;

    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex3f(-barWidth/2, barPosY, 0);
    glVertex3f(barWidth/2, barPosY, 0);
    glVertex3f(barWidth/2, barPosY + barHeight, 0);
    glVertex3f(-barWidth/2, barPosY + barHeight, 0);
    glEnd();

    glColor3f(1.0f - healthPercent, healthPercent, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(-barWidth/2, barPosY, 0);
    glVertex3f(-barWidth/2 + barWidth * healthPercent, barPosY, 0);
    glVertex3f(-barWidth/2 + barWidth * healthPercent, barPosY + barHeight, 0);
    glVertex3f(-barWidth/2, barPosY + barHeight, 0);
    glEnd();

    glEnable(GL_LIGHTING);
}

float Enemy::getTerrainHeight(float x, float z)
{
    float height = 0.0f;
    height += std::sin(x * 0.1f) * 0.5f;
    height += std::cos(z * 0.1f) * 0.5f;

    height += (std::sin(x * 0.3f + z * 0.5f) * 0.3f);

    std::vector<std::pair<float, float>> lakeCenters = {
        {5.0f, 5.0f},
        {-7.0f, -3.0f},
        {8.0f, -6.0f},
        {-4.0f, 7.0f}};

    for (const auto &center : lakeCenters)
    {
        float dist = std::sqrt((x - center.first) * (x - center.first) + (z - center.second) * (z - center.second));
        if (dist < 3.5f)
        {
            height -= (3.5f - dist) * 0.4f;
        }
    }

    return height;
}

// Getters
float Enemy::getHealth() const { return health; }
float Enemy::getMaxHealth() const { return maxHealth; }
int Enemy::getLevel() const { return level; }
bool Enemy::isInCombat() const { return isCombatActive; }
float Enemy::getExperienceValue() const { return level * 20.0f; }
