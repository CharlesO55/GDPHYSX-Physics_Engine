#pragma once
#include "physics_engine.h"

/*FORCE GENERATORS
* Contains force generators for linear and rotational forces:
* //Linear forces
*   GravityForce
*   ConstantForce
*   DragForce
*   BasicSpring
*   AnchoredSpring
*   ElasticBungee
*   CentripetalForce (custom force for hw activity)
* 
* //Rotational forces
*   DragTorque
* 
* ForceRegistry link to call Generator's updateForce and add to Particle forceAccum 
*/

//Parent class for calculating different forces 
class ParticleForceGenerator {
public:
    //glm::vec3 force;

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

        DragForce(const float constant1, const float constant2);
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




//CALCULATES ROTATIONAL FORCES
class ParticleTorqueGenerator {
    virtual void updateForce(RigidBody* rbCube);
};
    //ROTATIONAL DRAG
    class DragTorque : public ParticleTorqueGenerator {
    public:
        float k1, k2;

        DragTorque(const float constant1, const float constant2);
        void updateForce(RigidBody* rbCube);
    };




//Links a Particle to a ParticleForceGenerator's updateForceMethod
class ForceRegistry {
public:
    void add(Particle* part, ParticleForceGenerator* forceGen);
};