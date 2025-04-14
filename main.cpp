#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>
#include <map>
#include <memory>
#include <algorithm>

//global functions
float getTerrainHeight(float x, float z) {
        float height = 0.0f;

        // Colinas suaves
        height += std::sin(x * 0.1f) * 0.5f;
        height += std::cos(z * 0.1f) * 0.5f;

        // Ruído sutil
        height += (std::sin(x * 0.3f + z * 0.5f) * 0.3f);

        // Depressão simulando lago
        float distToLakeCenter = std::sqrt((x - 5)*(x - 5) + (z - 5)*(z - 5));
        if (distToLakeCenter < 4.0f) {
            height -= (4.0f - distToLakeCenter) * 0.4f;
        }

        return height;
    }

// Forward declarations
class GameObject;
class SkillTree;

// Enumeração para tipos de objeto
enum ObjectType {
    TREE,
    ROCK,
    HOUSE,
    ENEMY,
    ITEM,
    NPC
};

// Enumeração para tipos de habilidade
enum SkillType {
    ATTACK,
    DEFENSE,
    MAGIC,
    SPEED
};

struct GrassPatch {
    float x, z;     // centro
    float radius;   // raio
};

struct TrailPoint {
    float x, y, z;
};

//Variavéis Globais
std::vector<TrailPoint> trailCurvePoints;
std::vector<TrailPoint> trailClearings;
std::vector<TrailPoint> trailPoints;
std::vector<GrassPatch> grassPatches;


// Classe para gerenciar habilidades
class Skill {
private:
    std::string name;
    SkillType type;
    int level;
    int maxLevel;
    float value;      // valor do efeito da habilidade
    std::vector<Skill*> prerequisites;  // Habilidades necessárias para desbloquear esta

public:
    Skill(const std::string& name, SkillType type, int maxLevel, float baseValue)
        : name(name), type(type), level(0), maxLevel(maxLevel), value(baseValue) {}

    void addPrerequisite(Skill* skill) {
        prerequisites.push_back(skill);
    }

    bool canLearn() const {
        if (level >= maxLevel) return false;

        for (const auto& prereq : prerequisites) {
            if (prereq->getLevel() == 0) return false;
        }
        return true;
    }

    bool upgrade() {
        if (!canLearn()) return false;

        if (level < maxLevel) {
            level++;
            value *= 1.2f;  // Aumento de 20% por nível
            return true;
        }
        return false;
    }

    // Getters
    const std::string& getName() const { return name; }
    SkillType getType() const { return type; }
    int getLevel() const { return level; }
    int getMaxLevel() const { return maxLevel; }
    float getValue() const { return value; }
};

// Classe para árvore de habilidades
class SkillTree {
private:
    std::vector<std::unique_ptr<Skill>> skills;
    int skillPoints;

public:
    SkillTree() : skillPoints(0) {
        // Criação de habilidades básicas
        auto attackSkill = std::make_unique<Skill>("Força de Ataque", ATTACK, 5, 5.0f);
        auto defenseSkill = std::make_unique<Skill>("Resistência", DEFENSE, 5, 3.0f);
        auto magicSkill = std::make_unique<Skill>("Poder Mágico", MAGIC, 5, 4.0f);
        auto speedSkill = std::make_unique<Skill>("Agilidade", SPEED, 5, 2.0f);

        // Habilidades avançadas
        auto criticalStrike = std::make_unique<Skill>("Golpe Crítico", ATTACK, 3, 10.0f);
        auto healthRegen = std::make_unique<Skill>("Regeneração", DEFENSE, 3, 2.0f);
        auto fireball = std::make_unique<Skill>("Bola de Fogo", MAGIC, 3, 15.0f);
        auto dodge = std::make_unique<Skill>("Esquiva", SPEED, 3, 5.0f);

        // Configuração de pré-requisitos
        criticalStrike->addPrerequisite(attackSkill.get());
        healthRegen->addPrerequisite(defenseSkill.get());
        fireball->addPrerequisite(magicSkill.get());
        dodge->addPrerequisite(speedSkill.get());

        // Adicionar habilidades à árvore
        skills.push_back(std::move(attackSkill));
        skills.push_back(std::move(defenseSkill));
        skills.push_back(std::move(magicSkill));
        skills.push_back(std::move(speedSkill));
        skills.push_back(std::move(criticalStrike));
        skills.push_back(std::move(healthRegen));
        skills.push_back(std::move(fireball));
        skills.push_back(std::move(dodge));
    }

    void addSkillPoint() {
        skillPoints++;
    }

    bool useSkillPoint(const std::string& skillName) {
        if (skillPoints <= 0) return false;

        for (auto& skill : skills) {
            if (skill->getName() == skillName && skill->canLearn()) {
                if (skill->upgrade()) {
                    skillPoints--;
                    return true;
                }
            }
        }
        return false;
    }

    int getSkillPoints() const {
        return skillPoints;
    }

    const std::vector<std::unique_ptr<Skill>>& getSkills() const {
        return skills;
    }

    float getSkillValue(SkillType type) const {
        float total = 0.0f;
        for (const auto& skill : skills) {
            if (skill->getType() == type) {
                total += skill->getValue() * skill->getLevel();
            }
        }
        return total;
    }
};

// Classe base para todos os objetos do jogo
class GameObject {
protected:
    float x, y, z;       // Posição
    float size;          // Tamanho
    ObjectType type;     // Tipo de objeto
    bool active;         // Se está ativo no mundo

public:
    GameObject(float x, float y, float z, float size, ObjectType type)
        : x(x), y(y), z(z), size(size), type(type), active(true) {}

    virtual ~GameObject() {}

    virtual void update(float deltaTime) {}
    virtual void draw() = 0;

    bool checkCollision(const GameObject& other) const {
        float dx = x - other.x;
        float dz = z - other.z;
        float distance = std::sqrt(dx*dx + dz*dz);

        return distance < (size + other.size);
    }

    // Getters e setters
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }
    float getSize() const { return size; }
    ObjectType getType() const { return type; }
    bool isActive() const { return active; }

    void setPosition(float newX, float newY, float newZ) {
        x = newX;
        y = newY;
        z = newZ;
    }

    void setActive(bool state) { active = state; }
};

// Classe para o jogador
class Player : public GameObject {
private:
    float rotY;          // Rotação em Y (direção que está olhando)
    float health;        // Vida do personagem
    float maxHealth;     // Vida máxima
    int level;           // Nível do personagem
    int experience;      // Experiência do personagem
    int experienceToNextLevel;  // XP necessário para o próximo nível
    SkillTree skillTree; // Árvore de habilidades
    float attackCooldown; // Tempo de espera entre ataques
    float attackTimer;    // Timer atual para atacar novamente

    // Constantes
    static constexpr float MOVEMENT_SPEED = 0.1f;
    static constexpr float ROTATION_SPEED = 5.0f;

public:
    Player(float x, float y, float z)
        : GameObject(x, y, z, 0.3f, ObjectType::NPC),
          rotY(0.0f), health(100.0f), maxHealth(100.0f),
          level(1), experience(0), experienceToNextLevel(100),
          attackCooldown(1.0f), attackTimer(0.0f) {}

    void update(float deltaTime) override {
        // Atualizar timer de ataque
        if (attackTimer > 0) {
            attackTimer -= deltaTime;
            if (attackTimer < 0) attackTimer = 0;
        }
    }

    void draw() override {
        glPushMatrix();

        // Posicionar o jogador
        glTranslatef(x, y, z);
        glRotatef(rotY, 0.0f, 1.0f, 0.0f);

        // Definir cor do jogador
        GLfloat ambient[]  = {0.0f, 0.0f, 0.3f, 1.0f};   // sombra azul
        GLfloat diffuse[]  = {0.1f, 0.5f, 0.8f, 0.8f};   //
        GLfloat specular[] = {0.9f, 0.4f, 0.4f, 1.0f};   // reflexo rosado
        GLfloat shininess  = 64.0f;

        glDisable(GL_COLOR_MATERIAL);
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);

        // Corpo do jogador
        glutSolidCube(0.6f);

