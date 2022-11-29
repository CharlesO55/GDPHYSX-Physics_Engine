#pragma once
#include <glm/glm.hpp>

#include "particle.h"



class Particle {
public:
    //Model modelParticle       //Ideally, possess a model for rendering but can't bring in due to header clashes with libs in main.cpp
    float scale;     //To scale by
    //float damp;    //Damping value
    float mass;
    float radius;
    int partType;                //Type of the particle
    int isGravityActive;           //Gravity toggle
    int isConstantForceActive;     //Constant force toggle
    int isDragForceActive;         //Drag force toggle
    float despawnTime; //despawnTime is its lifespan
    glm::vec3 partPos;  //Position
    glm::vec3 partVel;  //Velocity
    glm::vec3 partAcc;  //Acceleration
    glm::vec3 constAcc; //Const acc to apply
    glm::vec3 forceAccum; //Active forces
    Particle* partNext;

    Particle();
    Particle(glm::vec3 startPos);
    //Initialize particle variables
    void connectNextParticle(Particle *pNextParticle);
    void initParticle(int projType, glm::vec3 startPos);

    //Despawn the particle
    void despawnParticle(float deltaTime);

    //Update position
    void updateMotion(float deltaTime);
    void addForceAccum(glm::vec3 newForce);

    //Reset forces accumulated
    void clearForceAccum();
};






//Parent class for calculating different forces 
class ParticleForceGenerator {
public:
    glm::vec3 force;

    ParticleForceGenerator();
    virtual void updateForce(Particle* part);   //To be overwritten dependent on child class
};


    //GRAVITY FORCE
    class GravityForce : public ParticleForceGenerator {
    public:
        void updateForce(Particle* part);
    };


    //PUSH/PULL
    class ConstantForce : public ParticleForceGenerator {
    public:
        void updateForce(Particle* part);
    };


    //DRAG FORCE
    class DragForce : public ParticleForceGenerator {
    public:
        float k1, k2;   //Drag constants
        
        DragForce(float constant1, float constant2);
        void updateForce(Particle* part);
    };



    //BASIC SPRING - 2 Ends should move
    class BasicSpring : public ParticleForceGenerator {
    public:
        float k;
        float restLength;
        glm::vec3 oppositeEnd;          //Just note the opposite end as a vec3 instead of indirectly accessing

        BasicSpring();
        void linkOtherEnd(glm::vec3 otherEnd);
        void updateForce(Particle* part);
    };


    //ANCHORED - One particle moves only
    class AnchoredSpring : public ParticleForceGenerator {
    public:
        glm::vec3 springEnd;
        float k;
        float restLength;

        AnchoredSpring(glm::vec3 anchorEnd);
        void updateForce(Particle* part);
    };


    //CENTRIPETAL FORCE - One particle moves only
    class CentripetalForce : public ParticleForceGenerator {
    public:
        glm::vec3 centralOrigin;    //The center of the circle. Where particles will pivot around
        float k;
        float radius;
        float radiusMin;

        CentripetalForce(glm::vec3 centralOrigin);
        void updateForce(Particle* part);
    };


    //BUNGEE - Pull only when stretched past restLength
    class ElasticBungee : public ParticleForceGenerator {
    public:
        float k;
        float restLength;
        //Particle* oppositeEnd;        //Discarded. Causes render errors when accessing with pointer
        glm::vec3 oppositeEnd;

        ElasticBungee();
        void linkOtherEnd(glm::vec3 otherEnd);
        void updateForce(Particle* part);
    };


//Links a Particle to a ParticleForceGenerator's updateForceMethod
class ForceRegistry {
public:
    void add(Particle* part, ParticleForceGenerator* forceGen);
};




//Handles contacts
class ParticleContact {
public:
    Particle* part[2];          //2 Particles
    float k, elasticity;      //Adjust bounciness (<1 inelastic, >1 elastic)
    glm::vec3 contactNormal;    //Direction
    float deltaTime;

    ParticleContact();
    int checkCollision();

    void resolve(Particle* part0, Particle* part1);
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
    virtual unsigned fillContact(ParticleContact *contact);
};

    //Rod contact
    class Rod : public ParticleLinker {
    public:
        Rod();
        Rod(Particle* part0, Particle* part1);
        unsigned fillContact(ParticleContact* contact);
    };




//Logic system and handles all particles
class ParticleWorld {
public:
    Particle* massParticlesHead;    //The start of mass particles and generally all particles
    Particle* bulletParticlesHead;  //The head pointing to bullets start

    //FORCES
    ForceRegistry registryGeneral;  //Links forces and particles
    GravityForce gravityGeneral;
    ConstantForce constantGeneral;
    DragForce dragGeneral = DragForce(0.9f, 0.1f);   //Drag forces calculator ~ Higher constants = More drag
    //CentripetalForce centripetalGeneral = CentripetalForce(ORIGIN);

    //CONTACTS
    ParticleContact contactGeneral;     //For any mass/bullet
    ParticleContact contactRods;        //Specifically for rods only
    Rod rods[MAX_CUBE_POINTS - 1];      //Rods



    ParticleWorld(Particle* partHead);        //Constructor
        void createRods();

    //ParticleRegistration* partHead; //Head/starting particle

    void startFrame();                  //Clear force accumulated
    void checkInput(int* isFired, int projectileType, int* isSpaceBarPressed);  //Handles spawning and control inputs
    void runPhysics(float deltaTime);                   //Physics updates
        void accumAllForces(Particle* selectedPart);    //Adds forces and sends to force registry
        void resolveContacts();                         //Resolves collision and interpenetration
};