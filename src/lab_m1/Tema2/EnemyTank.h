#pragma once

namespace HomeWork {
    // Create FSM for Enemy Tanks
    enum class EnemyState {
        IDLE, // sta pe loc
        PATROL, // doar se misca, path finding
        ATTACKS
    };

    struct WayPoint {
        bool visited;
        glm::vec3 targetPosition;
        WayPoint(const glm::vec3& targetPosition) : visited(false), targetPosition(targetPosition) {}
    };

    class EnemyTank : public Tank {
        EnemyState tankState;
        std::vector<WayPoint> waypoints;
        bool hasRotated = false;
        Tank& player; // refference to the player for location etc..

        // The time the enemy is going to be angry >:( (hunt the player)
        float huntDurationMax = 4.f;
        float huntDuration = 0.f;

        // Reloading
        bool isReloading = false;
    public:
        float detectionRadius = 15.f;

        EnemyTank() = delete;
        EnemyTank(Tank& tank) : player(tank) {
            for (size_t i = 0; i < tank.tankComponents.size(); ++i) {
                tankComponents[i] = tank.tankComponents[i];
            }

            renderer = tank.renderer;
            tankState = EnemyState::PATROL;
            tankSpeed = 5;
            tankHealth = 75;
        }

        void patrolStateLogic(float deltaTime, std::vector<WayPoint>& waypoints) {
            if (waypoints.empty()) {
                return; 
            }

            // Find the first unvisited waypoint
            auto it = std::find_if(waypoints.begin(), waypoints.end(), [](const WayPoint& wp) {
                return !wp.visited;
                });

            // Cycle through them
            if (it == waypoints.end()) {
                for (auto& waypoint : waypoints) {
                    waypoint.visited = false;
                }
            
                it = waypoints.begin();
            }


            glm::vec3 currentPosition = Tank::getPosition();
            glm::vec3 targetPosition = it->targetPosition;


            // Check if the tank is facing the waypoint
            glm::vec3 tankForward = Tank::getForwardDirection();
            // directia tancului catre waypoint
            glm::vec3 waypointDirection = glm::normalize(targetPosition - currentPosition);
            float dot = glm::dot(tankForward, waypointDirection);

            // Rotate the tank so it faces the waypoint
            // Almost 1
            if (-dot < .99f && !hasRotated) {
                Tank::rotateLeft(deltaTime);
                return;
            }
            else {
                hasRotated = true;
            }

            glm::vec3 direction = glm::normalize(targetPosition - currentPosition);
            float distance = glm::distance(currentPosition, targetPosition);

            // Close enough
            if (distance < 1) { 
                hasRotated = false;
                it->visited = true; 
                return;
            }

            glm::vec3 translation = direction * tankSpeed * deltaTime;

            if (glm::length(translation) > distance) {
                translation = direction * distance;
            }

            moveTank(translation.x, 0, translation.z);
        }

        void attackStateLogic(float deltaTime) {
            setTurelaColor(glm::vec3(1, 0, 0));
            if (huntDuration == 0.f && tankState == EnemyState::ATTACKS) {
                huntDuration = huntDurationMax;
            }
            else {
                huntDuration -= deltaTime;

                // Calculam directia de la inamic catre player
                glm::vec3 playerPosition = player.getPosition();
                glm::vec3 enemyPosition = getPosition();
                glm::vec3 direction = glm::normalize(playerPosition - enemyPosition);

                glm::vec3 currentForward = getForwardDirection(); // deja normalizat
                float dotProduct = glm::dot(currentForward, direction);
                float angle = glm::degrees(acos(dotProduct));

                glm::vec3 crossProduct = glm::cross(currentForward, direction);
                if (crossProduct.y > 0) { // Adjusted the condition here
                    angle = -angle; // Rotate counterclockwise
                }

                float rotationThreshold = 5.0f;
                if (abs(angle) > rotationThreshold) {
           
                    float rotationAngle = glm::sign(angle) * rotationSpeed * deltaTime;
                    rotateTank(rotationAngle);
                }

                moveTank(direction.x * tankSpeed * deltaTime, 0, direction.z * tankSpeed * deltaTime);

                if (huntDuration <= .0f) {
                    huntDuration = 0.f;
                    setTurelaColor(ENEMY_CANNON_COLOR);
                    tankState = EnemyState::PATROL;
                }
            }

        }

        void updateState(float deltaTime) {
            switch (tankState) {
            case EnemyState::PATROL: {
                patrolStateLogic(deltaTime, waypoints);
                break;
            }
            case EnemyState::ATTACKS: {
                attackStateLogic(deltaTime);
                break;
            }
            default: break;
            }
        }

        void setNextState(EnemyState state) {
            tankState = state;
        }


        void shootProjectile(std::vector<std::unique_ptr<Projectile3D>>& projectiles) {
            // should look at the tank
            glm::vec3 cannonDirection = getCannonForwardPosition();
            // player forward
            glm::vec3 playerDirection = player.getCannonForwardPosition();

            float cos = glm::dot(cannonDirection, playerDirection);
            if (glm::degrees(cos) > .1f) {
                return;
            }

            if (reloadTimer <= 0) {
                isReloading = false;
            }

            if (canShoot() && !isReloading) {
                reloadTimer = maxReloadTimer;
                isReloading = true;

                // Get the position and direction of the cannon end
                glm::vec3 cannonEndPosition = getCannonEndPosition();

                // Create a new Projectile3D instance
                glm::mat4 projectileModelMat = glm::mat4(1.0f); // You may need to set an initial model matrix
                glm::vec3 projectileColor = glm::vec3(1.0f, 0.0f, 0.0f); // Set the color as desired

                // Position the projectile at the cannon end
                glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), cannonEndPosition);

                // Add the projectile to the list
                projectiles.emplace_back(std::move(
                    std::make_unique<Projectile3D>(translationMatrix, glm::vec3(0), cannonDirection, renderer)
                ));
            }
        }

        void setWaypoints(std::vector<WayPoint>& wayPoints) {
            waypoints = wayPoints;
        }

        EnemyState getState() const {
            return tankState;
        }
    };
}