        // Cabeça
        glPushMatrix();
        glTranslatef(0.0f, 0.5f, 0.0f);
        glutSolidSphere(0.2f, 10, 10);
        glPopMatrix();

        glPopMatrix();
    }

    void moveForward() {
        x += MOVEMENT_SPEED * std::sin(rotY * M_PI / 180.0f);
        z += MOVEMENT_SPEED * std::cos(rotY * M_PI / 180.0f);
    }

    void moveBackward() {
        x -= MOVEMENT_SPEED * std::sin(rotY * M_PI / 180.0f);
        z -= MOVEMENT_SPEED * std::cos(rotY * M_PI / 180.0f);
    }

    void strafeLeft() {
        x -= MOVEMENT_SPEED * std::cos(rotY * M_PI / 180.0f);
        z += MOVEMENT_SPEED * std::sin(rotY * M_PI / 180.0f);
    }

    void strafeRight() {
        x += MOVEMENT_SPEED * std::cos(rotY * M_PI / 180.0f);
        z -= MOVEMENT_SPEED * std::sin(rotY * M_PI / 180.0f);
    }

    void rotateRight(float delta = ROTATION_SPEED) {
        rotY += delta;
    }

    void rotateLeft(float delta = ROTATION_SPEED) {
        rotY -= delta;
    }

    bool attack() {
        if (attackTimer <= 0) {
            attackTimer = attackCooldown;
            return true;
        }
        return false;
    }

    void takeDamage(float amount) {
        // Aplicar redução de dano baseado na habilidade de defesa
        float defenseValue = skillTree.getSkillValue(DEFENSE);
        float damageReduction = defenseValue / (defenseValue + 100.0f);  // Fórmula de redução de dano
        amount *= (1.0f - damageReduction);

        health -= amount;
        if (health < 0) health = 0;
    }

    void heal(float amount) {
        health += amount;
        if (health > maxHealth) health = maxHealth;
    }

    void addExperience(int xp) {
        experience += xp;
        while (experience >= experienceToNextLevel) {
            levelUp();
        }
    }

    void levelUp() {
        level++;
        experience -= experienceToNextLevel;
        experienceToNextLevel = level * 100;  // XP Linear: próximo nível = nível atual * 100
        maxHealth += 10.0f;
        health = maxHealth;  // Recupera vida ao subir de nível
        skillTree.addSkillPoint();  // Ganha um ponto de habilidade

        std::cout << "Nível aumentado para " << level << "! Ganhou um ponto de habilidade." << std::endl;
    }

    // Getters e setters específicos do jogador
    float getRotY() const { return rotY; }
    float getHealth() const { return health; }
    float getMaxHealth() const { return maxHealth; }
    int getLevel() const { return level; }
    int getExperience() const { return experience; }
    int getExperienceToNextLevel() const { return experienceToNextLevel; }
    float getAttackCooldown() const { return attackCooldown; }
    float getAttackTimer() const { return attackTimer; }

    SkillTree& getSkillTree() { return skillTree; }

    float getAttackDamage() const {
        float baseAttack = 40.0f;
        float attackBonus = skillTree.getSkillValue(ATTACK);
        return baseAttack + attackBonus;
    }

    float getMovementSpeed() const {
        float baseSpeed = MOVEMENT_SPEED;
        float speedBonus = skillTree.getSkillValue(SPEED) * 0.01f;
        return baseSpeed * (1.0f + speedBonus);
    }
};

// Classe para inimigos
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
    Enemy(float x, float y, float z, float size, int level)
        : GameObject(x, y, z, size, ObjectType::ENEMY),
          health(50.0f * level), maxHealth(50.0f * level),
          level(level), moveSpeed(0.05f), attackDamage(5.0f * level),
          attackRange(1.0f), attackCooldown(2.0f), attackTimer(0.0f),
          detectionRange(5.0f), isCombatActive(false) {}

    void update(float deltaTime) override {
        if (!active) return;

        // Atualizar timer de ataque
        if (attackTimer > 0) {
            attackTimer -= deltaTime;
            if (attackTimer < 0) attackTimer = 0;
        }
    }

    void moveTowardsPlayer(const Player& player, float deltaTime) {
        if (!active) return;

        float dx = player.getX() - x;
        float dz = player.getZ() - z;
        float dist = std::sqrt(dx*dx + dz*dz);

        isCombatActive = (dist < detectionRange);

        if (isCombatActive && dist > attackRange) {
            // Mover em direção ao jogador
            x += (dx / dist) * moveSpeed * deltaTime * 60.0f;
            z += (dz / dist) * moveSpeed * deltaTime * 60.0f;
            y = getTerrainHeight(x, z) + 0.3f;
        }
    }

    bool attackPlayer(Player& player, float deltaTime) {
        if (!active) return false;

        float dx = player.getX() - x;
        float dz = player.getZ() - z;
        float dist = std::sqrt(dx*dx + dz*dz);

        if (dist <= attackRange && attackTimer <= 0) {
            player.takeDamage(attackDamage);
            attackTimer = attackCooldown;
            return true;
        }

        return false;
    }

    void takeDamage(float amount) {
        health -= amount;
        if (health <= 0) {
            health = 0;
            active = false;
        }
    }

    void draw() override {
        if (!active) return;

        glPushMatrix();
        glTranslatef(x, y, z);

        // Definir cor do inimigo
        GLfloat ambient[]  = {0.3f, 0.0f, 0.0f, 1.0f};   // sombra avermelhada
        GLfloat diffuse[]  = {0.8f, 0.1f, 0.1f, 1.0f};   // vermelho vivo
        GLfloat specular[] = {0.9f, 0.4f, 0.4f, 1.0f};   // reflexo rosado
        GLfloat shininess  = 64.0f;

        glDisable(GL_COLOR_MATERIAL);
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);

        // Corpo
        glPushMatrix();
        glScalef(size, size, size);
        glutSolidCube(0.8f);
        glPopMatrix();

        // Cabeça
        glPushMatrix();
        glTranslatef(0.0f, size * 0.6f, 0.0f);
        glutSolidSphere(size * 0.3f, 8, 8);
        glPopMatrix();

        // Barra de vida sobre o inimigo
        drawHealthBar();

        glPopMatrix();
    }

    void drawHealthBar() {
        // Desativar iluminação para desenhar a barra de vida
        glDisable(GL_LIGHTING);

        // Calcular posição da barra acima do inimigo
        float barWidth = size * 1.5f;
        float barHeight = size * 0.2f;
        float barPosY = size * 1.2f;

        // Converter para coordenadas de tela
        float healthPercent = health / maxHealth;

        // Desenhar fundo da barra (cinza)
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glVertex3f(-barWidth/2, barPosY, 0);
        glVertex3f(barWidth/2, barPosY, 0);
        glVertex3f(barWidth/2, barPosY + barHeight, 0);
        glVertex3f(-barWidth/2, barPosY + barHeight, 0);
        glEnd();

        // Desenhar barra de vida atual (vermelho)
        glColor3f(1.0f - healthPercent, healthPercent, 0.0f);
        glBegin(GL_QUADS);
        glVertex3f(-barWidth/2, barPosY, 0);
        glVertex3f(-barWidth/2 + barWidth * healthPercent, barPosY, 0);
        glVertex3f(-barWidth/2 + barWidth * healthPercent, barPosY + barHeight, 0);
        glVertex3f(-barWidth/2, barPosY + barHeight, 0);
        glEnd();

        // Reativar iluminação
        glEnable(GL_LIGHTING);
    }

    // Getters
    float getHealth() const { return health; }
    float getMaxHealth() const { return maxHealth; }
    int getLevel() const { return level; }
    bool isInCombat() const { return isCombatActive; }
    float getExperienceValue() const { return level * 20.0f; }
};

// Classe para objetos estáticos (árvores, pedras, casas)
class StaticObject : public GameObject {
private:
    GLfloat color[3];

public:
    StaticObject(float x, float y, float z, float size, ObjectType type,
                 float colorR, float colorG, float colorB)
        : GameObject(x, y, z, size, type) {
        color[0] = colorR;
        color[1] = colorG;
        color[2] = colorB;
    }

