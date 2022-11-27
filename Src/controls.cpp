#include <iostream>
#include <GLFW/glfw3.h>     //Keyboard and mouse library
#include "Includes/particle.h"

int projectileType = BULLET;
int isFired = 0;
int isSwitched = 0;
int spaceBarPressed = 0;

int isPointLight = 1;
int isPerspMode = 1;
int movementInput[3] = { 0 };

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_1:
            projectileType = BASIC_SPRING;
            isSwitched = 1;
            break;
        case GLFW_KEY_2:
            projectileType = ANCHORED_SPRING;
            isSwitched = 1;
            break;
        case GLFW_KEY_3:
            projectileType = ELASTIC_BUNGEE;
            isSwitched = 1;
            break;
        case GLFW_KEY_4:
            projectileType = LASER;
            isSwitched = 1;
            break;
        case GLFW_KEY_5:
            //projectileType = COIL;
            projectileType = BASIC_SPRING;
            isSwitched = 1;
            break;
        case GLFW_KEY_6:
            //projectileType = FIREWORK;
            projectileType = ANCHORED_SPRING;
            isSwitched = 1;
            break;
        case GLFW_KEY_7:
            projectileType = ELASTIC_BUNGEE;
            isSwitched = 1;
            break;
        case GLFW_KEY_SPACE:
            spaceBarPressed++;
            if (spaceBarPressed > 1)
                spaceBarPressed = 0;
            break;
        case GLFW_KEY_TAB:
            exit(0);
        default:
            break;
        }
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        isFired = 1;
}