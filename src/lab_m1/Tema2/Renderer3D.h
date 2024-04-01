#pragma once
#include <unordered_map>
#include <string>
#include "components/simple_scene.h"
#include "lab_m1/Tema2/lab_camera.h"

namespace HomeWork {


    using u_mapStrMesh = std::unordered_map<std::string, Mesh*>;
    using u_mapStrShader = std::unordered_map<std::string, Shader*>;
    using implemented::Camera;

    enum class ObjectType {
        NOT_AN_OBJ = 0,
        Tank,
        Terrain,
        Projectile,
        Building
    };

    extern std::unordered_map<ObjectType, std::string> shaderName;

    class Renderer3D {
        u_mapStrMesh& meshesRef;
        u_mapStrShader& shadersRef;
        glm::mat4& projectionMat;
        Camera* camera;
    public:

        Renderer3D(u_mapStrMesh& meshes, u_mapStrShader& shaders, glm::mat4& projMat, Camera* cam)
            : meshesRef(meshes), shadersRef(shaders), projectionMat(projMat), camera(cam) {}

        template<typename T>
        void RenderObject(const std::string& meshName, const std::string& shaderName,
            const T& objToRender, int tankHealth = 0) {

            Mesh* mesh = meshesRef[meshName];
            Shader* shader = shadersRef[shaderName];
            if (!mesh || !shader || !shader->program)
                return;

            const glm::mat4& modelMatrix = objToRender.getModelMat();
            const glm::vec3& color = objToRender.color;
            const ObjectType type = objToRender.objectType;

            shader->Use();
            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMat));
            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

            unsigned int colorLoc = glGetUniformLocation(shader->program, "u_color");
            glUniform3fv(colorLoc, 1, &color[0]);

            if (type == ObjectType::Tank) {
                if (const auto component = dynamic_cast<const TankComponent*>(&objToRender)) {
                    unsigned int tankHealthLoc = glGetUniformLocation(shader->program, "u_tankHealth");
                    glUniform1f(tankHealthLoc, static_cast<float>(tankHealth));

                    unsigned int isTankBodyLoc = glGetUniformLocation(shader->program, "u_isTankBody");
                    glUniform1i(isTankBodyLoc, (component->id == 0) ? 1 : 0);
                }
            }

            mesh->Render();
        }

        void RenderEffect(const std::string& meshName, const std::string& shaderName, glm::mat4& modelMat) {
            Mesh* mesh = meshesRef[meshName];
            Shader* shader = shadersRef[shaderName];
            if (!mesh || !shader || !shader->program)
                return;

            shader->Use();

            glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
            glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMat));

            glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMat));
            mesh->Render();
        }
    };
}