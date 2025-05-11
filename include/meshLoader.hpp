#ifndef MESH_LOADER_HPP
#define MESH_LOADER_HPP

#include <vector>
#include "mesh.hpp"

class MeshLoader
{
public:
    std::vector<Mesh> modelos;

    bool loadModel(const char *caminhoObj, const char *caminhoMtl, 
                           Translation trans, Rotation rot, Scale esc,
                           Color modeloColor, Color ambientColor, Color diffuseColor,
                           Color specularColor, float shininess, ModelType tipoArquivo);
    void drawAll();
    void drawForId(int id);
    void updateModelTranslationXById(int id, float x);
    void updateModelTranslationYById(int id, float y);
    void updateModelTranslationZById(int id, float z);

    // Funções de atualização de Rotação por Coordenada
    void updateModelRotationXById(int id, float x);
    void updateModelRotationYById(int id, float y);
    void updateModelRotationZById(int id, float z);
    void updateModelRotationAngleById(int id, float angle);

    // Funções de atualização de Escala por Coordenada
    void updateModelScaleXById(int id, float x);
    void updateModelScaleYById(int id, float y);
    void updateModelScaleZById(int id, float z);

    // Funções de atualização de Cor por Componente
    void updateModelColorRById(int id, float r);
    void updateModelColorGById(int id, float g);
    void updateModelColorBById(int id, float b);

};

#endif // GERENCIADOR_DE_MODELOS_HPP
