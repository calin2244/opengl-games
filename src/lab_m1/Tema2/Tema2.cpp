#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace HomeWork;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

 // A map with the Object Type and its associated Shader
namespace HomeWork {
    std::unordered_map<ObjectType, std::string> shaderName = {
        {ObjectType::NOT_AN_OBJ, ""}, {ObjectType::Tank, "tankShader"},
        {ObjectType::Terrain, "terrainShader"}, {ObjectType::Projectile, "terrainShader"},
        {ObjectType::Building, "terrainShader"}
    };
}

Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}

void Tema2::addNewMesh(const char* meshName, const char* path, const char* modelName, WindowObject* window) {
    Mesh* mesh = new Mesh(meshName);
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, path), modelName);
    meshes[mesh->GetMeshID()] = mesh;
}

void Tema2::addNewShader(const char* shaderName, const char* path, const char* shaderFileName, WindowObject* window) {
    return;
}

void Tema2::generateRandomBuildings() {
    for (int i = 0; i < 20; ++i) {
        float posX = static_cast<float>(rand() % 100 - 50);
        float posY = 0.0f;
        float posZ = static_cast<float>(rand() % 100 - 50);

        posX *= 2.25f;
        posZ *= 3.75f;

        float scaleX = static_cast<float>(rand() % 30 + 10);
        float scaleY = static_cast<float>(rand() % 50 + 10);
        float scaleZ = static_cast<float>(rand() % 30 + 10);

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(posX, posY, posZ));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleX, scaleY, scaleZ));

        buildings[i].init(std::move(modelMatrix), BUILDING_COLOR, ObjectType::Building, renderer);
    }
}

void Tema2::generateRandomEnemies() {
    for (int i = 0; i < 8; ++i) {
        float randomX = static_cast<float>(rand() % 75 - 25);
        float randomZ = static_cast<float>(rand() % 125);
        float randomRotation = static_cast<float>(rand() % 360);

        enemies.emplace_back(std::make_unique<EnemyTank>(*player));
        enemies.back()->moveTank(randomX, 0, randomZ);
        enemies.back()->rotateTank(glm::radians(randomRotation));
        enemies.back()->setTurelaColor(ENEMY_CANNON_COLOR);
    }
}

void Tema2::generateRandomWaypoints() {
    for (int i = 0; i < enemies.size(); ++i) {
        std::vector<WayPoint> enemyWaypoints;

        float randomDirection = static_cast<float>(rand()) / RAND_MAX * glm::two_pi<float>();

        for (int j = 0; j < 3; ++j) {
            float randomDistance = static_cast<float>(rand() % 51 - 25);
            float randomZ = static_cast<float>(rand() % 51 - 25);

            glm::vec3 waypointPosition(
                randomDistance * cos(randomDirection),
                0,
                randomDistance * sin(randomDirection)
            );

            waypointPosition.z += randomZ;
            enemyWaypoints.emplace_back(waypointPosition);
        }

        enemies[i]->setWaypoints(std::move(enemyWaypoints));
    }
}


