#pragma once
#include "lab_m1/Tema2/Tema2.h"

namespace HomeWork {

    class Entity3D;
    class Tank;
    class EnemyTank;
    class Projectile3D;

    // Pentru Building Collisions
    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
    };

    class CollisionDetection {
    public:
        static void PlayerEnemyCollision(Tank& player, std::vector<std::unique_ptr<EnemyTank>>& enemies);
        static void ProjectileEnemyCollision(std::vector<std::unique_ptr<EnemyTank>>& enemies, std::vector<std::unique_ptr<Projectile3D>>& projectiles);
        static void EnemyEnemyCollision(std::vector<std::unique_ptr<EnemyTank>>& enemies);
        [[nodiscard]] static bool PlayerBuildingCollision(Tank& player, std::array<Entity3D, 20>& buildings);
        static void ProjectileBuildingCollision(std::vector<std::unique_ptr<Projectile3D>>& projectiles, std::array<Entity3D, 20>& buildings);
        static void ProjectilePlayerCollision(Tank& player, std::vector<std::unique_ptr<Projectile3D>>& projectiles);
    };

}