void Game::initSkybox() 
{
    // Carrega as texturas do skybox
    // Normalmente você precisará de 6 texturas para um skybox completo
    // (direita, esquerda, cima, baixo, frente, trás)
    skyboxTextures[0] = loadTexture("textures/skybox_right.png");
    skyboxTextures[1] = loadTexture("textures/skybox_left.png");
    skyboxTextures[2] = loadTexture("textures/skybox_top.png");
    skyboxTextures[3] = loadTexture("textures/skybox_bottom.png");
    skyboxTextures[4] = loadTexture("textures/skybox_front.png");
    skyboxTextures[5] = loadTexture("textures/skybox_back.png");
    
    // Também é possível ter skyboxes diferentes para cada mapa
    if (currentMap == MapType::PARASIDE) {
        skyboxTextures[0] = loadTexture("textures/paradise_right.png");
        skyboxTextures[1] = loadTexture("textures/paradise_left.png");
        // ... carregue as outras 4 texturas para o paraíso
    }
    else if (currentMap == MapType::BOSS) {
        skyboxTextures[0] = loadTexture("textures/boss_right.png");
        skyboxTextures[1] = loadTexture("textures/boss_left.png");
        // ... carregue as outras 4 texturas para a área do boss
    }
    // etc. para outros mapas
}

