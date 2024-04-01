#pragma once

#include "components/simple_scene.h"
#include <memory>
#include "lab_m1/Tema1/transform2D.h"
#include <random>
#include <iostream>
#include <array>
#include <unordered_map>

namespace HomeWork {

    enum class EntityType {
        Green,
        Purple,
        Orange
    };

    //class CircleCollider {
    //    glm::vec3 props;
    //public:
    //    CircleCollider() = default;
    //    CircleCollider(glm::vec3 props) : props(props) {}
    //    CircleCollider& operator=(const CircleCollider& other) {
    //        if (this != &other) {
    //            props = other.props;
    //            std::cout << "Updated Collider Props: " << props.x << ' ' << props.y << ' ' << props.z << '\n';
    //        }
    //        return *this;
    //    }

    //    glm::vec2 getCenter() const { return { props.x, props.y}; }
    //    float getRadius() const { return props.z; }

    //    void setCenter(glm::vec3 newProps) { props = newProps; }

    //    void updateCollider(glm::mat3 modelM) {
    //        glm::vec3 scaledCenter = modelM * glm::vec3(props.x, props.y, .5f);
    //        props.x = scaledCenter.x / 100; // Compensating for the scale on x
    //        props.y = scaledCenter.y / 100; // Compensating for the scale on y
    //        std::cout << "Updated Collider Position: " << props.x << ' ' << props.y << '\n';
    //    }
    //};

    class Entity {
        glm::mat3 modelM;
        std::string name;
        //CircleCollider collider;
    public:
        bool readyToDestroy = false;
        glm::vec2 coords; 

        Entity(glm::mat3 modelM, const std::string& name) : modelM(modelM), name(name) {
            coords = Entity::getWorldPos();
        }

        void Rotate(float angle) {
            this->modelM *= transform2DT::Rotate(angle);
        }

        void Translate(float tx, float ty) {
            this->modelM *= transform2DT::Translate(tx, ty);
        }

        virtual bool isColliding(glm::vec2 cursorPos, float objWidth = 10, float objHeight = 10, float margin = 0) noexcept {
            glm::vec2 entityPos = this->getWorldPos();

            // Expand hitbox(with margin)
            float leftBoundary = entityPos.x - margin;
            float rightBoundary = (entityPos.x + objWidth) + margin;
            float bottomBoundary = entityPos.y - margin;
            float topBoundary = (entityPos.y + objHeight) + margin;

            if (cursorPos.x >= leftBoundary && cursorPos.x <= rightBoundary &&
                cursorPos.y >= bottomBoundary && cursorPos.y <= topBoundary) {
                readyToDestroy = true;
                return true;
            }

            return false;
        }


        // Getters
        glm::mat3 getModelMatrix() const noexcept {
            return this->modelM;
        }

        glm::vec2 getWorldPos() const noexcept {
            return { this->modelM[2][0], this->modelM[2][1] };
        }

        std::string getName() const noexcept {
            return this->name;
        }

           // Setter
        void setModelMatrix(glm::mat3 modelMat) noexcept {
            modelM = modelMat;
        }

        /*void setCollider(const CircleCollider& other) {
            collider = other;
        }

        void updateCollider() {
            collider.updateCollider(modelM);
        }*/
    };

    class Square : public Entity {
        glm::mat3 modelM;
    public:
        bool isEmpty = true;
        Square(glm::mat3 modelM, const std::string& name) : Entity(modelM, name) {}

        Square operator*=(glm::mat3 other) {
            this->modelM *= other;
            return *this;
        }
    };

    class Projectile : public Entity {
        glm::mat3 modelM;
        EntityType type;
    public:
        float projectileSpeed = 6;
        int damage = 25;

        Projectile(EntityType type, glm::mat3 modelM, const std::string& name) : type(type), Entity(modelM, name) {}

        void moveProjectileToTheLeft(float deltaTime, float timeScale) {
            glm::mat3 modelMatrix = this->getModelMatrix();
            glm::mat3 translationMat = transform2DT::Translate(6 * deltaTime * timeScale, 0);
            this->setModelMatrix(modelMatrix * translationMat);
        }

        EntityType getType() const noexcept {
            return type;
        }
    };

    class Zombie : public Entity {
        glm::mat3 modelM;
        EntityType type;
    public:
        Zombie(EntityType type, glm::mat3 modelM, const std::string& name) : type(type), Entity(modelM, name) {}

        // Getters
        EntityType getType() const noexcept {
            return this->type;
        }

        std::unique_ptr<Zombie> clone() const {
            return std::make_unique<Zombie>(*this);
        }
    };

