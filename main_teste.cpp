#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>

// Definição de estruturas para o modelo 3D

// Estrutura para vértices
struct Vertex
{
    float x, y, z;
};

// Estrutura para normais
struct Normal
{
    float x, y, z;
};

// Estrutura para coordenadas de textura
struct TexCoord
{
    float u, v;
};

// Estrutura para cor
struct Color
{
    float r, g, b;
};

// Estrutura para material
struct Material
{
    std::string name;
    Color ambient;
    Color diffuse;
    Color specular;
    float shininess;
    std::string diffuseTexName; // Nome do arquivo de textura
    GLuint textureID;           // ID da textura no OpenGL
    bool hasTexture;            // Flag indicando se o material tem textura
};

// Estrutura para face
struct Face
{
    std::vector<int> vertexIndices;
    std::vector<int> normalIndices;
    std::vector<int> texCoordIndices;
    std::string materialName;
};

// Estrutura para grupo (conjunto de faces com mesmo material)
struct Group
{
    std::string name;
    std::vector<Face> faces;
    std::string materialName;
};

// Classe para gerenciar o modelo 3D
class Model
{
public:
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<TexCoord> texCoords;
    std::vector<Group> groups;
    std::map<std::string, Material> materials;

    // Transformações
    float posX = 0.0f, posY = 0.0f, posZ = 0.0f;
    float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
    float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;

    // Carrega arquivo OBJ
    bool loadOBJ(const char *objPath, const char *mtlBasePath = "./src/objs/")
    {
        std::ifstream file(objPath);
        if (!file.is_open())
        {
            std::cerr << "Erro ao abrir o arquivo OBJ: " << objPath << std::endl;
            return false;
        }

        // Diretório base para materiais e texturas
        std::string basePath = mtlBasePath;
        if (basePath.empty())
        {
            // Extrai o diretório do caminho do OBJ
            std::string objPathStr(objPath);
            size_t lastSlash = objPathStr.find_last_of("/\\");
            if (lastSlash != std::string::npos)
            {
                basePath = objPathStr.substr(0, lastSlash + 1);
            }
        }
        else if (basePath.back() != '/' && basePath.back() != '\\')
        {
            basePath += '/';
        }

        Group *currentGroup = nullptr;
        std::string line;
        std::string currentMaterial = "";

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v")
            { // Vértice
                Vertex v;
                iss >> v.x >> v.y >> v.z;
                vertices.push_back(v);
            }
            else if (prefix == "vn")
            { // Normal
                Normal n;
                iss >> n.x >> n.y >> n.z;
                normals.push_back(n);
            }
            else if (prefix == "vt")
            { // Coordenada de textura
                TexCoord t;
                iss >> t.u >> t.v;
                // Corrige coordenada V (OBJ: origem em baixo, OpenGL: origem em cima)
                t.v = 1.0f - t.v;
                texCoords.push_back(t);
            }
            else if (prefix == "mtllib")
            { // Biblioteca de materiais
                std::string mtlFilename;
                iss >> mtlFilename;
                loadMTL(basePath + mtlFilename);
            }
            else if (prefix == "usemtl")
            { // Uso de material
                iss >> currentMaterial;
                // Se estamos em um grupo, atualiza o material
                if (currentGroup)
                {
                    currentGroup->materialName = currentMaterial;
                }
            }
            else if (prefix == "g" || prefix == "o")
            { // Novo grupo ou objeto
                std::string groupName;
                iss >> groupName;
                groups.push_back({groupName, {}, currentMaterial});
                currentGroup = &groups.back();
            }
            else if (prefix == "f")
            { // Face
                Face face;
                face.materialName = currentMaterial;

                std::string vertexData;
                while (iss >> vertexData)
                {
                    std::replace(vertexData.begin(), vertexData.end(), '/', ' ');
                    std::istringstream viss(vertexData);

                    int vertIdx = 0, texIdx = 0, normIdx = 0;
                    viss >> vertIdx;
                    if (viss.good())
                        viss >> texIdx;
                    if (viss.good())
                        viss >> normIdx;

                    // OBJ índices começam em 1, ajustando para 0
                    face.vertexIndices.push_back(vertIdx > 0 ? vertIdx - 1 : vertices.size() + vertIdx);
                    if (texIdx != 0)
                    {
                        face.texCoordIndices.push_back(texIdx > 0 ? texIdx - 1 : texCoords.size() + texIdx);
                    }
                    if (normIdx != 0)
                    {
                        face.normalIndices.push_back(normIdx > 0 ? normIdx - 1 : normals.size() + normIdx);
                    }
                }

                // Se não temos um grupo atual, cria um
                if (!currentGroup)
                {
                    groups.push_back({"default", {}, currentMaterial});
                    currentGroup = &groups.back();
                }

                // Adiciona a face ao grupo atual
                currentGroup->faces.push_back(face);
            }
        }

