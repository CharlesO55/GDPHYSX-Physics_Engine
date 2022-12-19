#pragma once
#include "physics_engine.h"

/*COLLISIONS
* Contains ParticleContact to resolve collisions between particles.
*   resolve() - Main logic. Will call collision check and resolution
*       checkCollision()  - Collision checker
*       resolveVelocity() - Alters velocity as two particles collide/bounce
*       resolveInterpenetration - Repositions the particles apart
*
* ParticleLinker to link particles and keep them distance apart. FillContact calcs said distance and passes it to collision resolution
*   Rods - Keeps particles at certain distance between another
*   Cables - Keeps particles to not exceed a certain distance between another
*/


//Handles contacts
class ParticleContact {
public:
    Particle* part[2];          //2 Particles
    float k, elasticity;      //Adjust bounciness (<1 inelastic, >1 elastic)
    glm::vec3 contactNormal;    //Direction
    float deltaTime;
    RigidBody* rbCube;

    ParticleContact();
    int checkCollision();

    void resolve(Particle* part0, Particle* part1, RigidBody* rbCube);
    void resolveVelocity();

    float penetrationDepth;
    void resolveInterpenetration();
};



//Links particles with contact
class ParticleLinker {
public:
    ParticleLinker();
    Particle* part[2];
    float currLength, lengthLimit;
    glm::vec3 contactNormal;
    virtual unsigned fillContact(ParticleContact* contact);
};
    /*//Rod contact
    class Rod : public ParticleLinker {
    public:
        Rod();
        Rod(Particle* part0, Particle* part1);
        unsigned fillContact(ParticleContact* contact);
    };*/