    void draw() override {
        glPushMatrix();
        glTranslatef(x, y, z);

        // Define all variables before the switch
        GLfloat trunkColor[3] = {0.7f, 0.4f, 0.1f};
        GLfloat leavesColor[3] = {0.3f, 0.5f, 0.1f};
        GLfloat roofColor[3] = {0.5f, 0.5f, 0.5f};

        switch (type) {
            case TREE: {
                // Tronco
                GLfloat ambient[]  = {0.2f, 0.1f, 0.0f, 1.0f};
                GLfloat diffuse[]  = {0.5f, 0.3f, 0.1f, 1.0f};
                GLfloat specular[] = {0.1f, 0.05f, 0.02f, 1.0f};
                GLfloat shininess  = 10.0f;

                glDisable(GL_COLOR_MATERIAL);
                glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
                glMaterialf(GL_FRONT, GL_SHININESS, shininess);

                glPushMatrix();
                glScalef(0.3f, 1.0f, 0.3f);
                glTranslatef(0.0f, 0.5f, 0.0f);
                glutSolidCube(size);
                glPopMatrix();

                // Copa
                GLfloat topambient[]  = {0.1f, 0.2f, 0.1f, 1.0f};
                GLfloat topdiffuse[]  = {0.2f, 0.6f, 0.2f, 1.0f};
                GLfloat topspecular[] = {0.05f, 0.1f, 0.05f, 1.0f};
                GLfloat topshininess  = 5.0f;

                glDisable(GL_COLOR_MATERIAL);
                glMaterialfv(GL_FRONT, GL_AMBIENT, topambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, topdiffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, topspecular);
                glMaterialf(GL_FRONT, GL_SHININESS, topshininess);

                glPushMatrix();
                glTranslatef(0.0f, size * 0.8f, 0.0f);
                glutSolidSphere(size * 0.6f, 10, 10);
                glPopMatrix();
                break;
            }

            case ROCK: {
                GLfloat ambient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
                GLfloat diffuse[]  = {0.5f, 0.5f, 0.5f, 1.0f};
                GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
                GLfloat shininess  = 5.0f;

                glDisable(GL_COLOR_MATERIAL);
                glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
                glMaterialf(GL_FRONT, GL_SHININESS, shininess);

                glScalef(size, size * 0.7f, size);
                glutSolidSphere(0.5f, 10, 10);
                break;
            }

            case HOUSE: {
                // Base da casa
                GLfloat ambient[]  = {0.3f, 0.2f, 0.1f, 1.0f};
                GLfloat diffuse[]  = {0.6f, 0.4f, 0.2f, 1.0f};
                GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
                GLfloat shininess  = 10.0f;

                glDisable(GL_COLOR_MATERIAL);
                glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
                glMaterialf(GL_FRONT, GL_SHININESS, shininess);

                glPushMatrix();
                glScalef(size, size * 0.8f, size);
                glTranslatef(0.0f, 0.5f, 0.0f);
                glutSolidCube(1.0f);
                glPopMatrix();

                // Telhado
                GLfloat roofAmbient[]  = {0.2f, 0.2f, 0.2f, 1.0f};
                GLfloat roofDiffuse[]  = {0.5f, 0.5f, 0.5f, 1.0f};
                GLfloat roofSpecular[] = {0.05f, 0.05f, 0.05f, 1.0f};
                GLfloat roofShine      = 8.0f;

                glMaterialfv(GL_FRONT, GL_AMBIENT, roofAmbient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, roofDiffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, roofSpecular);
                glMaterialf(GL_FRONT, GL_SHININESS, roofShine);

                glPushMatrix();
                glTranslatef(0.0f, size * 0.8f, 0.0f);
                glBegin(GL_TRIANGLES);
                glVertex3f(-size * 0.6f, 0, -size * 0.6f);
                glVertex3f(size * 0.6f, 0, -size * 0.6f);
                glVertex3f(0, size * 0.6f, 0);

                glVertex3f(size * 0.6f, 0, -size * 0.6f);
                glVertex3f(size * 0.6f, 0, size * 0.6f);
                glVertex3f(0, size * 0.6f, 0);

                glVertex3f(size * 0.6f, 0, size * 0.6f);
                glVertex3f(-size * 0.6f, 0, size * 0.6f);
                glVertex3f(0, size * 0.6f, 0);

                glVertex3f(-size * 0.6f, 0, size * 0.6f);
                glVertex3f(-size * 0.6f, 0, -size * 0.6f);
                glVertex3f(0, size * 0.6f, 0);
                glEnd();
                glPopMatrix();
                break;
            }

            case ITEM: {
                // Desenhar um item coletável
                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
                glutSolidSphere(size * 0.5f, 8, 8);

                // Rotação para efeito de flutuação
                static float angle = 0.0f;
                angle += 0.5f;
                glRotatef(angle, 0.0f, 1.0f, 0.0f);
                glutSolidTetrahedron();
                break;
            }

            case ENEMY:
            case NPC:
            default:
                // Handle other cases or do nothing
                break;
            }

        glPopMatrix();
    }
};


class GrassBlade : public GameObject {
public:
    GrassBlade(float x, float y, float z)
        : GameObject(x, y, z, 0.1f, ObjectType::ITEM) {}  // Pode usar outro tipo

    void draw() override {
        glPushMatrix();
        glTranslatef(x, y, z);

        // Calcular ângulo de balanço baseado no tempo
        float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
        float sway = sin(time * 2.0f + x * 0.5f + z * 0.5f) * 10.0f; // ângulo de oscilação em graus

        // Aplicar rotação suave para simular o vento
        glRotatef(sway, 0.0f, 0.0f, 1.0f);  // Rotaciona ao redor do eixo Z (ou tente eixo X para variar)

        // Material da grama
        GLfloat ambient[]  = {0.0f, 0.2f, 0.0f, 1.0f};
        GLfloat diffuse[]  = {0.2f, 0.8f, 0.2f, 1.0f};
        GLfloat specular[] = {0.0f, 0.05f, 0.0f, 1.0f};
        GLfloat shininess  = 5.0f;

        glDisable(GL_COLOR_MATERIAL);
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);

        // Desenhar folhas de grama
        glBegin(GL_TRIANGLES);

        float h = 0.3f;
        float w = 0.02f;

        for (int i = 0; i < 3; ++i) {
            float angle = i * 60.0f * M_PI / 180.0f;
            float dx = std::cos(angle) * w;
            float dz = std::sin(angle) * w;

            glVertex3f(0.0f, 0.0f, 0.0f);
            glVertex3f(dx, h, dz);
            glVertex3f(-dx, 0.0f, -dz);
        }

        glEnd();
        glPopMatrix();
    }

};


// Classe para gerenciamento do jogo
class Game {
private:
    Player player;
    std::vector<std::unique_ptr<GameObject>> gameObjects;

    float cameraDistance;
    float cameraHeight;
    float cameraAngle;

    int gameMode;  // 0: exploração, 1: combate, 2: menu de habilidades

    const float WORLD_SIZE = 20.0f;
    float lastFrameTime;
    float deltaTime;

    // Variáveis para controle do mouse
    int lastMouseX, lastMouseY;
    bool mouseLeftDown;
    bool mouseRightDown;
    float mouseSensitivity;

    // Para mouse picking na árvore de habilidades
    struct SkillNode {
        float x, y;          // Posição na tela
        float radius;        // Raio do nó
        int skillIndex;      // Índice da habilidade
        bool hovering;       // Se o mouse está sobre o nó
    };
    std::vector<SkillNode> skillNodes;

    // Constantes para cores
    GLfloat skyColor[3] = {0.4f, 0.6f, 0.9f};

public:
    Game() : player(0.0f, 0.5f, 0.0f),
         cameraDistance(5.0f),
         cameraHeight(2.0f),
         cameraAngle(0.0f),
         gameMode(0),
         lastFrameTime(0.0f),
         deltaTime(0.0f),
         lastMouseX(0), lastMouseY(0),
         mouseLeftDown(false), mouseRightDown(false),
         mouseSensitivity(0.2f) {

        initObjects();
    }
    bool topDownView = false;

