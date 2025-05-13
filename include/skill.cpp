#include "skill.hpp"

Skill::Skill(const std::string& name, SkillType type, int maxLevel, float baseValue)
        : name(name), type(type), level(0), maxLevel(maxLevel), value(baseValue) {}

void Skill::addPrerequisite(Skill* skill){
    prerequisites.push_back(skill);
}

bool Skill::canLearn() const{
    if (level >= maxLevel) return false;
    
    for (const auto &prereq : prerequisites){
        if (prereq->getLevel() == 0) return false;
    }

    return true;
}

bool Skill::upgrade(){
    if (!canLearn()) return false;
    
    if (level < maxLevel){
        level++;
        value *= 1.2f; 
        return true;
    }
    
    return false;
}

const std::string& Skill::getName() const{ return name; }
SkillType Skill::getType() const { return type; }
int Skill::getLevel() const { return level;}
int Skill::getMaxLevel() const { return maxLevel; }
float Skill::getValue() const { return value;}
void Skill::reset() { level = 0; }
