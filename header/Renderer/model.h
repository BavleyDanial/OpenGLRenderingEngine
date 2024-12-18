#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Renderer/mesh.h>
#include <Renderer/shader.h>
#include <Renderer/Texture2D.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <string>
#include <iostream>
#include <vector>

namespace OGLR {

    inline Texture2D LoadTexture(const std::string& path, const std::string& typeName, const std::string& directory) {
        TextureSpecs specs;
        specs.path = directory + "/" + path;
        specs.type = typeName;

        int width, height, nrComponents;
        uint8_t* data = stbi_load(specs.path.c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            if (nrComponents == 1)
                specs.format = GL_RED;
            else if (nrComponents == 3)
                specs.format = GL_RGB;
            else if (nrComponents == 4)
                specs.format = GL_RGBA;

            specs.width = width;
            specs.height = height;

            Texture2D texture(data, specs);
            stbi_image_free(data);
            return texture;
        }
        std::cout << "Texture failed to load at path: " << path << '\n';
        data = new uint8_t[3]{1, 1, 1};
        specs.width = 3;
        specs.height = 1;
        Texture2D texture(data, specs);
        stbi_image_free(data);
        return texture;
    }

    class Model {
    public:
        Model(const std::string& path)
            :mModelMatrix(1.0f) {
            loadModel(path);
        }

        void Translate(const glm::vec3& world_pos) {
            mModelMatrix = glm::translate(mModelMatrix, world_pos);
        }

        void Rotate(float degrees, const glm::vec3& axis) {
            mModelMatrix = glm::rotate(mModelMatrix, glm::radians(degrees), axis);
        }

        void Scale(const glm::vec3& world_scale) {
            mModelMatrix = glm::scale(mModelMatrix, world_scale);
        }

        void Draw(Shader* shader, const glm::mat4& view, const glm::mat4& proj) {
            for (uint32_t i = 0; i < mMeshes.size(); i++)
                mMeshes[i].Draw(shader, mModelMatrix, view, proj);
        }
    private:
        void loadModel(const std::string& path) {
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(path,  aiProcess_Triangulate               |
                                                            aiProcess_FixInfacingNormals        |
                                                            aiProcess_PreTransformVertices      |
                                                            aiProcess_GenNormals                |
                                                            aiProcess_GenUVCoords               |
                                                            //aiProcess_OptimizeMeshes            |
                                                            //aiProcess_JoinIdenticalVertices     |
                                                            aiProcess_FlipUVs);
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << "\n";
                return;
            }
            mDirectory = path.substr(0, path.find_last_of('/'));
            processNode(scene->mRootNode, scene);
        }

        void processNode(aiNode* node, const aiScene* scene) {
            for (uint32_t i = 0; i < node->mNumMeshes; i++) {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                mMeshes.push_back(processMesh(mesh, scene));
            }
            for (uint32_t i = 0; i < node->mNumChildren; i++) {
                processNode(node->mChildren[i], scene);
            }

        }

        Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
            std::vector<Vertex> vertices;
            std::vector<uint32_t> indices;
            std::vector<Texture2D> textures;

            for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
                Vertex vertex;
                glm::vec3 vector;
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.position = vector;

                if (mesh->HasNormals()) {
                    vector.x = mesh->mNormals[i].x;
                    vector.y = mesh->mNormals[i].y;
                    vector.z = mesh->mNormals[i].z;
                    vertex.normal = vector;
                }

                if(!mesh->mTextureCoords[0]) {
                    vertex.tex_coords = glm::vec2(0.0f, 0.0f);
                } else {
                    glm::vec2 vec;
                    vec.x = mesh->mTextureCoords[0][i].x; 
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.tex_coords = vec;
                }

                vertices.push_back(vertex);
            }

            for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (uint32_t j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }

            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            std::vector<Texture2D> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            std::vector<Texture2D> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            std::vector<Texture2D> shininessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "texture_shininess");
            textures.insert(textures.end(), shininessMaps.begin(), shininessMaps.end());

            return Mesh(vertices, indices, textures);
        }

        std::vector<Texture2D> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName) {
            std::vector<Texture2D> textures;
            for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
                aiString str;
                mat->GetTexture(type, i, &str);
                bool skip = false;
                for (uint32_t j = 0; j < mTexturesLoaded.size(); j++) {
                    if(std::strcmp(mTexturesLoaded[j].GetPath().c_str(), (mDirectory + '/' + std::string(str.C_Str())).c_str()) == 0) {
                        textures.push_back(mTexturesLoaded[j]);
                        skip = true;
                        break;
                    }
                }
                if(!skip) {
                    Texture2D texture = LoadTexture(str.C_Str(), typeName, mDirectory);
                    textures.push_back(texture);
                    mTexturesLoaded.push_back(texture);
                }
            }
            return textures;
        }
    private:
        std::vector<Texture2D> mTexturesLoaded;
        std::vector<Mesh>    mMeshes;
        glm::mat4 mModelMatrix;
        std::string mDirectory;
    };


}
