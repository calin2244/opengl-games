#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/lab_camera.h"
#include "CollisionDetection.h"
#include "Renderer3D.h"
#include "Entity3D.h"
#include "TankComponent.h"
#include "Tank.h"
#include "Projectile3D.h"
#include "EnemyTank.h"
#include <array>
#include <iostream>
#include <random>
#include <typeinfo>

namespace HomeWork
{   
    // Effects Scaling
    #define DEATH_EFFECT_SCALE 7.5f
    #define SMOKE_EFFECT_SCALE 2.5f

    // EFFECTS
    enum class EffectType {
        NOT_AN_EFFECT = 0,
        DeathEffect,
        ShootingEffect
    };

    class Effects {
    protected:
        glm::mat4 modelMat;
        glm::vec3 forward;
        std::shared_ptr<Renderer3D> renderer;
        float aliveTimer = 1.f;
        bool hasScaled = false;
    public:
        EffectType effectType;
        bool shouldDestroy = false;

        Effects(const glm::mat4& modelM, const glm::vec3& forward, std::shared_ptr<Renderer3D> rend)
            : modelMat(modelM), forward(forward), renderer(rend) {

            effectType = EffectType::NOT_AN_EFFECT;
            // rotate so it looks forward
            glm::vec3 position = glm::normalize(glm::vec3(modelMat[3]));

            // get the cosine of the angle
            float rotAngle = glm::dot(forward, position);
            modelMat = glm::rotate(modelMat, rotAngle, OY_AXIS);
        }

        virtual void render(float scale = 1.f) {
            return;
        }

        virtual void init(const glm::mat4& modelM, const glm::vec3& forward) {
            modelMat = modelM;
            this->forward = forward;
            shouldDestroy = false;
            hasScaled = false;
            aliveTimer = 1.f;
        }

        void decreaseTimer(float deltaTime) {
            aliveTimer -= deltaTime;
            if (aliveTimer <= 0) {
                shouldDestroy = true;
            }
        }
    };

    class DeathEffect : public Effects {
    public:
        DeathEffect(const glm::mat4& modelM, const glm::vec3& forward, std::shared_ptr<Renderer3D> rend)
            : Effects(modelM, forward, rend) {

            aliveTimer = 1.f;
            effectType = EffectType::DeathEffect;
        }

        void render(float scale = 1.f) override {
            if (scale != 1.f && !hasScaled) {
                modelMat = glm::scale(modelMat, glm::vec3(scale));
                hasScaled = true;
            }

            renderer->RenderEffect("explosion", "textureShader", modelMat);
        }
    };

    class ShootingEffect : public Effects {
        float scale = 0;
        float maxScale = 2.5f;
        float scaleFactor = 5.f;
        glm::mat4 initialModelMat;
    public:
        ShootingEffect(const glm::mat4& modelM, const glm::vec3& forward, std::shared_ptr<Renderer3D> rend)
            : Effects(modelM, forward, rend), initialModelMat(modelM) {

            aliveTimer = 1.9f;
            effectType = EffectType::ShootingEffect;
        }

        void render(float deltaTime) {
            if (scale < maxScale && !hasScaled) {
                scale += deltaTime * scaleFactor;
                
                if (scale >= maxScale) {
                    scale = maxScale;
                    hasScaled = true;
                }

                modelMat = glm::scale(initialModelMat, glm::vec3(scale));
            }

            if (scale > 0 && hasScaled) {
                scale -= deltaTime * (scaleFactor / 2);
                
                modelMat = glm::scale(initialModelMat, glm::vec3(scale));
            }

            renderer->RenderEffect("muzzleSmoke", "textureShader", modelMat);
        }

        void init(const glm::mat4& modelM, const glm::vec3& forward) override {
            modelMat = modelM;
            initialModelMat = modelM;
            this->forward = forward;
            shouldDestroy = false;
            hasScaled = false;
            aliveTimer = 1.9f;
            scale = 0;
        }
    };

    // Cool design pattern for recycling objects
    class EffectsPool {
    private:
        std::list<std::unique_ptr<Effects>> effPool;

    public:

        template<typename T>
        std::unique_ptr<T> createEffect(const glm::mat4& modelMat, const glm::vec3& forward, std::shared_ptr<Renderer3D> renderer) {
            if (effPool.empty()) {
                return std::make_unique<T>(modelMat, forward, renderer);
            }
            else {
                for (auto it = effPool.begin(); it != effPool.end(); ++it) {
                    if (typeid(**it) == typeid(T)) {
                        std::unique_ptr<T> effect = std::unique_ptr<T>(static_cast<T*>(it->release()));
                        static_cast<T*>(effect.get())->init(modelMat, forward);
                        it = effPool.erase(it);
                        return effect;
                    }
                }

            }
        }

        void recycleEffect(std::unique_ptr<Effects> effect) {
            effPool.push_back(std::move(effect));
        }

        static void recycleEffects(std::vector<std::unique_ptr<Effects>>& activeEffs, EffectsPool* effPool) {
            for (size_t i = 0; i < activeEffs.size(); ++i) {
                if (activeEffs[i]->shouldDestroy) {
                    effPool->recycleEffect(std::move(activeEffs[i]));
                    activeEffs.erase(activeEffs.begin() + i);
                    std::cout << "RECYCLEED\n";
                }
                else {
                    ++i; // no element removed
                }
            }
        }

        size_t size() {
            return effPool.size();
        }

    };


    class Tema2 : public gfxc::SimpleScene
    {


    public:
        enum class CameraState {
            NORMAL_STATE = 0,
            FREEROAM_STATE
        };

        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void RenderScene(float deltaTime);
        void DestroyGameObjects(float deltaTime);
        void addNewMesh(const char* meshName, const char* path, const char* modelName, WindowObject* window);
        void addNewShader(const char* shaderName, const char* path, const char* shaderFileName, WindowObject* window);
        void generateRandomBuildings();
        void generateRandomEnemies();
        void generateRandomWaypoints();

    protected:
        float fov = 100;
        implemented::Camera* camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        // The Renderer
        std::shared_ptr<Renderer3D> renderer;

        // The terrain(plane)
        Entity3D terrain;

        // Tank(Player)
        std::unique_ptr<Tank> player;

        // Tank(Enemies)
        std::vector<std::unique_ptr<EnemyTank>> enemies;

        // Projectiles
        std::vector<std::unique_ptr<Projectile3D>> projectiles;

        // Camera State
        CameraState camState = CameraState::NORMAL_STATE;
        float lookBack = 1; // For looking backwards


        // Death Effect
        std::vector<std::unique_ptr<Effects>> activeEffects;

        // Object Pool for Effects
        EffectsPool effectsPool;

        // Buildings
        std::array<Entity3D, 20> buildings;
    };
}   // namespace HomeWork
