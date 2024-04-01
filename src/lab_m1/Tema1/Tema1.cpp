#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/Tema1/object2D.h"
#include "CustomColors.h"

using namespace std;
using namespace HomeWork;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}

template<typename T>
static std::unique_ptr<T> Tema1::createObject(float x, float y, const std::string& name, float sx, float sy) {
    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2DT::Translate(x, y);
    modelMatrix *= transform2DT::Scale(sx, sy);
    return std::make_unique<T>(modelMatrix, name);
}

template<typename T, typename Type>
static std::unique_ptr<T> Tema1::createObject(Type type, float x, float y, const std::string& name, float sx, float sy) {
    glm::mat3 modelMatrix = glm::mat3(1);
    modelMatrix *= transform2DT::Translate(x, y);
    modelMatrix *= transform2DT::Scale(sx, sy);
    return std::make_unique<T>(type, modelMatrix, name);
}

float Tema1::generateRandomNumber(int lowerBound, int higherBound) {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(lowerBound, higherBound);
    return static_cast<float>(dist(rd));
}

void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    float squareSide = 100;

    squareSelected = false;

    randomTime = generateRandomNumber(1, 3);
    starSpawnTime = generateRandomNumber(1, 5);

    // line 0 - 50, line 1 - 200, line 2 - 350

    Mesh* square = object2D::CreateSquare("square", glm::vec3(0, 0, -1), 100, TERRAIN_COLOR, true);
    AddMeshToList(square);

    Mesh* selBox = object2D::CreateSquare("selBox", glm::vec3(0, 0, -1), 100, LIGHT_RED, true);
    AddMeshToList(selBox);

    Mesh* frame = object2D::CreateSquare("frame", glm::vec3(0, 0, -1), 50, GRAY, false);
    AddMeshToList(frame);

    Mesh* nonEmpty = object2D::CreateSquare("nonEmpty", glm::vec3(0, 0, -1), 100, DARK_GREEN, true);
    AddMeshToList(nonEmpty);

    Mesh* greenPeaShooter = object2D::CreatePeaShooter("greenPea", LIGHT_GREEN, glm::vec3(0, .5f, 1), true);
    AddMeshToList(greenPeaShooter);

    Mesh* purplePeaShooter = object2D::CreatePeaShooter("purplePea", LIGHT_PURPLE, glm::vec3(.9f, 1, .3f), true);
    AddMeshToList(purplePeaShooter);

    Mesh* orangePeaShooter = object2D::CreatePeaShooter("orangePea", ORANGE, glm::vec3(.2f, .5f, .1f), true);
    AddMeshToList(orangePeaShooter);

    Mesh* greenZombie = object2D::createZombie("greenZombie", GRAY, DARK_BROWN, LIGHT_GREEN, true);
    AddMeshToList(greenZombie);

    Mesh* purpleZombie = object2D::createZombie("purpleZombie", GRAY, DARK_BROWN, LIGHT_PURPLE, true);
    AddMeshToList(purpleZombie);

    Mesh* orangeZombie = object2D::createZombie("orangeZombie", GRAY, DARK_BROWN, ORANGE, true);
    AddMeshToList(orangeZombie);

    Mesh* greenProjectile = object2D::CreateSun("greenProjectile", 100, glm::vec3(0, 0, .25f), LIGHT_GREEN, true);
    AddMeshToList(greenProjectile);

    Mesh* orangeProjectile = object2D::CreateSun("orangeProjectile", 100, glm::vec3(0, 0, .25f), ORANGE, true);
    AddMeshToList(orangeProjectile);

    Mesh* purpleProjectile = object2D::CreateSun("purpleProjectile", 100, glm::vec3(0, 0, .25f), LIGHT_PURPLE, true);
    AddMeshToList(purpleProjectile);

    Mesh* star = object2D::CreateStar("star", GRAY, true);
    AddMeshToList(star);

    Mesh* purpleStar = object2D::CreateStar("purpleStar", LIGHT_PURPLE, true);
    AddMeshToList(purpleStar);

    Mesh* heart = object2D::CreateHeart("heart", LIGHT_RED, true);
    AddMeshToList(heart);

    // Init the options*(plants) and each plant's frame
    options.emplace_back(createObject<Plant>(EntityType::Green, 100, 600, "greenPea"));
    options.emplace_back(createObject<Plant>(EntityType::Purple, 300, 600, "purplePea"));
    options.emplace_back(createObject<Plant>(EntityType::Orange, 500, 600, "orangePea"));

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2DT::Translate(50, 530);
    modelMatrix *= transform2DT::Scale(3, 3.5f);
    plantFrames.emplace_back(modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2DT::Translate(250, 530);
    modelMatrix *= transform2DT::Scale(3, 3.5f);
    plantFrames.emplace_back(modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2DT::Translate(460, 530);
    modelMatrix *= transform2DT::Scale(3, 3.5f);
    plantFrames.emplace_back(modelMatrix);

    // Init the zombies
    zombies.emplace_back(createObject<Zombie>(EntityType::Green, 700, 50, "greenZombie"));
    zombies.emplace_back(createObject<Zombie>(EntityType::Purple, 700, 200, "purpleZombie"));
    zombies.emplace_back(createObject<Zombie>(EntityType::Orange, 700, 350, "orangeZombie"));

    // Init the 3 x 3 grid
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            grid[i + j * 3] = (createObject<Square>(
                100 + j * 150, 350 - i * 150, "square", 1, 1
            ));
        }
    }

    // Init the dead zone
    deadZone = createObject<Square>(0, 450, "selBox", .6f, -4);
}

