#include "particle_world.h"


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



//CONSTRUCTOR MARKS THE HEADS
ParticleWorld::ParticleWorld(Particle* partHead, RigidBody* rbCube) {
    this->rbCube = rbCube;
    //MARK THE HEADS
    massParticlesHead = partHead;
    bulletParticlesHead = partHead;

    while (bulletParticlesHead->partType == STATIONARY) {   //Find the first bullet (non-stationary or inactive particle)
        bulletParticlesHead = bulletParticlesHead->partNext;
    }

    //createRods();    
}

//CREATE RODS
/*void ParticleWorld::createRods() {
    Particle* currPart = massParticlesHead;
/*
      4--------6          4--------6
     /|       /|         /|       b|
    2--------0 |        2----a---0 |
    | |      | |        | |      | |
    | 5--------7        | 5------c-7
    |/       |/         |/       |/
    1--------3          1--------3
*/
/*    int i = 0;
    while (i < MAX_CUBE_POINTS - 1) {
        currPart = currPart->partNext;
        rods[i] = Rod(massParticlesHead, currPart);
        i++;
    }
}*/



//CLEAR ALL ACCUMULATED FORCES
void ParticleWorld::startFrame() {
    rbCube->clearForceAccum();
    rbCube->clearTorqueAccum();
    Particle* currPart = this->massParticlesHead;      //Start at the head of all particles

    while (currPart) {  //While the current particle is existing/not null
        currPart->clearForceAccum();  //Clear the particle's force
        currPart = currPart->partNext;      //Loop to the next linked particle
    }
    free(currPart);
}


//SPAWNS PARTICLES OR OTHER ACTIONS BASED ON INPUT
void ParticleWorld::checkInput(int* isFired, int projectileType, int* isSpaceBarPressed) {
    //SPAWN A BULLET IF THERE'S AN INACTIVE BULLET
    if (*isFired) {
        Particle* currPart = bulletParticlesHead;

        //FIND AN INACTIVE PARTICLE AND INIT IT
        while (currPart != NULL) {
            if (currPart->partType == INACTIVE) {
                currPart->initParticle(projectileType, ORIGIN);
                break;
            }
            currPart = currPart->partNext;
        }
    }

    //RESET POSITION OF CUBE PARTICLES
    if (*isSpaceBarPressed) {
        //RIGID BODY REPOSITION
        rbCube->partPos = glm::vec3(12.f, 20.f, 0.f);// -cos(glm::radians(45.f)) * 8, cos(glm::radians(45.f)) * 8);
        rbCube->orientation = { 0.f, 0.f, 0.f };

        //OLD PARTICLE REPOSITIONING
        /*Particle* currPart = massParticlesHead;
        int i = 0;

        while (currPart->partType == STATIONARY) {
            currPart->partPos = cubePoints[i];
            currPart = currPart->partNext;
            i++;
        }*/
    }

    *isFired = INACTIVE;
    *isSpaceBarPressed = INACTIVE;
}



//CALC PARTICLE ACC, VEL, AND POS
void ParticleWorld::runPhysics(float deltaTime) {
    //Integrate all particles
    Particle* currPart = bulletParticlesHead;//massParticlesHead;

    //CALCULATE THE BULLETS MOTION AND MATRIX
    while (currPart != NULL)
    {
        if (currPart->partType != INACTIVE) {
            accumAllForces(currPart);
            currPart->updateMotion(deltaTime);
            currPart->calculateDerivedData();
        }
        currPart = currPart->partNext;
    }

    //CALCULATE THE RIGIDBODY PARTICLES
    accumAllForces(rbCube);     //Get all forces
    accumAllTorque(rbCube);     //Get all torque
    rbCube->updateMotion(deltaTime);    //Update rigid body pos and orientation
    rbCube->calcRbParticles();  //Calc transform matrix from pos and orientation

    resolveContacts();  //Resolve collisions between rigid body and projectiles
    free(currPart);
}


//ACCUMULATES ALL LINEAR FORCES
void ParticleWorld::accumAllForces(Particle* selectedPart) {
    registryGeneral.add(selectedPart, &dragGeneral);        //Drag force
    registryGeneral.add(selectedPart, &gravityGeneral);     //Gravity force
    registryGeneral.add(selectedPart, &constantGeneral);    //Constant force (i.e. pushes)
}


//ACCUMULATES ALL TORQUE
void ParticleWorld::accumAllTorque(RigidBody* rbCube) {
    dragTorque.updateForce(rbCube);     //Drag force affecting torque
    //gravity has no effect.
}


//CALLS THE INTERPENETRATION AND COLLISION RESOLUTION
void ParticleWorld::resolveContacts() {
    //RESOLVE ACTIVE BULLETS HITTING MASS PARTICLE
    Particle* bullCurr = bulletParticlesHead;
    while (bullCurr != NULL) {
        if (bullCurr->partType != INACTIVE) {

            //CHECK THE PARTICLES FOR COLLISION
            Particle* massCurr = massParticlesHead;
            while (massCurr->partType == STATIONARY) {
                contactGeneral.resolve(massCurr, bullCurr, rbCube);
                massCurr = massCurr->partNext;
            }

            //[BASIC VER] PROCESSES COLLISIONS OF ANY BULLET COLLIDING WITH TOP RIGHT CUBE PARTICLE ONLY
            //    contactGeneral.resolve(massParticlesHead, bullCurr);
        }
        bullCurr = bullCurr->partNext;
    }
    free(bullCurr);


    /*//RESOLVE RODS
    if (massParticlesHead->partVel == glm::vec3(0.f, 0.f, 0.f)) {
        return; //If not moving. Then assume no need to solve rods interpenetration
    }
    for (int i = 0; i < MAX_CUBE_POINTS - 1; i++) {
        rods[i].fillContact(&contactRods);  //fillContact will check and call resolveInterpenetration
    }*/
}