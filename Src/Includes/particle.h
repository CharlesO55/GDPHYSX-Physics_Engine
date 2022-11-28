#ifndef PARTICLE_FILE
#define PARTICLE_FILE

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
//#define MAX_SPRINGS 2

static glm::vec3 ORIGIN = glm::vec3(0.f, 0.f, 0.f);

/*
//Preset values for damp, v, a
                            //BASIC//ANCHORED//BUNGEE
static float dampSettings[] = { 0.99f , 0.99f, 0.9f, 0.99f };   //Replaced by DragForce


static int gravitySettings[] = { INACTIVE , ACTIVE, INACTIVE, INACTIVE };   //Activate gravity for springs
static int constantForceSettings[] = { ACTIVE , ACTIVE, ACTIVE, ACTIVE };   //Allow user to apply force per click
static int dragForceSettings[] = { ACTIVE , ACTIVE, ACTIVE, INACTIVE };       //Activate drag for all springs   




static glm::vec3 velocitySettings[] = {
    //IMPORTANT BUG: Model will occasionally not render if no velocity is present. Put at least a miniscule v. Something wrong with Model::translate()
    glm::vec3(0.f, 0.1f, 0.f),  //1 Basic Spring
    glm::vec3(0.1f, 0.f, 0.f),  //2 Anchored Spring 
    glm::vec3(0.1f, 0.f, 0.f),  //3 Elastic Bungee
    glm::vec3(2.f, 0.f, 0.f), //4 Centripetal Force
};
static glm::vec3 accelerationSettings[] = { //Constant force. Partnered with ACTIVE/INACTIVE constantForceSettings[]
    glm::vec3(-10.f, 5.f, 0.f),     //1 BASIC: Glide right
    glm::vec3(100.f, 0.f, 0.f),     //2 ANCHORED: Move left
    glm::vec3(100.f, 0.f, 0.f),     //3 ELASTIC: Move left
    glm::vec3(0.f, 100.f, 0.f),       //4
};
*/



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