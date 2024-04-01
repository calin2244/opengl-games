#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object2D::CreateSquare(const std::string &name, glm::vec3 leftBottomCorner, float length, glm::vec3 bodyColor, bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, bodyColor),
        VertexFormat(corner + glm::vec3(length, 0, 0), bodyColor),
        VertexFormat(corner + glm::vec3(length, length, 0), bodyColor),
        VertexFormat(corner + glm::vec3(0, length, 0), bodyColor)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreatePeaShooter(const std::string& name, glm::vec3 bodyColor, glm::vec3 tailColor, bool fill)
{
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(0, 1, 0), bodyColor),
        VertexFormat(glm::vec3(-.19f, .91f, 0), bodyColor),
        VertexFormat(glm::vec3(-.19f, .72f, 0), bodyColor),
        VertexFormat(glm::vec3(0, .6f, 0), bodyColor),
        VertexFormat(glm::vec3(0, .29f, 0), bodyColor),
        VertexFormat(glm::vec3(-.13f, .03f, 0), bodyColor),
        VertexFormat(glm::vec3(.27f, .03f, 0), bodyColor),
        VertexFormat(glm::vec3(.16f, .29f, 0), bodyColor),
        VertexFormat(glm::vec3(.16f, .59f, 0), bodyColor),
        VertexFormat(glm::vec3(.53f, .58f, 0), bodyColor),
        VertexFormat(glm::vec3(.44f, .79f, 0), bodyColor),
        VertexFormat(glm::vec3(.27f, .86f, 0), bodyColor),
        VertexFormat(glm::vec3(.2f, 1, 0), bodyColor),
        // TAIL
        VertexFormat(glm::vec3(-.19f, .83f, 0), tailColor),
        VertexFormat(glm::vec3(-.27f, .89f, 0), tailColor),
        VertexFormat(glm::vec3(-.31f, .83f, 0), tailColor),
        VertexFormat(glm::vec3(-.27f, .79f, 0), tailColor),
        // EYE
        VertexFormat(glm::vec3(.11f, .88f, 1), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(.03f, .79f, 1), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(.13f, .76f, 1), glm::vec3(0, 0, 0)),
        VertexFormat(glm::vec3(.24f, .77f, 1), glm::vec3(0, 0, 0)),

    };

    Mesh* peaShooter = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3, 4, 5, 6, 7, 8 ,9, 10, 11, /* TAIL */ 12, 13, 14, 15, 16, /* EYE */ 17, 18, 19, 20};
    if (!fill) {
        peaShooter->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices = { 
            2, 1, 0, 
            0, 2, 12,
            2, 11, 12,
            2, 10, 11,
            2, 9, 10,
            2, 8, 9,
            2, 3, 8,
            3, 7, 8,
            3, 4, 7,
            4, 5, 7,
            5, 6, 7,

            // TAIL
            13, 14, 15,
            13, 16, 15,

            // EYEBROW
            17, 18, 19,
            17, 19, 20
        };
    }

    peaShooter->InitFromData(vertices, indices);

    return peaShooter;

}

Mesh* object2D::createZombie(const std::string& name, glm::vec3 faceColor, glm::vec3 bodyColor, glm::vec3 tieColor, bool fill) {
    std::vector<VertexFormat> vertices = {
        // HEAD
        VertexFormat(glm::vec3(.06f, .79f, 0), faceColor),
        VertexFormat(glm::vec3(.29f, .82f, 0), faceColor),
        VertexFormat(glm::vec3(.51f, .69f, 0), faceColor),
        VertexFormat(glm::vec3(.55f, .47f, 0), faceColor),
        VertexFormat(glm::vec3(.38f, .35f,  0), faceColor),
        VertexFormat(glm::vec3(.1f, .35f, 0), faceColor),
        VertexFormat(glm::vec3(-0.06f, .45f, 0), faceColor),

        // BODY
        VertexFormat(glm::vec3(.36f, .35f, 0), bodyColor),
        VertexFormat(glm::vec3(.57f, 0, 0), bodyColor),
        VertexFormat(glm::vec3(.08f, 0, 0), bodyColor),
        VertexFormat(glm::vec3(.19f, .35f, 0), bodyColor),

        // TIE
        VertexFormat(glm::vec3(.29f, .35f, 1), tieColor),
        VertexFormat(glm::vec3(.37f, .18f, 1), tieColor),
        VertexFormat(glm::vec3(.31f, .08f, 1), tieColor),
        VertexFormat(glm::vec3(.22f, .17f, 1), tieColor),
    };
        
    Mesh* zombie = new Mesh(name);
    std::vector<unsigned int> indices{};

    if (!fill) {
        indices = {
            0, 1, 2, 3, 4, 5, 6, 0, 6, 5, 4, 8, 9, 19, 11, 14, 13, 12, 11, 7
        };

        zombie->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices = {
            // HEAD
            2, 0, 1,
            3, 2, 0,
            4, 3, 0,
            5, 4, 0,
            6, 5, 0,

            //BODY
            8, 7, 10,
            9, 8, 10,

            // TIE
            13, 12, 11,
            14, 13, 11
        };
    }

    zombie->InitFromData(vertices, indices);
    
    return zombie;
}

