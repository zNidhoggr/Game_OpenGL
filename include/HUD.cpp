#include "HUD.hpp"
#include "skillTree.hpp"
#include <GL/glut.h>
#include <cstdio>
#include "data.hpp"

HUD::HUD(Player &player) : player(player) {}

void HUD::drawHUD(Player &player, STATE_GAME gameMode, bool showPortalMessage, bool isOpenHouse)
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    
    float barWidth = 220.0f;
    float barHeight = 24.0f;
    float barSpacing = 34.0f;
    float barStartY = windowHeight - 40;
    float barStartX = 20;
    
    float healthPercent = player.getHealth() / player.getMaxHealth();
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(barStartX, barStartY);
    glVertex2f(barStartX + barWidth, barStartY);
    glVertex2f(barStartX + barWidth, barStartY - barHeight);
    glVertex2f(barStartX, barStartY - barHeight);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.0f, 0.0f); 
    glVertex2f(barStartX, barStartY);
    glVertex2f(barStartX, barStartY - barHeight);
    glColor3f(1.0f, 0.3f, 0.3f); 
    glVertex2f(barStartX + barWidth * healthPercent, barStartY - barHeight);
    glVertex2f(barStartX + barWidth * healthPercent, barStartY);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    char buffer[128];
    sprintf(buffer, "Vida: %.1f/%.1f", player.getHealth(), player.getMaxHealth());
    drawText(barStartX + 5, barStartY - barHeight/2 - 5, buffer, 12);

    float expPercent = (float)player.getExperience() / (float)player.getExperienceToNextLevel();
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(barStartX, barStartY - barSpacing);
    glVertex2f(barStartX + barWidth, barStartY - barSpacing);
    glVertex2f(barStartX + barWidth, barStartY - barSpacing - barHeight);
    glVertex2f(barStartX, barStartY - barSpacing - barHeight);
    glEnd();


    glBegin(GL_QUADS);
    glColor3f(0.1f, 0.3f, 0.8f);  
    glVertex2f(barStartX, barStartY - barSpacing);
    glVertex2f(barStartX, barStartY - barSpacing - barHeight);
    glColor3f(0.4f, 0.7f, 1.0f);   
    glVertex2f(barStartX + barWidth * expPercent, barStartY - barSpacing - barHeight);
    glVertex2f(barStartX + barWidth * expPercent, barStartY - barSpacing);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    sprintf(buffer, "Nivel: %d   XP: %d/%d", player.getLevel(), player.getExperience(), player.getExperienceToNextLevel());
    drawText(barStartX + 5, barStartY - barSpacing - barHeight/2 - 5, buffer, 12);

    float cooldownPercent = player.getAttackTimer() / player.getAttackCooldown();
    if (cooldownPercent > 0) {
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(barStartX, barStartY - barSpacing * 2);
        glVertex2f(barStartX + barWidth, barStartY - barSpacing * 2);
        glVertex2f(barStartX + barWidth, barStartY - barSpacing * 2 - barHeight);
        glVertex2f(barStartX, barStartY - barSpacing * 2 - barHeight);
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0.8f, 0.6f, 0.1f); 
        glVertex2f(barStartX, barStartY - barSpacing * 2);
        glVertex2f(barStartX, barStartY - barSpacing * 2 - barHeight);
        glColor3f(1.0f, 0.8f, 0.2f); 
        glVertex2f(barStartX + barWidth * (1.0f - cooldownPercent), barStartY - barSpacing * 2 - barHeight);
        glVertex2f(barStartX + barWidth * (1.0f - cooldownPercent), barStartY - barSpacing * 2);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        sprintf(buffer, "Ataque: %.1f", player.getAttackCooldown() - player.getAttackTimer());
        drawText(barStartX + 5, barStartY - barSpacing * 2 - barHeight/2 - 5, buffer, 12);
    } else {
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(barStartX, barStartY - barSpacing * 2);
        glVertex2f(barStartX + barWidth, barStartY - barSpacing * 2);
        glVertex2f(barStartX + barWidth, barStartY - barSpacing * 2 - barHeight);
        glVertex2f(barStartX, barStartY - barSpacing * 2 - barHeight);
        glEnd();

        glColor3f(0.2f, 0.8f, 0.2f);
        glBegin(GL_QUADS);
        glVertex2f(barStartX, barStartY - barSpacing * 2);
        glVertex2f(barStartX + barWidth, barStartY - barSpacing * 2);
        glVertex2f(barStartX + barWidth, barStartY - barSpacing * 2 - barHeight);
        glVertex2f(barStartX, barStartY - barSpacing * 2 - barHeight);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(barStartX + 5, barStartY - barSpacing * 2 - barHeight/2 - 5, "Ataque Pronto!", 12);
    }

    if (gameMode == STATE_GAME::COMBAT) {
        glColor3f(1.0f, 0.2f, 0.2f);
        drawText(windowWidth / 2 - 50, windowHeight - 25, "COMBATE", 14);
        drawText(10, 20, "Pressione 1 para atacar", 12);
    } else if (gameMode == STATE_GAME::SKILL_TREE) {
        glColor3f(0.2f, 0.7f, 1.0f);
        drawText(windowWidth / 2 - 100, windowHeight - 25, "MENU DE HABILIDADES", 14);
        drawText(10, 20, "Use as teclas numericas (1-9) para melhorar habilidades", 12);
    } else {
        glColor3f(0.2f, 1.0f, 0.2f);
        drawText(windowWidth / 2 - 60, windowHeight - 25, "EXPLORACAO", 14);
    }

    if (player.getSkillTree().getSkillPoints() > 0) {
        glColor3f(1.0f, 1.0f, 0.0f);
        sprintf(buffer, "Pontos de Habilidade: %d (Pressione K para abrir menu)", player.getSkillTree().getSkillPoints());
        drawText(windowWidth - 400, windowHeight - 25, buffer, 12);
    }

    if (showPortalMessage) {
        glColor3f(1.0f, 1.0f, 0.0f);
        drawText(windowWidth / 2 - 100, 100, "Pressione [Enter] para entrar", 12);
    }
    if (isOpenHouse) {
        glColor3f(1.0f, 1.0f, 0.0f);
        drawText(windowWidth / 2 - 100, 100, "Pressione [Enter] para entrar", 12);
    }


    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
}void HUD::drawSkillTree(std::vector<SkillNode> &skillN, SkillTooltip &skillTooltip)
{
    calculateSkillTreeLayout(skillN);
    

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    const int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    const int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    gluOrtho2D(0, windowWidth, 0, windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    const float panelPadding = 50.0f;
    const float panelAlpha = 0.85f;
    glColor4f(0.0f, 0.0f, 0.2f, panelAlpha);
    glBegin(GL_QUADS);
    glVertex2f(panelPadding, panelPadding);
    glVertex2f(windowWidth - panelPadding, panelPadding);
    glVertex2f(windowWidth - panelPadding, windowHeight - panelPadding);
    glVertex2f(panelPadding, windowHeight - panelPadding);
    glEnd();

    const float centerX = windowWidth / 2.0f;
    glColor3f(1.0f, 1.0f, 0.0f);
    drawText(centerX - 80, windowHeight - 80, "ÁRVORE DE HABILIDADES", 14);

    char buffer[128];
    const int availablePoints = player.getSkillTree().getSkillPoints();
    std::snprintf(buffer, sizeof(buffer), "Pontos Disponíveis: %d", availablePoints);
    
    if (availablePoints > 0) {
        glColor3f(0.2f, 1.0f, 0.2f);
    } else {
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    drawText(centerX - 80, windowHeight - 110, buffer, 14);

    drawSkillTreeConnections(skillN);
    drawSkillTreeNodes(skillN);
    drawSkillTooltip(skillTooltip);

    glColor3f(0.9f, 0.9f, 0.9f);
    drawText(centerX - 100, 80, "Pressione 'K' para voltar ao jogo", 12);
    drawText(centerX - 140, 60, "Use o mouse para selecionar habilidades", 12);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}

void HUD::calculateSkillTreeLayout(std::vector<SkillNode> &skillNodes)
{
    const int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    const int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    const auto &skills = player.getSkillTree().getSkills();

    skillNodes.clear();

    const float centerX = windowWidth / 2.0f;
    const float centerY = windowHeight / 2.0f;
    const float baseRadius = 32.0f; 
    const float nodeSpacing = 160.0f; 

    const float angles[4] = {0, 90, 180, 270}; 
    for (int i = 0; i < 4; i++)
    {
        const float angleRad = angles[i] * M_PI / 180.0f;
        const float x = centerX + cos(angleRad) * nodeSpacing;
        const float y = centerY + sin(angleRad) * nodeSpacing;

        SkillNode node;
        node.x = x;
        node.y = y;
        node.radius = baseRadius;
        node.skillIndex = i;
        node.hovering = false;

        skillNodes.push_back(node);
    }

    // Definir posições das habilidades secundárias (nível 2)
    for (int i = 0; i < 4; i++)
    {
        const float angleRad = angles[i] * M_PI / 180.0f;
        const float x = centerX + cos(angleRad) * nodeSpacing * 2.0f;
        const float y = centerY + sin(angleRad) * nodeSpacing * 2.0f;

        SkillNode node;
        node.x = x;
        node.y = y;
        node.radius = baseRadius * 0.9f; // Nós secundários ligeiramente menores
        node.skillIndex = i + 4;
        node.hovering = false;

        skillNodes.push_back(node);
    }
}

void HUD::drawSkillTreeConnections(std::vector<SkillNode> &skillNodes)
{
    const auto &skills = player.getSkillTree().getSkills();
    
    // Desenhar linhas de conexão entre nós
    for (int i = 0; i < 4; i++)
    {
        const int parentIndex = i;
        const int childIndex = i + 4;
        const auto &parentSkill = skills[parentIndex];
        const auto &childSkill = skills[childIndex];
        
        // Determinar cor da conexão baseada no estado das habilidades
        if (childSkill->getLevel() > 0) {
            // Conexão ativa (ambas habilidades desbloqueadas)
            glColor4f(0.2f, 0.8f, 0.2f, 0.9f);
            glLineWidth(3.0f);
        }
        else if (parentSkill->getLevel() > 0) {
            // Conexão potencial (pai desbloqueado, filho disponível)
            glColor4f(0.9f, 0.9f, 0.2f, 0.8f);
            glLineWidth(2.5f);
        }
        else {
            // Conexão inativa (bloqueada)
            glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
            glLineWidth(1.5f);
        }

        // Desenhar linha com gradiente
        glBegin(GL_LINES);
        const float &x1 = skillNodes[parentIndex].x;
        const float &y1 = skillNodes[parentIndex].y;
        const float &x2 = skillNodes[childIndex].x;
        const float &y2 = skillNodes[childIndex].y;
        
        if (childSkill->getLevel() > 0 || parentSkill->getLevel() > 0) {
            // Adicionar efeito de brilho para linhas ativas ou potenciais
            const int segments = 8;
            for (int s = 0; s < segments; s++) {
                float t1 = (float)s / segments;
                float t2 = (float)(s + 1) / segments;
                
                float xA = x1 + (x2 - x1) * t1;
                float yA = y1 + (y2 - y1) * t1;
                float xB = x1 + (x2 - x1) * t2;
                float yB = y1 + (y2 - y1) * t2;
                
                glVertex2f(xA, yA);
                glVertex2f(xB, yB);
            }
        } else {
            // Linha simples para conexões inativas
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
        }
        glEnd();
    }
}

void HUD::drawSkillTreeNodes(std::vector<SkillNode> &skillNodes)
{
    const auto &skills = player.getSkillTree().getSkills();
    const int availablePoints = player.getSkillTree().getSkillPoints();
    
    // Primeira passagem: desenhar bordas pulsantes para nós que podem ser melhorados
    static float pulseValue = 0.0f;
    pulseValue += 0.1f;
    if (pulseValue > 4.0f * M_PI) pulseValue = 0.0f;
    const float pulse = 0.1f + 1.0f * sin(pulseValue);
    
    // Primeira passagem: desenhar círculos de fundo
    for (size_t i = 0; i < skillNodes.size(); i++)
    {
        const auto &node = skillNodes[i];
        const auto &skill = skills[node.skillIndex];
        
        // Determinar cor e estilo do nó baseado no estado
        float alpha = node.hovering ? 0.95f : 0.8f;
        
        if (skill->getLevel() == 0) {
            if (skill->canLearn() && availablePoints > 0) {
                // Disponível para aprender
                glColor4f(0.9f, 0.9f, 0.2f, alpha);
            } else {
                // Bloqueado
                glColor4f(0.5f, 0.5f, 0.5f, alpha * 0.8f);
            }
        } else if (skill->getLevel() == skill->getMaxLevel()) {
            // Nível máximo
            glColor4f(1.0f, 0.6f, 0.0f, alpha);
        } else {
            if (skill->canLearn() && availablePoints > 0) {
                // Pode ser melhorado
                glColor4f(0.2f, 0.8f, 0.2f, alpha);
            } else {
                // Desbloqueado mas não pode ser melhorado agora
                glColor4f(0.2f, 0.6f, 0.8f, alpha);
            }
        }

        // Desenhar círculo de fundo
        drawFilledCircle(node.x, node.y, node.radius, 24);
        
        // Desenhar borda
        if (node.hovering) {
            glLineWidth(2.0f);
            glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
        } else if (skill->canLearn() && availablePoints > 0) {
            // Efeito pulsante para nós que podem ser melhorados
            glLineWidth(2.0f + pulse);
            glColor4f(1.0f, 1.0f, 0.5f, 0.6f + 0.4f * pulse);
        } else {
            glLineWidth(1.5f);
            glColor4f(0.8f, 0.8f, 0.8f, 0.7f);
        }
        
        drawCircle(node.x, node.y, node.radius, 24);
        
        // Desenhar ícone de habilidade
        drawSkillIcon(node.x, node.y - 16, skill->getType());
        
        // Desenhar texto do nível
        char levelText[8];
        sprintf(levelText, "%d/%d", skill->getLevel(), skill->getMaxLevel());
        float textWidth = strlen(levelText) * 8.0f;
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(node.x - textWidth / 2, node.y + 6, levelText, 12);
    }
}

void HUD::drawSkillIcon(float x, float y, SkillType type)
{
    glLineWidth(2.5f);
    const float iconSize = 10.0f; // Tamanho aumentado
    
    // Definir cores específicas para cada tipo de habilidade
    switch (type)
    {
    case ATTACK:
        glColor3f(1.0f, 0.4f, 0.4f); // Vermelho para ataque
        break;
    case DEFENSE:
        glColor3f(0.4f, 0.7f, 1.0f); // Azul para defesa
        break;
    case MAGIC:
        glColor3f(0.8f, 0.4f, 1.0f); // Roxo para magia
        break;
    case SPEED:
        glColor3f(0.4f, 1.0f, 0.4f); // Verde para velocidade
        break;
    default:
        glColor3f(1.0f, 1.0f, 1.0f);
        break;
    }
    
    // Desenhar ícones mais elaborados para cada tipo
    switch (type)
    {
    case ATTACK:
        // Símbolo de espada
        glBegin(GL_LINES);
        glVertex2f(x - iconSize, y + iconSize);
        glVertex2f(x + iconSize, y - iconSize);
        glVertex2f(x - iconSize / 2, y - iconSize / 2);
        glVertex2f(x + iconSize / 2, y + iconSize / 2);
        glEnd();
        
        // Punho da espada
        glBegin(GL_QUADS);
        glVertex2f(x - iconSize/4, y - iconSize/4);
        glVertex2f(x + iconSize/4, y + iconSize/4);
        glVertex2f(x + iconSize/2, y);
        glVertex2f(x - iconSize/2, y - iconSize/2);
        glEnd();
        break;

    case DEFENSE:
        // Escudo
        drawFilledCircle(x, y, iconSize * 0.8f, 12);
        glColor3f(0.2f, 0.3f, 0.6f);
        drawFilledCircle(x, y, iconSize * 0.5f, 12);
        glColor3f(0.4f, 0.7f, 1.0f);
        drawCircle(x, y, iconSize * 0.8f, 12);
        break;

    case MAGIC:
    {
        // Estrela mágica mais elaborada
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // Centro
        const int points = 5;
        const float innerRadius = iconSize * 0.4f;
        const float outerRadius = iconSize;
        
        for (int i = 0; i <= points * 2; i++) {
            float theta = M_PI * i / points - M_PI / 2;
            float r = (i % 2 == 0) ? outerRadius : innerRadius;
            float px = x + r * cosf(theta);
            float py = y + r * sinf(theta);
            glVertex2f(px, py);
        }
        glEnd();
        break;
    }

    case SPEED:
        // Seta de velocidade mais elaborada
        glBegin(GL_TRIANGLES);
        // Ponta da seta
        glVertex2f(x + iconSize * 1.2f, y);
        glVertex2f(x, y + iconSize * 0.6f);
        glVertex2f(x, y - iconSize * 0.6f);
        glEnd();
        
        // Corpo da seta
        glBegin(GL_QUADS);
        glVertex2f(x, y + iconSize * 0.3f);
        glVertex2f(x - iconSize, y + iconSize * 0.3f);
        glVertex2f(x - iconSize, y - iconSize * 0.3f);
        glVertex2f(x, y - iconSize * 0.3f);
        glEnd();
        break;
    }
}

void HUD::drawSkillTooltip(SkillTooltip &skillTooltip)
{
    if (!skillTooltip.visible)
        return;
        
    const auto &skills = player.getSkillTree().getSkills();
    const auto &skill = skills[skillTooltip.skillIndex];
    const int availablePoints = player.getSkillTree().getSkillPoints();

    // Adicionar efeito de transição suave
    static float tooltipAlpha = 0.0f;
    tooltipAlpha = std::min(1.0f, tooltipAlpha + 0.1f);
    
    // Fundo do tooltip com borda gradiente
    glColor4f(0.1f, 0.1f, 0.3f, 0.95f * tooltipAlpha);
    glBegin(GL_QUADS);
    glVertex2f(skillTooltip.x, skillTooltip.y);
    glVertex2f(skillTooltip.x + skillTooltip.width, skillTooltip.y);
    glVertex2f(skillTooltip.x + skillTooltip.width, skillTooltip.y + skillTooltip.height);
    glVertex2f(skillTooltip.x, skillTooltip.y + skillTooltip.height);
    glEnd();

    // Borda com gradiente
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    // Cores diferentes para cada lado
    glColor4f(0.8f, 0.4f, 0.8f, tooltipAlpha);
    glVertex2f(skillTooltip.x, skillTooltip.y);
    glColor4f(0.4f, 0.8f, 0.8f, tooltipAlpha);
    glVertex2f(skillTooltip.x + skillTooltip.width, skillTooltip.y);
    glColor4f(0.8f, 0.8f, 0.4f, tooltipAlpha);
    glVertex2f(skillTooltip.x + skillTooltip.width, skillTooltip.y + skillTooltip.height);
    glColor4f(0.4f, 0.8f, 0.4f, tooltipAlpha);
    glVertex2f(skillTooltip.x, skillTooltip.y + skillTooltip.height);
    glEnd();

    // Título
    glColor4f(1.0f, 1.0f, 0.0f, tooltipAlpha);
    drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 25, skill->getName().c_str(), 12);

    // Informações
    glColor4f(1.0f, 1.0f, 1.0f, tooltipAlpha);
    char buffer[128];

    // Tipo de habilidade com cores correspondentes
    const char *typeStr = "";
    float typeRed = 1.0f, typeGreen = 1.0f, typeBlue = 1.0f;
    
    switch (skill->getType())
    {
    case ATTACK:
        typeStr = "Ataque";
        typeRed = 1.0f; typeGreen = 0.4f; typeBlue = 0.4f;
        break;
    case DEFENSE:
        typeStr = "Defesa";
        typeRed = 0.4f; typeGreen = 0.7f; typeBlue = 1.0f;
        break;
    case MAGIC:
        typeStr = "Magia";
        typeRed = 0.8f; typeGreen = 0.4f; typeBlue = 1.0f;
        break;
    case SPEED:
        typeStr = "Velocidade";
        typeRed = 0.4f; typeGreen = 1.0f; typeBlue = 0.4f;
        break;
    }
    
    sprintf(buffer, "Tipo: %s", typeStr);
    glColor4f(typeRed, typeGreen, typeBlue, tooltipAlpha);
    drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 45, buffer, 10);

    // Nível
    sprintf(buffer, "Nivel: %d/%d", skill->getLevel(), skill->getMaxLevel());
    if (skill->getLevel() == skill->getMaxLevel()) {
        glColor4f(1.0f, 0.8f, 0.0f, tooltipAlpha); // Dourado para nível máximo
    } else {
        glColor4f(1.0f, 1.0f, 1.0f, tooltipAlpha);
    }
    drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 65, buffer, 10);

    // Valor com bônus destacado se tiver níveis
    float currentValue = skill->getValue();
    // Como não temos o método getNextLevelValue(), vamos estimar um valor aproximado
    // Supondo que cada nível aumente o valor em ~25%
    float valueIncrement = currentValue * 0.25f;
    float nextValue = currentValue + valueIncrement;
    
    sprintf(buffer, "Valor Atual: %.1f", currentValue);
    glColor4f(1.0f, 1.0f, 1.0f, tooltipAlpha);
    drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 85, buffer, 10);
    
    if (skill->getLevel() < skill->getMaxLevel() && skill->canLearn()) {
        sprintf(buffer, "Próximo Nível: %.1f (+%.1f)", nextValue, valueIncrement);
        glColor4f(0.2f, 1.0f, 0.2f, tooltipAlpha);
        drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 105, buffer, 10);
    }

    // Custo
    sprintf(buffer, "Custo: 1 ponto de habilidade");
    glColor4f(1.0f, 1.0f, 1.0f, tooltipAlpha);
    drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 125, buffer, 10);

    // Status de desbloqueio
    if (!skill->canLearn()) {
        glColor4f(1.0f, 0.3f, 0.3f, tooltipAlpha);
        if (skill->getLevel() >= skill->getMaxLevel()) {
            drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 145, "Nível máximo atingido", 10);
        } else {
            drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 145, "Pré-requisitos não cumpridos", 10);
            
            // Adicionar informações sobre pré-requisitos
            int parentIndex = skillTooltip.skillIndex % 4; // Obter índice da habilidade pai
            const auto &parentSkill = skills[parentIndex];
            sprintf(buffer, "Requer: %s Nível %d", parentSkill->getName().c_str(), 1);
            drawText(skillTooltip.x + 20, skillTooltip.y + skillTooltip.height - 165, buffer, 10);
        }
    } else if (availablePoints <= 0) {
        glColor4f(1.0f, 0.3f, 0.3f, tooltipAlpha);
        drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 145, "Pontos insuficientes", 10);
    } else {
        glColor4f(0.2f, 1.0f, 0.2f, tooltipAlpha);
        drawText(skillTooltip.x + 10, skillTooltip.y + skillTooltip.height - 145, "Disponível para aprender!", 10);
    }

    // Botões de confirmação
    if (skillTooltip.showConfirmation && skill->canLearn() && availablePoints > 0) {
        // Botão Aprender
        static float buttonPulse = 0.0f;
        buttonPulse += 0.1f;
        if (buttonPulse > 2.0f * M_PI) buttonPulse = 0.0f;
        
        // Botão Aprender com efeito pulsante
        const float pulse = 0.7f + 0.3f * sin(buttonPulse);
        glColor4f(0.2f, 0.7f * pulse, 0.2f, 0.9f * tooltipAlpha);
        glBegin(GL_QUADS);
        glVertex2f(skillTooltip.x + 30, skillTooltip.y + 20);
        glVertex2f(skillTooltip.x + 110, skillTooltip.y + 20);
        glVertex2f(skillTooltip.x + 110, skillTooltip.y + 45);
        glVertex2f(skillTooltip.x + 30, skillTooltip.y + 45);
        glEnd();

        // Borda do botão Aprender
        glLineWidth(1.5f);
        glColor4f(0.4f, 1.0f, 0.4f, tooltipAlpha);
        glBegin(GL_LINE_LOOP);
        glVertex2f(skillTooltip.x + 30, skillTooltip.y + 20);
        glVertex2f(skillTooltip.x + 110, skillTooltip.y + 20);
        glVertex2f(skillTooltip.x + 110, skillTooltip.y + 45);
        glVertex2f(skillTooltip.x + 30, skillTooltip.y + 45);
        glEnd();

        // Botão Cancelar
        glColor4f(0.7f, 0.2f, 0.2f, 0.8f * tooltipAlpha);
        glBegin(GL_QUADS);
        glVertex2f(skillTooltip.x + 140, skillTooltip.y + 20);
        glVertex2f(skillTooltip.x + 220, skillTooltip.y + 20);
        glVertex2f(skillTooltip.x + 220, skillTooltip.y + 45);
        glVertex2f(skillTooltip.x + 140, skillTooltip.y + 45);
        glEnd();

        // Borda do botão Cancelar
        glLineWidth(1.5f);
        glColor4f(1.0f, 0.4f, 0.4f, tooltipAlpha);
        glBegin(GL_LINE_LOOP);
        glVertex2f(skillTooltip.x + 140, skillTooltip.y + 20);
        glVertex2f(skillTooltip.x + 220, skillTooltip.y + 20);
        glVertex2f(skillTooltip.x + 220, skillTooltip.y + 45);
        glVertex2f(skillTooltip.x + 140, skillTooltip.y + 45);
        glEnd();

        // Texto dos botões
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(skillTooltip.x + 45, skillTooltip.y + 30, "Aprender", 12);
        drawText(skillTooltip.x + 165, skillTooltip.y + 30, "Cancelar", 12);
    }
}

