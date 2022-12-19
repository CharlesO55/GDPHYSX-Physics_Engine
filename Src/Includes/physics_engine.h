#pragma once
#include <glm/glm.hpp>

#include "particle.h"

/*PARTICLES & RIGID BODIES
* Contains particle and rigid body values and methods
*   partType:                   Particle's type. Will not render/calculate if INACTIVE
*   despawnTime:                Reamining lifetime. Will despawn if reaches 0.
*   Force toggles:              isGravityActive, isConstantForceActive, isDragForceActive
*   Matrix generation:          calculateDerivedData(), calcTransMatrix(), calcRotateMatrix()
*   Particle initialization:    initParticle(), connectNextParticle()
*   Physics update:             updateMotion(), clearForceAccum(), addForceAccum(), clearTorqueAccum(), addTorqueAccum()
*/

class Particle {
public:
    //Model modelParticle       //Ideally, possess a model for rendering but can't bring in due to header clashes with libs in main.cpp
    float scale;     //To scale by
    //float damp;    //Damping value. Replaced with dragForce
    float mass;
    float radius;   //Size of the particle
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
    Particle* partNext;     //Next particle in a linked list (if any)



    Particle();
    Particle(glm::vec3 startPos);
    //Initialize particle variables
    void connectNextParticle(Particle *pNextParticle);
    void initParticle(int projType, glm::vec3 startPos);

    //Despawn the particle
    void despawnParticle(float deltaTime);

    //Update position
    virtual void updateMotion(float deltaTime);
    void addForceAccum(glm::vec3 newForce);

    //Reset forces accumulated
    void clearForceAccum();

    glm::mat4 transformMatrix;  //Will be used/copied by the model class
    glm::vec3 orientation;      //Empty for particles but filled for rigidBodies

    //Calculate the transform matrix. Not rigidbody exclusive since model class will also use the transform matrix.
    void calculateDerivedData();
        void calcTranslateMatrix();
        void calcRotateMatrix();
    glm::mat4 rotMatrix;
    glm::mat4 transMatrix;
};



    class RigidBody : public Particle {
    public:
        //Particle properties from before...

        float fLength = 8.f;        //Distance/size of the cube
        Particle* cubeParticles[8]; //8 particles to serve as contact points.

        glm::mat3 inertiaTensor;
        glm::vec3 angularVel;   //Angular velocity

        glm::vec3 torque;       //Rotational force
        glm::vec3 torqueAccum;  //Total torque

        RigidBody(glm::vec3 originPos, Particle massParticle[8]);   //Constructor

        void clearTorqueAccum();
        void addTorqueAccum(const glm::vec3 torqueForce);
        void updateMotion(const float deltaTime);     //Overwritten version with additional rotation calculations
        void calcRbParticles();                          //Makes sure the particles retain the cube position
        void calcInertiaTensor(const glm::vec3 point);   //Gets the intertiaTensor from a contact point's pos
        //Useful other functions from particle class: calculateDerivedData, calcTranslateMatrix, calcRotateMatrix
    };