/* https://stackoverflow.com/questions/22444450/drawing-circle-with-opengl */
Mesh* object2D::CreateSun(const std::string& name, uint16_t points, glm::vec3 pos, glm::vec3 color, bool fill) {
    std::vector<VertexFormat> vertices{};
    float cx = pos.x;
    float cy = pos.y;
    float r = pos.z; // Radius of the sun
    float longRayMultiplier = 2.0f; // Multiplier for long rays
    float shortRayMultiplier = 1.5f; // Multiplier for short rays
    float angleStep = (2.0f * PI) / points;

    // Centrul
    vertices.emplace_back(VertexFormat(glm::vec3(cx, cy, 0), color));

    // Razele si punctele
    for (uint16_t i = 0; i < points; ++i) {
        float theta = i * angleStep;

        // Inner point on sun circle
        float inX = r * cosf(theta) + cx;
        float inY = r * sinf(theta) + cy;

        vertices.emplace_back(VertexFormat(glm::vec3(inX, inY, 0), color)); // Inner point

        // Outer point for the ray
        float outerRadius = (i % 2 == 0) ? r * longRayMultiplier : r * shortRayMultiplier; // Alternate long and short rays
        float outX = outerRadius * cosf(theta) + cx;
        float outY = outerRadius * sinf(theta) + cy;

        vertices.emplace_back(VertexFormat(glm::vec3(outX, outY, 0), color)); // Outer point
    }


    Mesh* sun = new Mesh(name);
    std::vector<unsigned int> indices{};

    // Create the triangle fan for the sun's body
    for (uint16_t i = 1; i < points * 2; i += 2) {
        indices.push_back(0);                      // Center of the sun
        indices.push_back(i);                      // Inner point on sun circle
        indices.push_back((i + 2) % (points * 2)); // Next inner point on sun circle
    }

    // Create the rays
    for (uint16_t i = 1; i < points * 2; i += 2) {
        indices.push_back(i);                          // Current inner point
        indices.push_back(i + 1);                      // Current outer point
        indices.push_back((i + 2) % (points * 2));     // Next inner point
    }

    if (!fill) {
        sun->SetDrawMode(GL_LINE_LOOP);
    }

    sun->InitFromData(vertices, indices);

    return sun;
}

Mesh* object2D::CreateStar(const std::string& name, glm::vec3 color, bool fill) {
    std::vector<VertexFormat> vertices{};

    for (int i = 0; i < 5; ++i) {
        vertices.emplace_back(VertexFormat(
            glm::vec3(-sin(2 * i * PI / 5), cos(2 * i * PI / 5), 0), color
        ));
    }

    vertices.emplace_back(VertexFormat(glm::vec3(0, -.5f, 0), color));

    Mesh* star = new Mesh(name);
    std::vector<unsigned int> indices{};

    if (!fill) {
        indices = { 3, 0, 2, 4, 1, 5, 3 };
        star->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices = {
            3, 5, 0,
            2, 0, 5,
            5, 1, 4
        };
    }

    star->InitFromData(vertices, indices);

    return star;
}

Mesh* object2D::CreateHeart(const std::string& name, glm::vec3 color, bool fill) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(-.4f, .6f, 0), color),
        VertexFormat(glm::vec3(0, .46f, 0), color),
        VertexFormat(glm::vec3(.4f, .6f, 0), color),
        VertexFormat(glm::vec3(0, 0, 0), color),
    };

    Mesh* heart = new Mesh(name);
    std::vector<unsigned int> indices{};

    if (!fill) {
        indices = { 0, 1, 2, 3 };
        heart->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices = {
            3, 1, 0,
            3, 2, 1
        };
    }

    heart->InitFromData(vertices, indices);

    return heart;
}

// This will be a GL_LINE_LOOP ALWAYS
Mesh* object2D::CreateCircleCollider(const std::string& name, uint16_t points, glm::vec3 pos, glm::vec3 color){

    std::vector<VertexFormat> vertices{};
    float cx = pos.x;
    float cy = pos.y;
    float r = pos.z; // Radius of the sun
    float angleStep = (2.0f * PI) / points;

    // Razele si punctele
    for (uint16_t i = 0; i < points; ++i) {
        float theta = angleStep * i;

        // Inner point on sun circle
        float x = r * cosf(theta) + cx;
        float y = r * sinf(theta) + cy;

        vertices.emplace_back(VertexFormat(glm::vec3(x, y, 0), color)); // Inner point
    }


    Mesh* circleCollider = new Mesh(name);
    std::vector<unsigned int> indices{};

    for (unsigned int i = 0; i < points; ++i) {
        indices.push_back(i);
    }

    // Set Thickness
    glLineWidth(3);
    circleCollider->SetDrawMode(GL_LINE_LOOP);
    circleCollider->InitFromData(vertices, indices);

    return circleCollider;
}