void HUD::drawFilledCircle(float x, float y, float radius, int segments)
{
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++)
    {
        float theta = 2.0f * M_PI * i / segments;
        float px = x + radius * cosf(theta);
        float py = y + radius * sinf(theta);
        glVertex2f(px, py);
    }
    glEnd();
}

void HUD::drawCircle(float x, float y, float radius, int segments)
{
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++)
    {
        float theta = 2.0f * M_PI * i / segments;
        float px = x + radius * cosf(theta);
        float py = y + radius * sinf(theta);
        glVertex2f(px, py);
    }
    glEnd();
}

void HUD::drawStar(float x, float y, float radius, int points)
{
    const float innerRadius = radius * 0.4f;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < points * 2; i++)
    {
        float theta = M_PI * i / points;
        float r = (i % 2 == 0) ? radius : innerRadius;
        float px = x + r * cosf(theta);
        float py = y + r * sinf(theta);
        glVertex2f(px, py);
    }
    glEnd();
}

void HUD::drawText(float x, float y, const char *text, int fontSize = 12)
{
    void *font;
    switch (fontSize)
    {
    case 10:
        font = GLUT_BITMAP_HELVETICA_10;
        break;
    case 12:
        font = GLUT_BITMAP_HELVETICA_12;
        break;
    case 18:
        font = GLUT_BITMAP_HELVETICA_18;
        break;
    default:
        font = GLUT_BITMAP_HELVETICA_12;
        break;
    }
    
    glRasterPos2f(x, y);
    for (const char *c = text; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }
}
std::vector<Botao> HUD::getButtonMenu()
{
    return botoesMenu;
}

