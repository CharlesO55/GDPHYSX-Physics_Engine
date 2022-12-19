#ifndef PARTICLE_FILE
#define PARTICLE_FILE

/*This header file contains most Particle initialization related items. 
* Properties to alter are:
*   PARTICLE_INIT_SETTINGS - Particle's initialized velocity and acceleration
*   GRAVITY_SETTINGS - Particle's gravity toggling between ACTIVE and INACTIVE
*   DRAG_SETTINGS - Particle's drag force toggle betweem ACTIVE and INACTIVE
*/


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GRAVITY -10.f
#define FORCE -750.f

#define STATIONARY -1
#define BULLET    1
#define ARTILLERY 2
#define FIREBALL  3
#define LASER     4

#define BASIC_SPRING    5
#define ANCHORED_SPRING 6
#define ELASTIC_BUNGEE  7

//DEACTIVATED
#define COIL 999
#define FIREWORK 999
#define CENTRIPETAL_FORCE 999



#define SPRING_CONSTANT 0.5f        //1 - Bouncy
#define SPRING_REST_LENGTH 5.f

#define INACTIVE 0
#define ACTIVE 1
#define SPAWN 1
#define MAX_PARTICLES 8
#define MAX_CUBE_POINTS 8

#define DEFAULT_SCALE 10.f

#define TOTAL_PARTICLE_TYPES 8

static glm::vec3 ORIGIN = glm::vec3(0.f, 0.f, 0.f);


//THE PARTICLE TYPES' VELOCITY AND ACCELERATION
static glm::vec3 PARTICLE_INIT_SETTINGS[TOTAL_PARTICLE_TYPES][2] = {
            //VELOCITY                 //ACCELERATION
    { glm::vec3(0.f, 0.f, 0.f),     glm::vec3(0.f, 0.f, 0.f) }, //STATIONARY

    { glm::vec3(5.f, 0.f, 0.f),     glm::vec3(0.f, 9.f, 0.f) }, //BULLET
    { glm::vec3(10.f, 20.f, 0.f),   glm::vec3(5.f, 3.f, 0.f) }, //ARTILLERY
    { glm::vec3(4.f, 0.f, 0.f),     glm::vec3(2.f, 10.f, 0.f)}, //FIREBALL
    { glm::vec3(10.f, 0.f, 0.f),    glm::vec3(0.f, 0.f, 0.f) }, //LASER

    { glm::vec3(0.f, 0.1f, 0.f),    glm::vec3(-10.f, 5.f, 0.f) }, //BASIC SPRING
    { glm::vec3(0.1f, 0.f, 0.f),    glm::vec3(100.f, 0.f, 0.f) }, //ANCHORED SPRING
    { glm::vec3(0.1f, 0.f, 0.f),    glm::vec3(100.f, 0.f, 0.f) }  //BUNGEE SPRING
};

//THE PARTICLE TYPES' GRAVITY TOGGLE
static int GRAVITY_SETTINGS[TOTAL_PARTICLE_TYPES] = {
    INACTIVE,   //STATIONARY
    ACTIVE,     //BULLET
    ACTIVE,     //ARTILLERY
    INACTIVE,   //FIREBALL
    INACTIVE,   //LASER
    INACTIVE,   //BASIC SPRING
    ACTIVE,     //ANCHORED SPRING
    INACTIVE    //BUNGEE
};

//THE PARTICLE TYPES' DRAG FORCE TOGGLE
static int DRAG_SETTINGS[TOTAL_PARTICLE_TYPES] = {
    ACTIVE,     //STATIONARY
    ACTIVE,     //BULLET
    ACTIVE,     //ARTILLERY
    INACTIVE,   //FIREBALL
    INACTIVE,   //LASER
    ACTIVE,     //BASIC SPRING
    ACTIVE,     //ANCHORED SPRING
    ACTIVE      //BUNGEE
};


/*
static float cubeSideHalfLength = 5.f;
static glm::vec3 cubeOrigin = glm::vec3(10.f, 0.f, 0.f) + glm::vec3(cubeSideHalfLength, -cubeSideHalfLength, cubeSideHalfLength);

static glm::vec3 cubePoints[MAX_CUBE_POINTS] = {
    //FRONT
    glm::vec3(cubeOrigin + glm::vec3(-cubeSideHalfLength, cubeSideHalfLength, 0.f - cubeSideHalfLength)),    //0 Top Right
    glm::vec3(cubeOrigin + glm::vec3(cubeSideHalfLength, -cubeSideHalfLength, 0.f - cubeSideHalfLength)),    //1 Bot Left
    glm::vec3(cubeOrigin + glm::vec3(cubeSideHalfLength, cubeSideHalfLength, 0.f - cubeSideHalfLength)),     //2 Top Left
    glm::vec3(cubeOrigin + glm::vec3(-cubeSideHalfLength, -cubeSideHalfLength, 0.f - cubeSideHalfLength)),   //3 Bot Right
    //BACK
    glm::vec3(cubeOrigin + glm::vec3(cubeSideHalfLength, cubeSideHalfLength, 0.f + cubeSideHalfLength)),     //4 Top Left
    glm::vec3(cubeOrigin + glm::vec3(cubeSideHalfLength, -cubeSideHalfLength, 0.f + cubeSideHalfLength)),    //5 Bot Left
    glm::vec3(cubeOrigin + glm::vec3(-cubeSideHalfLength, cubeSideHalfLength, 0.f + cubeSideHalfLength)),    //6 Top Right
    glm::vec3(cubeOrigin + glm::vec3(-cubeSideHalfLength, -cubeSideHalfLength, 0.f + cubeSideHalfLength))   //7 Bot Right
};*/

/*
//Firework rule values
static int velRange[3][2] = { {4, 6} , //(min, max) velocity ranges of firework
    {3, 5} , {3, 4}
};
static int ageRange[3][2] = { {10, 15} , //(min, max) age ranges of firework
    {2, 4} , {1, 2}
};
static glm::vec3 fireworkDirections[3] = { //Directions for firework vel
    glm::vec3(0.f, 1.f, 0.f),       //Only upwards in launch stage
    glm::vec3(0.1f, 0.5f, 0.1f),    //Still mostly upwards detonation
    glm::vec3(0.3f, 0.3f, 0.3f)     //Scatter in all directions
};*/

#endif