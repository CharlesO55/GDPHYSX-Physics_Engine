#pragma once

#include "particle.h"
#include "physics_engine.h"
#include "particle_force_generators.h"
#include "particle_collisions.h"

/*PARTICLE WORLD
*   Handles most of the logic in spawning/despawning and updating particles. This is called every frame from main's loop
*       startFrame() - Clears all accumulated forces/torque
*       checkInput() - Spawns/repositions particles
*       runPhysics() - Update physics motion
* 
*   Stores pointers to particles created in main:
*       *rbCube
*       *bulletParticlesHead
*   
*   Force generators are also constructed and contained here:
*       gravityGeneral,
*       constantGeneral,
*       dragGeneral,
*       dragTorque
*/



class ParticleWorld {
public:
    Particle* massParticlesHead;    //The start of mass particles and generally all particles
    Particle* bulletParticlesHead;  //The head pointing to bullets start
    RigidBody* rbCube;

    //FORCES
    ForceRegistry registryGeneral;  //Links forces and particles
    GravityForce gravityGeneral;
    ConstantForce constantGeneral;
    DragForce dragGeneral = DragForce(0.9f, 0.1f);   //Drag forces calculator ~ Higher constants = More drag
    DragTorque dragTorque = DragTorque(0.9f, 0.1f);  //Drag torque for rigid body
    //CentripetalForce centripetalGeneral = CentripetalForce(ORIGIN);   //Old custom physics property submission

    //CONTACTS
    ParticleContact contactGeneral;     //For any mass/bullet
    //ParticleContact contactRods;        //Specifically for rods only
    //Rod rods[MAX_CUBE_POINTS - 1];      //Rods



    ParticleWorld(Particle* partHead, RigidBody* rbCube);        //Constructor
    //void createRods();


    void startFrame();                  //Clear force accumulated
    void checkInput(int* isFired, int projectileType, int* isSpaceBarPressed);  //Handles spawning and control inputs
    void runPhysics(float deltaTime);                   //Physics updates
    void accumAllForces(Particle* selectedPart);    //Adds forces and sends to force registry
    void accumAllTorque(RigidBody* rbCube);         //Adds torque to rigid bodies
    void resolveContacts();                         //Resolves collision and interpenetration
};