void HUD::drawMainHUD(Player &player, STATE_GAME &gameMode, ACTION_BUTTON &action, Volume &volume)
{
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    const int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    const int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(100.f, 100.f);
    glVertex2f(windowWidth - 100.f, 100.f);
    glVertex2f(windowWidth - 100.f, windowHeight - 100.f);
    glVertex2f(100.f, windowHeight - 100.f);
    glEnd();

    if (gameMode == STATE_GAME::MENU)
    {
        switch (action)
        {
        case ACTION_BUTTON::CONFIG:
        case ACTION_BUTTON::VOLUME_MUSIC_DECREASE: // Controls omnia_poetree_theme.wav, city_of_ingrigues.wav, bury-the-light-deep-within.wav
        case ACTION_BUTTON::VOLUME_MUSIC_INCREASE:

        case ACTION_BUTTON::VOLUME_EFFECTS_DECREASE: // Controls punch.wav, swipe.wav, classiccoin.wav, espacial-attack.wav, etc.
        case ACTION_BUTTON::VOLUME_EFFECTS_INCREASE:

        case ACTION_BUTTON::VOLUME_AMBIENT_DECREASE: // Controls birds_backgorund.wav, walk-on-grass.wav, dark-void-ambience.wav, etc.
        case ACTION_BUTTON::VOLUME_AMBIENT_INCREASE:

        case ACTION_BUTTON::VOLUME_UI_DECREASE: // Controls one_beep.wav, up-habilidade.wav, etc.
        case ACTION_BUTTON::VOLUME_UI_INCREASE:
            renderizarMenuConfiguracoes(volume);
            break;
        case ACTION_BUTTON::CREDITS:
            renderizarMenuCreditos();
            break;
        case ACTION_BUTTON::NONE:
            renderizarMenuPrincipal();
            break;
        case ACTION_BUTTON::CONTROLS:
            renderizarControles();
            break;
        default:
            drawText(150, windowHeight - 150, "Estado de menu desconhecido.");
            break;
        }
    }
    if (gameMode == STATE_GAME::GAME_OVER)
    {
        renderizarTelaDesejaJogar();
    }
    

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
}
void HUD::renderizarMenuPrincipal()
{
    const int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    const int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    float backgorund_color[3] = {0.05f, 0.1f, 0.2f}; // Azul escuro para fundo
    float button_color[3] = {0.1f, 0.2f, 0.35f};     // Azul médio para botões
    float hover_color[3] = {0.2f, 0.35f, 0.5f};      // Azul mais claro para hover
    float light_color[3] = {0.3f, 0.5f, 0.8f};       // Azul brilhante para destaques

    const float buttonWidth = 300.f, buttonHeight = 50.f, buttonSpacing = 20.f;
    float startX = (windowWidth - buttonWidth) / 2.0f;
    float startY = windowHeight - 200.f;
    botoesMenu.clear();

    botoesMenu = {
        {"Configuracoes", startX, startY, buttonWidth, buttonHeight, button_color[0], button_color[1], button_color[2], false, ACTION_BUTTON::CONFIG},
        {"Creditos", startX, startY - (buttonHeight + buttonSpacing), buttonWidth, buttonHeight, backgorund_color[0], backgorund_color[1], backgorund_color[2], false, ACTION_BUTTON::CREDITS},
        {"Controles", startX, startY - 2 * (buttonHeight + buttonSpacing), buttonWidth, buttonHeight, 0.2f, 0.3f, 0.4f, false, ACTION_BUTTON::CONTROLS},
        {"Sair do Jogo", startX, startY - 3 * (buttonHeight + buttonSpacing), buttonWidth, buttonHeight, 0.15f, 0.25f, 0.4f, false, ACTION_BUTTON::EXIT}};

    glColor3f(light_color[0], light_color[1], light_color[2]);
    drawText(windowWidth / 2 - 60, windowHeight - 50, "MENU PRINCIPAL");

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor4f(hover_color[0], hover_color[1], hover_color[2], 0.7f);
    glVertex2f(windowWidth / 2 - 100, windowHeight - 70);
    glVertex2f(windowWidth / 2 + 100, windowHeight - 70);
    glEnd();

    for (size_t i = 0; i < botoesMenu.size(); ++i)
    {
        auto &botao = botoesMenu[i];

        glBegin(GL_QUADS);
        glColor4f(botao.r, botao.g, botao.b, 0.8f);
        glVertex2f(botao.x, botao.y);
        glVertex2f(botao.x + botao.width, botao.y);
        glVertex2f(botao.x + botao.width, botao.y + botao.height);
        glVertex2f(botao.x, botao.y + botao.height);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(botao.x + 20, botao.y + 15, botao.texto.c_str());

        if (static_cast<int>(i) == hoveredButtonID)
        {
            glLineWidth(3.0f);
            glColor3f(0.3f, 0.5f, 0.8f); // Cor da borda (azul claro)
            glBegin(GL_LINE_LOOP);
            glVertex2f(botao.x - 3, botao.y - 3);
            glVertex2f(botao.x + botao.width + 3, botao.y - 3);
            glVertex2f(botao.x + botao.width + 3, botao.y + botao.height + 3);
            glVertex2f(botao.x - 3, botao.y + botao.height + 3);
            glEnd();
        }
    }
    drawText(windowWidth / 2 - 80, windowHeight - 278, "ADVENTURE QUEST");
    drawText(windowWidth / 2 - 25, windowHeight - 300, "v1.0.2");
}

