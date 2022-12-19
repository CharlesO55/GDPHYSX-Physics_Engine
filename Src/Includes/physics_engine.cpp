#include "physics_engine.h"
#include <iostream>

/*PARTICLES & RIGID BODIES
* Contains particle and rigid body values and methods
*   partType:                   Particle's type. Will not render/calculate if INACTIVE
*   despawnTime:                Reamining lifetime. Will despawn if reaches 0.
*   Force toggles:              isGravityActive, isConstantForceActive, isDragForceActive
*   Matrix generation:          calculateDerivedData(), calcTransMatrix(), calcRotateMatrix()
*   Particle initialization:    initParticle(), connectNextParticle()
*   Physics update:             updateMotion(), clearForceAccum(), addForceAccum(), clearTorqueAccum(), addTorqueAccum()
*/



//PARTICLE
//DEFAULT CONSTRUCTOR
Particle::Particle() {
    scale = DEFAULT_SCALE;     //Default To scale by
    mass = 1.f;
    radius = 1.f;
    partType = INACTIVE;                //Type of the particle
    isGravityActive = ACTIVE;           //Gravity toggle
    isConstantForceActive = ACTIVE;     //Constant force toggle
    constAcc = { 0.f, 0.f, 0.f };
    isDragForceActive = ACTIVE;         //Drag force toggle
    despawnTime = 0.f; //despawnTime is its lifespan
    partPos = ORIGIN;
    partVel = { 0.f, 0.f, 0.f };
    partAcc = { 0.f, 0.f, 0.f };
    forceAccum = { 0.f, 0.f, 0.f };
    partNext = NULL;
    orientation = { 0.f, 0.f, 0.f };
}
//CONSTRUCTOR WITH POSITION FOR STATIONARY PARTICLES
Particle::Particle(glm::vec3 startPos) {
    scale = DEFAULT_SCALE;
    radius = 1.f;
    despawnTime = 0.f; //despawnTime is its lifespan
    forceAccum = { 0.f, 0.f, 0.f };
    partAcc = { 0.f, 0.f, 0.f };
    partNext = NULL;
    orientation = { 0.f, 0.f, 0.f };

    initParticle(STATIONARY, startPos);
}

    //CONNECTS PARTICLES FOR LINKED LIST USES
    void Particle::connectNextParticle(Particle* pNextParticle) {
        this->partNext = pNextParticle;
    }

    //Initialize particle variable
    void Particle::initParticle(int projType, glm::vec3 startPos) {	//Initialize values for damp, m, v, a, etc.
        mass = 1.f;
        partType = projType;

        partPos = startPos;
        isConstantForceActive = ACTIVE;     //For pushing


        switch (projType) {
        case STATIONARY:
            despawnTime = 1000.f;
            partVel = PARTICLE_INIT_SETTINGS[0][0];  //{ 0.f, 0.f, 0.f };
            constAcc = PARTICLE_INIT_SETTINGS[0][1]; //{ 0.f, 0.f, 0.f };
            isGravityActive = GRAVITY_SETTINGS[0];  //INACTIVE;
            isDragForceActive = DRAG_SETTINGS[0];   //ACTIVE;
            break;
        case BULLET:
            despawnTime = 5.f;
            partVel = PARTICLE_INIT_SETTINGS[1][0]; //{ 5.f, 0.f, 0.f };
            constAcc = PARTICLE_INIT_SETTINGS[1][1]; //{ 0.f, 9.f, 0.f };
            isGravityActive = GRAVITY_SETTINGS[1]; //ACTIVE;
            isDragForceActive = DRAG_SETTINGS[1]; //ACTIVE;
            break;
        case ARTILLERY:
            despawnTime = 5.f;
            partVel = PARTICLE_INIT_SETTINGS[2][0]; //{ 10.f, 20.f, 0.f };
            constAcc = PARTICLE_INIT_SETTINGS[2][1]; //{ 5.f, 3.f, 0.f };
            isGravityActive = GRAVITY_SETTINGS[2]; //ACTIVE;
            isDragForceActive = DRAG_SETTINGS[2]; //ACTIVE;
            break;
        case FIREBALL:
            despawnTime = 5.f;
            partVel = PARTICLE_INIT_SETTINGS[3][0]; //{ 5.f, 0.f, 0.f };
            constAcc = PARTICLE_INIT_SETTINGS[3][1]; //{ 2.f, 10.f, 0.f };
            isGravityActive = GRAVITY_SETTINGS[3];  //INACTIVE;
            isDragForceActive = DRAG_SETTINGS[3]; //ACTIVE;
            break;
        case LASER:
            despawnTime = 5.f;
            partVel = PARTICLE_INIT_SETTINGS[4][0]; //{ 10.f, 0.f, 0.f };
            constAcc = PARTICLE_INIT_SETTINGS[4][1]; //{ 0.f, 0.f, 0.f };
            isGravityActive = GRAVITY_SETTINGS[4];  //INACTIVE;
            isDragForceActive = DRAG_SETTINGS[4]; //INACTIVE;
            break;
        //(SPRINGS DEACTIVATED)
        //For springs need to change partPos to keep restLength
        case BASIC_SPRING:
            despawnTime = 100.f;
            partVel = PARTICLE_INIT_SETTINGS[5][0]; //{ 0.f, 0.1f, 0.f };
            constAcc = PARTICLE_INIT_SETTINGS[5][1]; // {-10.f, 5.f, 0.f };
            isGravityActive = GRAVITY_SETTINGS[5]; //INACTIVE;
            isDragForceActive = DRAG_SETTINGS[5]; //ACTIVE;
            break;
        case ANCHORED_SPRING:
            despawnTime = 100.f;
            partVel = PARTICLE_INIT_SETTINGS[6][0]; // { 0.1f, 0.f, 0.f };
            constAcc = PARTICLE_INIT_SETTINGS[6][1]; //{ 100.f, 0.f, 0.f };
            isGravityActive = GRAVITY_SETTINGS[6]; //ACTIVE;
            isDragForceActive = DRAG_SETTINGS[6];  //ACTIVE;
            break;
        case ELASTIC_BUNGEE:
            despawnTime = 100.f;
            partVel = PARTICLE_INIT_SETTINGS[7][0]; //{ 0.1f, 0.f, 0.f };
            constAcc = PARTICLE_INIT_SETTINGS[7][1]; //{ 100.f, 0.f, 0.f };
            isGravityActive = GRAVITY_SETTINGS[7]; //INACTIVE;
            isDragForceActive = DRAG_SETTINGS[7];  //ACTIVE;
            break;
        default:
            std::cout << "INITIALIZED NOT IN SCOPE" << std::endl;
        }
    }

    //Check and despawn the particle
    void Particle::despawnParticle(float deltaTime) {
        despawnTime -= deltaTime;

        //Check if still active and already pass lifespan
        if (partType != INACTIVE && despawnTime <= 0) {
            partType = INACTIVE;    //Set type to inactive and will not be rendered in main()
            
            std::cout << "DESPAWNPART\n";
        }
    }

    //UPDATE PARTICLE POSITION
    void Particle::updateMotion(float deltaTime) {    //Update particle's motion
        despawnParticle(deltaTime);     //Attempt to despawn particle and update its lifetime
        if (!partType || !mass) {
            return;     //End when particle isn't active or is unmovable
        }

        //Assume particle is still active if still not exited
        
        partAcc = (1 / mass) * forceAccum;      //Calculate acc from total forces
        partVel += partAcc * deltaTime;         //Add to velocity
        //partVel *= damp;      //Replaced by DragForce
        partPos += partVel * deltaTime * scale;  //Update position

        //clearForceAccum();  //Clear total forces for next loop. CLEARED IN STARTFRAME
    }

    //ADDS FORCES
    void Particle::addForceAccum(glm::vec3 newForce) {
        forceAccum += newForce;
    }
    //RESET FORCES ACCUMULATED
    void Particle::clearForceAccum() {
        forceAccum = { 0.f, 0.f, 0.f };
    }


    //CALC THE TRANSFORM MATRIX
    void Particle::calculateDerivedData() {
        transformMatrix = glm::mat3(1.0f);  //Reset the transform matrix every run
        
        calcTranslateMatrix();
        calcRotateMatrix();

        transformMatrix = transMatrix * rotMatrix;  //Translate to local then rotate
        return;
    }

        //GENERATES A TRANSLATE MATRIX FOR POSITION TRANSLATION
        void Particle::calcTranslateMatrix() {
            glm::mat4 identity = glm::mat3(1.0f);
            transMatrix = glm::translate(identity, partPos);    //Matrix with particle position
        }

        //GENERATES A ROTATION MATRIX
        void Particle::calcRotateMatrix() {
            glm::mat4 identity = glm::mat3(1.0f);
            //Rotate by each axis
            rotMatrix = glm::rotate(identity,
                glm::radians(orientation.x),
                glm::vec3(1, 0, 0));
            rotMatrix = glm::rotate(rotMatrix,
                glm::radians(orientation.y),
                glm::vec3(0, 1, 0));
            rotMatrix = glm::rotate(rotMatrix,
                glm::radians(orientation.z),
                glm::vec3(0, 0, 1));
        }



