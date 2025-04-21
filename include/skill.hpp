#ifndef SKILL_HPP
#define SKILL_HPP

#include <string>
#include <vector>
#include "data.hpp"

class Skill {
private:
    std::string name;
    SkillType type;
    int level;
    int maxLevel;
    float value;                       // valor do efeito da habilidade
    std::vector<Skill*> prerequisites; // Habilidades necessárias para desbloquear esta

public:
    Skill(const std::string &name, SkillType type, int maxLevel, float baseValue);
    
    void addPrerequisite(Skill* skill);
    bool canLearn() const;
    bool upgrade();
    
    // Getters
    const std::string& getName() const;
    SkillType getType() const;
    int getLevel() const;
    int getMaxLevel() const;
    float getValue() const;
};

#endif // SKILL_HPP