    float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    bool isInTrail(float x, float z) {
        float trailWidth = 2.0f;
        for (const auto& p : trailCurvePoints) {
            float dx = x - p.x;
            float dz = z - p.z;
            float distSq = dx * dx + dz * dz;
            if (distSq < trailWidth * trailWidth)
                return true;
        }

        // Verificar clareiras
        for (const auto& c : trailClearings) {
            float dx = x - c.x;
            float dz = z - c.z;
            float distSq = dx * dx + dz * dz;
            if (distSq < 4.0f * 4.0f)  // raio das clareiras
                return true;
        }

        return false;
    }


    bool hasGrass(float x, float z) {
        if (isInTrail(x, z)) {
            return false; // Sem grama na trilha
        }

        for (const GrassPatch& patch : grassPatches) {
            float dx = x - patch.x;
            float dz = z - patch.z;
            if (std::sqrt(dx * dx + dz * dz) < patch.radius) {
                return true; // Dentro de alguma mancha de grama
            }
        }

        return false;
    }

    void generateBranch(TrailPoint origin, float baseDirection, float safeMargin) {
        float direction = baseDirection + ((rand() % 2 == 0) ? M_PI / 3 : -M_PI / 3); // 60° esquerda ou direita
        int branchLength = 8 + rand() % 5; // comprimento variável

        TrailPoint current = origin;

        for (int i = 0; i < branchLength; ++i) {
            float length = 1.5f;
            TrailPoint next;
            next.x = current.x + cos(direction) * length;
            next.z = current.z + sin(direction) * length;
            next.y = getTerrainHeight(next.x, next.z);

            if (fabs(next.x) > safeMargin || fabs(next.z) > safeMargin) break;

            trailCurvePoints.push_back(next);
            if (i % 4 == 0) trailClearings.push_back(next);

            direction += sin(i * 0.3f) * (M_PI / 18); // zigue-zague leve
            current = next;
        }
    }


    void generateNaturalTrail(float worldSize) {
        trailCurvePoints.clear();
        trailClearings.clear();

        float safeMargin = worldSize * 0.99f;
        float length = 1.5f;
        float curveAngle = M_PI / 12;

        TrailPoint current = {
            (float)(rand() % (int)(safeMargin * 2)) - safeMargin,
            (float)(rand() % (int)(safeMargin * 2)) - safeMargin
        };

        float direction = atan2(-current.z, -current.x); // mira no centro
        int segments = 60;
        bool crossedCenter = false;

        for (int i = 0; i < segments; ++i) {
            if (!crossedCenter && fabs(current.x) < 2.0f && fabs(current.z) < 2.0f) {
                crossedCenter = true; // passou pelo centro
            }

            int type = rand() % 4;
            if (crossedCenter) {
                switch (type) {
                    case 1: direction += curveAngle; break;
                    case 2: direction -= curveAngle; break;
                    case 3: direction += sin(i * 0.5f) * curveAngle * 2; break;
                }
            }

            TrailPoint next;
            next.x = current.x + cos(direction) * length;
            next.z = current.z + sin(direction) * length;
            next.y = getTerrainHeight(next.x, next.z);

            if (fabs(next.x) > safeMargin || fabs(next.z) > safeMargin) {
                float dx = -current.x;
                float dz = -current.z;
                direction = atan2(dz, dx);
                next.x = current.x + cos(direction) * length;
                next.z = current.z + sin(direction) * length;
            }

            trailCurvePoints.push_back(next);

            if (i % 6 == 0 && fabs(next.x) < safeMargin && fabs(next.z) < safeMargin) {
                trailClearings.push_back(next);
            }

            // ✨ Bifurcação ocasional
            if (i > 10 && i % 15 == 0 && rand() % 100 < 40) {
                generateBranch(next, direction, safeMargin);
            }

            current = next;
        }
    }

    void initObjects() {
        // Criar árvores aleatórias
        for (int i = 0; i < 20; i++) {
            float x = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
            float z = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
            float size = 0.5f + ((float)rand() / RAND_MAX) * 0.5f;
            float y = getTerrainHeight(x, z);

            gameObjects.push_back(std::make_unique<StaticObject>(
                x, y, z, size, TREE, 0.3f, 0.5f, 0.1f
            ));
        }

        // Criar pedras
        for (int i = 0; i < 10; i++) {
            float x = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
            float z = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
            float size = 0.3f + ((float)rand() / RAND_MAX) * 0.4f;
            float y = getTerrainHeight(x, z);

            gameObjects.push_back(std::make_unique<StaticObject>(
                x, y, z, size, ROCK, 0.5f, 0.5f, 0.5f
            ));
        }

        // Criar casas
        for (int i = 0; i < 5; i++) {
            float x = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
            float z = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
            float size = 1.0f + ((float)rand() / RAND_MAX) * 0.5f;
            float y = getTerrainHeight(x, z);

            gameObjects.push_back(std::make_unique<StaticObject>(
                x, y, z, size, HOUSE, 0.7f, 0.4f, 0.1f
            ));
        }

        // Criar inimigos
        for (int i = 0; i < 5; i++) {
            float x = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
            float z = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
            float size = 0.4f + ((float)rand() / RAND_MAX) * 0.3f;
            int level = 1 + rand() % 3;  // Inimigos de nível 1 a 3
            float y = getTerrainHeight(x, z);

            gameObjects.push_back(std::make_unique<Enemy>(
                x, y, z, size, level
            ));
        }

        // Criar itens
        for (int i = 0; i < 3; i++) {
            float x = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
            float z = (float)(rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
            float size = 0.2f + ((float)rand() / RAND_MAX) * 0.2f;
            float y = getTerrainHeight(x, z) + 0.3f;

            gameObjects.push_back(std::make_unique<StaticObject>(
                x, y, z, size, ITEM, 0.9f, 0.8f, 0.1f
            ));
        }

        generateNaturalTrail(WORLD_SIZE);

        // Criar manchas
        int numPatches = 20;
        for (int i = 0; i < numPatches; ++i) {
            GrassPatch patch;
            patch.x = (rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
            patch.z = (rand() % (int)(WORLD_SIZE * 2)) - WORLD_SIZE;
            patch.radius = 2.0f + (rand() % 300) / 100.0f; // raio entre 2 e 5
            grassPatches.push_back(patch);
        }

        // Criar grama com densidade controlada
        for (float x = -WORLD_SIZE; x <= WORLD_SIZE; x += 0.5f) {
            for (float z = -WORLD_SIZE; z <= WORLD_SIZE; z += 0.5f) {
                if (hasGrass(x, z)) {
                    float offsetX = ((rand() % 100) / 100.0f - 0.5f) * 0.3f;
                    float offsetZ = ((rand() % 100) / 100.0f - 0.5f) * 0.3f;
                    float y = getTerrainHeight(x + offsetX, z + offsetZ) + 0.01f;
                    gameObjects.push_back(std::make_unique<GrassBlade>(x + offsetX, y, z + offsetZ));
                }
            }
        }

    }

    void update() {
        // Calcular deltaTime
        float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
        deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        // Atualizar o jogador
        player.update(deltaTime);

        // Verificar modo de jogo atual
        if (gameMode != 2) {
            gameMode = 0;  // Padrão: exploração
        }
        // Atualizar objetos
        for (auto& object : gameObjects) {
            object->update(deltaTime);

            // Verificar inimigos para combate
            Enemy* enemy = dynamic_cast<Enemy*>(object.get());
            if (enemy && enemy->isActive()) {
                // Atualizar comportamento do inimigo
                enemy->moveTowardsPlayer(player, deltaTime);
                if (enemy->attackPlayer(player, deltaTime)) {
                    std::cout << "Você foi atacado! Vida restante: " << player.getHealth() << std::endl;
                }

                // Se algum inimigo estiver em combate, ativar modo de combate
                if (enemy->isInCombat()) {
                    gameMode = 1;
                }

                // Verificar se o jogador derrotou o inimigo
                if (!enemy->isActive()) {
                    player.addExperience(static_cast<int>(enemy->getExperienceValue()));
                    std::cout << "Inimigo derrotado! Ganhou " << enemy->getExperienceValue() << " XP." << std::endl;
                }
            }
        }

        // Verificar colisões com objetos
        checkCollisions();

        // Atualizar altura do jogador conforme o terreno
        float targetY = getTerrainHeight(player.getX(), player.getZ())+ 0.3f;
        float currentY = player.getY();
        float smoothY = lerp(currentY, targetY, 0.1f); // 0.1 = suavização
        player.setPosition(player.getX(), smoothY, player.getZ());

        // Manter o jogador dentro dos limites do mundo
        constrainPlayer();
    }

    void render() {
        // Limpar o buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Configurar projeção
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0f, glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f, 100.0f);

        // Configurar câmera
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Posicionar câmera em 3ª pessoa
        float camX = player.getX() - cameraDistance * std::sin(player.getRotY() * M_PI / 180.0f);
        float camZ = player.getZ() - cameraDistance * std::cos(player.getRotY() * M_PI / 180.0f);

        if (topDownView) {
            gluLookAt(player.getX(), 50.0f, player.getZ(),
                      player.getX(), 0.0f, player.getZ(),
                      0.0f, 0.0f, -1.0f);
        } else {
            gluLookAt(
                camX, player.getY() + cameraHeight, camZ,  // Posição da câmera
                player.getX(), player.getY(), player.getZ(), // Ponto para onde a câmera olha
                0.0f, 1.0f, 0.0f                      // Vetor "up" (para cima)
            );
        }

        // Desenhar o mundo
        drawGround();

        // Desenhar objetos
        for (auto& object : gameObjects) {
            if (object->isActive()) {
                object->draw();
            }
        }

        // Desenhar o jogador
        player.draw();

        // Desenhar HUD
        drawHUD();

        // Se estiver no menu de habilidades, desenhar a árvore
        if (gameMode == 2) {
            drawSkillTree();
        }
    }

    void drawGround() {
        const float step = 1.0f;
        const float size = WORLD_SIZE;

        GLfloat ambient[]  = {0.1f, 0.6f, 0.35f, 1.0f};
        GLfloat diffuse[]  = {0.1f, 0.3f, 0.1f, 1.0f};
        GLfloat specular[] = {0.05f, 0.05f, 0.05f, 1.0f};
        GLfloat shininess  = 5.0f;

        glDisable(GL_COLOR_MATERIAL);
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialf(GL_FRONT, GL_SHININESS, shininess);

        for (float x = -size; x < size; x += step) {
            for (float z = -size; z < size; z += step) {
                float y1 = getTerrainHeight(x, z);
                float y2 = getTerrainHeight(x + step, z);
                float y3 = getTerrainHeight(x + step, z + step);
                float y4 = getTerrainHeight(x, z + step);

                glBegin(GL_QUADS);
                glVertex3f(x, y1, z);
                glVertex3f(x + step, y2, z);
                glVertex3f(x + step, y3, z + step);
                glVertex3f(x, y4, z + step);
                glEnd();
            }
        }
        // Desenhar trilhas por cima do relevo
        GLfloat trailColor[] = {0.4f, 0.3f, 0.1f};
        glDisable(GL_COLOR_MATERIAL);
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, trailColor);

        glBegin(GL_QUADS);
        for (const auto& p : trailCurvePoints) {
            float y = getTerrainHeight(p.x, p.z) + 0.01f;  // levemente acima do solo
            float size = 1.0f;

            glVertex3f(p.x - size, y, p.z - size);
            glVertex3f(p.x + size, y, p.z - size);
            glVertex3f(p.x + size, y, p.z + size);
            glVertex3f(p.x - size, y, p.z + size);
        }

        // Clareiras maiores
        for (const auto& c : trailClearings) {
            float y = getTerrainHeight(c.x, c.z) + 0.01f;
            float radius = 2.5f;
            for (float angle = 0; angle < 360.0f; angle += 10.0f) {
                float rad1 = angle * M_PI / 180.0f;
                float rad2 = (angle + 10.0f) * M_PI / 180.0f;

                glVertex3f(c.x, y, c.z);
                glVertex3f(c.x + cos(rad1) * radius, y, c.z + sin(rad1) * radius);
                glVertex3f(c.x + cos(rad2) * radius, y, c.z + sin(rad2) * radius);
                glVertex3f(c.x, y, c.z);
            }
        }
        glEnd();
    }