void Game::drawSkybox() 
{
    // Salva o estado atual da matriz
    glPushMatrix();
    
    // Desativa iluminação para o skybox
    glDisable(GL_LIGHTING);
    
    // Obtém a posição da câmera para centralizar o skybox
    float posX = cameraPos.x;
    float posY = cameraPos.y;
    float posZ = cameraPos.z;
    
    // Tamanho do skybox
    float size = WORLD_SIZE * 2;
    
    // Desenha cada lado do skybox
    
    // Define a matriz de modelagem para seguir a câmera
    glTranslatef(posX, posY, posZ);
    
    // Ajusta configurações de OpenGL para skybox
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    
    // Direita
    glBindTexture(GL_TEXTURE_2D, skyboxTextures[0]);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(size, -size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size, -size, size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size, size, size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(size, size, -size);
    glEnd();
    
    // Esquerda
    glBindTexture(GL_TEXTURE_2D, skyboxTextures[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, -size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, size, -size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size, size);
    glEnd();
    
    // Topo
    glBindTexture(GL_TEXTURE_2D, skyboxTextures[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size, size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size, size, size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size, size);
    glEnd();
    
    // Base
    glBindTexture(GL_TEXTURE_2D, skyboxTextures[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size, -size, size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size, -size, -size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, -size, -size);
    glEnd();
    
    // Frente
    glBindTexture(GL_TEXTURE_2D, skyboxTextures[4]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(size, -size, size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-size, -size, size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-size, size, size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(size, size, size);
    glEnd();
    
    // Fundo
    glBindTexture(GL_TEXTURE_2D, skyboxTextures[5]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-size, -size, -size);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(size, -size, -size);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(size, size, -size);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-size, size, -size);
    glEnd();
    
    // Restaura configurações OpenGL
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    
    // Reativa a iluminação
    glEnable(GL_LIGHTING);
    
    // Restaura a matriz
    glPopMatrix();
}

// Alternativa: Implementação de céu usando dome (hemisfério)
void Game::drawSkyDome() 
{
    // Salva o estado atual da matriz
    glPushMatrix();
    
    // Desativa iluminação para o céu
    glDisable(GL_LIGHTING);
    
    // Obtém a posição da câmera
    float posX = cameraPos.x;
    float posY = cameraPos.y;
    float posZ = cameraPos.z;
    
    // Define a matriz de modelagem para seguir a câmera (apenas X e Z)
    glTranslatef(posX, 0, posZ);
    
    // Ajusta configurações de OpenGL
    glDepthMask(GL_FALSE);
    
    // Seleciona a textura do céu adequada ao mapa atual
    GLuint skyTexture;
    if (currentMap == MapType::PARASIDE) {
        skyTexture = skyTextureParadise;
    } 
    else if (currentMap == MapType::DUNGEON_ONE_LEVEL) {
        skyTexture = skyTextureDungeon1;
    }
    // ... casos para outros mapas
    else {
        skyTexture = skyTextureDefault; // Céu padrão
    }
    
    // Ativa e vincula a textura
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, skyTexture);
    
    // Define cor base do céu
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Tamanho do domo
    float radius = WORLD_SIZE * 1.5f;
    float height = WORLD_SIZE;
    
    // Parâmetros para divisões do domo
    int segments = 32;
    int rings = 16;
    
    // Desenha o domo do céu
    for (int i = 0; i < rings; i++) {
        float phi1 = M_PI * 0.5f * (float)(i) / rings;
        float phi2 = M_PI * 0.5f * (float)(i + 1) / rings;
        
        float y1 = height * cosf(phi1);
        float y2 = height * cosf(phi2);
        
        float r1 = radius * sinf(phi1);
        float r2 = radius * sinf(phi2);
        
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= segments; j++) {
            float theta = 2.0f * M_PI * (float)j / segments;
            
            float x = sinf(theta);
            float z = cosf(theta);
            
            float tx1 = (float)j / segments;
            float ty1 = (float)i / rings;
            float ty2 = (float)(i + 1) / rings;
            
            glTexCoord2f(tx1, ty1);
            glVertex3f(r1 * x, y1, r1 * z);
            
            glTexCoord2f(tx1, ty2);
            glVertex3f(r2 * x, y2, r2 * z);
        }
        glEnd();
    }
    
    // Restaura configurações OpenGL
    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    
    // Reativa a iluminação
    glEnable(GL_LIGHTING);
    
    // Restaura a matriz
    glPopMatrix();
}

// Função para alternar o tipo de céu (skybox ou domo)
void Game::toggleSkyType() {
    useSkybox = !useSkybox;
}

// Função para desenhar o céu no ciclo de renderização principal
void Game::drawSky() {
    if (useSkybox) {
        drawSkybox();
    } else {
        drawSkyDome();
    }
}

// Função para carregar uma textura de arquivo
GLuint Game::loadTexture(const char* filename) {
    GLuint textureID;
    
    // Criar ID de textura
    glGenTextures(1, &textureID);
    
    // Vincular a textura
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Aqui você deve usar uma biblioteca de imagem para carregar o arquivo
    // como SDL_image, SOIL, stb_image, etc.
    // Este é um exemplo usando stb_image:
    
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
    
    if (image) {
        // Carregar dados da imagem para a textura
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        
        // Gerar mipmaps
        glGenerateMipmap(GL_TEXTURE_2D);
        
        // Configurar parâmetros de textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // Liberar memória da imagem
        stbi_image_free(image);
    } else {
        // Lidar com erro de carregamento
        fprintf(stderr, "Erro ao carregar textura: %s\n", filename);
    }
    
    return textureID;
}

// Função para atualizar o céu com base no horário do jogo
void Game::updateSky(float gameTime) {
    // Você pode implementar um ciclo dia/noite alterando o céu
    // com base no tempo do jogo
    
    // Exemplo de ciclo dia/noite
    float dayLength = 600.0f; // 10 minutos = um dia completo
    float timeOfDay = fmod(gameTime, dayLength) / dayLength; // 0.0 a 1.0
    
    // Ajustar cor do céu ao longo do dia
    if (timeOfDay < 0.25f) { // Amanhecer
        float t = timeOfDay / 0.25f;
        skyColor[0] = 0.5f + t * 0.5f;  // R: 0.5 -> 1.0
        skyColor[1] = 0.5f + t * 0.5f;  // G: 0.5 -> 1.0
        skyColor[2] = 0.7f + t * 0.3f;  // B: 0.7 -> 1.0
    } 
    else if (timeOfDay < 0.75f) { // Dia
        skyColor[0] = 1.0f;
        skyColor[1] = 1.0f;
        skyColor[2] = 1.0f;
    } 
    else { // Entardecer/noite
        float t = (timeOfDay - 0.75f) / 0.25f;
        skyColor[0] = 1.0f - t * 0.5f;  // R: 1.0 -> 0.5
        skyColor[1] = 1.0f - t * 0.5f;  // G: 1.0 -> 0.5
        skyColor[2] = 1.0f - t * 0.3f;  // B: 1.0 -> 0.7
    }
    
    // Aplica a cor ao desenhar o céu
    // Esta cor será usada como fator multiplicativo à textura
    glColor3fv(skyColor);
}