#include "CollisionDetection.h"

using namespace HomeWork;

void CollisionDetection::PlayerEnemyCollision(Tank& player, std::vector<std::unique_ptr<EnemyTank>>& enemies) {
    glm::vec3 pos1 = player.getPosition();
    for (const auto& enemy : enemies) {
        glm::vec3 pos2 = enemy->getPosition();
        float distanceBetweenTanks = glm::distance(pos1, pos2);
        float overlap = player.tankRadius + enemy->tankRadius - distanceBetweenTanks;
        
        // If they actually touch
        if (overlap > 0) {

            glm::vec3 difference = enemy->getCenter() - player.getCenter();
            glm::vec3 P = glm::normalize(difference) * overlap;

            // Move each tank away from the other by half the overlap
            enemy->moveTank(P.x * 0.5f, P.y * 0.5f, P.z * 0.5f);
        }

        // Enemy should start attacking
        float attackOverlap = player.tankRadius + enemy->detectionRadius - distanceBetweenTanks;
        if (attackOverlap > 0) {
            enemy->setNextState(EnemyState::ATTACKS);
        }
    }
}

void CollisionDetection::ProjectileEnemyCollision(std::vector<std::unique_ptr<EnemyTank>>& enemies, std::vector<std::unique_ptr<Projectile3D>>& projectiles) {
    for (const auto& projectile : projectiles) {
        glm::vec3 projectilePos = projectile->getModelMat()[3];
        for (const auto& enemy : enemies) {
            glm::vec3 enemyPos = enemy->getPosition();
            float distance = glm::distance(projectilePos, enemyPos);
            float overlap = projectile->projectileRadius + enemy->tankRadius - distance;

            if (overlap > 0) {
                projectile->shouldDestroy = true;

                // Take Damage
                enemy->takeDamage(projectile->projectileDamage);
            }
        }
    }
}

void CollisionDetection::EnemyEnemyCollision(std::vector<std::unique_ptr<EnemyTank>>& enemies) {
    for (size_t i = 0; i < enemies.size(); ++i) {
        for (size_t j = 0; j < enemies.size(); ++j) {
            if (i == j) {
                continue;
            }

            glm::vec3 enemy1Pos = enemies[i]->getPosition();
            glm::vec3 enemy2Pos = enemies[j]->getPosition();
            float distanceBetweenTanks = glm::distance(enemy1Pos, enemy2Pos);

            float overlap = enemies[i]->tankRadius * 2 - distanceBetweenTanks;

            if (overlap > 0) {
                glm::vec3 difference = enemies[j]->getCenter() - enemies[i]->getCenter();
                glm::vec3 P = glm::normalize(difference) * overlap;

                // Move each tank away from the other by half the overlap
                enemies[j]->moveTank(P.x * 0.5f, P.y * 0.5f, P.z * 0.5f);
            }
        }
    }
}

// Simplified function to calculate an AABB
AABB calculateAABB(const glm::vec3& position, const glm::vec3& size) {
    AABB box;
    box.min = position - size / 2.0f;
    box.max = position + size / 2.0f;
    return box;
}

// AABB collision from a forum
bool isColliding(const AABB& a, const AABB& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
        (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

// Vom avea o coliziune practic 2D, avand in vedere ca nu ne intereseaza Y-ul
bool CollisionDetection::PlayerBuildingCollision(Tank& player, std::array<Entity3D, 20>& buildings) {
    // Assume an approximate size for the player/tank
    glm::vec3 playerSize = glm::vec3(3.5f, .5f, 3.5f); // Adjust this based on your tank size
    glm::vec3 playerPos = player.getPosition();
    AABB playerBox = calculateAABB(playerPos, playerSize);

    for (const auto& building : buildings) {
        glm::vec3 buildingPos = glm::vec3(building.getModelMat()[3]);
        glm::vec3 scaleFactors = glm::vec3(building.getModelMat()[0][0], building.getModelMat()[1][1], building.getModelMat()[2][2]);
        glm::vec3 buildingSize = scaleFactors; // Box-ul initial este 1x1x1
        AABB buildingBox = calculateAABB(buildingPos, buildingSize);

        if (isColliding(playerBox, buildingBox))
            return true;
    }

    return false;
}

// Vom avea o coliziune practic 2D, avand in vedere ca nu ne intereseaza Y-ul
void CollisionDetection::ProjectileBuildingCollision(std::vector<std::unique_ptr<Projectile3D>>& projectiles, std::array<Entity3D, 20>& buildings){
    // Assume an approximate size for the player/tank
    glm::vec3 projectileSize = glm::vec3(1.5f, .5f, 1.5f); // Adjust this based on your tank size
    for (const auto& projectile : projectiles) {
        glm::vec3 projectilePos = glm::vec3(projectile->getModelMat()[3]);
        AABB projectileBox = calculateAABB(projectilePos, projectileSize);
   
        for (const auto& building : buildings) {
            glm::vec3 buildingPos = glm::vec3(building.getModelMat()[3]);
            glm::vec3 scaleFactors = glm::vec3(building.getModelMat()[0][0], building.getModelMat()[1][1], building.getModelMat()[2][2]);
            glm::vec3 buildingSize = scaleFactors; // Box-ul initial este 1x1x1
            AABB buildingBox = calculateAABB(buildingPos, buildingSize);
            if (isColliding(projectileBox, buildingBox) && !projectile->hasCollided()){
                projectile->projectileSpeed = 0;
                projectile->setHasCollided(true);
            }
        }
    }
}

void CollisionDetection::ProjectilePlayerCollision(Tank& player, std::vector<std::unique_ptr<Projectile3D>>& projectiles) {
    glm::vec3 playerPos = player.getPosition();

    for (const auto& projectile : projectiles) {
        glm::vec3 projectilePos = projectile->getModelMat()[3];
        float distance = glm::distance(projectilePos, playerPos);
        float overlap = projectile->projectileRadius + player.tankRadius - distance;

        if (overlap > 0) {
            std::cout << "BOOOM\n";
            projectile->shouldDestroy = true;

                // Take Damage
            player.takeDamage(15);
        }
        
    }
}