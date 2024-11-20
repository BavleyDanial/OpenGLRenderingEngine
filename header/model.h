#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mesh.h>
#include <Renderer/shader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

namespace OGLR {

    uint32_t LoadTexture(const std::string& path, const std::string& directory) {
        std::string file_name = directory + "/" + path;
        int width, height, nrComponents;
        Texture texture;
        uint8_t* data = stbi_load(file_name.c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            int format = GL_RGBA;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glGenTextures(1, &texture.id);
            glBindTexture(GL_TEXTURE_2D, texture.id);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        } else {
            std::cout << "Texture failed to load at path: " << path << '\n';
            stbi_image_free(data);
        }
        return texture.id;
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
            const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate          |
                                                            aiProcess_FixInfacingNormals        |
                                                            aiProcess_PreTransformVertices      |
                                                            aiProcess_GenNormals                |
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
            std::vector<Texture> textures;

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

                
                if(!mesh->mTextureCoords[0]) 
                    vertex.tex_coords = glm::vec2(0.0f, 0.0f);
                else {
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
            // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
            // Same applies to other texture as the following list summarizes:
            // diffuse: texture_diffuseN
            // specular: texture_specularN
            // normal: texture_normalN 
            // 1. diffuse maps
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            
            std::vector<Texture> shininessMaps = loadMaterialTextures(material, aiTextureType_SHININESS, "texture_shininess");
            textures.insert(textures.end(), shininessMaps.begin(), shininessMaps.end());
            
            // return a mesh object created from the extracted mesh data
            return Mesh(vertices, indices, textures);
        }

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName) {
            std::vector<Texture> textures;
            for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
                aiString str;
                mat->GetTexture(type, i, &str);
                // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
                bool skip = false;
                for (uint32_t j = 0; j < mTexturesLoaded.size(); j++) {
                    if(std::strcmp(mTexturesLoaded[j].path.c_str(), str.C_Str()) == 0) {
                        textures.push_back(mTexturesLoaded[j]);
                        skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                        break;
                    }
                }
                if(!skip) {   // if texture hasn't been loaded already, load it
                    Texture texture;
                    texture.path = std::string(str.C_Str());
                    texture.id = LoadTexture(str.C_Str(), mDirectory);
                    texture.type = typeName;
                    textures.push_back(texture);
                    mTexturesLoaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
                }
            }
            return textures;
        }
    private:
        std::vector<Texture> mTexturesLoaded;
        std::vector<Mesh>    mMeshes;
        glm::mat4 mModelMatrix;
        std::string mDirectory;
    };


    
    
}