    void drawHUD() {
        // Desabilitar iluminação e profundidade para desenhar a HUD
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);

        // Configurar projeção ortogonal para HUD
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

        // Barra de vida
        float barWidth = 200.0f;
        float barHeight = 20.0f;
        float healthPercent = player.getHealth() / player.getMaxHealth();

        // Fundo da barra
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(10, windowHeight - 30);
        glVertex2f(10 + barWidth, windowHeight - 30);
        glVertex2f(10 + barWidth, windowHeight - 30 - barHeight);
        glVertex2f(10, windowHeight - 30 - barHeight);
        glEnd();

        // Barra de vida atual
        glColor3f(1.0f - healthPercent, healthPercent, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(10, windowHeight - 30);
        glVertex2f(10 + barWidth * healthPercent, windowHeight - 30);
        glVertex2f(10 + barWidth * healthPercent, windowHeight - 30 - barHeight);
        glVertex2f(10, windowHeight - 30 - barHeight);
        glEnd();

        // Barra de experiência
        float expPercent = (float)player.getExperience() / (float)player.getExperienceToNextLevel();

        // Fundo da barra
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(10, windowHeight - 60);
        glVertex2f(10 + barWidth, windowHeight - 60);
        glVertex2f(10 + barWidth, windowHeight - 60 - barHeight);
        glVertex2f(10, windowHeight - 60 - barHeight);
        glEnd();

        // Barra de experiência atual
        glColor3f(0.2f, 0.4f, 0.8f);
        glBegin(GL_QUADS);
        glVertex2f(10, windowHeight - 60);
        glVertex2f(10 + barWidth * expPercent, windowHeight - 60);
        glVertex2f(10 + barWidth * expPercent, windowHeight - 60 - barHeight);
        glVertex2f(10, windowHeight - 60 - barHeight);
        glEnd();

        // Barra de cooldown de ataque
        float cooldownPercent = player.getAttackTimer() / player.getAttackCooldown();

        if (cooldownPercent > 0) {
            // Fundo da barra
            glColor3f(0.3f, 0.3f, 0.3f);
            glBegin(GL_QUADS);
            glVertex2f(10, windowHeight - 90);
            glVertex2f(10 + barWidth, windowHeight - 90);
            glVertex2f(10 + barWidth, windowHeight - 90 - barHeight);
            glVertex2f(10, windowHeight - 90 - barHeight);
            glEnd();

            // Barra de cooldown atual
            glColor3f(0.8f, 0.8f, 0.0f);
            glBegin(GL_QUADS);
            glVertex2f(10, windowHeight - 90);
            glVertex2f(10 + barWidth * (1.0f - cooldownPercent), windowHeight - 90);
            glVertex2f(10 + barWidth * (1.0f - cooldownPercent), windowHeight - 90 - barHeight);
            glVertex2f(10, windowHeight - 90 - barHeight);
            glEnd();
        }

        // Texto da HUD
        glColor3f(1.0f, 1.0f, 1.0f);

        char buffer[128];
        sprintf(buffer, "Vida: %.1f/%.1f", player.getHealth(), player.getMaxHealth());
        drawText(15, windowHeight - 25, buffer);

        sprintf(buffer, "Nivel: %d   XP: %d/%d", player.getLevel(),
                player.getExperience(), player.getExperienceToNextLevel());
        drawText(15, windowHeight - 55, buffer);

        if (cooldownPercent > 0) {
            sprintf(buffer, "Ataque: %.1f", player.getAttackCooldown() - player.getAttackTimer());
            drawText(15, windowHeight - 85, buffer);
        } else {
            glColor3f(0.0f, 1.0f, 0.0f);
            drawText(15, windowHeight - 85, "Ataque Pronto!");
        }

        // Status do jogo
        if (gameMode == 1) {
            glColor3f(1.0f, 0.2f, 0.2f);
            drawText(windowWidth / 2 - 50, windowHeight - 25, "COMBATE");
            drawText(10, 20, "Pressione 1 para atacar");
        } else if (gameMode == 2) {
            glColor3f(0.2f, 0.7f, 1.0f);
            drawText(windowWidth / 2 - 60, windowHeight - 25, "MENU DE HABILIDADES");
            drawText(10, 20, "Use as teclas numericas (1-9) para melhorar habilidades");
        } else {
            glColor3f(0.2f, 1.0f, 0.2f);
            drawText(windowWidth / 2 - 60, windowHeight - 25, "EXPLORACAO");
        }

        // Pontos de habilidade
        if (player.getSkillTree().getSkillPoints() > 0) {
            glColor3f(1.0f, 1.0f, 0.0f);
            sprintf(buffer, "Pontos de Habilidade: %d (Pressione K para abrir menu)",
                    player.getSkillTree().getSkillPoints());
            drawText(windowWidth - 400, windowHeight - 25, buffer);
        }

        // Controles
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(windowWidth - 200, 80, "Controles:");
        drawText(windowWidth - 200, 60, "W,A,S,D - Movimento");
        drawText(windowWidth - 200, 40, "Q,E - Rotacao");
        drawText(windowWidth - 200, 20, "Espaco - Ataque");

        // Restaurar matrizes e estados
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        // Reativar estados
        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
    }

