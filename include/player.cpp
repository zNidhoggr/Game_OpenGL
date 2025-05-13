#include "player.hpp"
#include <GL/glut.h>
#include <GL/gl.h>
#include <cmath>
#include <iostream>

extern unsigned int texturaJogador;
extern unsigned int texturaJogadorCabeca;

Player::Player(float x, float y, float z)
    : GameObject(x, y, z, 0.3f, ObjectType::NPC),
      rotY(0.0f), health(150.0f), maxHealth(150.0f),
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
    if (attackTimer > 0){
        attackTimer -= deltaTime;
        if (attackTimer < 0)  attackTimer = 0;
    }

    wAndNotS = wPressed && !sPressed;
    sAndNotW = sPressed && !wPressed;
    aAndNotD = aPressed && !dPressed;
    dAndNotA = dPressed && !aPressed;

    handleMovement();
}

void Player::handleMovement(){
    float actualSpeed = getMovementSpeed();
    
    if (wAndNotS)moveForward();
    else if (sAndNotW) moveBackward();

    if (aAndNotD) strafeLeft();
    else if (dAndNotA) strafeRight();
}

void Player::draw()
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBindTexture(GL_TEXTURE_2D, texturaJogador);

    GLfloat bodyAmbient[] = {0.4f, 0.4f, 0.4f, 1.0f};
    GLfloat bodyDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat bodySpecular[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat bodyShininess = 10.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, bodyAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, bodyDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, bodySpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, bodyShininess);

    float size = 0.3f;

    glBegin(GL_QUADS);
    
    glNormal3f(0.0f, 0.0f, 1.0f); 
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size,  size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( size, -size,  size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( size,  size,  size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size,  size,  size);

    glNormal3f(0.0f, 0.0f, -1.0f); 
    glTexCoord2f(0.0f, 1.0f); glVertex3f( size, -size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, -size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-size,  size, -size);
    glTexCoord2f(0.0f, 0.0f); glVertex3f( size,  size, -size);

    glNormal3f(1.0f, 0.0f, 0.0f); 
    glTexCoord2f(0.0f, 0.0f); glVertex3f( size, -size,  size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( size, -size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( size,  size, -size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( size,  size,  size);

    glNormal3f(-1.0f, 0.0f, 0.0f);  
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, -size,  size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-size,  size,  size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size,  size, -size);
    
    glNormal3f(0.0f, 1.0f, 0.0f);  
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size,  size,  size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( size,  size,  size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( size,  size, -size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size,  size, -size);

    glNormal3f(0.0f, -1.0f, 0.0f); 
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( size, -size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( size, -size,  size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, -size,  size);
    
    glEnd();

    glBindTexture(GL_TEXTURE_2D, texturaJogadorCabeca);

    GLfloat headAmbient[] = {0.3f, 0.3f, 0.4f, 1.0f};
    GLfloat headDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat headSpecular[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat headShininess = 30.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, headAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, headDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, headSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, headShininess);

    glPushMatrix();
    glTranslatef(0.0f, size + 0.2f, 0.0f); 
    glRotatef(115.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(75.0f, 0.0f, 1.0f, 0.0f);
    
    GLUquadric* headQuad = gluNewQuadric();
    gluQuadricTexture(headQuad, GL_TRUE);
    gluQuadricNormals(headQuad, GLU_SMOOTH);
    gluSphere(headQuad, 0.3f, 24, 24);
    gluDeleteQuadric(headQuad);
    
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void Player::moveForward(){
    float radians = rotY * M_PI / 180.0f;
    x += (getMovementSpeed() + (running? 0.05f: 0.0f)) * std::sin(radians);
    z += (getMovementSpeed() + (running? 0.05f: 0.0f))  * std::cos(radians);
}

void Player::moveBackward(){
    float radians = rotY * M_PI / 180.0f;
    x -= (getMovementSpeed() + (running? 0.05f: 0.0f)) * std::sin(radians);
    z -= (getMovementSpeed() + (running? 0.05f: 0.0f))  * std::cos(radians);
}

void Player::strafeLeft(){
    float radians = rotY * M_PI / 180.0f;
    x -= (getMovementSpeed() + (running? 0.05f: 0.0f)) * std::cos(radians);
    z += (getMovementSpeed() + (running? 0.05f: 0.0f)) * std::sin(radians);
}

void Player::strafeRight(){
    float radians = rotY * M_PI / 180.0f;
    x += (getMovementSpeed() + (running? 0.05f: 0.0f)) * std::cos(radians);
    z -= (getMovementSpeed() + (running? 0.05f: 0.0f)) * std::sin(radians);
}

void Player::rotateRight(float delta){ rotY += delta; }
void Player::rotateLeft(float delta) { rotY -= delta; }
bool Player::attack(){
    if (attackTimer <= 0){
        attackTimer = attackCooldown;
        return true;
    }

    return false;
}

void Player::takeDamage(float amount, const AttackType &attack){
    float defenseValue = skillTree.getSkillValue(DEFENSE);
    float damageReduction = defenseValue / (defenseValue + 100.0f);
    amount *= (1.0f - damageReduction);

    health -= amount;
    if (health < 0) health = 0;

    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
    
    glEnable(GL_COLOR_MATERIAL);
    
    switch (attack){
    case AttackType::PHYSICAL:
        std::cout << "Displaying physical effect: impact!" << std::endl;
        glColor3f(1.0f, 0.0f, 0.0f);
        glPushMatrix();
        glTranslatef(x, y, z); 
        glutSolidSphere(0.4f, 16, 16);  
        glPopMatrix();
        break;

    case AttackType::FIRE:
        std::cout << "Displaying fire effect: flame burst!" << std::endl;
        glColor3f(1.0f, 0.5f, 0.0f);
        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidSphere(0.3f, 16, 16); 
        glPopMatrix();
        break;

    case AttackType::ICE:
        std::cout << "Displaying ice effect: freezing!" << std::endl;
        glColor3f(0.0f, 0.7f, 1.0f);
        glPushMatrix();
        glTranslatef(x, y, z);
        glutSolidSphere(0.4f, 16, 16); 
        glPopMatrix();
        break;

    case AttackType::POISON:
        std::cout << "Displaying poison effect: toxic pools!" << std::endl;
        glColor3f(0.0f, 1.0f, 0.0f);
        glPushMatrix();
        glTranslatef(x, y, z); 
        glutSolidSphere(0.35f, 16, 16); 
        glPopMatrix();
        break;

    case AttackType::MAGIC:
        std::cout << "Displaying magic effect: magic glow!" << std::endl;
        glColor3f(0.5f, 0.0f, 0.5f);
        glPushMatrix();
        glTranslatef(x, y, z); 
        glutSolidSphere(0.45f, 16, 16);
        glPopMatrix();
        break;

    default:
        std::cout << "Unknown effect type!" << std::endl;
    }
    glPopAttrib();
}

void Player::heal(float amount)
{
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

void Player::addExperience(int xp)
{
    experience += xp;
    while (experience >= experienceToNextLevel) { levelUp(); }
}

void Player::levelUp(){
    level++;
    experience -= experienceToNextLevel;
    experienceToNextLevel = level * 100;
    maxHealth += 10.0f;
    health = maxHealth;
    skillTree.addSkillPoint();

    std::cout << "Level increased to " << level << "! Gained a skill point." << std::endl;
}

float Player::getRotY() const { return rotY; }
float Player::getHealth() const { return health; }
float Player::getMaxHealth() const { return maxHealth; }
int Player::getLevel() const { return level; }
int Player::getExperience() const { return experience; }
int Player::getExperienceToNextLevel() const { return experienceToNextLevel; }
float Player::getAttackCooldown() const { return attackCooldown; }
float Player::getAttackTimer() const { return attackTimer; }
float Player::getAttackDamage() const{
    float baseAttack = 30.0f;
    float attackBonus = skillTree.getSkillValue(ATTACK);
    return baseAttack + attackBonus;
}
float Player::getAttackDamageMagic() const{
    float baseAttack = 30.0f;
    float attackBonus = skillTree.getSkillValue(SkillType::MAGIC);
    return baseAttack + attackBonus;
}

float Player::getMovementSpeed() const{
    float baseSpeed = MOVEMENT_SPEED;
    float speedBonus = skillTree.getSkillValue(SPEED) * 0.01f;
    return baseSpeed * (1.0f + speedBonus);
}

SkillTree &Player::getSkillTree(){
    return skillTree;
}

void Player::setSpeed(float movementSpeed){
    this->MOVEMENT_SPEED = movementSpeed;
}

void Player::reset(){
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

    skillTree.reset(); 
}