inline void Tema1::spawnZombie() {
    if (randomTime < 0) {
        randomLine = generateRandomNumber(1, 3);
        randomTime = generateRandomNumber(2, 4);
        int randomIdx = generateRandomNumber(0, 2);

        int xCoord = 1280;
        int yCoord = 60; // randomLine == 1

        switch (randomLine) {
        case 2: { yCoord = 210; break; }
        case 3: { yCoord = 360; break; }
        default: break;
        }

        modelMatrix = transform2DT::Scale(110, 110);
        modelMatrix *= transform2DT::Translate(xCoord, yCoord);

        //std::unique_ptr<Zombie> zombie = zombies[randomIdx].get()->clone();
        std::unique_ptr<ZombieGO> zombieGO = std::make_unique<ZombieGO>(
            createObject<Zombie>(
                zombies[randomIdx]->getType(),
                xCoord, yCoord, zombies[randomIdx]->getName()
            )
        );

        aliveSpawnedZombies.emplace_back(std::move(zombieGO));
        // RenderMesh2D(meshes[zombies[randomIdx].get()->getName()], shaders["VertexColor"], zombie.get()->getModelMatrix());
    }
}

void Tema1::spawnStars() {
    if (starSpawnTime < 0) {
        starSpawnTime = generateRandomNumber(2, 4);

        int xCoord = generateRandomNumber(10, 1200);
        int yCoord = generateRandomNumber(20, 400);

        starsOnScreen.emplace_back(createObject<Entity>(xCoord, yCoord, "purpleStar", 40, 40));
    }
}

void Tema1::plantAttack(float deltaTime) {
    for (const auto& plant : plants) {

        plant->reloadTimer -= deltaTime;

        if (plant->canAttack && plant->reloadTimer < 0) {
            auto& pos = plant->getPlant()->getWorldPos();
            auto projectile = createObject<Projectile>(plant->getPlant()->getType(), pos.x + 50, pos.y + 30, colorMap[plant->getPlant()->getType()], 50, 50);
            projectiles.emplace_back(std::move(projectile));
            plant->reloadTimer = plant->maxReloadTimer;
        }
        
    }

    for (const auto& projectile: projectiles) {

        // See if a projectile hits a zombie
        for (const auto& zombie : aliveSpawnedZombies) {
            //std::cout << zombie->getZombie()->getWorldPos().x << ' ' << zombie->getZombie()->getWorldPos().y << '\n';
            if (projectile->getType() == zombie->getZombie()->getType() &&
                projectile->isColliding(zombie->getZombie()->getWorldPos(), 0, 0, 40)) {
                    zombie->health -= projectile->damage;
            }
        }

        projectile->moveProjectileToTheLeft(deltaTime, Tema1::timeScale);
        RenderMesh2D(meshes[projectile->getName() + "Projectile"], shaders["VertexColor"], projectile->getModelMatrix());
    }    

    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
        [](auto& projectile) {
            return projectile->getWorldPos().x > 1280 || projectile->readyToDestroy;
        }
    ), projectiles.end());
}