    void drawSkillTree() {

        // Calcular layout da árvore de habilidades
        calculateSkillTreeLayout();

        // Desabilitar iluminação e profundidade para desenhar o menu
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Configurar projeção ortogonal para menu
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

        // Fundo semi-transparente
        glColor4f(0.0f, 0.0f, 0.2f, 0.8f);
        glBegin(GL_QUADS);
        glVertex2f(50, 50);
        glVertex2f(windowWidth - 50, 50);
        glVertex2f(windowWidth - 50, windowHeight - 50);
        glVertex2f(50, windowHeight - 50);
        glEnd();

        // Título
        glColor3f(1.0f, 1.0f, 0.0f);
        drawText(windowWidth / 2 - 80, windowHeight - 80, "ARVORE DE HABILIDADES");

        // Pontos disponíveis
        char buffer[128];
        sprintf(buffer, "Pontos Disponiveis: %d", player.getSkillTree().getSkillPoints());
        drawText(windowWidth / 2 - 80, windowHeight - 110, buffer);



        // Desenhar conexões entre nós
        drawSkillTreeConnections();

        // Desenhar nós da árvore de habilidades
        drawSkillTreeNodes();

        // Desenhar tooltips para nós com hover
        drawSkillNodeTooltips();

        // Instrução para sair
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(windowWidth / 2 - 100, 80, "Pressione K para voltar ao jogo");
        drawText(windowWidth / 2 - 140, 60, "Use o mouse para selecionar habilidades");

        // Restaurar matrizes e estados
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        // Reativar estados
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
        glEnable(GL_DEPTH_TEST);
    }

    void calculateSkillTreeLayout() {
        int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

        const auto& skills = player.getSkillTree().getSkills();

        // Limpar nós existentes
        skillNodes.clear();

        // Definir layout básico com nós de habilidades básicas no centro
        float centerX = windowWidth / 2.0f;
        float centerY = windowHeight / 2.0f;
        float baseRadius = 30.0f;
        float nodeSpacing = 150.0f;

        // Disposição em estrela das habilidades básicas (0-3)
        float angles[4] = {0, 90, 180, 270};
        for (int i = 0; i < 4; i++) {
            float angle = angles[i] * M_PI / 180.0f;
            float x = centerX + cos(angle) * nodeSpacing;
            float y = centerY + sin(angle) * nodeSpacing;

            SkillNode node;
            node.x = x;
            node.y = y;
            node.radius = baseRadius;
            node.skillIndex = i;
            node.hovering = false;

            skillNodes.push_back(node);
        }

        // Habilidades avançadas (4-7) como filhos das básicas
        for (int i = 0; i < 4; i++) {
            float angle = angles[i] * M_PI / 180.0f;
            float x = centerX + cos(angle) * nodeSpacing * 2.0f;
            float y = centerY + sin(angle) * nodeSpacing * 2.0f;

            SkillNode node;
            node.x = x;
            node.y = y;
            node.radius = baseRadius * 0.9f;
            node.skillIndex = i + 4;
            node.hovering = false;

            skillNodes.push_back(node);
        }
    }

    void drawSkillTreeConnections() {
        glLineWidth(2.0f);

        const auto& skills = player.getSkillTree().getSkills();

        // Desenhar linhas conectando habilidades básicas (0-3) com avançadas (4-7)
        for (int i = 0; i < 4; i++) {
            int parentIndex = i;
            int childIndex = i + 4;

            // Determinar cor da linha baseada no estado das habilidades
            if (skills[childIndex]->getLevel() > 0) {
                // Conexão ativa
                glColor3f(0.2f, 0.8f, 0.2f);
            } else if (skills[parentIndex]->getLevel() > 0) {
                // Conexão disponível
                glColor3f(0.8f, 0.8f, 0.2f);
            } else {
                // Conexão indisponível
                glColor3f(0.5f, 0.5f, 0.5f);
            }

            glBegin(GL_LINES);
            glVertex2f(skillNodes[parentIndex].x, skillNodes[parentIndex].y);
            glVertex2f(skillNodes[childIndex].x, skillNodes[childIndex].y);
            glEnd();
        }
    }

    void drawSkillTreeNodes() {
        const auto& skills = player.getSkillTree().getSkills();

        // Desenhar cada nó
        for (size_t i = 0; i < skillNodes.size(); i++) {
            const auto& node = skillNodes[i];
            const auto& skill = skills[node.skillIndex];

            // Determinar cor do nó baseada no estado da habilidade
            if (skill->getLevel() == 0) {
                if (skill->canLearn() && player.getSkillTree().getSkillPoints() > 0) {
                    // Nó disponível para aprendizado
                    glColor4f(0.8f, 0.8f, 0.2f, node.hovering ? 0.9f : 0.7f);
                } else {
                    // Nó indisponível
                    glColor4f(0.5f, 0.5f, 0.5f, node.hovering ? 0.9f : 0.7f);
                }
            } else if (skill->getLevel() == skill->getMaxLevel()) {
                // Nó com nível máximo
                glColor4f(1.0f, 0.6f, 0.0f, node.hovering ? 0.9f : 0.7f);
            } else {
                if (skill->canLearn() && player.getSkillTree().getSkillPoints() > 0) {
                    // Nó com nível parcial e pode melhorar
                    glColor4f(0.2f, 0.8f, 0.2f, node.hovering ? 0.9f : 0.7f);
                } else {
                    // Nó com nível parcial
                    glColor4f(0.2f, 0.6f, 0.8f, node.hovering ? 0.9f : 0.7f);
                }
            }

            // Desenhar círculo do nó
            drawFilledCircle(node.x, node.y, node.radius, 20);

            // Borda do nó
            if (node.hovering) {
                glLineWidth(2.5f);
                glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
            } else {
                glLineWidth(1.5f);
                glColor4f(0.8f, 0.8f, 0.8f, 0.7f);
            }
            drawCircle(node.x, node.y, node.radius, 20);

            // Desenhar ícone ou indicador de nível dentro do nó
            char levelText[8];
            sprintf(levelText, "%d/%d", skill->getLevel(), skill->getMaxLevel());

            // Centralizar texto
            float textWidth = strlen(levelText) * 8.0f;
            glColor3f(1.0f, 1.0f, 1.0f);
            drawText(node.x - textWidth/2, node.y - 6, levelText);

            // Desenhar ícone baseado no tipo de habilidade
            drawSkillIcon(node.x, node.y - 20, skill->getType());
        }
    }

