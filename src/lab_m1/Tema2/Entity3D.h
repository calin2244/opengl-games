#pragma once
#include <memory>
#include "Renderer3D.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace HomeWork {

    class Entity3D {
    protected:
        glm::mat4 modelMat;
        std::shared_ptr<Renderer3D> renderer;
    public:
        ObjectType objectType;
        glm::vec3 color;

        // Constructors
        Entity3D() = default;

        explicit Entity3D(const glm::mat4& modelM, const glm::vec3& color)
            : modelMat(modelM), color(color), objectType(ObjectType::NOT_AN_OBJ) {}

        explicit Entity3D(const glm::mat4& modelM, const glm::vec3& color, ObjectType objType, std::shared_ptr<Renderer3D> rend)
            : modelMat(modelM), color(color), renderer(rend), objectType(objType) {}

        // Class Functions
        void rotateOx(float angle) {
            modelMat = glm::rotate(modelMat, angle, glm::vec3(1, 0, 0));
        }

        void rotateOy(float angle) {
            modelMat = glm::rotate(modelMat, angle, glm::vec3(0, 1, 0));
        }

        void rotateOz(float angle) {
            modelMat = glm::rotate(modelMat, angle, glm::vec3(0, 0, 1));
        }

        void scale(const glm::vec3& scale) {
            modelMat = glm::scale(modelMat, scale);
        }

        virtual void translate(float tx, float ty, float tz) {
            modelMat = glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, tz)) * modelMat;
        }

        virtual void render(const std::string& meshName) const {
            if (!renderer || objectType == ObjectType::NOT_AN_OBJ) {
                std::cout << "NO RENDERER AVAILABLE/DIDN'T SPECIFY OBJECT TYPE, RETURNING\n";
                return;
            }

            renderer->RenderObject<Entity3D>(meshName, shaderName[objectType], *this);
        }

        void setRenderer(std::shared_ptr<Renderer3D>& rend) {
            renderer = rend;
        }

        void setModelMatrix(glm::mat4& modelM) {
            modelMat = modelM;
        }

        void init(glm::mat4& modelM, const glm::vec3& color, ObjectType objType = ObjectType::NOT_AN_OBJ, std::shared_ptr<Renderer3D> rend = nullptr) {
            modelMat = modelM;
            this->color = color;
            objectType = objType;
            renderer = rend;
        }

        //Getters
        glm::mat4 getModelMat() const noexcept {
            return modelMat;
        }

        glm::mat4& getModelMatRef() noexcept {
            return modelMat;
        }
    };
}