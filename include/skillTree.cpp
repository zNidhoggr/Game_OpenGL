
#include "skillTree.hpp"

SkillTree::SkillTree() : skillPoints(0) {
    auto attackSkill = std::make_unique<Skill>("Força de Ataque", ATTACK, 5, 5.0f);
    auto defenseSkill = std::make_unique<Skill>("Resistência", DEFENSE, 5, 3.0f);
    auto magicSkill = std::make_unique<Skill>("Poder Mágico", MAGIC, 5, 4.0f);
    auto speedSkill = std::make_unique<Skill>("Agilidade", SPEED, 5, 2.0f);

    auto criticalStrike = std::make_unique<Skill>("Golpe Crítico", ATTACK, 3, 10.0f);
    auto healthRegen = std::make_unique<Skill>("Regeneração", DEFENSE, 3, 2.0f);
    auto fireball = std::make_unique<Skill>("Bola de Fogo", MAGIC, 3, 15.0f);
    auto dodge = std::make_unique<Skill>("Esquiva", SPEED, 3, 5.0f);

    criticalStrike->addPrerequisite(attackSkill.get());
    healthRegen->addPrerequisite(defenseSkill.get());
    fireball->addPrerequisite(magicSkill.get());
    dodge->addPrerequisite(speedSkill.get());

    skills.push_back(std::move(attackSkill));
    skills.push_back(std::move(defenseSkill));
    skills.push_back(std::move(magicSkill));
    skills.push_back(std::move(speedSkill));
    skills.push_back(std::move(criticalStrike));
    skills.push_back(std::move(healthRegen));
    skills.push_back(std::move(fireball));
    skills.push_back(std::move(dodge));
}

void SkillTree::addSkillPoint() {
    skillPoints++;
}

bool SkillTree::useSkillPoint(const std::string& skillName) {
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

int SkillTree::getSkillPoints() const {
    return skillPoints;
}

const std::vector<std::unique_ptr<Skill>>& SkillTree::getSkills() const {
    return skills;
}

float SkillTree::getSkillValue(SkillType type) const {
    float total = 0.0f;
    for (const auto& skill : skills) {
        if (skill->getType() == type) {
            total += skill->getValue() * skill->getLevel();
        }
    }
    return total;
}

void SkillTree::reset()
{
    skillPoints = 0;

    for (auto &skill : skills)
    {
        skill->reset();
    }
}
