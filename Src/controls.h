#ifndef CONTROLS_FILE
#define CONTROLS_FILE


extern int projectileType;
extern int isFired;
extern int isSwitched;

extern int isPointLight;
extern int isPerspMode;
extern int movementInput[3];

extern int spaceBarPressed;

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

#endif
