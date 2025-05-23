#pragma once

#include <vector>
#include <memory>
#include <string>
#include "Skill.hpp" 

class SkillTree {
private:
    std::vector<std::unique_ptr<Skill>> skills;
    int skillPoints;

public:
    SkillTree();

    void addSkillPoint();
    bool useSkillPoint(const std::string& skillName);

    int getSkillPoints() const;
    const std::vector<std::unique_ptr<Skill>>& getSkills() const;
    float getSkillValue(SkillType type) const;
    void reset();
};