//RIGID BODY CONSTRUCTOR
RigidBody::RigidBody(glm::vec3 originPos, Particle massParticle[8]) {
    //The origin particle's basic properties
    scale = DEFAULT_SCALE;
    mass = 1.f;
    radius = 1.f;
    despawnTime = 0.f; //despawnTime is its lifespan
    forceAccum = { 0.f, 0.f, 0.f };
    partAcc = { 0.f, 0.f, 0.f };
    partNext = NULL;

    torque = { 0.f, 0.f, 0.f };
    torqueAccum = { 0.f, 0.f, 0.f };
    angularVel = { 0.f, 0.f, 0.f };
    orientation = { 0.f, 0.f, 0.f };

    initParticle(STATIONARY, originPos);

    //Connect the particles with the rigid body
    int i;
    for (i = 0; i < MAX_CUBE_POINTS; i++) {
        cubeParticles[i] = &massParticle[i];
    }
    calcRbParticles();
}

    //RESETS TORQUE
    void RigidBody::clearTorqueAccum() {
        torqueAccum = { 0.f, 0.f, 0.f };
    }

    //ADD TO TORQUE ACCUMULATED
    void RigidBody::addTorqueAccum(const glm::vec3 newTorque) {
        torqueAccum += newTorque;
    }

    //CALCULATE LINEAR AND ROTATIONAL CHANGES
    void RigidBody::updateMotion(const float deltaTime) {
        //despawnParticle(deltaTime);     //Rigid body shouldn't despawn
        if (!partType || !mass) {
            return;     //End when particle isn't active or is unmovable
        }

        //LINEAR MOTION
        partAcc = (1 / mass) * forceAccum;      //Calculate acc from total forces
        partVel += partAcc * deltaTime;         //Add to velocity
        //partVel *= 0.99f;      //Replacd by dragForce
        partPos += partVel * deltaTime * scale;  //Update position

        //ROTATIONAL MOTION
        angularVel += (1 / mass) * torqueAccum * deltaTime;
        //angularVel *= 0.95f;  //Replaced with drag torque
        orientation += angularVel * deltaTime * scale;
    }

    //Calculates the particles position with respect to RigidBody's origin position and orientation
    void RigidBody::calcRbParticles() {

        //CREATE THE LOCAL SPACE MATRIX
        for (int i = 0; i < 8; i++) {
            cubeParticles[i]->partPos = this->partPos;          //Particle set to local origin
            cubeParticles[i]->orientation = this->orientation;  //Particle copy the rigidbody's orientation
            cubeParticles[i]->calculateDerivedData();           //Sets the transform matrix from the calculated transMatrix and rotateMatrix
        }

        //TRANSLATE TO NEW POSITION RESPECTIVE OF THE LOCAL SPACE
        float distance = cos(glm::radians(45.f)) * fLength;     //Apply distance 
        cubeParticles[0]->partPos = glm::vec3(-distance, distance, -distance);
        cubeParticles[1]->partPos = glm::vec3(distance, -distance, -distance);
        cubeParticles[2]->partPos = glm::vec3(distance, distance, -distance);
        cubeParticles[3]->partPos = glm::vec3(-distance, -distance, -distance);
        cubeParticles[4]->partPos = glm::vec3(distance, distance, distance);
        cubeParticles[5]->partPos = glm::vec3(distance, -distance, distance);
        cubeParticles[6]->partPos = glm::vec3(-distance, distance, distance);
        cubeParticles[7]->partPos = glm::vec3(-distance, -distance, distance);

        for (int i = 0; i < 8; i++) {
            cubeParticles[i]->calcTranslateMatrix();    //Generate a new transMatrix
            cubeParticles[i]->transformMatrix *= cubeParticles[i]->transMatrix; //Apply it to the existing local transform matrix
        }
    }

    //Gets the inertia tensor based on cuboid formula
    void RigidBody::calcInertiaTensor(const glm::vec3 point) {
        float cubeLength = cos(glm::radians(45.f)) * fLength;
        float cuboidTensor = (1.f / 12.f) * this->mass;
        inertiaTensor = {
            {cuboidTensor * (point.y * point.y + point.z * point.z), 0.f, 0.f},
            {0.f, cuboidTensor * (point.x * point.x + point.z * point.z), 0.f},
            {0.f, 0.f, cuboidTensor * (point.x * point.x + point.y * point.y)}
        };
    }