        file.close();

        // Se não temos nenhum grupo, cria um padrão
        if (groups.empty() && !vertices.empty())
        {
            std::cerr << "Aviso: Nenhum grupo encontrado, criando grupo padrão." << std::endl;
            groups.push_back({"default", {}, currentMaterial});
        }

        // Carrega as texturas dos materiais
        loadTextures(basePath);

        return true;
    }

    // Carrega arquivo MTL (materiais)
    bool loadMTL(const std::string &mtlPath)
    {
        std::ifstream file(mtlPath);
        if (!file.is_open())
        {
            std::cerr << "Aviso: Não foi possível abrir o arquivo MTL: " << mtlPath << std::endl;
            return false;
        }

        Material *currentMaterial = nullptr;
        std::string line;

        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "newmtl")
            { // Novo material
                std::string materialName;
                iss >> materialName;
                materials[materialName] = Material();
                materials[materialName].name = materialName;
                materials[materialName].ambient = {0.2f, 0.2f, 0.2f};
                materials[materialName].diffuse = {0.8f, 0.8f, 0.8f};
                materials[materialName].specular = {1.0f, 1.0f, 1.0f};
                materials[materialName].shininess = 32.0f;
                materials[materialName].hasTexture = false;
                currentMaterial = &materials[materialName];
            }
            else if (currentMaterial)
            {
                if (prefix == "Ka")
                { // Cor ambiente
                    iss >> currentMaterial->ambient.r >> currentMaterial->ambient.g >> currentMaterial->ambient.b;
                }
                else if (prefix == "Kd")
                { // Cor difusa
                    iss >> currentMaterial->diffuse.r >> currentMaterial->diffuse.g >> currentMaterial->diffuse.b;
                }
                else if (prefix == "Ks")
                { // Cor especular
                    iss >> currentMaterial->specular.r >> currentMaterial->specular.g >> currentMaterial->specular.b;
                }
                else if (prefix == "Ns")
                { // Brilho
                    iss >> currentMaterial->shininess;
                }
                else if (prefix == "map_Kd")
                { // Mapa de textura difusa
                    iss >> currentMaterial->diffuseTexName;
                    currentMaterial->hasTexture = true;
                }
            }
        }

        file.close();
        return true;
    }

    // Carrega as texturas para os materiais
    void loadTextures(const std::string &basePath)
    {
        // Função auxiliar para carregar texturas
        auto loadTexture = [](const std::string &filePath) -> GLuint
        {
            if (filePath.empty())
            {
                return 0;
            }

            GLuint textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            // Configurações básicas da textura
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Carrega dados de imagem aqui - normalmente seria com uma biblioteca como stb_image
            // Como não podemos incluir bibliotecas externas aqui, apenas exibimos uma mensagem
            std::cout << "Simulando carregamento de textura: " << filePath << std::endl;
            std::cout << "Em um aplicativo real, você usaria stb_image.h ou similar para carregar." << std::endl;

            // Cria uma textura de teste (xadrez vermelho e branco)
            const int size = 64;
            unsigned char *data = new unsigned char[size * size * 3];
            for (int y = 0; y < size; y++)
            {
                for (int x = 0; x < size; x++)
                {
                    unsigned char color = ((x / 8 + y / 8) % 2) ? 255 : 0;
                    data[(y * size + x) * 3 + 0] = color;     // R
                    data[(y * size + x) * 3 + 1] = color / 2; // G
                    data[(y * size + x) * 3 + 2] = color / 2; // B
                }
            }

            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, size, size, GL_RGB, GL_UNSIGNED_BYTE, data);

            delete[] data;
            return textureID;
        };

        // Carrega texturas para cada material
        for (auto &materialPair : materials)
        {
            Material &material = materialPair.second;
            if (material.hasTexture && !material.diffuseTexName.empty())
            {
                material.textureID = loadTexture(basePath + material.diffuseTexName);
            }
        }
    }

    // Desenha o modelo
    void draw()
    {
        glPushMatrix();

        // Aplicar transformações
        glTranslatef(posX, posY, posZ);
        glRotatef(rotX, 1.0f, 0.0f, 0.0f);
        glRotatef(rotY, 0.0f, 1.0f, 0.0f);
        glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
        glScalef(scaleX, scaleY, scaleZ);

        // Para cada grupo no modelo
        for (const Group &group : groups)
        {
            // Aplica o material do grupo, se existir
            const Material *material = nullptr;
            if (!group.materialName.empty())
            {
                auto it = materials.find(group.materialName);
                if (it != materials.end())
                {
                    material = &it->second;

                    // Define as propriedades do material
                    GLfloat ambient[4] = {material->ambient.r, material->ambient.g, material->ambient.b, 1.0f};
                    GLfloat diffuse[4] = {material->diffuse.r, material->diffuse.g, material->diffuse.b, 1.0f};
                    GLfloat specular[4] = {material->specular.r, material->specular.g, material->specular.b, 1.0f};

                    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
                    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
                    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);

                    // Ativa a textura, se existir
                    if (material->hasTexture && material->textureID != 0)
                    {
                        glEnable(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, material->textureID);
                    }
                    else
                    {
                        glDisable(GL_TEXTURE_2D);
                    }
                }
            }

            // Para cada face no grupo
            for (const Face &face : group.faces)
            {
                // Se a face tiver material diferente do grupo, aplica
                if (!face.materialName.empty() && face.materialName != group.materialName)
                {
                    auto it = materials.find(face.materialName);
                    if (it != materials.end())
                    {
                        material = &it->second;

                        GLfloat ambient[4] = {material->ambient.r, material->ambient.g, material->ambient.b, 1.0f};
                        GLfloat diffuse[4] = {material->diffuse.r, material->diffuse.g, material->diffuse.b, 1.0f};
                        GLfloat specular[4] = {material->specular.r, material->specular.g, material->specular.b, 1.0f};

                        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
                        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
                        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
                        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);

                        if (material->hasTexture && material->textureID != 0)
                        {
                            glEnable(GL_TEXTURE_2D);
                            glBindTexture(GL_TEXTURE_2D, material->textureID);
                        }
                        else
                        {
                            glDisable(GL_TEXTURE_2D);
                        }
                    }
                }

                // Determina o tipo de primitiva
                GLenum primitiveType = GL_POLYGON;
                if (face.vertexIndices.size() == 3)
                {
                    primitiveType = GL_TRIANGLES;
                }
                else if (face.vertexIndices.size() == 4)
                {
                    primitiveType = GL_QUADS;
                }

                // Desenha a face
                glBegin(primitiveType);
                for (size_t i = 0; i < face.vertexIndices.size(); ++i)
                {
                    // Coordenada de textura
                    if (!face.texCoordIndices.empty() && i < face.texCoordIndices.size() &&
                        face.texCoordIndices[i] >= 0 && face.texCoordIndices[i] < static_cast<int>(texCoords.size()))
                    {
                        const TexCoord &tc = texCoords[face.texCoordIndices[i]];
                        glTexCoord2f(tc.u, tc.v);
                    }

                    // Normal
                    if (!face.normalIndices.empty() && i < face.normalIndices.size() &&
                        face.normalIndices[i] >= 0 && face.normalIndices[i] < static_cast<int>(normals.size()))
                    {
                        const Normal &n = normals[face.normalIndices[i]];
                        glNormal3f(n.x, n.y, n.z);
                    }

                    // Vértice
                    if (face.vertexIndices[i] >= 0 && face.vertexIndices[i] < static_cast<int>(vertices.size()))
                    {
                        const Vertex &v = vertices[face.vertexIndices[i]];
                        glVertex3f(v.x, v.y, v.z);
                    }
                }
                glEnd();
            }
        }

        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
    }
};

