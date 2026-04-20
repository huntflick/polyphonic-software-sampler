#include "Engine.hpp"
#include <cstdlib>
#include "ObjectTypes.hpp"

using namespace std::chrono_literals;

constexpr int CONST_AMOUNT = 9;

void PlayerComputerCollision(GameObject*, GameObject*);

void initialize() {
    createSystems();
}

int main() {
    initialize();

    glm::vec3 lightPositions[CONST_AMOUNT] = {glm::vec3(1, 0, 0), glm::vec3(0, 2, 0), glm::vec3(0, 0, 2),
                                                glm::vec3(2, 2, 0), glm::vec3(0, -2, -2), glm::vec3(2, 0, 2),
                                                glm::vec3(1, 1, 1), glm::vec3(-2, 0, 0), glm::vec3(0, -2, 0)};
    glm::vec3 lightColors[CONST_AMOUNT] = {glm::vec3(0, 0, 1), glm::vec3(1, 0, 1), glm::vec3(0, 1, 0),
                                            glm::vec3(1, 0, 0), glm::vec3(1, 1, 1), glm::vec3(0, 1, 1),
                                            glm::vec3(0, 0, 1), glm::vec3(1, 1, 0), glm::vec3(1, 1, 1)};

    GLuint texShader = loadShaders("Shaders/texture.vert", "Shaders/texture.frag");
    GLuint defShader = loadShaders("Shaders/deferred.vert", "Shaders/deferred.frag");

    if (defShader != -1) {
        glUseProgram(defShader);
        GLint lightPos = glGetUniformLocation(defShader, "lightPos");
        GLint lightColor = glGetUniformLocation(defShader, "lightColor");
        GLint lightAmount = glGetUniformLocation(defShader, "CONST_AMOUNT");
        glUniform3fv(lightPos, CONST_AMOUNT, glm::value_ptr(lightPositions[0]));
        glUniform3fv(lightColor, CONST_AMOUNT, glm::value_ptr(lightColors[0]));
        glUniform1i(lightAmount, CONST_AMOUNT);
    }

    playSound("SFX/At_Dooms_Gate_E1M1.wav", 1);

    std::string monk = addObject("Meshes/monk_smooth.cse", "monk", ObjectTypes::COMPUTER_OBJECT);
    // setPosition(monk, glm::vec3(0.0, 0.0, 2.0));
    std::string robot = addObject("Meshes/robot.cse", "robot", ObjectTypes::PLAYER_OBJECT);
    // setPosition(robot, glm::vec3(-1.5, 0.0, 0.0));
    loadAnimations(robot, "Animations/robot_animations.cse");
    // playAnimation(robot, "Walking", true);
    addPhysicsShape(monk, "Meshes/monk_physics.cse");
    addPhysicsShape(robot, "Meshes/robot_physics.cse");

    std::string wall = addObject("Meshes/wall.cse", "wall", ObjectTypes::STATIC_OBJECT);
    std::string ball = addObject("Meshes/cacodemon.cse", "ball", ObjectTypes::DYNAMIC_OBJECT);

    setPosition(wall, glm::vec3(2.0, 0.0, 0.0));
    setPosition(ball, glm::vec3(-2.0, 0.0, 0.0));

    setCollisionFunction(ObjectTypes::PLAYER_OBJECT, ObjectTypes::COMPUTER_OBJECT, &PlayerComputerCollision);


    bool exit = false;
    SDL_Event e;

    // int numkeys;
    glm::vec3 cameraPos = glm::vec3(0.0f, -3.0f, 3.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    setCamera(cameraPos, target);

    size_t prevTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    size_t currTime;
    float secondsTime;
    while (!exit) {
        currTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        size_t delta = currTime - prevTime;
        prevTime = currTime;
        secondsTime = static_cast<float>(delta / 1000.0f);
        setDeltaSeconds(secondsTime);
        setDeltaTime(delta);

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    exit = true;
                    break;
                // case SDL_EVENT_MOUSE_BUTTON_DOWN:
                //     createWindow("window", 300, 300);
                //     break;
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    destroyWindow(e.window.windowID);
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (e.key.scancode == SDL_SCANCODE_W) {
                        cameraPos.z += 1.0f;
                        target.z += 1.0f;
                    }
                    else if (e.key.scancode == SDL_SCANCODE_A) {
                        cameraPos.x -= 1.0f;
                        target.x -= 1.0f;
                    }
                    else if (e.key.scancode == SDL_SCANCODE_S) {
                        cameraPos.z -= 1.0f;
                        target.z -= 1.0f;
                    }
                    else if (e.key.scancode == SDL_SCANCODE_D) {
                        cameraPos.x += 1.0f;
                        target.x += 1.0f;
                    }
                    else if (e.key.scancode == SDL_SCANCODE_Q) {
                        cameraPos.y += 1.0f;
                        target.y += 1.0f;
                    }
                    else if (e.key.scancode == SDL_SCANCODE_E) {
                        cameraPos.y -= 1.0f;
                        target.y -= 1.0f;
                    }
                    else if (e.key.scancode == SDL_SCANCODE_I) {
                        target.z += 1.0f;
                    }
                    else if (e.key.scancode == SDL_SCANCODE_J) {
                        target.x += 1.0f;
                    }
                    else if (e.key.scancode == SDL_SCANCODE_K) {
                        target.z -= 1.0f;
                    }
                    else if (e.key.scancode == SDL_SCANCODE_L) {
                        target.x -= 1.0f;
                    }
                    else if (e.key.scancode == SDL_SCANCODE_U) {
                        target.y += 1.0f;
                    }
                    else if (e.key.scancode == SDL_SCANCODE_O) {
                        target.y -= 1.0f;
                    }
            }
        }
        // if (broadPhase(monk, robot)) {
        //     // std::cout << "collide\n";
        // }
        // const bool* keyboardState = SDL_GetKeyboardState(&numkeys);
        

        setCamera(cameraPos, target);
        update();

        std::this_thread::sleep_for(1ms);
    }

    closeOut();
    return 0;
}

void PlayerComputerCollision(GameObject* player, GameObject* computer) {
    // player->setLinearVelocity(glm::vec3(0.0, 0.0, -1.0));
    glm::vec3 pos = player->getPosition();
    player->setPosition(glm::vec3(pos.x, pos.y, pos.z - 0.001));
}