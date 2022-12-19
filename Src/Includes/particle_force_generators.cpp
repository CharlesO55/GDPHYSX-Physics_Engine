#include "particle_force_generators.h"
#include <iostream>


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



//PARENT FORCE GENERATORS
ParticleForceGenerator::ParticleForceGenerator() {
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

    //DRAG FORCE CONSTRUCTOR
    DragForce::DragForce(const float constant1, const float constant2) {
        k1 = constant1;
        k2 = constant2;
    }   
        //DRAG FORCE
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
        void CentripetalForce::updateForce(Particle* part) {
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




//TORQUE GENERATORS
void ParticleTorqueGenerator::updateForce(RigidBody* rbCube) {
    std::cout << "NO TORQUE GENERATED\n";
}
    DragTorque::DragTorque(const float constant1, const float constant2) {
        this->k1 = constant1;
        this->k2 = constant2;
    }
        void DragTorque::updateForce(RigidBody* rbCube) {
            if (rbCube->isDragForceActive && rbCube->angularVel != glm::vec3(0.f, 0.f, 0.f)) {
                float drag = glm::length(rbCube->angularVel);    //Magnitude of current particle velocity
                drag = (k1 * drag) + (k2 * drag * drag);          //Drag using constants

                glm::vec3 norm = glm::normalize(rbCube->angularVel); //Normalized velocity for a direction
                rbCube->addTorqueAccum(norm * -drag);                //Add drag * opposite dir into total force          
            }
        }




//FORCE REGISTRY
void ForceRegistry::add(Particle* part, ParticleForceGenerator* forceGen) {
    forceGen->updateForce(part);            //Calculate the force
}