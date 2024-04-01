#pragma once
#include <string>

namespace HomeWork {
    class TankComponent : public Entity3D {
    public:
        std::string meshName;
        glm::vec3 localOrigin; // Local origin for rotation
        int id = -1;

        TankComponent() = default;

        TankComponent(const glm::mat4& modelM, const std::string& meshName, const glm::vec3& color, const glm::vec3& origin, int id)
            : Entity3D(modelM, color), meshName(meshName), localOrigin(origin) {

            objectType = ObjectType::Tank;
            this->id = id;
        }

        void TankComponent::rotateY(float angle) {
            glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0));
            glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), localOrigin);
            glm::mat4 inverseTranslationMat = glm::translate(glm::mat4(1.0f), -localOrigin);

            modelMat = translationMat * rotationMat * inverseTranslationMat * modelMat;
        }

        void translate(float tx, float ty, float tz) override {
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, tz));
            modelMat = translationMatrix * modelMat;
            localOrigin += glm::vec3(tx, 0, tz);
        }
    };
}

