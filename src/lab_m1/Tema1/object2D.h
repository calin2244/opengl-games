#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2D
{
    #define PI 3.1415926f

    // Create square with given bottom left corner, length and color
    Mesh* CreateSquare(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreatePeaShooter(const std::string& name, glm::vec3 bodyColor, glm::vec3 tailColor, bool fill = false);
    Mesh* createZombie(const std::string& name, glm::vec3 faceColor, glm::vec3 bodyColor, glm::vec3 tieColor, bool fill = false);

    // pos - x = cx, y = cy, z = radius
    Mesh* CreateSun(const std::string& name, uint16_t points, glm::vec3 pos, glm::vec3 color, bool fill = false);
    Mesh* CreateStar(const std::string& name, glm::vec3 color, bool fill = false);
    Mesh* CreateHeart(const std::string& name, glm::vec3 color, bool fill = false);
    Mesh* CreateCircleCollider(const std::string& name, uint16_t points, glm::vec3 pos, glm::vec3 color);
}