    // Zombie Game Object
    class ZombieGO {
        std::unique_ptr<Zombie> zombieProto;

        // Zombie Stats
        // Shrinking Animation
        float shrinkScale = 1;
        float shrinkAmount = .1f;
    public:
        int health = 50;
        float zombieSpeed = 1.5f;
        bool readyToDestroy = false;
        glm::vec2 coords;

        ZombieGO(std::unique_ptr<Zombie> proto) : zombieProto(std::move(proto)) {
            coords = zombieProto->getWorldPos();
        }

        Zombie* getZombie() const {
            return zombieProto.get();
        }

        void playDeathAnimation(float deltaTime) {
            glm::mat3 modelM = zombieProto->getModelMatrix();
            if (shrinkScale > .9f)
                shrinkScale -= shrinkAmount * deltaTime;
            else {
                readyToDestroy = true;
                return;
            }

            modelM *= transform2DT::Rotate(.05f);
            modelM *= transform2DT::Scale(shrinkScale, shrinkScale);

            zombieProto->setModelMatrix(modelM);
        }

        void moveZombieToTheLeft(float deltaTime, float timeScale) {
            glm::mat3 modelM = zombieProto->getModelMatrix();
            glm::mat3 translationMat = transform2DT::Translate(-zombieSpeed * deltaTime * timeScale, 0);
            modelM *= translationMat;

            zombieProto->setModelMatrix(modelM);
        }

        bool shouldDie() {
            return zombieProto->getWorldPos().x < 20 || health <= 0;
        }

        // Getters
        int getHealth() const noexcept {
            return this->health;
        }
    };

    class Plant : public Entity {
        glm::mat3 modelM;
        EntityType type;
    public:
        uint16_t starPrice;
        Plant(EntityType type, glm::mat3 modelM, const std::string& name) : type(type), Entity(modelM, name) {
            switch (type) {
            case EntityType::Green: { this->starPrice = 1; break; }
            case EntityType::Purple: { this->starPrice = 2; break; }
            case EntityType::Orange: { this->starPrice = 3; break; }
            default: break;
            }
        }

        // Getters
        EntityType getType() const noexcept {
            return this->type;
        }
    };

    class PlantGO {
        std::unique_ptr<Plant> plant;
    public:
        bool canAttack = false;
        uint16_t posInGrid;
        glm::vec2 coords;
        float maxReloadTimer = 2.5f;
        float reloadTimer = 1; // Doar prima data va fi o secunda

        PlantGO(std::unique_ptr<Plant>& plant, uint16_t pos) : plant(std::move(plant)), posInGrid(pos) {
            coords = this->plant->getWorldPos();
        }

        Plant* getPlant() const {
            return plant.get();
        }
    };

    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

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
        void RenderScene(float deltaTimeSeconds);

        template<typename T>
        static std::unique_ptr<T> createObject(float x, float y, const std::string& name, float sx = 80, float sy = 80);

        template<typename T, typename Type>
        static std::unique_ptr<T> createObject(Type type, float x, float y, const std::string& name, float sx = 80, float sy = 80);

        float generateRandomNumber(int lowerBound, int higherBound);
        void spawnZombie();
        void spawnStars();
        void plantAttack(float deltaTime);

    protected:
        glm::mat3 modelMatrix;
        std::array<std::unique_ptr<Square>, 9> grid;
        bool squareSelected; // Something is Selected - 1, Nothing Selected - 0
        std::vector<std::unique_ptr<Entity>> options; // plants
        std::vector<std::unique_ptr<Zombie>> zombies; // prototypes
        std::vector<std::unique_ptr<ZombieGO>> aliveSpawnedZombies;
        std::unique_ptr<Square> deadZone;
        int heartCount = 3;
        float randomTime;
        float starSpawnTime;
        int randomLine;

        // Mouse Buttons
        bool leftMouseButtonPressed = false;
        bool draggingPlant = false;
        std::unique_ptr<Plant> plantClone;

        // Star Container
        std::vector<std::unique_ptr<Entity>> starsOnScreen;
        int totalStars = 4;

        // Plants on the GRID
        std::vector<std::unique_ptr<PlantGO>> plants;
        
        // Game Utils
        bool gamePaused = false;

        // Game Speed
        float timeScale = 1;

        // Plant frames
        std::vector<glm::mat3> plantFrames;

        // TYPE - COLOR Map
        std::unordered_map<EntityType, std::string> colorMap = {
            {EntityType::Green, "green"},
            {EntityType::Purple, "purple"},
            {EntityType::Orange, "orange"},

        };

        // All the projectiles
        std::vector<std::unique_ptr<Projectile>> projectiles;
    };

}