    void drawSkillIcon(float x, float y, SkillType type) {
        glLineWidth(2.0f);
        glColor3f(1.0f, 1.0f, 1.0f);

        float iconSize = 8.0f;

        switch (type) {
            case ATTACK:
                // Ícone de espada
                glBegin(GL_LINES);
                glVertex2f(x - iconSize, y + iconSize);
                glVertex2f(x + iconSize, y - iconSize);
                glVertex2f(x - iconSize/2, y - iconSize/2);
                glVertex2f(x + iconSize/2, y + iconSize/2);
                glEnd();
                break;

            case DEFENSE:
                // Ícone de escudo
                drawCircle(x, y, iconSize, 8);
                break;

            case MAGIC:
                // Ícone de estrela
                drawStar(x, y, iconSize, 5);
                break;

            case SPEED:
                // Ícone de velocidade
                glBegin(GL_LINES);
                glVertex2f(x - iconSize, y);
                glVertex2f(x + iconSize, y);
                glVertex2f(x + iconSize/2, y - iconSize/2);
                glVertex2f(x + iconSize, y);
                glVertex2f(x + iconSize/2, y + iconSize/2);
                glVertex2f(x + iconSize, y);
                glEnd();
                break;
        }
    }

    void drawSkillNodeTooltips() {
        for (const auto& node : skillNodes) {
            if (node.hovering) {
                const auto& skills = player.getSkillTree().getSkills();
                const auto& skill = skills[node.skillIndex];

                // Definir tamanho do tooltip
                float tooltipWidth = 200.0f;
                float tooltipHeight = 100.0f;
                float tooltipX = node.x + node.radius + 10;
                float tooltipY = node.y + tooltipHeight/2;

                // Ajustar posição se sair da tela
                int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
                int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

                if (tooltipX + tooltipWidth > windowWidth - 50)
                    tooltipX = node.x - tooltipWidth - node.radius - 10;

                if (tooltipY + tooltipHeight/2 > windowHeight - 50)
                    tooltipY = windowHeight - 50 - tooltipHeight/2;

                if (tooltipY - tooltipHeight/2 < 50)
                    tooltipY = 50 + tooltipHeight/2;

                // Desenhar fundo do tooltip
                glColor3f(0.1f, 0.1f, 0.3f);
                glBegin(GL_QUADS);
                glVertex2f(tooltipX, tooltipY - tooltipHeight/2);
                glVertex2f(tooltipX + tooltipWidth, tooltipY - tooltipHeight/2);
                glVertex2f(tooltipX + tooltipWidth, tooltipY + tooltipHeight/2);
                glVertex2f(tooltipX, tooltipY + tooltipHeight/2);
                glEnd();

                // Desenhar borda do tooltip
                glLineWidth(1.0f);
                glColor4f(0.5f, 0.5f, 0.7f, 0.9f);
                glBegin(GL_LINE_LOOP);
                glVertex2f(tooltipX, tooltipY - tooltipHeight/2);
                glVertex2f(tooltipX + tooltipWidth, tooltipY - tooltipHeight/2);
                glVertex2f(tooltipX + tooltipWidth, tooltipY + tooltipHeight/2);
                glVertex2f(tooltipX, tooltipY + tooltipHeight/2);
                glEnd();

                // Desenhar texto do tooltip
                glColor3f(1.0f, 1.0f, 1.0f);
                float textY = tooltipY + tooltipHeight/2 - 20;

                glColor3f(1.0f, 1.0f, 1.0f); // Branco puro
                drawText(tooltipX + 10, textY, "TESTE TOOLTIP");

                // Nome da habilidade
                char buffer[128];
                sprintf(buffer, "%s", skill->getName().c_str());
                drawText(tooltipX + 10, textY, buffer);
                textY -= 20;

                // Nível atual/máximo
                sprintf(buffer, "Nível: %d/%d", skill->getLevel(), skill->getMaxLevel());
                drawText(tooltipX + 10, textY, buffer);
                textY -= 15;

                // Valor atual
                sprintf(buffer, "Valor: %.1f", skill->getValue());
                drawText(tooltipX + 10, textY, buffer);
                textY -= 15;

                // Status de disponibilidade
                if (skill->getLevel() == skill->getMaxLevel()) {
                    glColor3f(1.0f, 0.6f, 0.0f);
                    drawText(tooltipX + 10, textY, "Nível Máximo Alcançado");
                } else if (skill->canLearn() && player.getSkillTree().getSkillPoints() > 0) {
                    glColor3f(0.2f, 1.0f, 0.2f);
                    drawText(tooltipX + 10, textY, "Clique para Melhorar");
                } else if (!skill->canLearn()) {
                    glColor3f(1.0f, 0.2f, 0.2f);
                    drawText(tooltipX + 10, textY, "Requisitos não Atendidos");
                } else {
                    glColor3f(1.0f, 1.0f, 0.0f);
                    drawText(tooltipX + 10, textY, "Pontos de Habilidade Insuficientes");
                }

                break; // Mostrar apenas um tooltip por vez
            }
        }
    }

    // Funções auxiliares de desenho
    void drawFilledCircle(float x, float y, float radius, int segments) {
        glBegin(GL_POLYGON);
        for (int i = 0; i < segments; i++) {
            float theta = 2.0f * M_PI * i / segments;
            float px = x + radius * cosf(theta);
            float py = y + radius * sinf(theta);
            glVertex2f(px, py);
        }
        glEnd();
    }

