#include "meshLoader.hpp"
#include "mesh.hpp"
#include "data.hpp"

bool MeshLoader::loadModel(const char *caminhoObj, const char *caminhoMtl, 
                           Translation trans, Rotation rot, Scale esc,
                           Color modeloColor, Color ambientColor, Color diffuseColor,
                           Color specularColor, float shininess, ModelType tipoArquivo)
{
    Mesh modelo;
    modelo.setTranslation(trans.x, trans.y, trans.z);
    modelo.setRotation(rot.x, rot.y, rot.z, rot.angulo);
    modelo.setScale(esc.x, esc.y, esc.z);
    modelo.setColor(modeloColor.r, modeloColor.g, modeloColor.b);

    Material material;
    material.ambient = {ambientColor.r, ambientColor.g, ambientColor.b};
    material.diffuse = {diffuseColor.r, diffuseColor.g, diffuseColor.b};
    material.specular = {specularColor.r, specularColor.g, specularColor.b};
    material.shininess = shininess;

    modelo.setMaterial(material);

    bool carregado = false;

    switch (tipoArquivo)
    {
    case ModelType::OBJ:
        if (caminhoObj != nullptr) {
            carregado = modelo.loadOBJ(caminhoObj, caminhoMtl);
        }else{
            std::cerr << "Caminho do arquivo OBJ não fornecido!" << std::endl;
            return false;
        }
        break;
    case ModelType::GLB:
        break;
    case ModelType::GLTF:
        break;
    case ModelType::CUSTOM:
        break;
    default:
        std::cerr << "Tipo de arquivo não suportado!" << std::endl;
        return false;
    }

    if (carregado){
        modelos.push_back(std::move(modelo));
        return true;
    }

    return false;
}
void MeshLoader::drawAll(){
    for (auto &modelo : modelos){
        modelo.draw();
    }
}

void MeshLoader::drawForId(int id){
    if (id < 0 || id >= modelos.size()) return;
    modelos[id].draw();
}

void MeshLoader::updateModelTranslationXById(int id, float x){
    modelos[id].setTranslation(x, modelos[id].getTranslation().y, modelos[id].getTranslation().z);
}

void MeshLoader::updateModelTranslationYById(int id, float y){
    modelos[id].setTranslation(modelos[id].getTranslation().x, y, modelos[id].getTranslation().z);
}

void MeshLoader::updateModelTranslationZById(int id, float z){
    modelos[id].setTranslation(modelos[id].getTranslation().x, modelos[id].getTranslation().y, z);
}

void MeshLoader::updateModelRotationXById(int id, float x){
    modelos[id].setRotation( modelos[id].getRotation().angulo,x, modelos[id].getRotation().y, modelos[id].getRotation().z);
}

void MeshLoader::updateModelRotationYById(int id, float y){
    if (id < 0 || id >= modelos.size()) return;
    modelos[id].setRotation(modelos[id].getRotation().angulo, modelos[id].getRotation().x, y, modelos[id].getRotation().z);
}

void MeshLoader::updateModelRotationZById(int id, float z){
    modelos[id].setRotation( modelos[id].getRotation().angulo,modelos[id].getRotation().x, modelos[id].getRotation().y, z);
}

void MeshLoader::updateModelRotationAngleById(int id, float angle){
    modelos[id].setRotation(angle,modelos[id].getRotation().x, modelos[id].getRotation().y, modelos[id].getRotation().z);
}

void MeshLoader::updateModelScaleXById(int id, float x){
    modelos[id].setScale(x, modelos[id].getScale().y, modelos[id].getScale().z);
}

void MeshLoader::updateModelScaleYById(int id, float y){
    modelos[id].setScale(modelos[id].getScale().x, y, modelos[id].getScale().z);
}

void MeshLoader::updateModelScaleZById(int id, float z){
    modelos[id].setScale(modelos[id].getScale().x, modelos[id].getScale().y, z);
}

void MeshLoader::updateModelColorRById(int id, float r){
    modelos[id].setColor(r, modelos[id].getColor().g, modelos[id].getColor().b);
}

void MeshLoader::updateModelColorGById(int id, float g){
    modelos[id].setColor(modelos[id].getColor().r, g, modelos[id].getColor().b);
}

void MeshLoader::updateModelColorBById(int id, float b){
    modelos[id].setColor(modelos[id].getColor().r, modelos[id].getColor().g, b);
}
