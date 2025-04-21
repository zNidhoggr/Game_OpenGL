
#ifndef DATA_HPP
#define DATA_HPP

#include <vector>
#include <string>

// Enumeração para tipos de objetos
enum ObjectType
{
    TREE,
    ROCK,
    HOUSE,
    GRASS,
    WALL,
    ENEMY,
    ITEM,
    NPC,
    PORTAL
};

// Enumeração para tipos de habilidade
enum SkillType
{
    ATTACK,
    DEFENSE,
    MAGIC,
    SPEED
};

enum class MapType
{
    MAIN,
    DUNGEON
};

struct GrassPatch
{
    float x, z;   // centro
    float radius; // raio
};

struct TrailPoint
{
    float x, y, z;
};

struct SkillTooltip
{
    bool visible;
    int skillIndex;
    float x, y; // Posição na tela
    float width, height;
    bool showConfirmation;
};

struct SkillNode
{
    float x, y;     // Posição na tela
    float radius;   // Raio do nó
    int skillIndex; // Índice da habilidade
    bool hovering;  // Se o mouse está sobre o nó
};

#endif