    void drawCircle(float x, float y, float radius, int segments) {
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; i++) {
            float theta = 2.0f * M_PI * i / segments;
            float px = x + radius * cosf(theta);
            float py = y + radius * sinf(theta);
            glVertex2f(px, py);
        }
        glEnd();
    }

    void drawStar(float x, float y, float radius, int points) {
        float innerRadius = radius * 0.4f;

        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < points * 2; i++) {
            float theta = M_PI * i / points;
            float r = (i % 2 == 0) ? radius : innerRadius;
            float px = x + r * cosf(theta);
            float py = y + r * sinf(theta);
            glVertex2f(px, py);
        }
        glEnd();
    }

    void drawText(float x, float y, const char* string) {
        // Posiciona o texto
        glRasterPos2f(x, y);

        // Desenha cada caractere
        for (const char* c = string; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
        }
    }

    void checkCollisions() {
        // Verificar colisões entre o jogador e objetos
        for (auto& object : gameObjects) {
            if (!object->isActive()) continue;

            if (player.checkCollision(*object)) {
                // Se for um item, coletar
                if (object->getType() == ITEM) {
                    object->setActive(false);
                    player.heal(10.0f);  // Cura
                    player.addExperience(10000);  // Pequena quantidade de XP
                    std::cout << "Item coletado! +10 XP, +10 Vida" << std::endl;
                    continue;
                }

                // Para outros objetos, implementar resposta à colisão
                float dx = player.getX() - object->getX();
                float dz = player.getZ() - object->getZ();
                float dist = std::sqrt(dx*dx + dz*dz);

                if (dist > 0.1f) {  // Evitar divisão por zero
                    float overlap = (player.getSize() + object->getSize()) - dist;
                    float x = player.getX() + (dx / dist) * overlap * 1.1f;
                    float z = player.getZ() + (dz / dist) * overlap * 1.1f;
                    player.setPosition(x, player.getY(), z);
                } else {
                    // Se muito próximo, empurrar em uma direção aleatória
                    float x = player.getX() + ((float)rand() / RAND_MAX * 2.0f - 1.0f) * 0.1f;
                    float z = player.getZ() + ((float)rand() / RAND_MAX * 2.0f - 1.0f) * 0.1f;
                    player.setPosition(x, player.getY(), z);
                }
            }
        }
    }

    void constrainPlayer() {
        // Manter o jogador dentro dos limites do mundo
        float x = player.getX();
        float z = player.getZ();

        if (x > WORLD_SIZE) x = WORLD_SIZE;
        if (x < -WORLD_SIZE) x = -WORLD_SIZE;
        if (z > WORLD_SIZE) z = WORLD_SIZE;
        if (z < -WORLD_SIZE) z = -WORLD_SIZE;

        player.setPosition(x, player.getY(), z);
    }

    void handleKeyPress(unsigned char key, int x, int y) {
        if (gameMode == 2) {
            // Menu de habilidades
            if (key == 'k') {  // K
                gameMode = 0;  // Voltar para exploração
            } else if (key >= '1' && key <= '9') {
                int skillIndex = key - '1';
                const auto& skills = player.getSkillTree().getSkills();

                if (skillIndex < static_cast<int>(skills.size())) {
                    std::string skillName = skills[skillIndex]->getName();
                    if (player.getSkillTree().useSkillPoint(skillName)) {
                        std::cout << "Habilidade melhorada: " << skillName << std::endl;
                    } else {
                        std::cout << "Não foi possível melhorar a habilidade." << std::endl;
                    }
                }
            }
            return;
        }

        // Controles normais
        switch (key) {
            case 'w': case 'W':
                player.moveForward();
                break;
            case 's': case 'S':
                player.moveBackward();
                break;
            case 't':
                topDownView = !topDownView;
                break;
            case 'a': case 'A':
                player.strafeLeft();
                break;
            case 'd': case 'D':
                player.strafeRight();
                break;
            case 'q': case 'Q':
                player.rotateLeft();
                break;
            case 'e': case 'E':
                player.rotateRight();
                break;
            case 'c': case 'C':
                // Alternar distância da câmera
                cameraDistance = (cameraDistance == 5.0f) ? 2.0f : 5.0f;
                break;
            case ' ':
                // Atacar
                if (player.attack()) {
                    std::cout << "Atacando!" << std::endl;

                    for (auto& object : gameObjects) {
                        Enemy* enemy = dynamic_cast<Enemy*>(object.get());
                        if (enemy && enemy->isActive()) {
                            float dx = enemy->getX() - player.getX();
                            float dz = enemy->getZ() - player.getZ();
                            float dist = std::sqrt(dx*dx + dz*dz);

                            if (dist < 2.0f) {
                                enemy->takeDamage(player.getAttackDamage());
                                std::cout << "Dano causado em inimigo: " << player.getAttackDamage() << std::endl;
                            }
                        }
                    }
                }
                break;
            case '1':
                // Ataque alternativo
                if (gameMode == 1 && player.attack()) {
                    std::cout << "Ataque Especial!" << std::endl;

                    for (auto& object : gameObjects) {
                        Enemy* enemy = dynamic_cast<Enemy*>(object.get());
                        if (enemy && enemy->isActive()) {
                            float dx = enemy->getX() - player.getX();
                            float dz = enemy->getZ() - player.getZ();
                            float dist = std::sqrt(dx*dx + dz*dz);

                            if (dist < 3.0f) {  // Alcance maior
                                float damage = player.getAttackDamage() * 1.5f;  // Dano aumentado
                                enemy->takeDamage(damage);
                                std::cout << "Dano crítico causado: " << damage << std::endl;
                            }
                        }
                    }
                }
                break;
            case 'k': case 'K':  // K
                // Alternar para o menu de habilidades
                gameMode = (gameMode == 2) ? 0 : 2;
                break;
            case 27:  // ESC
                exit(0);
                break;
        }
    }

    void handleSpecialKeyPress(int key, int x, int y) {
        switch (key) {
            case GLUT_KEY_UP:
                cameraHeight += 0.1f;
                break;
            case GLUT_KEY_DOWN:
                cameraHeight -= 0.1f;
                if (cameraHeight < 0.5f) cameraHeight = 0.5f;
                break;
        }
    }

    // Adicionar estes métodos à classe Game
    void handleMouseClick(int button, int state, int x, int y) {
        lastMouseX = x;
        lastMouseY = y;

        if (button == GLUT_LEFT_BUTTON) {
            mouseLeftDown = (state == GLUT_DOWN);

            // Verificar clique na árvore de habilidades
            if (mouseLeftDown && gameMode == 2) {
                checkSkillTreeClick(x, y);
            }
        } else if (button == GLUT_RIGHT_BUTTON) {
            mouseRightDown = (state == GLUT_DOWN);
        }
    }

    void handleMouseMotion(int x, int y) {
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        if (mouseRightDown) {
            // Rotacionar a câmera com o mouse direito
            player.rotateRight(deltaX * mouseSensitivity);

            // Ajustar altura da câmera com movimento vertical do mouse
            cameraHeight -= deltaY * mouseSensitivity * 0.05f;
            if (cameraHeight < 0.5f) cameraHeight = 0.5f;
            if (cameraHeight > 5.0f) cameraHeight = 5.0f;
        }

        if (gameMode == 2) {
            // Atualizar estado de hover nos nós da árvore de habilidades
            updateSkillNodeHover(x, y);
        }

        lastMouseX = x;
        lastMouseY = y;
    }

    void handlePassiveMouseMotion(int x, int y) {
        lastMouseX = x;
        lastMouseY = y;

        if (gameMode == 2) {
            // Atualizar estado de hover nos nós da árvore de habilidades
            updateSkillNodeHover(x, y);
        }
    }

    void checkSkillTreeClick(int x, int y) {
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
        y = windowHeight - y; // Inverter coordenada Y para corresponder ao nosso sistema

        for (const auto& node : skillNodes) {
            float dx = x - node.x;
            float dy = y - node.y;
            float distSq = dx*dx + dy*dy;

            if (distSq < node.radius * node.radius) {
                // Nó clicado
                const auto& skills = player.getSkillTree().getSkills();
                if (node.skillIndex < static_cast<int>(skills.size())) {
                    std::string skillName = skills[node.skillIndex]->getName();
                    if (player.getSkillTree().useSkillPoint(skillName)) {
                        std::cout << "Habilidade melhorada: " << skillName << std::endl;
                    } else {
                        std::cout << "Não foi possível melhorar a habilidade." << std::endl;
                    }
                }
                break;
            }
        }
    }

    void updateSkillNodeHover(int x, int y) {
        int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
        y = windowHeight - y; // Inverter coordenada Y


        for (auto& node : skillNodes) {
            float dx = x - node.x;
            float dy = y - node.y;
            float distSq = dx*dx + dy*dy;

            node.hovering = (distSq < node.radius * node.radius);

        }
    }

    // Getters e setters
    Player& getPlayer() { return player; }
    int getGameMode() const { return gameMode; }
    void setGameMode(int mode) { gameMode = mode; }

    // Responders de eventos
    static void mouseCallback(int button, int state, int x, int y) {
        GetInstance().handleMouseClick(button, state, x, y);
    }

    static void motionCallback(int x, int y) {
        GetInstance().handleMouseMotion(x, y);
    }

    static void passiveMotionCallback(int x, int y) {
        GetInstance().handlePassiveMouseMotion(x, y);
    }
    static void displayCallback() {
        GetInstance().render();
        glutSwapBuffers();
    }

    static void reshapeCallback(int w, int h) {
        glViewport(0, 0, w, h);
    }

    static void keyboardCallback(unsigned char key, int x, int y) {
        GetInstance().handleKeyPress(key, x, y);
    }

    static void specialCallback(int key, int x, int y) {
        GetInstance().handleSpecialKeyPress(key, x, y);
    }

    static void timerCallback(int value) {
        GetInstance().update();
        glutPostRedisplay();
        glutTimerFunc(16, timerCallback, 0);  // 60 FPS aproximadamente
    }

    // Singleton para facilitar callbacks
    static Game& GetInstance() {
        static Game instance;
        return instance;
    }
};

// Funções globais para callbacks do GLUT
void display() { Game::displayCallback(); }
void reshape(int w, int h) { Game::reshapeCallback(w, h); }
void keyboard(unsigned char key, int x, int y) { Game::keyboardCallback(key, x, y); }
void specialKeys(int key, int x, int y) { Game::specialCallback(key, x, y); }
void update(int value) { Game::timerCallback(value); }
void mouse(int button, int state, int x, int y) { Game::mouseCallback(button, state, x, y); }
void motion(int x, int y) { Game::motionCallback(x, y); }
void passiveMotion(int x, int y) { Game::passiveMotionCallback(x, y); }

void init() {
    // Configurações de OpenGL
    glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Configurar luz
    GLfloat light_position[] = {0.0f, 10.0f, 0.0f, 1.0f};
    GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

    // Inicializar a semente aleatória
    srand(static_cast<unsigned int>(time(nullptr)));

    // Inicializar o jogo (já é feito no construtor do singleton)
}

int main(int argc, char** argv) {
    // Inicializar GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("RPG 3D em C++ com OpenGL");

    // Registrar callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(16, update, 0);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(passiveMotion);

    // Inicializar OpenGL
    init();

    // Iniciar loop principal
    glutMainLoop();

    return 0;
}