void Tema2::Init()
{
    renderCameraTarget = false;

    camera = new implemented::Camera();
    camera->Set(glm::vec3(0, 4, -5.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    // Load Meshes into Memory
    addNewMesh("plane", "primitives", "plane50.obj", window);
    addNewMesh("box", "primitives", "box.obj", window);
    addNewMesh("sphere", "primitives", "sphere.obj", window);
    addNewMesh("body", "tank/components/corp", "body.fbx", window);
    addNewMesh("tun", "tank/components/tun", "tun.fbx", window);
    addNewMesh("senile", "tank/components/senile", "senile.fbx", window);
    addNewMesh("turela", "tank/components/turela", "turela.fbx", window);
    addNewMesh("explosion", "explosion", "explosion.fbx", window);
    addNewMesh("muzzleSmoke", "smoke", "smoke.fbx", window);

    // Load Shaders into Memory
    {
        Shader* shader = new Shader("tankShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "TankVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "TankFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("terrainShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2/shaders/TerrainVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "TankFragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("textureShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::SHADERS, "MVP.Texture.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::SHADERS, "Default.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    projectionMatrix = glm::perspective(glm::radians(fov), window->props.aspectRatio, 0.01f, 200.0f);


    // Create the Renderer
    renderer = std::make_unique<Renderer3D>(meshes, shaders, projectionMatrix, camera);

    // Initialize the plane
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0, .15f, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(30));
    terrain.setModelMatrix(std::move(modelMatrix));
    terrain.color = glm::vec3(.5f, .65f, .5f);
    terrain.objectType = ObjectType::Terrain;
    terrain.setRenderer(renderer);

    // Tank Components Center of Mass
    glm::vec3 tankBodyCenterOfMass = glm::vec3(0, 0, 0);
    glm::vec3 turelaCenterOfMass = glm::vec3(0, 0, -.15f);
    glm::vec3 cannonCenterOfMass = glm::vec3(0, 0, -.9f);
    // -.9f

    // Initializing the tank body
    TankComponent tankBody{ glm::mat4(1), "body", glm::vec3(.1f, .1f, .1f), tankBodyCenterOfMass, 0 };
    TankComponent tankWheels{ glm::mat4(1), "senile", glm::vec3(1), tankBodyCenterOfMass, 1 };
    TankComponent tankTurela{ glm::mat4(1), "turela", glm::vec3(.6f, .87f, .49f), turelaCenterOfMass, 2 };
    TankComponent tankCannon{ glm::mat4(1), "tun", glm::vec3(1), cannonCenterOfMass, 3 };

    // Correct the models as they were imported incorrectly
    tankBody.rotateOx(glm::radians(-90.0f));
    tankBody.scale(glm::vec3(1, .9f, .35f));
    tankBody.translate(0, .95f, 0); // Translate so that the center of mass is at the origin after rotation and scaling

    tankWheels.rotateOx(glm::radians(90.0f));
    tankWheels.scale(glm::vec3(0.26f, 0.5f, 0.3f));
    tankWheels.translate(0, .5f, 0); // Translate relative to the body's center of mass

    tankTurela.rotateOx(glm::radians(-90.0f));
    tankTurela.scale(glm::vec3(.8f, .45f, .3f));
    tankTurela.translate(0, 1.5f, .15f); // Translate relative to the body's center of mass

    tankCannon.scale(glm::vec3(.2f, .2f, .3f));
    tankCannon.translate(0, 1.5f, .9f); // Translate relative to the body's center of mass

    // Set the Renderer
    player = std::make_unique<Tank>(
        std::move(tankBody), std::move(tankWheels),
        std::move(tankTurela), std::move(tankCannon),
        renderer
    );
    player->setRenderer(renderer);
    player->moveTank(50, 0, 10);

    // Generate the Buildings
    generateRandomBuildings();
    // Generate the enemies
    generateRandomEnemies();
    // Generate random directions
    generateRandomWaypoints();
}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(.6f, .6f, .87f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::RenderScene(float deltaTime) {
    // Render the Terrain
    terrain.render("plane");

    // Render the Player
    player->render();

    // Render the enemies
    for (const auto& enemy : enemies) {
        enemy->render();
    }

    // Render the projectiles
    for (const auto& projectile : projectiles) {
        if (!projectile->hasCollided()) {
            projectile->translateProjectile(deltaTime);
        }
        else {
            projectile->goofyDeathAnimationXD(deltaTime);
        }

        projectile->render("sphere");
    }

    // Render the active effects
    for (auto& eff : activeEffects) {
        eff->decreaseTimer(deltaTime);

        // Check the Effect Type
        switch (eff->effectType) {
        case EffectType::NOT_AN_EFFECT: {break; }
        case EffectType::DeathEffect: {
            static_cast<DeathEffect*>(eff.get())->render(DEATH_EFFECT_SCALE);
            break;
        }
        case EffectType::ShootingEffect: {
            static_cast<ShootingEffect*>(eff.get())->render(deltaTime);
            break;
        }
        default: break;
        };

    }

    // Render the buildings AND ACTIVATE WALL HACK >:)
    if (camState == CameraState::NORMAL_STATE) {
        for (const auto& building : buildings) {
            building.render("box");
        }
    }
}

void Tema2::DestroyGameObjects(float deltaTime) {
    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
        [](const auto& projectile) {
            return projectile->shouldDestroy || projectile->outOfScreen();
        }
    ), projectiles.end());

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [this, deltaTime](const auto& enemy) {
            if (enemy->shouldDestroy) {
                auto forward = enemy->getCannonForwardPosition(); // vec3
                auto bodyPosition = enemy->getPosition(); // vec3
                auto effectModelMat = glm::translate(glm::mat4(1), bodyPosition); // mat4

                std::unique_ptr<Effects> eff = effectsPool.createEffect<DeathEffect>(effectModelMat, forward, renderer);
                activeEffects.push_back(std::move(eff)); // emplace_back beahves the same
            }

            return enemy->shouldDestroy;
        }
    ), enemies.end());
}

