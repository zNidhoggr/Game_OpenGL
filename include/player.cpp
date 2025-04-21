#include "player.hpp"

Player::Player(float x, float y, float z)
    : GameObject(x, y, z, 0.3f, ObjectType::NPC),
      rotY(0.0f), health(100.0f), maxHealth(100.0f),
      level(1), experience(0), experienceToNextLevel(100),
      attackCooldown(1.0f), attackTimer(0.0f) {}

void Player::update(float deltaTime) {
    if (attackTimer > 0) {
        attackTimer -= deltaTime;
        if (attackTimer < 0) attackTimer = 0;
    }
}

void Player::draw() {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    GLfloat ambient[]  = {0.0f, 0.0f, 0.3f, 1.0f};
    GLfloat diffuse[]  = {0.1f, 0.5f, 0.8f, 0.8f};
    GLfloat specular[] = {0.9f, 0.4f, 0.4f, 1.0f};
    GLfloat shininess  = 64.0f;

    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glutSolidCube(0.6f);

    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glutSolidSphere(0.2f, 10, 10);
    glPopMatrix();

    glPopMatrix();
}

void Player::moveForward() {
    x += MOVEMENT_SPEED * std::sin(rotY * M_PI / 180.0f);
    z += MOVEMENT_SPEED * std::cos(rotY * M_PI / 180.0f);
}

void Player::moveBackward() {
    x -= MOVEMENT_SPEED * std::sin(rotY * M_PI / 180.0f);
    z -= MOVEMENT_SPEED * std::cos(rotY * M_PI / 180.0f);
}

void Player::strafeLeft() {
    x -= MOVEMENT_SPEED * std::cos(rotY * M_PI / 180.0f);
    z += MOVEMENT_SPEED * std::sin(rotY * M_PI / 180.0f);
}

void Player::strafeRight() {
    x += MOVEMENT_SPEED * std::cos(rotY * M_PI / 180.0f);
    z -= MOVEMENT_SPEED * std::sin(rotY * M_PI / 180.0f);
}

void Player::rotateRight(float delta) {
    rotY += delta;
}

void Player::rotateLeft(float delta) {
    rotY -= delta;
}

bool Player::attack() {
    if (attackTimer <= 0) {
        attackTimer = attackCooldown;
        return true;
    }
    return false;
}

void Player::takeDamage(float amount) {
    float defenseValue = skillTree.getSkillValue(DEFENSE);
    float damageReduction = defenseValue / (defenseValue + 100.0f);
    amount *= (1.0f - damageReduction);

    health -= amount;
    if (health < 0) health = 0;
}

void Player::heal(float amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

void Player::addExperience(int xp) {
    experience += xp;
    while (experience >= experienceToNextLevel) {
        levelUp();
    }
}

void Player::levelUp() {
    level++;
    experience -= experienceToNextLevel;
    experienceToNextLevel = level * 100;
    maxHealth += 10.0f;
    health = maxHealth;
    skillTree.addSkillPoint();

    std::cout << "Nível aumentado para " << level << "! Ganhou um ponto de habilidade." << std::endl;
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

float Player::getAttackDamage() const {
    float baseAttack = 40.0f;
    float attackBonus = skillTree.getSkillValue(ATTACK);
    return baseAttack + attackBonus;
}

float Player::getMovementSpeed() const {
    float baseSpeed = MOVEMENT_SPEED;
    float speedBonus = skillTree.getSkillValue(SPEED) * 0.01f;
    return baseSpeed * (1.0f + speedBonus);
}

SkillTree& Player::getSkillTree() {
    return skillTree;
}
