#include "physics_engine.h"
#include <iostream>


//PARTICLE
Particle::Particle() {
    size = 1.f;     //To scale by
    damp = 0.9f;    //Damping value
    mass = 1.f;
    partType = INACTIVE;                //Type of the particle
    isGravityActive = ACTIVE;           //Gravity toggle
    isConstantForceActive = ACTIVE;     //Constant force toggle
    isDragForceActive = ACTIVE;         //Drag force toggle
    initTime = 0.f;
    despawnTime = 0.f; //despawnTime is its lifespan
    partPos = ORIGIN;
    partVel = { 0.f, 0.f, 0.f };
    partAcc = { 0.f, 0.f, 0.f };
    forceAccum = { 0.f, 0.f, 0.f };
}

    //Initialize particle variables
    void Particle::initParticle(int projType, float scale, float currTime, glm::vec3 startPos) {	//Initialize values for damp, m, v, a, etc.
        size = scale;
        despawnTime = 100;
        initTime = currTime;    //Time initialized
        partType = projType;

        //damp = dampSettings[projType - 1];  //Settings from particle.h (Replaced by DragForce)
        mass = massSettings[projType - 1];
        partPos = startPos;
        partVel = velocitySettings[projType - 1];
        //partAcc = accelerationSettings[projType - 1];

        isGravityActive = gravitySettings[projType - 1];
        isConstantForceActive = constantForceSettings[projType - 1];
        isDragForceActive = dragForceSettings[projType - 1];
        std::cout << "INITIALIZED\n";
    }

    //Despawn the particle
    void Particle::despawnParticle(int* particleSlots) {
        if (partType != INACTIVE) {
            partType = INACTIVE;    //Set type to inactive and will not be rendered in main()
            *particleSlots += 1;    //Restore amount of available slots for particles by 1
            std::cout << "DESPAWNPART\n";
        }
    }

    //Update position
    void Particle::updateMotion(float deltaTime, float currTime, int* particleSlots) {    //Update particle's motion
        if (!partType || !mass) {
            return;     //End when particle isn't active or is unmovable
        }

        //Assume particle is still active if still not exited
        if (currTime - initTime < despawnTime) {  //Check if should still be active before updating
            partAcc = (1 / mass) * forceAccum;      //Calculate acc from total forces
            partVel += partAcc * deltaTime;         //Add to velocity
            //partVel *= damp;      //Replaced by DragForce
            partPos += partVel * deltaTime * size;  //Update position
            //std::cout << "UPDARE\n";
        }
        else {  //Despawn when lifespan is exceeded
            despawnParticle(particleSlots);
        }
        clearForceAccum();  //Clear total forces for next loop
    }
    //Adds force
    void Particle::addForceAccum(glm::vec3 newForce) {
        forceAccum += newForce;
    }
    //Reset forces accumulated
    void Particle::clearForceAccum() {
        forceAccum = { 0.f, 0.f, 0.f };
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
                part->addForceAccum(accelerationSettings[(part->partType) - 1] * part->mass);    //Add to particle's force accumulated
        }

        //DRAG FORCE
        DragForce::DragForce(float constant1, float constant2) {
                k1 = constant1;
                k2 = constant2;
            }
            void DragForce::updateForce(Particle* part) {
                if (part->isDragForceActive) {
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