void Tema1::RenderScene(float deltaTimeSeconds) {
    // Render the 3 x 3 Grid
    for (const auto& el : grid) {
        if (!el->isEmpty) {
            RenderMesh2D(meshes["nonEmpty"], shaders["VertexColor"], el.get()->getModelMatrix());
        }
        else RenderMesh2D(meshes[el.get()->getName()], shaders["VertexColor"], el.get()->getModelMatrix());
    }

    // Render the Dead-Zone
    RenderMesh2D(meshes[deadZone.get()->getName()], shaders["VertexColor"], deadZone.get()->getModelMatrix());

    // Render the plant options and the frames
    for (const auto& el : options) {
        RenderMesh2D(meshes[el.get()->getName()], shaders["VertexColor"], el.get()->getModelMatrix());
        for (uint16_t i = 0; i < static_cast<Plant*>(el.get())->starPrice; ++i) {
            glm::vec2 plantPos = el.get()->getWorldPos();
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2DT::Translate(plantPos.x + i * 40, plantPos.y - 30);
            modelMatrix *= transform2DT::Scale(20, 20);
            RenderMesh2D(meshes["star"], shaders["VertexColor"], modelMatrix);
        }
    }

    for (const auto& frame : plantFrames) {
        RenderMesh2D(meshes["frame"], shaders["VertexColor"], frame);
    }

    // Test Zombie
    //modelMatrix = glm::mat3(1);
    //modelMatrix *= transform2DT::Translate(800, 400);
    //modelMatrix *= transform2DT::Scale(100, 100);
    //Zombie zomb = Zombie(EntityType::Purple, modelMatrix, "purpleZombie");
    //// Render the zombie
    //RenderMesh2D(meshes["purpleZombie"], shaders["VertexColor"], zomb.getModelMatrix());

    // Render the Hearts
    for (int i = 0; i < heartCount; ++i) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2DT::Translate(1000 + i * 70, 630);
        modelMatrix *= transform2DT::Scale(70, 70);
        RenderMesh2D(meshes["heart"], shaders["VertexColor"], modelMatrix);
    }

    // Reset canAttack for all plants at the beggining of each new frame
    for (auto& plant : plants) {
        plant->canAttack = false;
    }

    // Update and render alive zombies
    for (auto& zombie : aliveSpawnedZombies) {
        auto pos = zombie->getZombie()->getWorldPos();
        
        if (pos.x > 20) {
            zombie.get()->moveZombieToTheLeft(deltaTimeSeconds, timeScale);

            // Check if there is a plant on the same line and of the same type as the zombie
            for (auto& plant : plants) {
                if (plant->coords.y == pos.y && 
                    plant->coords.x <= pos.x &&
                    plant->getPlant()->getType() == zombie->getZombie()->getType()) {
                        plant->canAttack = true;
                    // No break here because we want all matching plants to be able to attack
                }
            }
        }
        
        if (zombie->shouldDie()) {
            zombie->playDeathAnimation(deltaTimeSeconds);
        }

        RenderMesh2D(meshes[zombie->getZombie()->getName()], shaders["VertexColor"], zombie->getZombie()->getModelMatrix());
    }

    // Remove zombies from alive zombies vector
    aliveSpawnedZombies.erase(std::remove_if(aliveSpawnedZombies.begin(), aliveSpawnedZombies.end(),
        [this](const std::unique_ptr<ZombieGO>& zombie) {
            if (zombie->readyToDestroy && zombie->getZombie()->getWorldPos().x < 20) {
                heartCount--;
            }

            return zombie->readyToDestroy;
        }),
        aliveSpawnedZombies.end());

    // Render the randomly-spawning stars
    for (const auto& star : starsOnScreen) {
        RenderMesh2D(meshes["purpleStar"], shaders["VertexColor"], star.get()->getModelMatrix());
    }

    // Remove star if it was clicked
    starsOnScreen.erase(std::remove_if(starsOnScreen.begin(), starsOnScreen.end(),
        [this](const std::unique_ptr<Entity>& star) {
            if (star->readyToDestroy)
                totalStars++;

            return star->readyToDestroy;
        }),
        starsOnScreen.end());

    // Render available stars
    int yOffset = 0;
    for (int i = 0; i < totalStars; ++i) {
        if (i % 7 == 0) {
            yOffset -= 30;
        }

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2DT::Translate(1000 + (i % 7) * 30, 600 + yOffset);
        modelMatrix *= transform2DT::Scale(15, 15);
        RenderMesh2D(meshes["star"], shaders["VertexColor"], modelMatrix);
    }

    for (const auto& p : plants) {
        RenderMesh2D(meshes[p->getPlant()->getName()], shaders["VertexColor"], p->getPlant()->getModelMatrix());
    }

    /*   modelMatrix = glm::mat3(1);
       modelMatrix *= transform2DT::Translate(800, 200);
       modelMatrix *= transform2DT::Scale(50, 50);

       RenderMesh2D(meshes["sun"], shaders["VertexColor"], modelMatrix);*/
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(.24f, .32f, .26f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema1::Update(float deltaTimeSeconds)
{
    RenderScene(deltaTimeSeconds);

    randomTime -= deltaTimeSeconds * Tema1::timeScale;
    starSpawnTime -= deltaTimeSeconds * Tema1::timeScale;

    spawnZombie();
    spawnStars();

    if (draggingPlant && plantClone) {
        RenderMesh2D(meshes[plantClone.get()->getName()], shaders["VertexColor"], plantClone.get()->getModelMatrix());
    }
    else {
        if (plantClone) {
            glm::vec2 cursorPos = window->GetCursorPosition();

            // INVERTED Y-AXIS
            cursorPos.y = window->GetResolution().y - cursorPos.y;
            
            // Placing plant on Grid :3
            for (size_t i = 0; i < grid.size(); ++i) {
                auto& square = grid[i];
                if (square->isColliding(cursorPos, 110, 110)) {
                    totalStars -= plantClone->starPrice;
                    glm::vec2 squarePos = square->getWorldPos();
                    squarePos.y += 10;
                    squarePos.x += 40;
                    std::unique_ptr<PlantGO> plant = std::make_unique<PlantGO>(
                        createObject<Plant>(plantClone->getType(), squarePos.x, squarePos.y, plantClone->getName(), 
                            70, 70),
                        i
                    );

                    square->isEmpty = false;
                    plants.emplace_back(std::move(plant));
                }
            }

            plantClone.release();
        }
    }

    plantAttack(deltaTimeSeconds);
}



void Tema1::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    if (window->MouseHold(0)) {
        leftMouseButtonPressed = true;
    }
    else {
        leftMouseButtonPressed = false;
    }

    if (window->MouseHold(0) && draggingPlant) {
        glm::vec2 cursorPos = window->GetCursorPosition();
        cursorPos.y = window->GetResolution().y - cursorPos.y;

        // Update the model matrix for the plantClone
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2DT::Translate(cursorPos.x, cursorPos.y);
        modelMatrix *= transform2DT::Scale(80, 80); // Assuming you want to scale the clone

        plantClone.get()->setModelMatrix(modelMatrix);
    }
    else {
        draggingPlant = false;
    }


}


