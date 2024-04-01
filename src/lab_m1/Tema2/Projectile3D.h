#pragma once

namespace HomeWork {
    class Projectile3D : public Entity3D {
        glm::vec3 direction;
        bool m_hasCollided = false;
    public:
        float projectileSpeed;
        bool shouldDestroy = false;
        float projectileRadius = .75f;
        int projectileDamage = 25;

        Projectile3D(const glm::mat4& modelM, const glm::vec3& color, const glm::vec3& dir, std::shared_ptr<Renderer3D>& rend)
            : Entity3D(modelM, color, ObjectType::Projectile, rend), direction(dir), projectileSpeed(25) {}

        void translateProjectile(float deltaTime) {
            glm::vec3 directieDeplasare = direction * projectileSpeed * deltaTime;
            modelMat = glm::translate(modelMat, directieDeplasare);
        }

        bool hasCollided() const noexcept {
            return m_hasCollided;
        }

        void setHasCollided(bool hasCollided) noexcept {
            m_hasCollided = hasCollided;
        }

        void goofyDeathAnimationXD(float deltaTime) noexcept {
            glm::vec3 translation(0.0f, 0.0f, 0.0f);
            float speed = 10.0f;

            translation.y -= deltaTime * speed;

            modelMat = glm::translate(modelMat, translation);
        }

        // Hardcoded thresholds :(
        bool outOfScreen() {
            glm::vec3 pos = glm::vec3(modelMat[3]);
            return pos.y <= 0.5f || pos.x >= 200 || pos.z >= 200;
        }

    };
};