#pragma once
#include "TankComponent.h"
#include <array>
#include <random>

namespace HomeWork {

    #define TURRET_DAMAGED_COLOR glm::vec3(.52f, .25f, .26f)
    #define ENEMY_CANNON_COLOR glm::vec3(1, .9f, 1)
    #define BODY_DAMAGED_COLOR /*glm::vec3(.78f, .09f, .32f)*/ ENEMY_CANNON_COLOR
    #define BUILDING_COLOR glm::vec3(.76f, .76f, .76f)

        // Axis Preprocessor
    #define OX_AXIS glm::vec3(1, 0, 0)
    #define OY_AXIS glm::vec3(0, 1, 0)
    #define OZ_AXIS glm::vec3(0, 0, 1)

    // Aka Tank-Player
    class Tank {
    protected:
        glm::vec3 forward;
        bool alreadyDamaged = false;

    public:
        // Reloading
        float maxReloadTimer = 2.0f;
        float reloadTimer = .0f;

        // Rendering
        std::shared_ptr<Renderer3D> renderer;

        // Tank Stats
        float tankSpeed = 20;
        int tankHealth = 100;
        float rotationSpeed = 2;
        float tankRadius = 2.0f;
        bool shouldDestroy = false;
        std::array<TankComponent, 4> tankComponents;

        Tank() = default;
        Tank(TankComponent& body, TankComponent& wheels, TankComponent& turret, TankComponent& cannon, std::shared_ptr<Renderer3D>& rend)
            : tankComponents{ {body, wheels, turret, cannon} }, renderer(rend), forward(glm::vec3(0, 0, 1)) {}

        void moveTank(float tx, float ty, float tz) {
            for (auto& component : tankComponents) {
                component.translate(tx, ty, tz);
            }
        }

        void rotateTank(float angle) {
            for (auto& component : tankComponents) {
                component.rotateY(angle);
            }
        }

        void updateForwardDirection() {
            glm::mat4 bodyMat = tankComponents[0].getModelMat();
            forward = glm::vec3(-bodyMat[2]);
            forward = glm::normalize(glm::vec3(forward.x, 0, forward.z)); // Flatten the vector to the ground plane
        }

        void setRenderer(std::shared_ptr<Renderer3D>& ptr_renderer) {
            renderer = ptr_renderer;
        }

        void translateForward(float deltaTime) {
            glm::vec3 translation = forward * tankSpeed * deltaTime;
            for (auto& component : tankComponents) {
                component.translate(translation.x, translation.y, translation.z);
            }
        }

        void translateBackwards(float deltaTime) {
            glm::vec3 translation = forward * tankSpeed * deltaTime;
            for (auto& component : tankComponents) {
                component.translate(-translation.x, -translation.y, -translation.z);
            }
        }

        void rotateLeft(float deltaTime) {
            float rotationAngle = rotationSpeed * deltaTime;
            for (auto& component : tankComponents) {
                component.rotateY(rotationAngle);
            }
            updateForwardDirection(); // Update the forward direction after rotation.
        }

        void rotateRight(float deltaTime) {
            float rotationAngle = -rotationSpeed * deltaTime;
            for (auto& component : tankComponents) {
                component.rotateY(rotationAngle);
            }

            updateForwardDirection();
        }

        void rotateTurelaAfterMousePosition(glm::vec2&& mouseDelta) {
            glm::mat4& turelaModMat = tankComponents[2].getModelMatRef();
            glm::mat4& cannonModMat = tankComponents[3].getModelMatRef();

            float rotSpeed = .1f;

            // Calculate the rotation angle based on the mouse movement
            float rotAngle = rotSpeed * mouseDelta.x;

            turelaModMat = glm::rotate(turelaModMat, -glm::radians(rotAngle), OY_AXIS);
            cannonModMat = glm::rotate(cannonModMat, -glm::radians(rotAngle), OY_AXIS);
        }

        glm::vec3 getForwardDirection() const {
            // Tank's direction, NU INTELEG DE CE E IN COLOANA A TREIA
            return glm::normalize(glm::vec3(tankComponents[0].getModelMat()[1]));
        }

        // This will change accordingly to the tanks rotation
        glm::vec3 getCannonForwardPosition() const {
            return glm::normalize(glm::vec3(tankComponents[3].getModelMat()[2]));
        }

        glm::vec3 getCannonEndPosition() const {
            glm::vec3 cannonForward = Tank::getCannonForwardPosition();

            glm::vec3 cannonPosition = glm::vec3(tankComponents[3].getModelMat()[3]);

            float cannonLength = 2.75f;

            glm::vec3 cannonEndPosition = cannonPosition + (cannonForward * cannonLength);

            return cannonEndPosition;
        }

        glm::vec3 getCenter() const {
            return tankComponents[2].localOrigin;
        }

        glm::vec3 Tank::getPosition() const {
            const glm::mat4& modelMat = tankComponents[0].getModelMat();
            return glm::vec3(modelMat[3]);
        }

        void setTurelaColor(const glm::vec3& o_color) noexcept {
            tankComponents[2].color = o_color;
        }

        void render() const {
            if (!renderer) {
                std::cout << "RENDERER FOR THE TANK NOT SPECIFIED, RETURNING\n";
                return;
            }

            for (const auto& component : tankComponents) {
                renderer->RenderObject<TankComponent>(component.meshName, shaderName[component.objectType], component, tankHealth);
            }
        }

        // Game Logic
        void takeDamage(int damage,
            glm::vec3&& damagedTurretColor = TURRET_DAMAGED_COLOR,
            glm::vec3&& damagedBodyColor = BODY_DAMAGED_COLOR) {

            tankHealth -= damage;

            if (tankHealth <= 0) {
                shouldDestroy = true;
                return;
            }

            if (tankHealth <= 50 && !alreadyDamaged) {

                if (!alreadyDamaged) {
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_real_distribution<> dis(-.5f, .5f);

                    glm::vec3 axis(dis(gen), dis(gen), dis(gen));
                    axis = glm::normalize(axis);
                    float angle = dis(gen);

                    glm::vec3 translation(dis(gen), dis(gen), dis(gen));

                    // Apply random transformations to the tank body
                    glm::mat4& modelMat = tankComponents[0].getModelMatRef();
                    modelMat = glm::rotate(tankComponents[0].getModelMat(), angle, axis);
                    modelMat = glm::translate(tankComponents[0].getModelMat(), translation);
                    alreadyDamaged = true;
                }

                // Change colors to show that its damage
                tankComponents[2].color = damagedTurretColor;
                tankComponents[1].color = damagedBodyColor;
            }
        }

        void decreaseReloadTimer(float deltaTime) {
            if (reloadTimer > 0) {
                reloadTimer -= deltaTime;
            }

            if (reloadTimer < 0) {
                reloadTimer = 0;
            }
        }

        bool canShoot() const {
            return reloadTimer <= 0;
        }
    };
}