#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include "data.hpp"

struct Vertex
{
    float x, y, z;
};

struct Normal
{
    float nx, ny, nz;
};

struct TexCoord
{
    float u, v;
};

struct Mesh
{
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<TexCoord> texCoords;
    std::vector<unsigned int> vertexIndices, normalIndices, texCoordIndices;

    Translation translacao;
    Rotation rotacao;
    Scale escala;
    Color color;
    Material material;

    bool loadOBJ(const char *caminho, const char *mtlPath = nullptr)
    {
        std::ifstream arquivo(caminho);
        if (!arquivo.is_open())
        {
            std::cerr << "Erro ao abrir o arquivo: " << caminho << std::endl;
            return false;
        }

        std::map<std::string, Material> materiais;

        if (mtlPath)
        {
            std::ifstream mtlArquivo(mtlPath);
            if (!mtlArquivo.is_open())
            {
                std::cerr << "Erro ao abrir o arquivo MTL: " << mtlPath << std::endl;
                return false;
            }

            std::string linha;
            Material materialAtual;
            std::string nomeMaterial;

            while (std::getline(mtlArquivo, linha))
            {
                std::istringstream iss(linha);
                std::string tipo;
                iss >> tipo;
                if (tipo == "newmtl")
                {
                    iss >> nomeMaterial;
                    materiais[nomeMaterial] = materialAtual;
                    materialAtual = Material();
                }
                else if (tipo == "Ka")
                    iss >> materialAtual.ambient.r >> materialAtual.ambient.g >> materialAtual.ambient.b;
                else if (tipo == "Kd")
                    iss >> materialAtual.diffuse.r >> materialAtual.diffuse.g >> materialAtual.diffuse.b;
                else if (tipo == "Ks")
                    iss >> materialAtual.specular.r >> materialAtual.specular.g >> materialAtual.specular.b;
                else if (tipo == "Ns")
                    iss >> materialAtual.shininess;
            }
        }

        std::string linha;
        std::string materialName = "";
        while (std::getline(arquivo, linha))
        {
            std::istringstream iss(linha);
            std::string tipo;
            iss >> tipo;
            if (tipo == "v")
            {
                Vertex v;
                iss >> v.x >> v.y >> v.z;
                vertices.push_back(v);
            }
            else if (tipo == "vn")
            {
                Normal n;
                iss >> n.nx >> n.ny >> n.nz;
                normals.push_back(n);
            }
            else if (tipo == "vt")
            {
                TexCoord t;
                iss >> t.u >> t.v;
                texCoords.push_back(t);
            }
            else if (tipo == "f")
            {
                std::string vertice_str;
                std::vector<unsigned int> vi, ti, ni;

                while (iss >> vertice_str)
                {
                    std::replace(vertice_str.begin(), vertice_str.end(), '/', ' ');
                    std::istringstream viss(vertice_str);
                    unsigned int v, t, n;
                    viss >> v >> t >> n;
                    vi.push_back(v - 1);
                    ti.push_back(t - 1);
                    ni.push_back(n - 1);
                }

                if (!materialName.empty())
                {
                    material = materiais[materialName];
                }

                for (size_t i = 1; i < vi.size() - 1; ++i)
                {
                    vertexIndices.push_back(vi[0]);
                    vertexIndices.push_back(vi[i]);
                    vertexIndices.push_back(vi[i + 1]);

                    texCoordIndices.push_back(ti[0]);
                    texCoordIndices.push_back(ti[i]);
                    texCoordIndices.push_back(ti[i + 1]);

                    normalIndices.push_back(ni[0]);
                    normalIndices.push_back(ni[i]);
                    normalIndices.push_back(ni[i + 1]);
                }
            }
            else if (tipo == "usemtl")
            {
                iss >> materialName;
            }
        }
        return true;
    }

    void draw()
    {
        GLfloat prevAmbient[4], prevDiffuse[4], prevSpecular[4], prevShininess[1];
        glGetMaterialfv(GL_FRONT, GL_AMBIENT, prevAmbient);
        glGetMaterialfv(GL_FRONT, GL_DIFFUSE, prevDiffuse);
        glGetMaterialfv(GL_FRONT, GL_SPECULAR, prevSpecular);
        glGetMaterialfv(GL_FRONT, GL_SHININESS, prevShininess);

        if (material.shininess > 0) {
            GLfloat matAmbient[] = {material.ambient.r, material.ambient.g, material.ambient.b, 1.0f};
            GLfloat matDiffuse[] = {material.diffuse.r, material.diffuse.g, material.diffuse.b, 1.0f};
            GLfloat matSpecular[] = {material.specular.r, material.specular.g, material.specular.b, 1.0f};
            GLfloat matShininess[] = {material.shininess};

            glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
            glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
        }

        glPushMatrix();
        glTranslatef(translacao.x, translacao.y, translacao.z);
        glRotatef(rotacao.angulo, rotacao.x, rotacao.y, rotacao.z);
        glScalef(escala.x, escala.y, escala.z);
        glColor3f(color.r, color.g, color.b);

        glBegin(GL_TRIANGLES);
        for (size_t i = 0; i < vertexIndices.size(); ++i)
        {
            unsigned int vIndex = vertexIndices[i];
            unsigned int nIndex = normalIndices.size() > i ? normalIndices[i] : 0;
            unsigned int tIndex = texCoordIndices.size() > i ? texCoordIndices[i] : 0;

            if (!normals.empty() && nIndex < normals.size()){
                const Normal &n = normals[nIndex];
                glNormal3f(n.nx, n.ny, n.nz);
            }

            if (!texCoords.empty() && tIndex < texCoords.size())
            {
                const TexCoord &t = texCoords[tIndex];
                glTexCoord2f(t.u, t.v);
            }

            const Vertex &v = vertices[vIndex];
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();

        glMaterialfv(GL_FRONT, GL_AMBIENT, prevAmbient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, prevDiffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, prevSpecular);
        glMaterialfv(GL_FRONT, GL_SHININESS, prevShininess);

        glPopMatrix();
    }

    void setTranslation(float x, float y, float z){
        translacao.x = x;
        translacao.y = y;
        translacao.z = z;
    }

    void setRotation(float angle, float x, float y, float z){
        rotacao.angulo = angle;
        rotacao.x = x;
        rotacao.y = y;
        rotacao.z = z;
    }

    void setScale(float x, float y, float z){
        escala.x = x;
        escala.y = y;
        escala.z = z;
    }

    void setScaleOneAttribut(float s){
        escala.x = s;
        escala.y = s;
        escala.z = s;
    }

    void setColor(float r, float g, float b){
        color.r = r;
        color.g = g;
        color.b = b;
    }

    Translation getTranslation(){
        return translacao;
    }

    Rotation getRotation(){
        return rotacao;
    }

    Scale getScale(){
        return escala;
    }

    Color getColor(){
        return color;
    }

    Material getMaterial(){
        return material;
    }

    void setMaterial(const Material &mat){
        material = mat;
    }

    Color getMaterialAmbient(){
        return material.ambient;
    }

    void setMaterialAmbient(const Color &ambient){
        material.ambient = ambient;
    }

    Color getMaterialDiffuse(){
        return material.diffuse;
    }

    void setMaterialDiffuse(const Color &diffuse){
        material.diffuse = diffuse;
    }

    Color getMaterialSpecular(){
        return material.specular;
    }

    void setMaterialSpecular(const Color &specular){
        material.specular = specular;
    }

    float getMaterialShininess(){
        return material.shininess;
    }

    void setMaterialShininess(float shininess){
        material.shininess = shininess;
    }
};

#endif