void Tema2::Update(float deltaTimeSeconds)
{
    // Bye bye :(
    if (Engine::GetElapsedTime() >= 80 || player->tankHealth <= 0) {
        window->Close();
    }

    // Check Collision Player-Enemy
    CollisionDetection::PlayerEnemyCollision(*player, enemies);
    // Check Projectile-Enemy Collision
    CollisionDetection::ProjectileEnemyCollision(enemies, projectiles);
    // Check Projectile-Player Collision
    CollisionDetection::ProjectilePlayerCollision(*player, projectiles);
    // Check Enemy-Enemy collision
    CollisionDetection::EnemyEnemyCollision(enemies);
    // Check Player-Building collision
    if (CollisionDetection::PlayerBuildingCollision(*player, buildings)) {
        player->translateBackwards(deltaTimeSeconds);
    }

    // Projectile-Building collision
    CollisionDetection::ProjectileBuildingCollision(projectiles, buildings);

    DestroyGameObjects(deltaTimeSeconds);
    EffectsPool::recycleEffects(activeEffects, &effectsPool); // reuse efects
    player->decreaseReloadTimer(deltaTimeSeconds); // Shooting Cooldown
    RenderScene(deltaTimeSeconds);

    for (auto& enemy : enemies) {
        enemy->updateState(deltaTimeSeconds);
    }

    // make enemies shoot
    for (auto& enemy : enemies) {
        if (enemy->getState() == EnemyState::ATTACKS) {
            enemy->shootProjectile(projectiles);
            enemy->decreaseReloadTimer(deltaTimeSeconds);
        }
    }
}