// Variáveis globais
Model model;
int windowWidth = 800;
int windowHeight = 600;
bool wireframe = false;
bool lighting = true;
bool texturing = true;
bool showNormals = false;

// Controle de câmera
float cameraDistance = 5.0f;
float cameraAngleX = 30.0f;
float cameraAngleY = -30.0f;
int lastMouseX = 0;
int lastMouseY = 0;
bool mouseLeftDown = false;
bool mouseRightDown = false;

// Funções de callback do GLUT
void reshape(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);

    glMatrixMode(GL_MODELVIEW);
}

void setupLighting()
{
    if (lighting)
    {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        // Luz principal (luz direcional do topo-direita-frente)
        GLfloat lightDir[] = {1.0f, 1.0f, 1.0f, 0.0f};
        GLfloat lightAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
        GLfloat lightDiffuse[] = {0.7f, 0.7f, 0.7f, 1.0f};
        GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};

        glLightfv(GL_LIGHT0, GL_POSITION, lightDir);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

        // Luz secundária (luz direcional da esquerda-baixo-trás)
        glEnable(GL_LIGHT1);
        GLfloat light2Dir[] = {-1.0f, -0.5f, -1.0f, 0.0f};
        GLfloat light2Ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
        GLfloat light2Diffuse[] = {0.3f, 0.3f, 0.4f, 1.0f};
        GLfloat light2Specular[] = {0.0f, 0.0f, 0.0f, 1.0f};

        glLightfv(GL_LIGHT1, GL_POSITION, light2Dir);
        glLightfv(GL_LIGHT1, GL_AMBIENT, light2Ambient);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light2Diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, light2Specular);
    }
    else
    {
        glDisable(GL_LIGHTING);
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Posicionamento da câmera
    float radianX = cameraAngleX * 3.14159f / 180.0f;
    float radianY = cameraAngleY * 3.14159f / 180.0f;

    float eyeX = cameraDistance * sin(radianY) * cos(radianX);
    float eyeY = cameraDistance * sin(radianX);
    float eyeZ = cameraDistance * cos(radianY) * cos(radianX);

    gluLookAt(eyeX, eyeY, eyeZ, 0, 0, 0, 0, 1, 0);

    // Configuração da iluminação
    setupLighting();

    // Configuração do modo de polígono (wireframe ou sólido)
    if (wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Configuração de texturização
    if (texturing)
    {
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }

    // Desenha o modelo
    model.draw();

    // Desenha normais se habilitado
    if (showNormals)
    {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_LINES);

        // Desenha uma linha para cada normal de vértice
        for (size_t i = 0; i < model.groups.size(); ++i)
        {
            for (size_t j = 0; j < model.groups[i].faces.size(); ++j)
            {
                const Face &face = model.groups[i].faces[j];
                for (size_t k = 0; k < face.vertexIndices.size(); ++k)
                {
                    if (k < face.normalIndices.size())
                    {
                        const Vertex &v = model.vertices[face.vertexIndices[k]];
                        const Normal &n = model.normals[face.normalIndices[k]];

                        // Ponto de início da linha (vértice)
                        glVertex3f(v.x, v.y, v.z);

                        // Ponto final da linha (vértice + normal escalada)
                        float scale = 0.1f;
                        glVertex3f(v.x + n.x * scale, v.y + n.y * scale, v.z + n.z * scale);
                    }
                }
            }
        }
        glEnd();

        if (lighting)
        {
            glEnable(GL_LIGHTING);
        }
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27: // ESC
        exit(0);
        break;
    case 'w': // Wireframe mode
    case 'W':
        wireframe = !wireframe;
        break;
    case 'l': // Lighting
    case 'L':
        lighting = !lighting;
        break;
    case 't': // Texturing
    case 'T':
        texturing = !texturing;
        break;
    case 'n': // Show normals
    case 'N':
        showNormals = !showNormals;
        break;
    case '+': // Zoom in
        cameraDistance -= 0.5f;
        if (cameraDistance < 0.5f)
            cameraDistance = 0.5f;
        break;
    case '-': // Zoom out
        cameraDistance += 0.5f;
        break;
    case 'r': // Reset view
    case 'R':
        cameraDistance = 5.0f;
        cameraAngleX = 30.0f;
        cameraAngleY = -30.0f;
        break;
    }
    glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y)
{
    lastMouseX = x;
    lastMouseY = y;

    if (button == GLUT_LEFT_BUTTON)
    {
        mouseLeftDown = (state == GLUT_DOWN);
    }
    else if (button == GLUT_RIGHT_BUTTON)
    {
        mouseRightDown = (state == GLUT_DOWN);
    }
}

