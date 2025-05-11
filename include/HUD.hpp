#ifndef HUD_HPP
#define HUD_HPP

#include <GL/glut.h>
#include "player.hpp"
#include "skillTree.hpp"
#include "skill.hpp"
#include "data.hpp"

class HUD
{
public:
    HUD(Player &player);
    void drawHUD(Player &player, STATE_GAME gameMode, bool showPortalMessage, bool isOpenHouse);
    void drawMainHUD(Player &player, STATE_GAME &gameMode, ACTION_BUTTON &action, Volume &volume);
    void drawSkillTree(std::vector<SkillNode> &skillN, SkillTooltip &skillTooltip);
    void addButtonMenu(float r, float g, float b, float x, float y, const char *text);
    void renderRoundedButton(float x, float y, float width, float height, float radius);
    std::vector<Botao> getButtonMenu();
    void setHoveredButton(int id) { hoveredButtonID = id; }
    int getHoveredBUtton() { return hoveredButtonID; }
    void TriggerScreenshotAnimation();
    void screenshotAnimation(float deltaTime);

private:
    Player &player;
    int hoveredButtonID = -1;
    std::vector<Botao> botoesMenu;

    void drawText(float x, float y, const char *text, int fontSize);
    void calculateSkillTreeLayout(std::vector<SkillNode> &skillNodes);
    void drawSkillTreeConnections(std::vector<SkillNode> &skillNodes);
    void drawSkillTreeNodes(std::vector<SkillNode> &skillNodes);
    void drawSkillIcon(float x, float y, SkillType type);
    void drawFilledCircle(float x, float y, float radius, int segments);
    void drawSkillTooltip(SkillTooltip &skillTooltip);
    void drawStar(float x, float y, float radius, int points);
    void drawCircle(float x, float y, float radius, int segments);

    void renderizarControles();
    void renderizarMenuCreditos();
    void renderizarMenuConfiguracoes(Volume &volume);
    void renderizarMenuPrincipal();
    void renderizarTelaDesejaJogar();
};

#endif // HUD_HPP