void Tema2::FrameEnd()
{
    // DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}
/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
    if (camState == CameraState::FREEROAM_STATE)
    {
        float cameraSpeed = 25.0f;

        if (window->KeyHold(GLFW_KEY_W)) {
            // TODO(student): Translate the camera forward
            camera->TranslateForward(cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            // TODO(student): Translate the camera to the left
            camera->TranslateRight(-cameraSpeed * deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            // TODO(student): Translate the camera backward
            camera->TranslateForward(-cameraSpeed * deltaTime);

        }

        if (window->KeyHold(GLFW_KEY_D)) {
            // TODO(student): Translate the camera to the right
            camera->TranslateRight(cameraSpeed * deltaTime);

        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            // TODO(student): Translate the camera downward
            camera->TranslateUpward(cameraSpeed * deltaTime);

        }

        if (window->KeyHold(GLFW_KEY_E)) {
            // TODO(student): Translate the camera upward
            camera->TranslateUpward(-cameraSpeed * deltaTime);

        }
    }
    else {
        // Tank Movement
        if (window->KeyHold(GLFW_KEY_W)) {
            player->translateForward(deltaTime);
        }
        else if (window->KeyHold(GLFW_KEY_S)) {
            player->translateBackwards(deltaTime);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            player->rotateLeft(deltaTime);
            camera->RotateThirdPerson_OY(player->rotationSpeed * deltaTime);
        }
        else if (window->KeyHold(GLFW_KEY_D)) {
            player->rotateRight(deltaTime);
            camera->RotateThirdPerson_OY(-player->rotationSpeed * deltaTime);
        }

        // Changing the FOV
        if (window->KeyHold(GLFW_KEY_1)) {
            fov += 50 * deltaTime;
            projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, .01f, 100.0f);

        }

        if (window->KeyHold(GLFW_KEY_2)) {
            fov -= 50 * deltaTime;
            projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, .01f, 100.0f);
        }

        if (window->KeyHold(GLFW_KEY_R)) {
            lookBack = -1;
        }
        else {
            lookBack = 1;
        }


        glm::vec3 tankPosition = player->getPosition();
        glm::vec3 tankForward = player->getForwardDirection();
        glm::vec3 cameraPositionOffset = glm::vec3(0, 4, -5.5f);  // 4 UP, -5.5 BEHIND
        glm::vec3 cameraLookAtOffset = glm::vec3(0, 2, 0);

        glm::vec3 cameraPosition = tankPosition - (tankForward * lookBack) * cameraPositionOffset.z + glm::vec3(0, cameraPositionOffset.y, 0);
        glm::vec3 cameraTarget = tankPosition + cameraLookAtOffset;

        camera->Set(cameraPosition, cameraTarget, glm::vec3(0, 1, 0));

        // CANNON + TURELA MOUSE FOLLOW
        glm::vec2 currentMousePos = window->GetCursorPosition();
        static glm::vec2 previousMousePos = currentMousePos;
        // Pass mouse delta
        player->rotateTurelaAfterMousePosition(currentMousePos - previousMousePos);

        previousMousePos = currentMousePos;
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }
    if (key == GLFW_KEY_O)
    {
        projectionMatrix = glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.f, 100.f);
    }

    if (key == GLFW_KEY_P)
    {
        projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, .01f, 100.0f);
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT))
    {
        float sensivityOX = 0.005f;
        float sensivityOY = 0.005f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            // TODO(student): Rotate the camera in first-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateFirstPerson_OX(-deltaY * sensivityOX);
            camera->RotateFirstPerson_OY(-deltaX * sensivityOY);
        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = true;
            // TODO(student): Rotate the camera in third-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateThirdPerson_OX(-deltaY * sensivityOX);
            camera->RotateThirdPerson_OY(-deltaX * sensivityOY);
        }
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (button == 1 && camState == CameraState::NORMAL_STATE && player->canShoot()) {
        // Start reloading
        player->reloadTimer = player->maxReloadTimer;

        // Get Cannon Details
        glm::vec3 projectileDirection = player->getCannonForwardPosition();
        glm::vec3 projectileStart = player->getCannonEndPosition();

        // Instantiate muzzle smoke effect
        glm::mat4 smokePos = glm::translate(glm::mat4(1), projectileStart);
        smokePos = glm::rotate(smokePos, glm::radians(-90.f), OX_AXIS);
        std::unique_ptr<Effects> muzzleSmoke = effectsPool.createEffect<ShootingEffect>(smokePos, projectileDirection, renderer);
        activeEffects.push_back(std::move(muzzleSmoke));

        // Projectile Position
        glm::vec3 projectileSpeed = projectileDirection;

        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), projectileStart);

        projectiles.emplace_back(std::move(
            std::make_unique<Projectile3D>(translationMatrix, glm::vec3(0), projectileDirection, renderer)
        ));

        std::cout << "spawned\n";
    }

    if (button == 2) {
        camState = (camState == CameraState::NORMAL_STATE) ? CameraState::FREEROAM_STATE : CameraState::NORMAL_STATE;
    }
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}