void mouseMotion(int x, int y)
{
    int dx = x - lastMouseX;
    int dy = y - lastMouseY;

    if (mouseLeftDown)
    {
        cameraAngleY += dx * 0.2f;
        cameraAngleX += dy * 0.2f;

        // Limita ângulo vertical para evitar inversão da câmera
        if (cameraAngleX > 89.0f)
            cameraAngleX = 89.0f;
        if (cameraAngleX < -89.0f)
            cameraAngleX = -89.0f;
    }
    else if (mouseRightDown)
    {
        cameraDistance += dy * 0.05f;
        if (cameraDistance < 0.5f)
            cameraDistance = 0.5f;
    }

    lastMouseX = x;
    lastMouseY = y;

    glutPostRedisplay();
}

void init()
{
    // Configuração do OpenGL
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    // Configuração da iluminação
    setupLighting();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Visualizador de Modelos 3D");

    init();

    // Se houver argumentos na linha de comando, tenta carregar o modelo

    if (!model.loadOBJ("Knife.obj"))
    {
        std::cerr << "Erro ao carregar modelo." << std::endl;
        return 1;
    }
    else
    {
        std::cout << "Modelo carregado com sucesso: " << argv[1] << std::endl;
        std::cout << "Vértices: " << model.vertices.size() << std::endl;
        std::cout << "Grupos: " << model.groups.size() << std::endl;
    }

    // Registro de callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);

    // Instruções básicas
    std::cout << "\n==== Visualizador de Modelos 3D ====\n";
    std::cout << "Controles:\n";
    std::cout << "  Mouse Esquerdo: Rotacionar a câmera\n";
    std::cout << "  Mouse Direito: Zoom (arraste para cima/baixo)\n";
    std::cout << "  W: Alternar modo wireframe\n";
    std::cout << "  L: Alternar iluminação\n";
    std::cout << "  T: Alternar texturização\n";
    std::cout << "  N: Mostrar/ocultar normais\n";
    std::cout << "  +/-: Zoom in/out\n";
    std::cout << "  R: Resetar visualização\n";
    std::cout << "  ESC: Sair\n";

    glutMainLoop();
    return 0;
}