void Tema1::OnKeyPress(int key, int mods) {
    
    switch (key) {
    //case GLFW_KEY_A: {
    //    glm::vec2 cursorPos = window->GetCursorPosition();

    //    //INVERTED Y-AXIS
    //    cursorPos.y = window->GetResolution().y - cursorPos.y;

    //    bool squareSelected = false;
    //    for (const auto& el : grid) {
    //        glm::vec2 squarePos = el.get()->getWorldPos();
    //        float squareSize = 100; // The length of each square

    //        if (cursorPos.x >= squarePos.x && cursorPos.x < squarePos.x + squareSize &&
    //            cursorPos.y >= squarePos.y && cursorPos.y < squarePos.y + squareSize) {

    //            el.get()->selected = true;
    //            squareSelected = true;
    //        }
    //        else {
    //            el.get()->selected = false;
    //        }
    //    }

    //    break;
    //}
    case GLFW_KEY_P: { 
        timeScale = (timeScale == 1) ? 0 : 1;
    }
    default: break;
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{

}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{   
    if (draggingPlant) {
        glm::vec2 cursorPos(mouseX, window->GetResolution().y - mouseY);

        glm::mat3 modelMatrix = glm::mat3(1);
        modelMatrix = transform2DT::Scale(80, 80);
        modelMatrix *= transform2DT::Translate(deltaX, -deltaY);

        plantClone.get()->setModelMatrix(modelMatrix);
    }
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
    glm::vec2 cursorPos = window->GetCursorPosition();
    cursorPos.y = window->GetResolution().y - cursorPos.y;

    if (button == 1) {
        // Check if we are over any plant
        for (const auto& plant : options) {
            float plantWidth = 100; // Assuming the plant's width
            float plantHeight = 100;

            // Check if the cursor is over the plant
            if (plant->isColliding(cursorPos, 20, 50, 100) && totalStars >= static_cast<Plant*>(plant.get())->starPrice) {

                draggingPlant = true;
                EntityType selectedPlantType = static_cast<Plant*>(plant.get())->getType();
                std::string plantName = static_cast<Plant*>(plant.get())->getName();

                // Create the clone at the cursor position
                plantClone = createObject<Plant>(selectedPlantType, cursorPos.x, cursorPos.y, plantName, 0.5f, 0.5f);
                break;
            }
        }

        // Check if the Star is Clicked
        for (const auto& star : starsOnScreen) {
            float starWidth = 100;
            float starHeight = 100;
            star->isColliding(cursorPos, 0, 0, 50);
        }
    }
    else if (button == 2) {
        for (size_t i = 0; i < grid.size(); ++i) {
            auto& square = grid[i];
            if (square->isColliding(cursorPos, 110, 110) && !square->isEmpty) {
                plants.erase(std::remove_if(plants.begin(), plants.end(),
                    [i](const std::unique_ptr<PlantGO>& plant) {
                        return plant->posInGrid == i;
                    }), plants.end());
                    
                square->isEmpty = true;
            }
        }
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    if (button == 0) {
        std::cout << "RELEASED BUTTON 0\n";
    }
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema1::OnWindowResize(int width, int height)
{
}
