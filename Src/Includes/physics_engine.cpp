#include "physics_engine.h"
#include <iostream>


//PARTICLE
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

RigidBody::RigidBody(glm::vec3 originPos, Particle massParticle[8]) {
    //The origin particle's basic properties
    scale = DEFAULT_SCALE;
    radius = 1.f;
    despawnTime = 0.f; //despawnTime is its lifespan
    forceAccum = { 0.f, 0.f, 0.f };
    partAcc = { 0.f, 0.f, 0.f };
    partNext = NULL;

    torque = { 0.f, 0.f, 0.f };
    torqueAccum = { 0.f, 0.f, 0.f };
    angularVel = 0.f;
    orientation = { 0.f, 0.f, 0.f };

    initParticle(STATIONARY, originPos);

    //Connect the particles with the rigid body
    int i;
    for (i = 0; i < MAX_CUBE_POINTS; i++) {
        cubeParticles[i] = &massParticle[i];
    }
    calcRbParticles();
}

    void RigidBody::clearTorqueAccum() {
        torqueAccum = { 0.f, 0.f, 0.f };
    }


    void RigidBody::updateMotion(float deltaTime) {
        despawnParticle(deltaTime);     //Attempt to despawn particle and update its lifetime
        if (!partType || !mass) {
            return;     //End when particle isn't active or is unmovable
        }

        
        //LINEAR MOTION
        partAcc = (1 / mass) * forceAccum;      //Calculate acc from total forces
        partVel += partAcc * deltaTime;         //Add to velocity
        //partVel *= 0.99f;      //Replacd by dragForce
        partPos += partVel * deltaTime * scale;  //Update position
        

        //ROTATIONAL MOTION
        //angularVel += torqueAccum * deltaTime;
        angularVel *= 0.95f;
        orientation += glm::vec3(0.f, angularVel * deltaTime * scale, 0.f);
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
            partVel = { 0.f, 0.f, 0.f };
            constAcc = { 0.f, 0.f, 0.f };
            isGravityActive = INACTIVE;
            isDragForceActive = ACTIVE;
            break;
        case BULLET:
            despawnTime = 5.f;
            partVel = { 5.f, 0.f, 0.f };
            constAcc = { 0.f, 9.f, 0.f };
            isGravityActive = ACTIVE;
            isDragForceActive = ACTIVE;
            break;
        case ARTILLERY:
            despawnTime = 5.f;
            partVel = { 10.f, 20.f, 0.f };
            constAcc = { 5.f, 3.f, 0.f };
            isGravityActive = ACTIVE;
            isDragForceActive = ACTIVE;
            break;
        case FIREBALL:
            despawnTime = 5.f;
            partVel = { 5.f, 0.f, 0.f };
            constAcc = { 2.f, 10.f, 0.f };
            isGravityActive = INACTIVE;
            isDragForceActive = ACTIVE;
            break;
        case LASER:
            despawnTime = 5.f;
            partVel = { 10.f, 0.f, 0.f };
            constAcc = { 0.f, 0.f, 0.f };
            isGravityActive = INACTIVE;
            isDragForceActive = INACTIVE;
            break;
        //(SPRINGS DEACTIVATED)
        //For springs need to change partPos to keep restLength
        case BASIC_SPRING:
            despawnTime = 100.f;
            partVel = { 0.f, 0.1f, 0.f };
            constAcc = {-10.f, 5.f, 0.f };
            isGravityActive = INACTIVE;
            isDragForceActive = ACTIVE;
            break;
        case ANCHORED_SPRING:
            despawnTime = 100.f;
            partVel = { 0.1f, 0.f, 0.f };
            constAcc = { 100.f, 0.f, 0.f };
            isGravityActive = ACTIVE;
            isDragForceActive = ACTIVE;
            break;
        case ELASTIC_BUNGEE:
            despawnTime = 100.f;
            partVel = { 0.1f, 0.f, 0.f };
            constAcc = { 100.f, 0.f, 0.f };
            isGravityActive = INACTIVE;
            isDragForceActive = ACTIVE;
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

    //Update position
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

        //clearForceAccum();  //Clear total forces for next loop
    }

    //Adds force
    void Particle::addForceAccum(glm::vec3 newForce) {
        forceAccum += newForce;
    }
    //Reset forces accumulated
    void Particle::clearForceAccum() {
        forceAccum = { 0.f, 0.f, 0.f };
    }

    //Calculate the transform matrix
    void Particle::calculateDerivedData() {
        transformMatrix = glm::mat3(1.0f);
        
        calcTranslateMatrix();
        calcRotateMatrix();

        transformMatrix = transMatrix * rotMatrix;  //Translate to local then rotate
        return;
    }

    void Particle::calcTranslateMatrix() {
        glm::mat4 identity = glm::mat3(1.0f);
        transMatrix = glm::translate(identity, partPos);    //Matrix with particle position
    }

    void Particle::calcRotateMatrix() {
        glm::mat4 identity = glm::mat3(1.0f);

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


    

//FORCE GENERATORS
ParticleForceGenerator::ParticleForceGenerator() {
    force = { 0.f, 0.f, 0.f };
}
    void ParticleForceGenerator::updateForce(Particle* part) {
        std::cout << "\nNO FORCE DETECTED" << std::endl;    //If not overwritten print msg
    }

        //GRAVITY FORCE
        void GravityForce::updateForce(Particle* part) {
            if (part->isGravityActive)
                part->addForceAccum(glm::vec3(0.f, GRAVITY, 0.f) * part->mass);  //f = mg
        }

        //CONSTANT FORCE
        void ConstantForce::updateForce(Particle* part) {
            if (part->isConstantForceActive)
                part->addForceAccum(part->constAcc * part->mass);    //Add to particle's force accumulated
        }

        //DRAG FORCE
        DragForce::DragForce(float constant1, float constant2) {
                k1 = constant1;
                k2 = constant2;
            }
            void DragForce::updateForce(Particle* part) {
                if (part->isDragForceActive && part->partVel != glm::vec3(0.f, 0.f, 0.f)) {
                    float drag = glm::length(part->partVel);    //Magnitude of current particle velocity
                    drag = (k1 * drag) + (k2 * drag * drag);    //Drag using constants

                    glm::vec3 norm = glm::normalize(part->partVel); //Normalized velocity for a direction

                    part->addForceAccum(norm * -drag);               //Add drag * opposite dir into total force
                }
            }
        



        //BASIC SPRING FORCE
        BasicSpring::BasicSpring() {
            k = SPRING_CONSTANT;
            restLength = SPRING_REST_LENGTH;
            oppositeEnd = ORIGIN;          //Just note the opposite end as a vec3 instead of indirectly accessing
        }

            void BasicSpring::linkOtherEnd(glm::vec3 otherEnd) {
                oppositeEnd = otherEnd;
            }
            void BasicSpring::updateForce(Particle* part) {
                glm::vec3 v = part->partPos - oppositeEnd;      //Vec from end to end
                float magnitude = glm::length(v) - restLength;      //Magnitude of force. Depends on distance from rest length
                glm::vec3 force = glm::normalize(v) * magnitude * -k;   //Dir * distance * -k

                part->addForceAccum(force);
                //oppositeEnd->addForceAccum(force);         //Equal opposite force experienced on other end
                //[NOTE]: It works. But the render creates a fiasco. Just do 2 runs bcuz render engine is big baby
            }


        //ANCHORED SPRING FORCE
        AnchoredSpring::AnchoredSpring(glm::vec3 anchorEnd) {
            k = SPRING_CONSTANT;
            restLength = SPRING_REST_LENGTH;
            springEnd = anchorEnd;
        }

            void AnchoredSpring::updateForce(Particle* part) {
                glm::vec3 v = part->partPos - springEnd;  //Vec pos relative to other end
                float magnitude = glm::length(v) - restLength;  //Distance from rest length

                part->addForceAccum(glm::normalize(v) * magnitude * -k); //Direction vector * magnitude * -k
            }
        


        //CENTRIPETAL FORCE - One particle moves only
        CentripetalForce::CentripetalForce(glm::vec3 centralOrigin) {
            this->centralOrigin = centralOrigin;    //The center of the circle. Where particles will pivot around
            k = 1;
            radius = 0;
            radiusMin = 5.f;
        }
                
            void CentripetalForce::updateForce(Particle* part){
                glm::vec3 v = centralOrigin - part->partPos;    //Vector from end to end
                radius = glm::length(v);
                
                if (radius < radiusMin) {
                    k = -1;     //Behavior to counteract drag slowing the object and decreasing radius. 
                                //Turns into a Push force
                                //Occassionally will draw a star when damp is too strong
                }
                else {
                    k = 1;      //Normal behavior. Pull force
                }

                float magnitude = glm::length(v) * glm::length(v) / radius;       //  v^2 / r

                glm::vec3 force = glm::normalize(v) * magnitude * k;    //direction to center * magnitude * (push or pull)
                part->addForceAccum(force);
            }


        //BUNGEE FORCE
        ElasticBungee::ElasticBungee() {
            k = SPRING_CONSTANT;
            restLength = SPRING_REST_LENGTH;
            oppositeEnd = ORIGIN;
        }
            void ElasticBungee::linkOtherEnd(glm::vec3 otherEnd) {
                oppositeEnd = otherEnd;
            }

            void ElasticBungee::updateForce(Particle* part) {
                glm::vec3 v = part->partPos - oppositeEnd;
                float magnitude = glm::length(v);
                if (magnitude > restLength) {
                    magnitude = k * (restLength - magnitude);

                    glm::vec3 force = glm::normalize(v) * magnitude; //A pulling force
            
                    part->addForceAccum(force);          //Pull main particle together
                    //oppositeEnd->forceAccum += force;    //Pull the opposite end towards moving particle
                }
            }
        
        





//FORCE REGISTRY
    void ForceRegistry::add(Particle* part, ParticleForceGenerator* forceGen) {
        forceGen->updateForce(part);            //Calculate the force
    }



//PARTICLE CONTACT
ParticleContact::ParticleContact() {
    part[0] = NULL;   part[1] = NULL;       //2 Particles
    k = 1.f;                                  //Adjust bounciness
    elasticity = 1.2f;
    contactNormal = { 0.f, 0.f, 0.f };      //Direction
    deltaTime = 0.f;
    penetrationDepth = 1.f;
    rbCube = NULL;
}
    void ParticleContact::resolve(Particle* part0, Particle* part1, RigidBody* rbCube) {
        this->deltaTime = deltaTime;    //Save the time
        this->part[0] = part0;
        this->part[1] = part1;
        this->rbCube = rbCube;



        if (checkCollision()) {     //If collision occurs
            std::cout << "COLLISION\n";
            if (part[1]) {
                contactNormal = glm::normalize(part[0]->partPos - part[1]->partPos);
            }
            resolveVelocity();
            resolveInterpenetration();
            //Despawn the projectile
            part[1]->partType = INACTIVE;
            part[1]->despawnParticle(1000);
        }
    }
        
        void ParticleContact::resolveVelocity() {
            //Check if they're already separating
            
            float sepVel = glm::dot((rbCube->partVel/*part[0]->partVel*/ - part[1]->partVel), contactNormal) - rbCube->fLength;
            if (sepVel > 0) {
                return;
            }

            //Start finding new velocities
            float impulse = (1.f + k) * -sepVel;
            impulse /= ( 1 / (part[0]->mass * 8)) + (1 / part[1]->mass);   //Make it proportional to their masses

            //Directly edit vel
            //part[0]->partVel += contactNormal * impulse / part[0]->mass;
            rbCube->partVel -= contactNormal * impulse / (part[0]->mass * 8);
            rbCube->angularVel -= impulse / part[0]->mass;
            part[1]->partVel -= contactNormal * impulse * elasticity / part[1]->mass;   //Need additional amplifier since adding -vel to curr will just negate bullet's vel
        }



        //CHECKS COLLISION AND DEPTH
        int ParticleContact::checkCollision() {
            float posDistance = glm::length(rbCube->partPos - part[1]->partPos);
            float radSum =  rbCube->fLength + part[1]->radius * 3;
            penetrationDepth = posDistance - radSum;

            /*if (part[1]) {
                float posDistance = glm::length(part[0]->partPos - part[1]->partPos);
                float radSum = part[0]->radius + part[1]->radius;
                penetrationDepth = posDistance - radSum;    //Actual distance between particles - Min distance between circles
            }*/
            if (penetrationDepth <= 0) {
                return 1;
            }
            return 0;
        }

        //RESOLVE INTERPENETRATIONS
        void ParticleContact::resolveInterpenetration() {
            //Check if there's movable mass
            float totalInverseMass = 1 / part[0]->mass;
            if (part[1]) {
                totalInverseMass += 1 / part[1]->mass;
            }
            if (totalInverseMass <= 0) {
                return;
            }
            std::cout << "PEN\n";
            //Find the proportional amount of movement to mass
            glm::vec3 movePerMass = (-penetrationDepth / totalInverseMass) * contactNormal;    //Magnitude to move by * Direction

            //Reposition the particles
            part[0]->partPos += movePerMass * part[0]->mass;        //Move according to normal
            if (part[1]) {
                part[1]->partPos += -movePerMass * part[1]->mass;   //Move in opposite pos
            }
        }


        ParticleLinker::ParticleLinker() {
            part[0] = NULL;     part[1] = NULL;
            lengthLimit = 0.f;
            currLength = 0.f;
            contactNormal = { 0.f,0.f,0.f };
        }
            /*
            Rod::Rod() {
                part[0] = NULL; 
            }
            Rod::Rod(Particle* part0, Particle* part1) {
                part[0] = part0;
                part[1] = part1;
                this->lengthLimit = glm::length(part[0]->partPos - part[1]->partPos);       //Remember the distance between each part
                this->contactNormal = glm::normalize(part[0]->partPos - part[1]->partPos);  //Remember the between angle each part
                currLength = 0.f;
            }*/
                unsigned ParticleLinker::fillContact(ParticleContact* contact) {
                    std::cout << "NO CONTACT PARTICLE LINKER\n";
                    return 0;
                }
        

            /*unsigned Rod::fillContact(ParticleContact* contact) {
                currLength = glm::length(part[0]->partPos - part[1]->partPos);
                if (currLength == lengthLimit) {
                    return 0;
                }
                
                //Just directly change the position since we know the angle and length they were init in
                part[1]->partPos = part[0]->partPos + (lengthLimit * -contactNormal);


                contact->part[0] = part[0];
                contact->part[1] = part[1];
                contact->k = 0.f;*/
                
                /*
                //contactNormal = glm::normalize(part[0]->partPos - part[1]->partPos);    //If need current angle
                if (currLength > lengthLimit) {
                    contact->contactNormal = -contactNormal;
                    contact->penetrationDepth = (currLength - lengthLimit);
                }
                
                else if (currLength < lengthLimit) {
                    contact->contactNormal = contactNormal;
                    contact->penetrationDepth = (currLength - lengthLimit);
                }*/
                

              //  return 1;
            //}
        







//PARTICLE WORLD

//CONSTRUCTOR MARKS THE HEADS
ParticleWorld::ParticleWorld(Particle* partHead, RigidBody* rbCube) {
    this->rbCube = rbCube;
    //MARK THE HEADS
    massParticlesHead = partHead;
    bulletParticlesHead = partHead;

    while (bulletParticlesHead->partType == STATIONARY) {   //Find the first bullet (non-stationary particle)
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
        if (*isFired){
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
            rbCube->partPos = glm::vec3(20.f, -cos(glm::radians(45.f)) * 8, cos(glm::radians(45.f)) * 8);
            rbCube->orientation = { 0.f, 0.f, 0.f };
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
        Particle* currPart = /*bulletParticlesHead;*/massParticlesHead;
        
        //CALCULATE THE BULLETS
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
        accumAllForces(rbCube);
        rbCube->updateMotion(deltaTime);
        rbCube->calcRbParticles();

        resolveContacts();
        free(currPart);
    }

        //ACCUMULATES ALL FORCES
        void ParticleWorld::accumAllForces(Particle* selectedPart) {
            registryGeneral.add(selectedPart, &dragGeneral);
            registryGeneral.add(selectedPart, &gravityGeneral);
            registryGeneral.add(selectedPart, &constantGeneral);
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
    

/*
class Coil : public Particle{
public:
    float age = 0;          //Age since initialized
    float frequency = 3.f;  //Frequecy of spirals
    int stage = 1;          //Stage of the firework (inactive, launch, 1st, 2nd explosion)
    int payload = 1;
    float direction = 0.f;
    float velMag = 0.f;


    void addCoil(float currTime, glm::vec3 *lastPos) {  //Initial launch movement either coil or straight
        switch (stage) {
        case 1:
            age = currTime - initTime;
            if (partType == COIL) {         //Coiling firework
                partAcc = glm::vec3(        //Increase acceleration in spiraling manner
                    1.f,                        //Accelerate left on x
                    cos(age * frequency) * age, //Increase cos waves in y axis with time
                    sin(age * frequency) * age  //Same as y but in sin for z axis
                );
            }
            else if (partType == FIREWORK) { //Straight firework
                partAcc = glm::vec3(0.f, 20.f, 0.f); //Accelerate up
            }
            break;
        }
        *lastPos = partPos;
    }
    void prepStage(glm::vec3 lastPos, int phase) {  //Mostly randomizes rng values but also initializes some
        stage = phase;
        //std::cout << "STAGE: " << stage << std::endl;
        despawnTime = (std::rand() % (ageRange[stage - 1][1] - ageRange[stage - 1][0] + 1)) + ageRange[stage - 1][0];   //Randomize its lifespan
        if (stage > 1) {    //Randomize the explosion particles
            partType = COIL;        //Reuse the same particle as parent for 1st new particle
            direction = (std::rand() % (180 + 1)) * 3.14159 / 180;     //Random direction
            velMag = (std::rand() % (velRange[stage - 1][1] - velRange[stage - 1][0] + 1)) + velRange[stage - 1][0];    //Vel multiplier from given range
            partVel = velMag * glm::vec3(cos(direction), cos(direction), 0.f);  //Burst in x,y directions
            partPos = lastPos;      //Set position to parent's last pos before detonation
        }
    }
};*/