void HUD::renderizarMenuCreditos()
{
    const int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    const int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    const float lineSpacing = 35.0f;        // Maior espaçamento entre linhas
    const float titleLineSpacing = 50.0f;   // Espaçamento para títulos
    const float baseY = windowHeight - 220; // Posição inicial mais alta
    const float padding = 30.0f;            // Espaçamento interno do painel

    botoesMenu.clear();
    botoesMenu.push_back({"Voltar",
                          windowWidth - 150.0f,
                          windowHeight - 150.0f,
                          100.0f,
                          40.0f,
                          0.1f, 0.2f, 0.4f,
                          false,
                          ACTION_BUTTON::NONE});

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glColor4f(0.03f, 0.08f, 0.15f, 0.9f);
    glVertex2f(100.f, windowHeight - 100.f);
    glVertex2f(windowWidth - 100.f, windowHeight - 100.f);
    glColor4f(0.07f, 0.15f, 0.25f, 0.9f);
    glVertex2f(windowWidth - 100.f, 100.f);
    glVertex2f(100.f, 100.f);
    glEnd();

    glColor4f(0.2f, 0.4f, 0.7f, 0.7f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(100.f, 100.f);
    glVertex2f(windowWidth - 100.f, 100.f);
    glVertex2f(windowWidth - 100.f, windowHeight - 100.f);
    glVertex2f(100.f, windowHeight - 100.f);
    glEnd();
    glLineWidth(1.0f);

    for (size_t i = 0; i < botoesMenu.size(); ++i)
    {
        const Botao &botao = botoesMenu[i];

        glBegin(GL_QUADS);
        glColor4f(botao.r, botao.g, botao.b, 0.7f);
        glVertex2f(botao.x, botao.y);
        glVertex2f(botao.x + botao.width, botao.y);
        glColor4f(botao.r + 0.1f, botao.g + 0.1f, botao.b + 0.1f, 0.8f);
        glVertex2f(botao.x + botao.width, botao.y + botao.height);
        glVertex2f(botao.x, botao.y + botao.height);
        glEnd();

        if (i == hoveredButtonID) // Verifica se o botão é o que está sendo hoverado
        {
            glColor4f(0.3f, 0.5f, 0.8f, 0.8f);
            glLineWidth(3.0f); // A borda será mais grossa
            glBegin(GL_LINE_LOOP);
            glVertex2f(botao.x - 3, botao.y - 3);
            glVertex2f(botao.x + botao.width + 3, botao.y - 3);
            glVertex2f(botao.x + botao.width + 3, botao.y + botao.height + 3);
            glVertex2f(botao.x - 3, botao.y + botao.height + 3);
            glEnd();
            glLineWidth(1.0f); // Restaura a linha para o valor padrão
        }

        float textWidth = botao.texto.length() * 10.0f;
        float textX = botao.x + (botao.width - textWidth) / 2;
        float textY = botao.y + (botao.height - 15) / 2;

        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(textX, textY, botao.texto.c_str());
    }

    glColor3f(0.4f, 0.6f, 0.9f);
    std::string titulo = "CRÉDITOS";
    float tituloWidth = titulo.length() * 20.0f;
    drawText((windowWidth / 2) - (tituloWidth / 2), windowHeight - 180, titulo.c_str(), 24);

    glColor4f(0.3f, 0.5f, 0.8f, 0.9f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(windowWidth / 2 - 150, windowHeight - 195);
    glVertex2f(windowWidth / 2 + 150, windowHeight - 195);
    glEnd();
    glLineWidth(1.0f);

    glColor3f(0.3f, 0.5f, 0.8f);
    drawText((windowWidth / 2) - 90, baseY, "DESENVOLVEDORES", 18);

    std::vector<std::string> creditos = {
        "Raphael Sousa Rabelo Rates",
        "Denis",
        "Gabriela Queiroga"};

    glColor3f(1.0f, 1.0f, 1.0f);
    for (size_t i = 0; i < creditos.size(); ++i)
    {
        const std::string &nome = creditos[i];
        float textoLarguraEstimado = nome.length() * 12.0f;
        float x = (windowWidth / 2) - (textoLarguraEstimado / 2);
        float y = baseY - titleLineSpacing - (i * lineSpacing);
        drawText(x, y, nome.c_str(), 16);
    }

    glColor4f(0.3f, 0.5f, 0.8f, 0.6f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(150, 150);
    glVertex2f(windowWidth - 150, 150);
    glEnd();
    glLineWidth(1.0f);

    glColor3f(0.5f, 0.7f, 0.9f);
    std::string footer = "Obrigado por jogar!";
    float footerWidth = footer.length() * 12.0f;
    drawText((windowWidth / 2) - (footerWidth / 2), 120, footer.c_str(), 16);

    glColor3f(0.7f, 0.7f, 0.7f);
    std::string version = "Versão 1.0 © 2024";
    drawText(windowWidth - 200, 120, version.c_str(), 12);
}
void HUD::renderizarTelaDesejaJogar() {
    const GLint windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    const GLint windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    
    botoesMenu.clear();
    botoesMenu.push_back({"JOGAR NOVAMENTE", windowWidth / 2 - 150.f, windowHeight / 2 + 20.0f, 140.0f, 50.0f, 0.0f, 0.7f, 1.0f, false, ACTION_BUTTON::RESET_ALL});
    botoesMenu.push_back({"SAIR", windowWidth / 2 + 50.f, windowHeight / 2 + 20.0f, 140.0f, 50.0f, 0.9f, 0.2f, 0.2f, false, ACTION_BUTTON::EXIT});
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Fundo com gradiente azul escuro
    glBegin(GL_QUADS);
    glColor4f(0.05f, 0.1f, 0.2f, 1.0f); // Azul escuro no topo
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glColor4f(0.1f, 0.2f, 0.35f, 1.0f); // Azul menos escuro no fundo
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();
    
    // Painel central clean e minimalista
    float panelWidth = windowWidth * 0.5f;
    float panelHeight = windowHeight * 0.4f;
    float panelX = (windowWidth - panelWidth) / 2;
    float panelY = (windowHeight - panelHeight) / 2;
    
    // Painel principal com azul transparente
    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.15f, 0.3f, 0.7f);
    glVertex2f(panelX, panelY);
    glVertex2f(panelX + panelWidth, panelY);
    glVertex2f(panelX + panelWidth, panelY + panelHeight);
    glVertex2f(panelX, panelY + panelHeight);
    glEnd();
    
    // Borda sutil do painel
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glColor4f(0.2f, 0.5f, 1.0f, 0.6f); // Azul mais claro para a borda
    glVertex2f(panelX, panelY);
    glVertex2f(panelX + panelWidth, panelY);
    glVertex2f(panelX + panelWidth, panelY + panelHeight);
    glVertex2f(panelX, panelY + panelHeight);
    glEnd();
    
    // Título "GAME OVER" clean
    glColor3f(1.0f, 1.0f, 1.0f); // Branco para melhor visibilidade
    std::string gameOverText = "GAME OVER";
    float gameOverSize = 32.0f;
    float gameOverWidth = gameOverText.length() * (gameOverSize * 0.6f);
    drawText((windowWidth / 2) - (gameOverWidth / 2), windowHeight / 2 - 80, gameOverText.c_str(), gameOverSize);
    
    // Texto de confirmação clean
    glColor3f(0.7f, 0.8f, 1.0f);
    std::string texto = "Deseja jogar novamente?";
    float textWidth = texto.length() * 12.0f;
    drawText((windowWidth / 2) - (textWidth / 2), windowHeight / 2 - 20, texto.c_str(), 20);
    
    // Renderizando os botões com estilo clean
    for (size_t i = 0; i < botoesMenu.size(); ++i) {
        const auto &botao = botoesMenu[i];
        
        // Botão com design clean
        glBegin(GL_QUADS);
        if (i == 0) { // Botão "JOGAR NOVAMENTE" - Azul
            glColor4f(botao.r, botao.g, botao.b, 0.8f);
        } else { // Botão "SAIR" - Vermelho
            glColor4f(botao.r, botao.g, botao.b, 0.8f);
        }
        glVertex2f(botao.x, botao.y);
        glVertex2f(botao.x + botao.width, botao.y);
        glVertex2f(botao.x + botao.width, botao.y + botao.height);
        glVertex2f(botao.x, botao.y + botao.height);
        glEnd();
        
        // Borda clean dos botões
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
        glColor4f(1.0f, 1.0f, 1.0f, 0.6f); // Borda branca sutil
        glVertex2f(botao.x, botao.y);
        glVertex2f(botao.x + botao.width, botao.y);
        glVertex2f(botao.x + botao.width, botao.y + botao.height);
        glVertex2f(botao.x, botao.y + botao.height);
        glEnd();
        
        // Verificando hover com efeito sutil
        if (i == hoveredButtonID) {
            glColor4f(1.0f, 1.0f, 1.0f, 0.3f); // Brilho branco sutil
            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(botao.x - 3, botao.y - 3);
            glVertex2f(botao.x + botao.width + 3, botao.y - 3);
            glVertex2f(botao.x + botao.width + 3, botao.y + botao.height + 3);
            glVertex2f(botao.x - 3, botao.y + botao.height + 3);
            glEnd();
        }
        
        // Texto do botão
        float textWidth = botao.texto.length() * 10.0f;
        float textX = botao.x + (botao.width - textWidth) / 2;
        float textY = botao.y + (botao.height - 15) / 2;
        
        glColor3f(1.0f, 1.0f, 1.0f); // Texto branco para contraste
        drawText(textX, textY, botao.texto.c_str(), 16);
    }
}


void HUD::renderizarControles()
{
    const int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    const int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    const float lineSpacing = 35.0f;
    const float baseY = windowHeight - 180;
    botoesMenu.clear();

    botoesMenu.push_back({"Voltar",
                          windowWidth - 150.0f,
                          windowHeight - 150.0f,
                          100.0f,
                          40.0f,
                          0.1f, 0.2f, 0.4f,
                          false,
                          ACTION_BUTTON::NONE});

    glColor4f(0.05f, 0.1f, 0.2f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(100.f, windowHeight - 100.f);
    glVertex2f(windowWidth - 100.f, windowHeight - 100.f);
    glVertex2f(windowWidth - 100.f, 100.f);
    glVertex2f(100.f, 100.f);
    glEnd();

    glColor3f(0.4f, 0.6f, 0.9f);
    drawText((windowWidth / 2) - 70, windowHeight - 120, "CONTROLES", 24);

    glColor3f(1.0f, 1.0f, 1.0f);
    std::vector<std::string> controles = {
        "W ,A, S, D ----------- Mover",
        "Espaço --------------- Atacar",
        "Enter ---------------- Interagir",
        "K -------------------- Menu de habilidade",
        "Esc ------------------ Saída",
        "Tab ------------------ Menu",
        "Q , E ---------------- Rotação do personagem",
        "T -------------------- Visão de topo do mapa",
        "Joystick X ----------- Atacar",
        "Joystick Circle ------ Usar Portal",
        "Joystick Triangle ---- Árvore de Habilidades",
        "Joystick TouchPad ---- Visão de topo do mapa",
        "Joystick Options ----- Menu",
        "Joystick Share ------- Captura de tela",
        "Joystick R3 ---------- Correr",
        "Joystick L2 ---------- Aumentar zoom",
        "Joystick R2 ---------- Diminuir zoom",
    };

    for (size_t i = 0; i < controles.size(); ++i)
    {
        drawText(150, baseY - (i * lineSpacing), controles[i].c_str(), 18);
    }

    for (size_t i = 0; i < botoesMenu.size(); ++i)
    {
        const Botao &botao = botoesMenu[i];

        glBegin(GL_QUADS);
        glColor4f(botao.r, botao.g, botao.b, 0.7f);
        glVertex2f(botao.x, botao.y);
        glVertex2f(botao.x + botao.width, botao.y);
        glVertex2f(botao.x + botao.width, botao.y + botao.height);
        glVertex2f(botao.x, botao.y + botao.height);
        glEnd();

        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(botao.x + 10, botao.y + (botao.height / 2) - 5, botao.texto.c_str());

        if (static_cast<int>(i) == hoveredButtonID)
        {
            glLineWidth(3.0f);
            glColor3f(0.3f, 0.5f, 0.8f); // Cor da borda (azul claro)
            glBegin(GL_LINE_LOOP);
            glVertex2f(botao.x - 3, botao.y - 3);
            glVertex2f(botao.x + botao.width + 3, botao.y - 3);
            glVertex2f(botao.x + botao.width + 3, botao.y + botao.height + 3);
            glVertex2f(botao.x - 3, botao.y + botao.height + 3);
            glEnd();
            glLineWidth(1.0f); // Restaura a largura da linha
        }
    }
}

void HUD::screenshotAnimation(float deltaTime) {
    static bool isScreenshotAnimationActive = false;
    static float alpha = 0.0f, timer = 0.0f;
    const float duration = 0.5f;

    if (!isScreenshotAnimationActive) return;

    timer += deltaTime;

    // Controle da opacidade
    if (timer < duration / 4) alpha = timer / (duration / 4);
    else if (timer < duration / 2) alpha = 1.0f;
    else if (timer < duration) alpha = 1.0f - ((timer - duration / 2) / (duration / 2));
    else { isScreenshotAnimationActive = false; alpha = 0.0f; }

    // Renderizando o efeito
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, -1, 1);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST); glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Efeito visual
    glColor4f(1.0f, 1.0f, 1.0f, alpha * 0.3f);
    glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(glutGet(GLUT_WINDOW_WIDTH), 0);
        glVertex2f(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
        glVertex2f(0, glutGet(GLUT_WINDOW_HEIGHT));
    glEnd();

    // Restaurando OpenGL
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
    glPopAttrib();
}

void HUD::TriggerScreenshotAnimation(){
   screenshotAnimation(0.016f); 
}

void HUD::renderizarMenuConfiguracoes(Volume &volume)
{
    const int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    const int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    const float backgorund_color[4] = {0.03f, 0.08f, 0.15f, 0.95f};
    const float button_color[4] = {0.08f, 0.15f, 0.25f, 0.9f};   
    const float hover_color[4] = {0.15f, 0.25f, 0.4f, 0.85f};     
    const float light_color[4] = {0.3f, 0.5f, 0.8f, 0.85f};       

    const float painelLargura = windowWidth * 0.6f;
    const float painelAltura = windowHeight * 0.7f;
    const float painelX = (windowWidth - painelLargura) / 2;
    const float painelY = (windowHeight - painelAltura) / 2;

    const float sliderLargura = painelLargura * 0.6f;
    const float sliderAltura = 24.0f;
    const float buttonLargura = 40.0f;
    const float buttonAltura = 36.0f;
    const float espacamento = 70.0f;
 
    botoesMenu.clear();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glColor4f(0.02f, 0.06f, 0.12f, 0.92f);
    glVertex2f(painelX, painelY);
    glVertex2f(painelX + painelLargura, painelY);
    glColor4f(0.08f, 0.16f, 0.28f, 0.92f); 
    glVertex2f(painelX + painelLargura, painelY + painelAltura);
    glVertex2f(painelX, painelY + painelAltura);
    glEnd();

    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glColor4f(light_color[0], light_color[1], light_color[2], light_color[3]);
    glVertex2f(painelX, painelY);
    glVertex2f(painelX + painelLargura, painelY);
    glVertex2f(painelX + painelLargura, painelY + painelAltura);
    glVertex2f(painelX, painelY + painelAltura);
    glEnd();

    glColor3f(0.4f, 0.6f, 0.9f);
    float textScale = 1.5f;
    glPushMatrix();
    glTranslatef(painelX + painelLargura / 2 - 120, painelY + painelAltura - 50, 0);
    glScalef(textScale, textScale, textScale);
    drawText(0, 0, "CONFIGURAÇÕES DE ÁUDIO");
    glPopMatrix();

    float startY = painelY + painelAltura - 120;
    float centerX = painelX + painelLargura / 2;
    float sliderStartX = centerX - sliderLargura / 2;

    struct AudioControl
    {
        std::string nome;
        float valAtual;
        ACTION_BUTTON acaoDiminuir;
        ACTION_BUTTON acaoAumentar;
    };

    std::vector<AudioControl> controles = {
        {"Volume Ambiente", volume.ambient, ACTION_BUTTON::VOLUME_AMBIENT_DECREASE, ACTION_BUTTON::VOLUME_AMBIENT_INCREASE},
        {"Volume Música", volume.musica, ACTION_BUTTON::VOLUME_MUSIC_DECREASE, ACTION_BUTTON::VOLUME_MUSIC_INCREASE},
        {"Volume Efeitos", volume.efeitos, ACTION_BUTTON::VOLUME_EFFECTS_DECREASE, ACTION_BUTTON::VOLUME_EFFECTS_INCREASE},
        {"Volume UI", volume.UI, ACTION_BUTTON::VOLUME_UI_DECREASE, ACTION_BUTTON::VOLUME_UI_INCREASE}};

    for (int i = 0; i < controles.size(); i++)
    {
        float posY = startY - i * espacamento;

        glColor3f(0.7f, 0.8f, 1.0f);
        drawText(sliderStartX, posY + 10, controles[i].nome.c_str());

        botoesMenu.push_back({"-", sliderStartX - buttonLargura - 10, posY - 5, buttonLargura, buttonAltura, button_color[0], button_color[1], button_color[2], false, controles[i].acaoDiminuir});

        glBegin(GL_QUADS);
        glColor4f(0.04f, 0.08f, 0.15f, 0.8f);
        glVertex2f(sliderStartX, posY);
        glVertex2f(sliderStartX + sliderLargura, posY);
        glVertex2f(sliderStartX + sliderLargura, posY + sliderAltura);
        glVertex2f(sliderStartX, posY + sliderAltura);
        glEnd();

        glBegin(GL_QUADS);
        glColor4f(hover_color[0], hover_color[1], hover_color[2], hover_color[3]);
        glVertex2f(sliderStartX, posY);
        glVertex2f(sliderStartX + sliderLargura * controles[i].valAtual, posY);
        glVertex2f(sliderStartX + sliderLargura * controles[i].valAtual, posY + sliderAltura);
        glVertex2f(sliderStartX, posY + sliderAltura);
        glEnd();

        botoesMenu.push_back({"+", sliderStartX + sliderLargura + 10, posY - 5, buttonLargura, buttonAltura, button_color[0], button_color[1], button_color[2], false, controles[i].acaoAumentar});

        char percentText[10];
        sprintf(percentText, "%d%%", (int)(controles[i].valAtual * 100));
        glColor3f(0.7f, 0.8f, 1.0f);
        drawText(sliderStartX + sliderLargura + buttonLargura + 20, posY + 10, percentText);
    }

    float buttonBackWidth = 150.0f;
    float buttonBackHeight = 40.0f;
    botoesMenu.push_back({"VOLTAR", centerX - buttonBackWidth / 2, painelY + 40, buttonBackWidth, buttonBackHeight,button_color[0], button_color[1], button_color[2], false, ACTION_BUTTON::NONE});

    for (size_t i = 0; i < botoesMenu.size(); ++i)
{
    const Botao &botao = botoesMenu[i];

    if (botao.hovering)
        glColor4f(hover_color[0], hover_color[1], hover_color[2], hover_color[3]);
    else
        glColor4f(botao.r, botao.g, botao.b, 0.9f);

    renderRoundedButton(botao.x, botao.y, botao.width, botao.height, 8.0f);

    glColor3f(0.9f, 0.95f, 1.0f);
    float textX = botao.x + botao.width / 2 - botao.texto.length() * 4;
    float textY = botao.y + botao.height / 2 - 5;
    drawText(textX, textY, botao.texto.c_str());

    if (static_cast<int>(i) == hoveredButtonID)
    {
        glLineWidth(3.0f);
        glColor3f(0.3f, 0.5f, 0.8f); 
        glBegin(GL_LINE_LOOP);
        glVertex2f(botao.x - 3, botao.y - 3);
        glVertex2f(botao.x + botao.width + 3, botao.y - 3);
        glVertex2f(botao.x + botao.width + 3, botao.y + botao.height + 3);
        glVertex2f(botao.x - 3, botao.y + botao.height + 3);
        glEnd();
        glLineWidth(1.0f); 
    }
}

    glDisable(GL_BLEND);
}

void HUD::renderRoundedButton(float x, float y, float width, float height, float radius)
{
    int segments = 10; 
    glBegin(GL_POLYGON);

    for (int i = 0; i <= segments; i++)
    {
        float angle = M_PI + (M_PI / 2) * i / segments;
        float xPos = x + radius + radius * cos(angle);
        float yPos = y + height - radius + radius * sin(angle);
        glVertex2f(xPos, yPos);
    }

    for (int i = 0; i <= segments; i++)
    {
        float angle = M_PI * 3 / 2 + (M_PI / 2) * i / segments;
        float xPos = x + width - radius + radius * cos(angle);
        float yPos = y + height - radius + radius * sin(angle);
        glVertex2f(xPos, yPos);
    }

    for (int i = 0; i <= segments; i++)
    {
        float angle = 0 + (M_PI / 2) * i / segments;
        float xPos = x + width - radius + radius * cos(angle);
        float yPos = y + radius + radius * sin(angle);
        glVertex2f(xPos, yPos);
    }

    for (int i = 0; i <= segments; i++)
    {
        float angle = M_PI / 2 + (M_PI / 2) * i / segments;
        float xPos = x + radius + radius * cos(angle);
        float yPos = y + radius + radius * sin(angle);
        glVertex2f(xPos, yPos);
    }

    glEnd();
}