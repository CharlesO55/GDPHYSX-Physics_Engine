#pragma once
#include <glm/glm.hpp>

#include "particle.h"



class Particle {
public:
    //Model modelParticle       //Ideally, possess a model for rendering but can't bring in due to header clashes with libs in main.cpp
    float size;     //To scale by
    float damp;    //Damping value
    float mass;
    int partType;                //Type of the particle
    int isGravityActive;           //Gravity toggle
    int isConstantForceActive;     //Constant force toggle
    int isDragForceActive;         //Drag force toggle
    float initTime, despawnTime; //despawnTime is its lifespan
    glm::vec3 partPos;  //Position
    glm::vec3 partVel;  //Velocity
    glm::vec3 partAcc;  //Acceleration
    glm::vec3 forceAccum; //Active forces

    Particle();
    //Initialize particle variables
    void initParticle(int projType, float scale, float currTime, glm::vec3 startPos);

    //Despawn the particle
    void despawnParticle(int* particleSlots);

    //Update position
    void updateMotion(float deltaTime, float currTime, int* particleSlots);
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